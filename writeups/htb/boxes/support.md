---
title: Tabby 
category: BOX - Tabby
type: htb_box
layout: page
img-link: 
desc: 10.10.10.194 - [EASY] Tabby
tags: tabby writeup hackthebox
---

# [EASY] Tabby <br/>


![Alt text](images/tabby/tabby-1.png)

# <span style="color:red">Introduction</span> 

**Tabby**, labeled as "*Easy*" on HackTheBox, proved to be a challenging yet instructive endeavor. 
<br />
Initial enumeration revealed a **Local File Inclusion** (LFI) vulnerability, permitting access to the critical **tomcat-users.xml** file, which held login credentials for the "**tomcat**" user. Exploiting this, "**manager-script**" access was gained, facilitating the upload of a Web Application Resource (**WAR**) reverse shell and leading to user escalation as "**ash**." The culmination was a creative use of **LXD** container technology for privilege escalation to root. 
<br />
This experience underscored the significance of exhaustive enumeration, vulnerability exploitation, and diverse attack vectors, significantly enhancing expertise in LFI, credential acquisition, and container-based privilege escalation within cybersecurity.






# <span style="color:red">Box Info</span>

<table>
  <thead>
    <tr>
      <th>Name</th>
    <th style="text-align: right"><a href="https://affiliate.hackthebox.com/box?box=tabby" target="_blank" style="font-size: xx-large; : 0 0 5px #ffffff, 0 0 3px #ffffff; color: #ffffff">
      Tabby
      </a><br /></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>OS</td>
      <td style="text-align: right"><a style="font-size: x-large; : 0 0 5px #ffffff, 0 0 7px #ffffff; color: #2020E">
      Linux
      </a></td>
    </tr>
     <tr>
      <td>1st User blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/310032"><img src="https://www.hackthebox.eu/badge/image/310032"  style="display: unset" /></a></td>
    </tr>
    <tr>
      <td>1st System blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/310032"><img src="https://www.hackthebox.eu/badge/image/310032"  style="display: unset" /></a></td>
    </tr>
  </tbody>
</table>




# <span style="color:red">Enuemeration</span>
## Scanning for open ports using Nmap

```nmap
┌──(yoon㉿kali)-[~/Documents/htb/support]
└─$ cat nmap/open-pport-scan 
# Nmap 7.93 scan initiated Thu Oct 12 11:24:39 2023 as: nmap -sS -p- -oN nmap/open-pport-scan -vv 10.10.11.174
Nmap scan report for localhost (10.10.11.174)
Host is up, received echo-reply ttl 127 (0.34s latency).
Scanned at 2023-10-12 11:24:40 EDT for 950s
Not shown: 65516 filtered tcp ports (no-response)
PORT      STATE SERVICE          REASON
53/tcp    open  domain           syn-ack ttl 127
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
49664/tcp open  unknown          syn-ack ttl 127
49667/tcp open  unknown          syn-ack ttl 127
49674/tcp open  unknown          syn-ack ttl 127
49686/tcp open  unknown          syn-ack ttl 127
49700/tcp open  unknown          syn-ack ttl 127
64527/tcp open  unknown          syn-ack ttl 127

Read data files from: /usr/bin/../share/nmap
# Nmap done at Thu Oct 12 11:40:30 2023 -- 1 IP address (1 host up) scanned in 950.82 seconds
```

## nmap version scan

```nmap
┌──(yoon㉿kali)-[~/Documents/htb/support]
└─$ cat nmap/verion-scan    
# Nmap 7.93 scan initiated Thu Oct 12 11:44:02 2023 as: nmap -sVC -p 53,88,135,139,389,445,464,593,636,3268,3269,5985,9389,49664,49667,49674,49686,49700,64527 -oN nmap/verion-scan -vv 10.10.11.174
Nmap scan report for localhost (10.10.11.174)
Host is up, received echo-reply ttl 127 (0.35s latency).
Scanned at 2023-10-12 11:44:03 EDT for 113s

PORT      STATE SERVICE       REASON          VERSION
53/tcp    open  domain        syn-ack ttl 127 Simple DNS Plus
88/tcp    open  kerberos-sec  syn-ack ttl 127 Microsoft Windows Kerberos (server time: 2023-10-12 15:44:11Z)
135/tcp   open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
139/tcp   open  netbios-ssn   syn-ack ttl 127 Microsoft Windows netbios-ssn
389/tcp   open  ldap          syn-ack ttl 127 Microsoft Windows Active Directory LDAP (Domain: support.htb0., Site: Default-First-Site-Name)
445/tcp   open  microsoft-ds? syn-ack ttl 127
464/tcp   open  kpasswd5?     syn-ack ttl 127
593/tcp   open  ncacn_http    syn-ack ttl 127 Microsoft Windows RPC over HTTP 1.0
636/tcp   open  tcpwrapped    syn-ack ttl 127
3268/tcp  open  ldap          syn-ack ttl 127 Microsoft Windows Active Directory LDAP (Domain: support.htb0., Site: Default-First-Site-Name)
3269/tcp  open  tcpwrapped    syn-ack ttl 127
5985/tcp  open  http          syn-ack ttl 127 Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
9389/tcp  open  mc-nmf        syn-ack ttl 127 .NET Message Framing
49664/tcp open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
49667/tcp open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
49674/tcp open  ncacn_http    syn-ack ttl 127 Microsoft Windows RPC over HTTP 1.0
49686/tcp open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
49700/tcp open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
64527/tcp open  msrpc         syn-ack ttl 127 Microsoft Windows RPC
Service Info: Host: DC; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
|_clock-skew: 0s
| smb2-security-mode: 
|   311: 
|_    Message signing enabled and required
| p2p-conficker: 
|   Checking for Conficker.C or higher...
|   Check 1 (port 59515/tcp): CLEAN (Timeout)
|   Check 2 (port 19493/tcp): CLEAN (Timeout)
|   Check 3 (port 45724/udp): CLEAN (Timeout)
|   Check 4 (port 10954/udp): CLEAN (Timeout)
|_  0/4 checks are positive: Host is CLEAN or ports are blocked
| smb2-time: 
|   date: 2023-10-12T15:45:07
|_  start_date: N/A

Read data files from: /usr/bin/../share/nmap
Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
# Nmap done at Thu Oct 12 11:45:56 2023 -- 1 IP address (1 host up) scanned in 114.28 seconds
```

## SMB

```smb
┌──(yoon㉿kali)-[~/Documents/htb/support]
└─$ smbclient -L //10.10.11.174 --no-pass 

	Sharename       Type      Comment
	---------       ----      -------
	ADMIN$          Disk      Remote Admin
	C$              Disk      Default share
	IPC$            IPC       Remote IPC
	NETLOGON        Disk      Logon server share 
	support-tools   Disk      support staff tools
	SYSVOL          Disk      Logon server share 
Reconnecting with SMB1 for workgroup listing.
do_connect: Connection to 10.10.11.174 failed (Error NT_STATUS_RESOURCE_NAME_NOT_FOUND)
Unable to connect with SMB1 -- no workgroup available
```


```bash
                                                                                   
┌──(yoon㉿kali)-[~/Documents/htb/support]
└─$ smbclient //10.10.11.174/support-tools
Password for [WORKGROUP\yoon]:

Try "help" to get a list of possible commands.
smb: \> ls
  .                                   D        0  Wed Jul 20 13:01:06 2022
  ..                                  D        0  Sat May 28 07:18:25 2022
  7-ZipPortable_21.07.paf.exe         A  2880728  Sat May 28 07:19:19 2022
  npp.8.4.1.portable.x64.zip          A  5439245  Sat May 28 07:19:55 2022
  putty.exe                           A  1273576  Sat May 28 07:20:06 2022
  SysinternalsSuite.zip               A 48102161  Sat May 28 07:19:31 2022
  UserInfo.exe.zip                    A   277499  Wed Jul 20 13:01:07 2022
  windirstat1_1_2_setup.exe           A    79171  Sat May 28 07:20:17 2022
  WiresharkPortable64_3.6.5.paf.exe      A 44398000  Sat May 28 07:19:43 2022

		4026367 blocks of size 4096. 962960 blocks available
```

```bash
┌──(yoon㉿kali)-[~/Documents/htb/support/smb-files]
└─$ ls       
7-ZipPortable_21.07.paf.exe  UserInfo.exe.zip
npp.8.4.1.portable.x64.zip   windirstat1_1_2_setup.exe
putty.exe                    WiresharkPortable64_3.6.5.paf.exe
SysinternalsSuite.zip
```