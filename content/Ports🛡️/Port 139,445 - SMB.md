---
title: Port 139,445 - SMB
draft: false
tags:
  - smb
  - smbclient
  - crackmapexec
  - enum4linux
  - smbmap
---
## crackmapexec

crackmapexec can reveal domain name:

![](https://i.imgur.com/g79Yojv.png)

With list of usernames and passwords, you can try bruteforcing:

`crackmapexec smb manager.htb -u Desktop/user.txt -p Desktop/user.txt --no-brute --continue-on-success`
## smbclient

Check on non-default shares such as Replication or Users.

`smbclient -N -L //10.10.10.100`

![](https://i.imgur.com/93M3k9H.png)

## smbmap

`smbmap -H 10.10.10.172 -u SABatchJobs -p SABatchJobs`

![](https://i.imgur.com/HVtuW9W.png)


## Download Share
### Recursively Download

```bash
smb: \> mask ""
smb: \> lcd .
smb: \> prompt OFF
smb: \> recurse ON
smb: \> mget *
```


### Mount

**Without Creds:**

`mount -t cifs //10.10.10.134/backups /mnt`

**With Creds:**

`sudo mount -t cifs -o 'username=audit2020,password=audit123~' //10.10.10.192/forensic /mnt`


## Password Hunting

First take a look at what files are in there:

`find .ls`

![](https://i.imgur.com/dIg37tH.png)

You can also list files only: `find . -type f`

Look for passwords:
- `grep -ir 'password' .`
- `grep -ir 'pwd' .`
- `grep -ir 'pass' .`

