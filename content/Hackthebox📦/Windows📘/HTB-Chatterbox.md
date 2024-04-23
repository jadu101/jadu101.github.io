---
title: "[MEDIUM] HTB-Chatterbox"
draft: false
tags:
  - htb
  - windows
  - achat
  - bufferoverflow
  - icacls
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/Chatterbox.png)

**Chatterbox** was more like an Easy level Windows box. I first gained initial foothold by exploiting AChat server with Buffer Overflow. For privilege escalation, user alfred had full access to most of the directories in Administrator folder which I abuse to change permission for root.txt to read. 
## Information Gathering
### Rustscan

Rustscan finds several ports open including port 9255 and 9256 which is uncommon:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/chatterbox]
└─$ rustscan --addresses 10.10.10.74 --range 1-65535 
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
Host is up, received conn-refused (0.36s latency).
Scanned at 2024-04-22 12:15:47 EDT for 3s

PORT      STATE    SERVICE      REASON
139/tcp   filtered netbios-ssn  no-response
445/tcp   open     microsoft-ds syn-ack
9255/tcp  open     mon          syn-ack
9256/tcp  open     unknown      syn-ack
49152/tcp open     unknown      syn-ack
49153/tcp open     unknown      syn-ack
49154/tcp open     unknown      syn-ack
49155/tcp open     unknown      syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.91 seconds
```


### Nmap

Nmap script scan identifies AChat Chat system is running on port 9255:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/chatte┌──(yoon㉿kali)-[~/Documents/htb/chatterbox]
└─$ sudo nmap -sVC -p 139,445,9255,49152,49153,49154,49155 10.10.10.74
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-22 12:17 EDT
Nmap scan report for 10.10.10.74
Host is up (0.40s latency).

PORT      STATE SERVICE      VERSION
139/tcp   open  netbios-ssn  Microsoft Windows netbios-ssn
445/tcp   open  microsoft-ds Windows 7 Professional 7601 Service Pack 1 microsoft-ds (workgroup: WORKGROUP)
9255/tcp  open  http         AChat chat system httpd
49152/tcp open  msrpc        Microsoft Windows RPC
49153/tcp open  msrpc        Microsoft Windows RPC
49154/tcp open  msrpc        Microsoft Windows RPC
49155/tcp open  unknown
Service Info: Host: CHATTERBOX; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
|_clock-skew: mean: 6h15m47s, deviation: 2h18m37s, median: 4h55m45s
| smb2-security-mode: 
|   2:1:0: 
|_    Message signing enabled but not required
| smb2-time: 
|   date: 2024-04-22T21:14:55
|_  start_date: 2024-04-22T21:08:29
| smb-os-discovery: 
|   OS: Windows 7 Professional 7601 Service Pack 1 (Windows 7 Professional 6.1)
|   OS CPE: cpe:/o:microsoft:windows_7::sp1:professional
|   Computer name: Chatterbox
|   NetBIOS computer name: CHATTERBOX\x00
|   Workgroup: WORKGROUP\x00
|_  System time: 2024-04-22T17:14:56-04:00
| smb-security-mode: 
|   account_used: guest
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: disabled (dangerous, but default)

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 96.64 seconds
```

## Enumeration
### SMB - TCP 445

I tried null login for SMB but it is not allowed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image.png)

Crackmapexec discovers computer name CHATTERBOX and that server is running on Windows 7 Professional:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-1.png)


### Achat - TCP 9256

It seems that AChat is vulnerable to Buffer Overflow:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-9.png)


## Shell as Alfred
### AChat Bufferoverflow


I will use [AChat-Reverse-TCP-Exploit](https://github.com/mpgn/AChat-Reverse-TCP-Exploit) that I found online.

Afer downloading both **AChat_payload.sh** and **AChat_Exploit.py** from the source above, I will first slightly modify AChat_payload.sh so that it will work with **nc**.

I can change the parameter `-p windows/meterpreter/reverse_tcp` to `-p windows/shell_reverse_tcp` to make it work with netcat.

After that, I will run it and input the correct value for RHOST, LHOST, and LPORT:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-4.png)

I will copy the output and paste it into **AChat_Exploit.py** as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-6.png)

I will also modify the target server address:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-5.png)

Now I can run the exploit with netcat listener running:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-7.png)

On my local listern, I have a shell as Alfred:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-8.png)

## Read root.txt

Running LinPEAS.exe found several interesting points.

AutoLogon credential for Alfred is discovered: **Welcome1!**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-10.png)

It seems like Alfred got **AllAccess** to most of the Administrator directories:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-11.png)

I can list the directories but I can't read the root.txt:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-12.png)

I can easily bypass this by giving Alfred read permission:

`icacls "C:\Users\Administrator\Desktop\root.txt" /grant Alfred:R`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/chatterbox/image-13.png)

Now I can read root.txt.

## References
- https://tenaka.gitbook.io/pentesting/boxes/achat
- https://github.com/mpgn/AChat-Reverse-TCP-Exploit