---
title: Port 21 - FTP
draft: false
tags:
  - ftp
  - telnet
  - nc
---
## Enumeration
Manual banner grab:

```bash
telnet <IP> 21
nc <IP> 21 
```

All Nmap scripts:

```bash
nmap --script ftp-* -p 21 <IP>
```

Anonymous check with Metasploit:

```bash
use auxiliary/scanner/ftp/anonymous
```
## Anonymous Login
Try following creds:
```bash
anonymous:anonymous
anonymous:
ftp:ftp
```

## Check ✔️

- [ ] Did you check for upload on FTP?

## FTP Bounce Attack

A device that is not exposed to the internet might use the FTP server on different device to outbound the traffic. 

We can use the connection to FTP server to scan the internal device that uses FTP server for outbound traffic. 

```shell-session
jadu101@htb[/htb]$ nmap -Pn -v -n -p80 -b anonymous:password@10.10.110.213 172.17.0.2

Starting Nmap 7.80 ( https://nmap.org ) at 2020-10-27 04:55 EDT
Resolved FTP bounce attack proxy to 10.10.110.213 (10.10.110.213).
Attempting connection to ftp://anonymous:password@10.10.110.213:21
Connected:220 (vsFTPd 3.0.3)
Login credentials accepted by FTP server!
Initiating Bounce Scan at 04:55
FTP command misalignment detected ... correcting.
Completed Bounce Scan at 04:55, 0.54s elapsed (1 total ports)
Nmap scan report for 172.17.0.2
Host is up.

PORT   STATE  SERVICE
80/tcp open http

<SNIP>
```



## References
- https://viperone.gitbook.io/pentest-everything/everything/ports/port-21-or-ftp
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-ftp