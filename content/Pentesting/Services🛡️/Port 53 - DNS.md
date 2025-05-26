---
title: Port 53 - DNS
draft: false
tags:
  - nslookup
  - dig
  - dnsadmins
---
DNS translates domain names to IP addresses. 



## nslookup
- nslookup
- server [ip]
- [ip]


![](https://i.imgur.com/hB7HCjy.png)


## dig
Verify that the domain exists -> ANSWER: 1 (positive)

`dig @10.10.13 cronos.htb`

![](https://i.imgur.com/MSydf6p.png)


| Command                         | Description                                                                                                                                       |
| ------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `dig domain.com`                | Performs a default A record lookup for the domain.                                                                                                |
| `dig domain.com A`              | Retrieves the IPv4 address (A record) associated with the domain.                                                                                 |
| `dig domain.com AAAA`           | Retrieves the IPv6 address (AAAA record) associated with the domain.                                                                              |
| `dig domain.com MX`             | Finds the mail servers (MX records) responsible for the domain.                                                                                   |
| `dig domain.com NS`             | Identifies the authoritative name servers for the domain.                                                                                         |
| `dig domain.com TXT`            | Retrieves any TXT records associated with the domain.                                                                                             |
| `dig domain.com CNAME`          | Retrieves the canonical name (CNAME) record for the domain.                                                                                       |
| `dig domain.com SOA`            | Retrieves the start of authority (SOA) record for the domain.                                                                                     |
| `dig @1.1.1.1 domain.com`       | Specifies a specific name server to query; in this case 1.1.1.1                                                                                   |
| `dig +trace domain.com`         | Shows the full path of DNS resolution.                                                                                                            |
| `dig -x 192.168.1.1`            | Performs a reverse lookup on the IP address 192.168.1.1 to find the associated host name. You may need to specify a name server.                  |
| `dig +short domain.com`         | Provides a short, concise answer to the query.                                                                                                    |
| `dig +noall +answer domain.com` | Displays only the answer section of the query output.                                                                                             |
| `dig domain.com ANY`            | Retrieves all available DNS records for the domain (Note: Many DNS servers ignore ANY queries to reduce load and prevent abuse, as per RFC 8482). |

## Zone Transfer

DNS Zone is a portion of the DNS namespace that a specific organization or administrator manages. 

DNS servers use DNS zone transfer to copy a portion of their database to another DNS server. 

Zone transfer is always worth a try because even if it fails, because sometimes it still provides bunch of information as such:

`dig axfr @10.10.10.13 cronos.htb`

![](https://i.imgur.com/myGSKsF.png)


Add all the discovered domain names to **/etc/hosts**



## DNSenum

```bash
dnsenum --enum inlanefreight.com -f /usr/share/seclists/Discovery/DNS/subdomains-top1million-110000.txt -r
```

## Subdomain Enumeration
### subfinder

```shell-session
jadu101@htb[/htb]# ./subfinder -d inlanefreight.com -v       
                                                                       
        _     __ _         _                                           
____  _| |__ / _(_)_ _  __| |___ _ _          
(_-< || | '_ \  _| | ' \/ _  / -_) '_|                 
/__/\_,_|_.__/_| |_|_||_\__,_\___|_| v2.4.5                                                                                                                                                                                                                                                 
                projectdiscovery.io                    
                                                                       
[WRN] Use with caution. You are responsible for your actions
[WRN] Developers assume no liability and are not responsible for any misuse or damage.
[WRN] By using subfinder, you also agree to the terms of the APIs used. 
                                   
[INF] Enumerating subdomains for inlanefreight.com
[alienvault] www.inlanefreight.com
[dnsdumpster] ns1.inlanefreight.com
[dnsdumpster] ns2.inlanefreight.com
...snip...
```

### subbrute

```shell-session
jadu101@htb[/htb]$ git clone https://github.com/TheRook/subbrute.git >> /dev/null 2>&1
jadu101@htb[/htb]$ cd subbrute
jadu101@htb[/htb]$ echo "ns1.inlanefreight.com" > ./resolvers.txt
jadu101@htb[/htb]$ ./subbrute inlanefreight.com -s ./names.txt -r ./resolvers.txt

Warning: Fewer than 16 resolvers per process, consider adding more nameservers to resolvers.txt.
inlanefreight.com
ns2.inlanefreight.com
www.inlanefreight.com
ms1.inlanefreight.com
support.inlanefreight.com

<SNIP>
```


## DNS Spoofing

DNS Cache Poisoning. 

Alters legitimate DNS records with false information so that they can be used to redirect online traffic to a fraudulent website. 

In local network, attacker can perform DNS cache poisoning using Ettercap or Bettercap.

### Ettercap

First modify etter.dns and map the target domain name to attacker's ip:

```shell-session
jadu101@htb[/htb]# cat /etc/ettercap/etter.dns

inlanefreight.com      A   192.168.225.110
*.inlanefreight.com    A   192.168.225.110
```

Next, start ettercap and scan for live hosts via `Hosts -> Scan for Hosts`.

Once done, add the target IP address to Target1 and add a default gateway to Target2.

Activate `dns_spoof` attack by navigating to `Plugins -> Manage Plugins`. this will send the target machine with fake DNS responses that will resolve the domain name back to the attacker's IP address. 

If it is successfull, target user accessing the domain will be rediercted to the fake website hosted on the attacker's IP address. 

