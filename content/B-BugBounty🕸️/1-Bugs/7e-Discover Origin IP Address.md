---
title: 7e-Discover Origin IP Address
draft: false
tags:
---
Let's say we are targeting `carabinersec.com` and we want to DoS or DDoS attacks on it. 

However, if WAF is blocking us, we won't be able to do so. 

By discovering the original IP address that is not under WAF, we should be able to conduct DoS attack. 

Here are hackerone reports about this bug:

- [https://hackerone.com/reports/1536299](https://hackerone.com/reports/1536299)
- [https://hackerone.com/reports/1556808](https://hackerone.com/reports/1556808)

## Identify WAF

First, try to ping the domain:

```
┌──(carabiner1㉿carabiner)-[~]
└─$ ping yahoo.com
PING yahoo.com (74.6.231.20) 56(84) bytes of data.
64 bytes from media-router-fp73.prod.media.vip.ne1.yahoo.com (74.6.231.20): icmp_seq=1 ttl=128 time=364 ms
64 bytes from media-router-fp73.prod.media.vip.ne1.yahoo.com (74.6.231.20): icmp_seq=2 ttl=128 time=389 ms
64 bytes from media-router-fp73.prod.media.vip.ne1.yahoo.com (74.6.231.20): icmp_seq=3 ttl=128 time=306 ms
^C
--- yahoo.com ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2001ms
rtt min/avg/max/mdev = 305.683/352.881/388.834/34.865 ms
```

We can see that the IP address is exposed. 

Try copy-pasting the IP address to any browser. We will get error.

This might happen because the website is behind cloudfare WAF.

Now let's try to discover the original IP.

## Discover Original IP

Using the various methods below, collect IP addresses from the domain.

| Tool/Service     | URL                                         | Query Example               |
|------------------|---------------------------------------------|-----------------------------|
| **Shodan**        | [https://www.shodan.io](https://www.shodan.io)             | `ssl.cert.subject.CN:"*.yahoo.com" 200` |
| **Censys**        | [https://search.censys.io/](https://search.censys.io/)      | `"yahoo.com"`               |
| **Fofa**          | [https://fofa.so](https://fofa.so)                         | `"yahoo.com"`               |
| **SecurityTrails**| [https://securitytrails.com/](https://securitytrails.com/)  | `"yahoo.com"`               |
| **NSLookup**      | [https://nslookup.io](https://nslookup.io)                  | N/A                         |
| **CompleteDNS**   | [https://completedns.com/](https://completedns.com/)        | N/A                         |
| **ZoomEye**       | [https://zoomeye.hk](https://zoomeye.hk)                    | N/A                         |
| **VirusTotal**    | [https://virustotal.com](https://virustotal.com)            | N/A                         |
| **Netlas**        | [https://netlas.io](https://netlas.io)                      | N/A                         |
| **DNSHistory**    | [https://dnshistory.org/](https://dnshistory.org/)          | N/A                         |
| **IPLocation**    | [https://iplocation.net](https://iplocation.net)            | N/A                         |
| **IntoDNS**       | [https://intodns.com](https://intodns.com)                  | N/A                         |
| **IPVoid**        | [https://ipvoid.com](https://ipvoid.com)                    | N/A                         |
| **Whois**         | [https://who.is](https://who.is)                            | N/A                         |
| **DNSWatch**      | [https://dnswatch.info](https://dnswatch.info)              | N/A                         |
| **ViewDNS**       | [https://viewdns.info](https://viewdns.info)                | N/A                         |
| **DNSChecker**    | [https://dnschecker.org](https://dnschecker.org)            | N/A                         |

After collecting all the IP addresses, use `httpx` for live IP address and use `aquatone` for screenshots:

```
cat collected-ip.txt | httpx | aquatone
```

## Automation

Instead, we can automate it:

- https://github.com/christophetd/CloudFlair
- https://github.com/spyboy-productions/CloakQuest3r
- https://github.com/zidansec/CloudPeler
- https://github.com/Aidennnn33/cloudgazer
- https://github.com/m0rtem/CloudFail
- https://github.com/Alaa-abdulridha/CloudSniffer
- https://github.com/mmarting/unwaf
- https://github.com/hakluke/hakoriginfinder
- https://github.com/gwen001/cloudflare-origin-ip
- https://github.com/mrh0wl/Cloudmare