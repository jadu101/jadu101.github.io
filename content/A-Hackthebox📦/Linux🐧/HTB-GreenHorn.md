---
title: HTB-GreenHorn
draft: false
tags:
  - htb
  - linux
  - easy
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/GreenHorn.png)

## Rustscan

Rustscan finds HTTP, SSH, and port 3000 open. I am not sure what is running on port 3000 so I should look into it later.

`rustscan --addresses 10.10.11.25 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image.png)

## Enumeration
### HTTP - TCP 80

After adding `greenhorn.htb` to `/etc/hosts`, I can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-1.png)

`http://greenhorn.htb/login.php` shows a login page and a Pluck version:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-2.png)

## Exploitation
### CVE-2023-50564

Googling for known exploits for `pluck 4.7.18`, I found **cve-2023-50564**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-3.png)

Let's use [this](https://github.com/Rai2en/CVE-2023-50564_Pluck-v4.7.18_PoC) github POC to exploit this web server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-6.png)

Reading the code, it seems like the default password is `iloveyou1`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-7.png)

I tried testing it out on `login.php` and it worked:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-8.png)

### Shell as www-data

Before running the exploit, let's first install related module using:

`pip install requests requests_toolbelt`

Now clone the exploit git repository:

`sudo git clone https://github.com/Rai2en/CVE-2023-50564_Pluck-v4.7.18_PoC.git`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-9.png)

Modify `ip` and `port` from the `shell.php`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-10.png)

Next, create `shell.zip` with `shell.php` in it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-11.png)

Modify the hostname in poc.py:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-12.png)

Lastly, let's run the exploit:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-13.png)

We get a reverse shell spawned as www-data on our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-14.png)


## Privesc: www-data to junior

Let's first make the shell more complete using Python:

`python3 -c 'import pty; pty.spawn("/bin/bash")'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-15.png)

Trying out the password `iloveyou1` for the user `junior`, it worked, and now we our privilege escalated:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-20.png)


## Privesc: junior to root

On `junior`'s home directory, there is a file `Using OpenVAS.pdf`. Let's transfer to our Kali attacker machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-21.png)

Reading the pdf, it has a pixelated password on it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-22.png)

Using [toolds.pdf24](https://tools.pdf24.org/en/extract-images?source=post_page-----e87e1cc07864--------------------------------), let's first convert pdf to image and download the image file.


### depix

Now that we have the pdf as image file, we will use [depix](https://github.com/spipm/Depix.git) to recover pixelated password.

Run depix and we get the recovered password:

`python3 depix.py -p ~/Downloads/0.png -s images/searchimages/debruinseq_notepad_Windows10_closeAndSpaced.png -o ~/Documents/htb/greenhorn/output.png`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-23.png)


Recovered password(`sidefromsidetheothersidesidefromsidetheotherside`) worked for root, and now we have a shell as root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/GreenHorn/image-24.png)

## References
- https://github.com/Rai2en/CVE-2023-50564_Pluck-v4.7.18_PoC