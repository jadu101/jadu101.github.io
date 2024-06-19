---
title: HTB-Editorial
draft: false
tags:
  - htb
  - linux
  - easy
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/Editorial.png)

## Information Gathering
### Rustscan

Rustscan finds ssh and http running on the system. This is a typical hackthebox Linux machine:

`rustscan --addresses 10.10.11.20 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-1.png)


## Enumeration
### HTTP - TCP 80

After adding **editorial.htb** to `/etc/hosts`, we can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image.png)

`/upload` path provides feature for URL priview:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-2.png)

This instantly reminded us with **SSRF** vulnerability. 

## SSRF


Let's interecept the request for preview and send in `http://127.0.0.1`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-3.png)

The response shows directory path to images. Interesting. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-4.png)


## SSH as Dev
### Internal Port Scan

Now that it seems SSRF is verified on this system, let's see if there are any other open ports on the system.

We will send the request over to Intruder and bruteforce on the ports (1-65535):

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-5.png)

Out of all the ports, port 5000 showed a different length of response:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-6.png)

Let's copy down the path to created preview:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-7.png)

After downloading the created file, we can take a look at it.

It seems like we have bunch of api endpoints path revealed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-8.png)


```
/api/latest/metadata/messages/promos
/api/latest/metadata/messages/coupons
/api/latest/metadata/messages/authors
/api/latest/metadata/messages/how_to_use_platform
/api/latest/metadata/changelog
/api/latest/metadata
```

Among the above exposed api endpoints, one path caught our attention. 

Let's take a look at it. 

We will send the preview request for it through Burp Suite repeater as we did earlier:

`http://127.0.0.1:5000/api/latest/metadata/messages/authors`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-9.png)

Downloading and examining on the result, we have credential leak for user dev: `dev080217_devAPI!@`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-10.png)

### SSH

Using the found password, we can ssh in:

`ssh dev@editorial.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-11.png)

## Privesc: Dev to Prod
### .git

There is user **prod** on the system as well. It seems like we need to first escalate our privilege to prod:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-12.png)

Let's enumerate local file system. 

Inside `apps` directory, there is `.git` directory:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-13.png)

Inside `.git`, we see bunch of juicy files:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-14.png)

Taking a look at `HEAD`, it seems like we would be able to obtain log file for the git commits made earlier:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-15.png)

Using the command `git log`, we can see all the previous commits:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-17.png)

Using `git show 1e84a036b2f33c59e2390730699a488c65643d28`, we can read the contents before being downgraded to dev, and inside of it, password for prod is exposed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-16.png)

Using the password `080217_Producti0n_2023!@`, we now have shell as **prod**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-18.png)

## Privesc: Prod to root
### Sudoers

Let's check on commands that can be ran with sudo privilege:

`sudo -l`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-19.png)

`/opt/internal_apps/clone_changes/clone_prod_change.py` could be ran with sudo privilege. Let's take a look at it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-20.png)

`clone_prod_change.py` is using **git** library. hmm, this is interesting. 

Checking on git version, it is `3.1.29`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-21.png)

Searching for known exploits regarding this version, it is vulnerable to CVE-2022-24439:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-22.png)

### CVE-2022-24439


[CVE-2022-24439](https://github.com/advisories/GHSA-hcpj-qp55-gfph) is a RCE vulnerability that is caused from improper user input validation:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-25.png)

[Here](https://github.com/gitpython-developers/GitPython/issues/1515?source=post_page-----0fba80ca64e8--------------------------------) we found a post that would help us to read root.txt.

Let's first create empty root.txt using the command: `echo "" > root.txt`

We will copy the actual root.txt to our empty root.txt inside `/home/prod` directory:

`sudo /usr/bin/python3 /opt/internal_apps/clone_changes/clone_prod_change.py "ext::sh -c cat% /root/root.txt% >% /home/prod/root.txt"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-23.png)

Now we have root.txt copied to our home directory:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/editorial/image-24.png)

Fun and easy box!

## References
- https://github.com/advisories/GHSA-hcpj-qp55-gfph
- https://github.com/gitpython-developers/GitPython/issues/1515?source=post_page-----0fba80ca64e8--------------------------------