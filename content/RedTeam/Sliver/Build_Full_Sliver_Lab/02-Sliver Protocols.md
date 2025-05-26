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
|   DNS Server VM   +--- UDP 53  <------------------+   Target VM       |
|     (Linux)       |                              |    (Windows)       |
|                   |                              |                   |
|  192.168.122.185  |                +-------------+ 192.168.122.160   |
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
         | 192.168.122.111                                       |
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





## References
- https://dominicbreuker.com/post/learning_sliver_c2_03_transports_in_detail_mtls_and_wg/
