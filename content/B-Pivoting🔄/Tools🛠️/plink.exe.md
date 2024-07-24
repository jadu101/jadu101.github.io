---
title: Windows - plink.exe
draft: false
tags:
  - plink
---
Plink( PuTTY Link) -  Windows command-line SSH tool.

Plink can be used for dynamic port forwarding and SOCKS Proxy.

When we need to use Windows as the PivotHost and if we have plink installed, it can provide attacker little bit of anonymity.

Scenario:

Attacker(Windows) -> Ubuntu -> Windows

## Plink.exe 

Command below will start SSH session between Windows Attack host and the Ubuntu Server. 

Plink starts listening on port 9050.

```cmd-session
plink -ssh -D 9050 ubuntu@10.129.15.50
```



