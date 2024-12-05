---
title: Windows - Netsh
draft: false
tags:
  - netsh
---
**Netsh** - Windows command line tool that helps with network configuration of a particular windows system. 

Scenario:

We compromised Windows 10 IT admin's workstation (`10.129.15.150`,`172.16.5.25`).

Attacker(10.10.15.5) -> Windows10(`10.129.15.150`,`172.16.5.25`) -> Windows Server (172.16.5.25)

## Netsh

We can use `netsh.exe` to forward all data received on a specific port like 8080 to a remote host on a remote port. 

```cmd-session
C:\Windows\system32> netsh.exe interface portproxy add v4tov4 listenport=8080 listenaddress=10.129.15.150 connectport=3389 connectaddress=172.16.5.25
```

Windows10 machine will listen for incoming connections on port 8080.

Traffic is forwarded to port 3389 on Windows Server.

On Windows10 machine, any incoming connections on port 8080 will be forwarded to port 3389 on Windows Server. 

We can verify port forward as such:

```cmd-session
C:\Windows\system32> netsh.exe interface portproxy show v4tov4

Listen on ipv4:             Connect to ipv4:

Address         Port        Address         Port
--------------- ----------  --------------- ----------
10.129.42.198   8080        172.16.5.25     3389
```

Now we can access Windows server's port 8080 via Windows10's port 8080:

```shell-session
xfreerdp /v:10.129.15.150:8080 /u:user1 /p:password123
```