---
title: Port 53 - DNS
draft: false
tags:
  - nslookup
  - dig
  - dnsadmins
---
## nslookup
- nslookup
- server [ip]
- [ip]


![](https://i.imgur.com/hB7HCjy.png)


## dig
Verify that the domain exists -> ANSWER: 1 (positive)

`dig @10.10.13 cronos.htb`

![](https://i.imgur.com/MSydf6p.png)

## Zone Transfer
Zone transfer is always worth a try because even if it fails, because sometimes it still provides bunch of information as such:

`dig axfr @10.10.10.13 cronos.htb`

![](https://i.imgur.com/myGSKsF.png)


Add all the discovered domain names to **/etc/hosts**