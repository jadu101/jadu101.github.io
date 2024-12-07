---
title: Crack Linux Hash
draft: false
tags:
  - shadow
  - passwd
  - opasswd
---
There are some ways to collect password hashes from Linux. 

opasswd:

```shell-session
jadu101@kali[/kali]$ sudo cat /etc/security/opasswd

jadu101:1000:2:$1$HjFAfYTG$qNDkF0zJ3v8ylCOrKB0kt0,$1$kcUjWZJX$E9uMSmiQeRh4pAAgzuvkq1
```

shadow:


```shell-session
[jadu101@kali]─[~]$ sudo cat /etc/shadow

root:*:18747:0:99999:7:::
sys:!:18747:0:99999:7:::
...SNIP...
jadu101:$6$wBRzy$...SNIP...x9cDWUxW1:18937:0:99999:7:::
```

## Crack

Let's unshadow the hash:

```shell-session
jadu101@htb[/htb]$ sudo cp /etc/passwd /tmp/passwd.bak 
jadu101@htb[/htb]$ sudo cp /etc/shadow /tmp/shadow.bak 
jadu101@htb[/htb]$ unshadow /tmp/passwd.bak /tmp/shadow.bak > /tmp/unshadowed.hashes
```

Now crack it using hashcat:

```shell-session
jadu101@htb[/htb]$ hashcat -m 1800 -a 0 /tmp/unshadowed.hashes rockyou.txt -o /tmp/unshadowed.cracked
```