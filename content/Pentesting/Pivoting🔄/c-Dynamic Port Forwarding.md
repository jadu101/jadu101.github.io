---
title: c-Dynamic Port Forwarding
draft: false
tags:
  - pivoting
---
**Pivoting**:

Accessing hosts to cross network boundaries you would not usually have access to. 


## SSH Tunneling over SOCKS Proxy

Use `ifconfig` or `ipconfig` for finding NICs.

Let's say we compromised Ubuntu server and it got multiple NICs:

```shell-session
ubuntu@WEB01:~$ ifconfig 

ens192: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.129.202.64  netmask 255.255.0.0  broadcast 10.129.255.255
        inet6 dead:beef::250:56ff:feb9:52eb  prefixlen 64  scopeid 0x0<global>
        inet6 fe80::250:56ff:feb9:52eb  prefixlen 64  scopeid 0x20<link>
        ether 00:50:56:b9:52:eb  txqueuelen 1000  (Ethernet)
        RX packets 35571  bytes 177919049 (177.9 MB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 10452  bytes 1474767 (1.4 MB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

ens224: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 172.16.5.129  netmask 255.255.254.0  broadcast 172.16.5.255
        inet6 fe80::250:56ff:feb9:a9aa  prefixlen 64  scopeid 0x20<link>
        ether 00:50:56:b9:a9:aa  txqueuelen 1000  (Ethernet)
        RX packets 8251  bytes 1125190 (1.1 MB)
        RX errors 0  dropped 40  overruns 0  frame 0
        TX packets 1538  bytes 123584 (123.5 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 270  bytes 22432 (22.4 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 270  bytes 22432 (22.4 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```


To access `ens224` network, we will have to perform dynamic port forwarding and pivot our network packets via the Ubuntu server. 

We will set up SOCKS listener on attacker machine and then configure SSH to forward that traffic via SSH to the network (172.16.5.0/23) after connecting to the target host. 

Let's first enable Dynamic Port Forwarding with SSH:

```shell-session
ssh -D 9050 ubuntu@10.129.202.64
```

`-D` argument requests to the SSH server to enable dynamic port forwarding. 

We now need proxychains to route all the packets over the port 9050.

Let's modify proxychains configuration file `/etc/proxychains.conf`:

```shell-session
jadu101@htb[/htb]$ tail -4 /etc/proxychains.conf

# meanwile
# defaults set to "tor"
socks4 	127.0.0.1 9050
```

Now all the traffic generated from proxychains will be routed to localhost port 9050, which will be SSH forwarded to 172.16.5.0/23 network. 

```shell-session
jadu101@htb[/htb]$ proxychains nmap -v -sn 172.16.5.1-200

ProxyChains-3.1 (http://proxychains.sf.net)

Starting Nmap 7.92 ( https://nmap.org ) at 2022-02-24 12:30 EST
Initiating Ping Scan at 12:30
Scanning 10 hosts [2 ports/host]
|S-chain|-<>-127.0.0.1:9050-<><>-172.16.5.2:80-<--timeout
|S-chain|-<>-127.0.0.1:9050-<><>-172.16.5.5:80-<><>-OK
|S-chain|-<>-127.0.0.1:9050-<><>-172.16.5.6:80-<--timeout
RTTVAR has grown to over 2.3 seconds, decreasing to 2.0
```

Note that when nmap through SOCKS tunneling:
- Only Full TCP Connect Scan is allowed over proxychains
- Host-alive checks may not work well with Windows because Firewall blocks ICMP requests by default

We can scan remote host as such:

```shell-session
proxychains nmap -v -Pn -sT 172.16.5.19
```

## Tools with Proxychains
### Metasploit

We can  use Metasploit along with proxychains. 

It is as simple as:

```shell-session
proxychains msfconsole
```

### RDP

We can also rdp via proxychains

```shell-session
proxychains xfreerdp /v:172.16.5.19 /u:victor /p:pass@123
```