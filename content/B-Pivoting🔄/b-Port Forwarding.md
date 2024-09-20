---
title: b-Port Forwarding
draft: false
tags:
  - port-forwarding
---

**Port Forwarding**: 

Technique that allows attacker to redirect a communication request from one port to another. 
Usually TCP is used for port forwarding. 

Sometimes different application layer protocols such as SSH and SOCKS(non-application layer) can be used for it as well. 

## SSH Local Port Forwarding

Let's say we have compromised Ubuntu server and it has SSH open and MySQL running locally. 

We can either SSH-in and access MySQL from inside or port forward it and access it locally. 

Let's port forward through port 1234 over SSH.

On Attacker's machine:

```shell-session
ssh -L 1234:localhost:3306 target-username@target-ip-addr
```

`-L`: Tells the SSH client to request the SSH server to forward all the data we send via port 1234 to localhost 3306 on the Ubuntu Server. 

Now we can access MySQL service locally on attacker machine through port 1234. 

We can use `netstat` to confirm it being forwarded:

```shell-session
jadu101@htb[/htb]$ netstat -antp | grep 1234

(Not all processes could be identified, non-owned process info
 will not be shown, you would have to be root to see it all.)
tcp        0      0 127.0.0.1:1234          0.0.0.0:*               LISTEN      4034/ssh            
tcp6       0      0 ::1:1234                :::*                    LISTEN      4034/ssh     
```

If we want to forward multiple ports from the Ubuntu target server, we can do so as below. The following command forwards the apache web server's port 80 to attacker host's local port 8080:

```shell-session
ssh -L 1234:localhost:3306 -L 8080:localhost:80 ubuntu@10.129.202.64
```

