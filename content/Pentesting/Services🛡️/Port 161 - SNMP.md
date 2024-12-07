---
title: UDP 161 - SNMP
draft: false
tags:
  - snmp
---

## Bruteforce Community String
### onesixtyone
`onesixtyone -c /usr/share/seclists/Discovery/SNMP/snmp.txt 10.129.154.68`

```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ onesixtyone -c /usr/share/seclists/Discovery/SNMP/snmp.txt 10.129.154.68  
Scanning 1 hosts, 3219 communities
10.129.154.68 [backup] Linux NIXHARD 5.4.0-90-generic #101-Ubuntu SMP Fri Oct 15 20:00:55 UTC 2021 x86_64
```

### hydra


```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ hydra -P /usr/share/seclists/Discovery/SNMP/snmp.txt 10.129.154.68 snmp
Hydra v9.6dev (c) 2023 by van Hauser/THC & David Maciejak - Please do not use in military or secret service organizations, or for illegal purposes (this is non-binding, these *** ignore laws and ethics anyway).

Hydra (https://github.com/vanhauser-thc/thc-hydra) starting at 2024-07-08 23:15:57
[DATA] max 16 tasks per 1 server, overall 16 tasks, 3217 login tries (l:1/p:3217), ~202 tries per task
[DATA] attacking snmp://10.129.154.68:161/
[STATUS] 128.00 tries/min, 128 tries in 00:01h, 3089 to do in 00:25h, 16 active
[161][snmp] host: 10.129.154.68   password: backup
[STATUS] attack finished for 10.129.154.68 (valid pair found)
1 of 1 target successfully completed, 1 valid password found
Hydra (https://github.com/vanhauser-thc/thc-hydra) finished at 2024-07-08 23:18:32
```

## Footprinting

Once we know the community string and the SNMP service that does not require authentication (versions 1, 2c), we can query internal system information.

### braa

```shell-session
jadu101@htb[/htb]$ sudo apt install braa
jadu101@htb[/htb]$ braa <community string>@<IP>:.1.3.6.*   # Syntax
jadu101@htb[/htb]$ braa public@10.129.14.128:.1.3.6.*

10.129.14.128:20ms:.1.3.6.1.2.1.1.1.0:Linux htb 5.11.0-34-generic #36~20.04.1-Ubuntu SMP Fri Aug 27 08:06:32 UTC 2021 x86_64
10.129.14.128:20ms:.1.3.6.1.2.1.1.2.0:.1.3.6.1.4.1.8072.3.2.10
10.129.14.128:20ms:.1.3.6.1.2.1.1.3.0:548
10.129.14.128:20ms:.1.3.6.1.2.1.1.4.0:mrb3n@inlanefreight.htb
10.129.14.128:20ms:.1.3.6.1.2.1.1.5.0:htb
10.129.14.128:20ms:.1.3.6.1.2.1.1.6.0:US
10.129.14.128:20ms:.1.3.6.1.2.1.1.7.0:78
...SNIP...
```

### snmpwalk

```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ snmpwalk -c public -v2c 10.129.129.4         
SNMPv2-MIB::sysDescr.0 = STRING: Linux NIX02 5.4.0-90-generic #101-Ubuntu SMP Fri Oct 15 20:00:55 UTC 2021 x86_64
SNMPv2-MIB::sysObjectID.0 = OID: NET-SNMP-MIB::netSnmpAgentOIDs.10
DISMAN-EVENT-MIB::sysUpTimeInstance = Timeticks: (20391) 0:03:23.91
SNMPv2-MIB::sysContact.0 = STRING: devadmin <devadmin@inlanefreight.htb>
SNMPv2-MIB::sysName.0 = STRING: NIX02
SNMPv2-MIB::sysLocation.0 = STRING: InFreight SNMP v0.91
```