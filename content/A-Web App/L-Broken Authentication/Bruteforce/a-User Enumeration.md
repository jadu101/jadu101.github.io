---
title: a-User Enumeration
draft: false
tags:
  - ffuf
---
User enumeration vulnerabilities arise when a web app responds differently to registered/valid and invalid inputs for authentication endpoints.

Web developers sometimes overlook user enumeration vectors thinking they are not very confidential but actually it should be considered very confidential.

Users tend to use the same username across various services.

## User Enumeration Theory

Web app might reveal whether a username exists or not, which can be helpful for identifying the existing usernames. 

e.g WordPress

> Wrong Username - Error: Unknown username.
> Correct Username - Message: Password you entered for the username is incorrect.


## Differ Error Messages

We can easily harvest list of potential usernames via OSINT.

Let's use `xato-net-10-million-usernames.txt` to enumerate valid users with `ffuf`.

Below `ffuf` command filters out `Unknown user` error message and finds for valid username:

- `-w` - Wordlist
- `-d` - POST data

```shell-session
jadu101@htb[/htb]$ ffuf -w /opt/useful/SecLists/Usernames/xato-net-10-million-usernames.txt -u http://172.17.0.2/index.php -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "username=FUZZ&password=invalid" -fr "Unknown user"

<SNIP>

[Status: 200, Size: 3271, Words: 754, Lines: 103, Duration: 310ms]
    * FUZZ: consuelo
```

## Side-Channel Attacks

User enumeration by differing error message is the most simple and obvious way but we might also be able to enumerate valid usernames via side channels. 

We can use something like a response timing, i.e the time it takes for the web application's response to reach us.

We might be able to measure a difference in the response time and enumerate valid usernames this way, even if the response is the same.

