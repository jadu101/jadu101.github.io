---
title: Subdomain Enumeration
draft: false
tags:
  - subdomainator
  - subdomain-enumeration
  - crt-sh
---
## Subdominator

Tool name is : [Subdominator](https://github.com/RevoltSecurities/Subdominator).

First round, it discovered 1620 subdomains:

```
┌──(carabiner1㉿carabiner)-[/opt/Subdominator/subdominator]
└─$ sudo subdominator -d who.int -o who_r1.txt
<SNIP>

                     @RevoltSecurities

[version]:subdominator current version v1.0.9 (latest)
[INFO]: Loading provider configuration file from /root/.config/Subdominator/provider-config.yaml
[INFO]: Enumerating subdomain for who.int
<SNIP>
yemhis.emro.who.int
za.afro.who.int
zm.afro.who.int
ztna.who.int
zw.afro.who.int
[INFO]: Total 1620 subdomains found for who.int in 324.49 seconds
```

Round 2: 

```
┌──(carabiner1㉿carabiner)-[/opt/Subdominator/subdominator]
└─$ sudo subdominator -dL who_r1.txt -o who_r2.txt  

```

## crt.sh

```
┌──(carabiner1㉿carabiner)-[/opt/Subdominator/subdominator]
└─$ curl -s "https://crt.sh/?q=who.int&output=json" | jq -r '.[].name_value' | sed 's/\*\.//g' | sort -u | grep -Po '(\w+\.\w+\.\w+)$' | sudo ~/go/bin/anew crt-who.int

academy.who.int
euro.who.int
emro.who.int
access.who.int
<SNIP>
```

101 results:

```
┌──(carabiner1㉿carabiner)-[/opt/Subdominator/subdominator]
└─$ wc -l crt-who.int 
101 crt-who.int
                
```

