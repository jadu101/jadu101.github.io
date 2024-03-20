---
title: Port 21 - FTP
draft: false
tags:
  - ftp
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
## References
- https://viperone.gitbook.io/pentest-everything/everything/ports/port-21-or-ftp
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-ftp