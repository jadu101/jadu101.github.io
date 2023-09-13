---
title: Jadu 
category: BOX - Jadu
type: htb_box
layout: page
img-link: images/Jadu/jadu-plum.png
desc: 10.10.10.123 - [MEDIUM] Jadu by Jadu101
tags: Jadu writeup hackthebox
---

# [TEST] Jadu <br/>

<img src="images/jadu/jadu-plum.png" alt="Jadu" style="max-width: 10%; height: auto;">

# <span style="color:red">Introduction</span> 

**Jadu** is not a Hackthebox machine. 

This post is for me to summarize how I make post to **Jadu101**. 

# <span style="color:red">Box Info</span>

<table>
  <thead>
    <tr>
      <th>Name</th>
      <th style="text-align: right"><a href="https://pixabay.com/illustrations/plum-fruit-drawing-cutout-vitamin-7571923/" target="_blank" style="font-size: xx-large; text-shadow: 0 0 5px #ffffff, 0 0 7px #ffffff; color: #1756a9">
      Jadu
      </a><br /></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>OS</td>
      <td style="text-align: right"><a style="font-size: x-large; : 0 0 5px #ffffff, 0 0 7px #ffffff; color: #2020E">
      Jadu
      </a></td>
    </tr>
     <tr>
      <td>1st User blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/1474111"><img src="https://www.hackthebox.eu/badge/image/1474111" alt="Jadu101" style="display: unset" /></a></td>
    </tr>
      <tr>
      <td>1st System blood</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/1474111"><img src="https://www.hackthebox.eu/home/users/profile/1474111" alt="Jadu101" style="display: unset" /></a></td>
    </tr>
    <tr>
      <td>Creator</td>
      <td style="text-align: right"><a href="https://www.hackthebox.eu/home/users/profile/1474111"><img src="https://www.hackthebox.eu/home/users/profile/1474111" alt="" style="display: unset" /></a><br /></td>
    </tr>
  </tbody>
</table>




# <span style="color:red">Recon</span>

## Nmap

### Scanning for open ports

```nmap``` finds three open ports, msrpc(135) and smb(139,445): 
<br />

```console
# Nmap 7.93 scan initiated Fri Jul  7 01:53:48 2023 as: nmap -sT -p- --min-rate 5000 -o open_port_scan -vv 10.10.10.40
Increasing send delay for 10.10.10.40 from 0 to 5 due to 18 out of 59 dropped probes since last increase.
Nmap scan report for 10.10.10.40 (10.10.10.40)
Host is up, received echo-reply ttl 127 (0.28s latency).
Scanned at 2023-07-07 01:53:48 EDT for 37s
Not shown: 65494 filtered tcp ports (no-response)
PORT      STATE  SERVICE          REASON
<snip>
135/tcp   open   msrpc            syn-ack
139/tcp   open   netbios-ssn      syn-ack
<snip>
445/tcp   open   microsoft-ds     syn-ack
<snip>

Read data files from: /usr/bin/../share/nmap
# Nmap done at Fri Jul  7 01:54:25 2023 -- 1 IP address (1 host up) scanned in 37.85 seconds
```

### Enumerating software versions

There's nothing interesting from version scan. 
<br />

```console
# Nmap 7.93 scan initiated Fri Jul  7 01:56:11 2023 as: nmap -sVC -p 135,139,445 -vv -o ver_scan 10.10.10.40
Nmap scan report for 10.10.10.40 (10.10.10.40)
Host is up, received echo-reply ttl 127 (0.34s latency).
Scanned at 2023-07-07 01:56:12 EDT for 26s

PORT    STATE SERVICE      REASON          VERSION
135/tcp open  msrpc        syn-ack ttl 127 Microsoft Windows RPC
139/tcp open  netbios-ssn  syn-ack ttl 127 Microsoft Windows netbios-ssn
445/tcp open  microsoft-ds syn-ack ttl 127 Windows 7 Professional 7601 Service Pack 1 microsoft-ds (workgroup: WORKGROUP)
Service Info: Host: HARIS-PC; OS: Windows; CPE: cpe:/o:microsoft:windows

Host script results:
|_clock-skew: mean: 1h39m00s, deviation: 34m35s, median: 1h58m58s
| smb2-security-mode: 
|   210: 
|_    Message signing enabled but not required
| smb-security-mode: 
|   account_used: guest
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: disabled (dangerous, but default)
| smb-os-discovery: 
|   OS: Windows 7 Professional 7601 Service Pack 1 (Windows 7 Professional 6.1)
|   OS CPE: cpe:/o:microsoft:windows_7::sp1:professional
|   Computer name: haris-PC
|   NetBIOS computer name: HARIS-PC\x00
|   Workgroup: WORKGROUP\x00
|_  System time: 2023-07-07T08:55:24+01:00
| smb2-time: 
|   date: 2023-07-07T07:55:25
|_  start_date: 2023-07-07T07:50:47
| p2p-conficker: 
|   Checking for Conficker.C or higher...
|   Check 1 (port 62671/tcp): CLEAN (Couldn't connect)
|   Check 2 (port 12383/tcp): CLEAN (Couldn't connect)
|   Check 3 (port 19006/udp): CLEAN (Failed to receive data)
|   Check 4 (port 59387/udp): CLEAN (Timeout)
|_  0/4 checks are positive: Host is CLEAN or ports are blocked

Read data files from: /usr/bin/../share/nmap
Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
# Nmap done at Fri Jul  7 01:56:38 2023 -- 1 IP address (1 host up) scanned in 26.91 seconds
```


### Nmap vulnerability script scan

This machine is vulnerable to **CVE-2017-0143**:
<br />

```console
└─$ sudo nmap -p 135,139,445 --script smb-vuln* -vv 10.10.10.40 -o vuln_scan
[sudo] password for yoon: 
<snip>

PORT    STATE SERVICE      REASON
135/tcp open  msrpc        syn-ack ttl 127
139/tcp open  netbios-ssn  syn-ack ttl 127
445/tcp open  microsoft-ds syn-ack ttl 127

Host script results:
|_smb-vuln-ms10-054: false
|_smb-vuln-ms10-061: NT_STATUS_OBJECT_NAME_NOT_FOUND
| smb-vuln-ms17-010: 
|   VULNERABLE:
|   Remote Code Execution vulnerability in Microsoft SMBv1 servers (ms17-010)
|     State: VULNERABLE
|     IDs:  CVE:CVE-2017-0143
|     Risk factor: HIGH
|       A critical remote code execution vulnerability exists in Microsoft SMBv1
|        servers (ms17-010).
|           
|     Disclosure date: 2017-03-14
|     References:
|       https://blogs.technet.microsoft.com/msrc/2017/05/12/customer-guidance-for-wannacrypt-attacks/
|       https://technet.microsoft.com/en-us/library/security/ms17-010.aspx
|_      https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2017-0143

NSE: Script Post-scanning.
NSE: Starting runlevel 1 (of 1) scan.
Initiating NSE at 04:34
Completed NSE at 04:34, 0.00s elapsed
Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 23.70 seconds
           Raw packets sent: 7 (284B) | Rcvd: 7 (280B)
```



# <span style="color:red">Exploitation</span>

## Metasploit

Run **msfconsole** on terminal and choose an exploit to use:
<br />

```console
msf6 > use windows/smb/ms17_010_eternalblue
[*] No payload configured, defaulting to windows/x64/meterpreter/reverse_tcp
```


<br />Thank you for reading!
<br />
## Sources:
- https://hackthebox.com/




# <span style="color:red">Uploading Write-Ups to Jadu101</span>


### _posts

First create a markdown file on **_posts**.
<br />

Content should be like the following, with the title such as "2023-09-13-jadu.md"

<br />
```markdown
---
title: HTB Box Writeup - Jadu
link: /writeups/htb/boxes/Jadu.html
---
```
<br />
This links markdown file on **_posts** to the actual writeup that is located in **/htb/boxes**

### /htb/boxes

If writeups is about HTB challenges, do the follow the same procedure on **/htb/challs**.
<br />

With "2023-09-13-jadu.md** existing on **_posts**, upload writeup as **/htb/boxes/Jadu.md**
<br />

**Jadu.md** should be as such:
<br />

```markdown
---
title: Jadu 
category: BOX - Jadu
type: htb_box
layout: page
img-link: images/Jadu/jadu-plum.png
desc: 10.10.10.123 - [MEDIUM] Jadu by Jadu101
tags: Jadu writeup hackthebox
---
```

<br />
For uploading images, it could be as such:
<br />
```
<img src="images/jadu/jadu-plum.png" alt="Jadu" style="max-width: 10%; height: auto;">
```
<br />
Images should be uploaded to **/htb/boxes/images/jadu**. 


