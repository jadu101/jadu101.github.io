---
title: Internal Recone
draft: false
tags:
  - active-directory
---
## Passive Recon

Start with reading internal network traffic. 

Wireshark, tcpdump, net-creds, NetMiner

Look for active hosts, etc

ARP packets might provide list of hosts.

MDNS might provide hosts name.

## Active Recon
### fping

Quick ICMP sweep of the subnet using **fping**.

fping is similar to ping but can ping on list, and faster.

`a` to show targets that are alive, `s` to print stats at the end of the scan, `g` to generate a target list from the CIDR network, and `q` to not show per-target results.

```shell-session
jadu101@htb[/htb]$ fping -asgq 172.16.5.0/23

172.16.5.5
172.16.5.25
172.16.5.50
172.16.5.100
172.16.5.125
172.16.5.200
172.16.5.225
172.16.5.238
172.16.5.240
```

## Nmap

Need to find out what services each host is running and identify critical hosts such as DC and web servers. 

```bash
sudo nmap -v -A -iL hosts.txt
```

## Identify Users

Most of the case, client won't provide list of users. 

We need credentials to establish foothold in the domain. 

Clear-text password or an NTLM password hash for a user would be helpful.

### Kerbrute

Kerbrute can be a stealthy option for domain account enumeraiton. 

Kerberos pre-authenticaiton failure often doesn't trigger alerts. 

`jsmith.txt` or `jsmith2.txt` user lists from [Insidetrust](https://github.com/insidetrust/statistically-likely-usernames) could be used.

#### Installation

```shell-session
sudo git clone https://github.com/ropnop/kerbrute.git
```

```shell-session
make help
```

```shell-session
sudo make linux
```


#### User Enum

```shell-session
jadu101@htb[/htb]$ kerbrute userenum -d INLANEFREIGHT.LOCAL --dc 172.16.5.5 jsmith.txt -o valid_ad_users

2021/11/17 23:01:46 >  Using KDC(s):
2021/11/17 23:01:46 >   172.16.5.5:88
2021/11/17 23:01:46 >  [+] VALID USERNAME:       jjones@INLANEFREIGHT.LOCAL
2021/11/17 23:01:46 >  [+] VALID USERNAME:       sbrown@INLANEFREIGHT.LOCAL
2021/11/17 23:01:46 >  [+] VALID USERNAME:       tjohnson@INLANEFREIGHT.LOCAL
2021/11/17 23:01:50 >  [+] VALID USERNAME:       evalentin@INLANEFREIGHT.LOCAL
```


## Attack Paths

A SYSTEM account on a domain-joined host is able to enumerate AD by impersonating the computer account. 

Having SYSTEM-level access within a domain environment is almost same as having a domain user account.

How to gain SYSTEM-level access on the host?

- MS08-067, EternalBlue, BlueKeep
- Abuse service running as SYSTEM account
- Abuse service account SeImpersonate privilege with Juicy Potato (Before Windows Server 2019)
- Local Privilege Escalation
- Lauching psexec with admin access on domain-joined host

 What can we do with the SYSTEM-level access on a domain-joined host?

- Enumerate domain using tools such as Bloodhound and PowerView
- Kerberoasting
- ASREPRoasting
- Run Inveight to gather Net-NTLMv2 hashes
- SMB Relay Attack
- Token Impersonation to hijack privileged domain user account
- ACL Attacks
