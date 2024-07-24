---
title: Port 139,445 - SMB
draft: false
tags:
  - smb
  - smbclient
  - crackmapexec
  - enum4linux
  - smbmap
  - smbclient-py
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

### NTLM Auth Disabled

In such case like [HTB-Scrambled](https://0xdf.gitlab.io/2022/10/01/htb-scrambled-linux.html), NTLM authentication might be disabled for security purpose and you won't be able to use standard tools and you won't be able to access any any service by IP address if it requires authentication.

Using impacket's **smbclient.py** you can access SMB:

`smbclient.py -k scrm.local/ksimpson:ksimpson@dc1.scrm.local -dc-ip dc1.scrm.local`

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

## Interaction On Windows
### CMD

On CMD, we can list the share as such:

`dir \\ip-addr\Finance`

To connect to a share, we can use `net use` as such:

`net use n: \\ip-addr\Finance`

If we need to authenticate, credentials can be provided as such:

`net use n: \\ip-addr\Finance /user:user1 Password123`

To find out how many files the share contains:

`dir n: /a-d /s /b | find /c ":\"`

We can look for files with certain names as such:

```cmd-session
C:\htb>dir n:\*cred* /s /b

n:\Contracts\private\credentials.txt


C:\htb>dir n:\*secret* /s /b

n:\Contracts\private\secret.txt
```

If we want to look for a specific word within a text file, we can use `findstr`:

```cmd-session
c:\htb>findstr /s /i cred n:\*.*

n:\Contracts\private\secret.txt:file with all credentials
n:\Contracts\private\credentials.txt:admin:SecureCredentials!
```


### PowerShell

List the share:

```powershell-session
Get-ChildItem \\ip-addr\Finance\
```

To connect to the share:

```powershell-session
New-PSDrive -Name "N" -Root "\\ip-addr\Finance" -PSProvider "FileSystem"
```

To provide a username and password, we need to create PSCredential object:

```powershell-session
PS C:\htb> $username = 'plaintext'
PS C:\htb> $password = 'Password123'
PS C:\htb> $secpassword = ConvertTo-SecureString $password -AsPlainText -Force
PS C:\htb> $cred = New-Object System.Management.Automation.PSCredential $username, $secpassword
PS C:\htb> New-PSDrive -Name "N" -Root "\\192.168.220.129\Finance" -PSProvider "FileSystem" -Credential $cred

Name           Used (GB)     Free (GB) Provider      Root                                                              CurrentLocation
----           ---------     --------- --------      ----                                                              ---------------
N                                      FileSystem    \\192.168.220.129\Finance
```

To look for a file with certain name:

```powershell-session
Get-ChildItem -Recurse -Path N:\ -Include *cred* -File
```

To look for a file that contains the certain keyword:

```powershell-session
Get-ChildItem -Recurse -Path N:\ | Select-String "cred" -List
```

