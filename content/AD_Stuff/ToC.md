---
title: Active Directory Attack Methodology 
draft: false
tags:
  - windows
  - ad
  - toc
---

# Active Directory Attack Methodology

## Rough Table of Contents

```text
1. Initial Enumeration
│
├── 1.1 Host Enumeration
│   ├── OS / Hostname
│   ├── Users / Groups
│   ├── AV / EDR
│   ├── Network Interfaces
│   ├── Installed Software
│   └── Running Services
│
├── 1.2 Domain Enumeration
│   ├── Current Domain
│   ├── Domain Controllers
│   ├── Forest / Trusts
│   ├── Password Policy
│   └── DNS
│
├── 1.3 User Enumeration
│   ├── Domain Users
│   ├── Service Accounts
│   ├── Admin Accounts
│   ├── Disabled Users
│   └── Description Fields
│
├── 1.4 Group Enumeration
│   ├── Privileged Groups
│   ├── Nested Groups
│   └── Interesting Memberships
│
├── 1.5 Computer Enumeration
│   ├── Workstations
│   ├── Servers
│   ├── Domain Controllers
│   └── Unconstrained Delegation Hosts
│
├── 1.6 Session Enumeration
│   ├── Logged-On Users
│   ├── SMB Sessions
│   └── Admin Sessions
│
├── 1.7 Share Enumeration
│   ├── SMB Shares
│   ├── SYSVOL
│   ├── NETLOGON
│   └── Sensitive Files
│
├── 1.8 SPN Enumeration
│   ├── Kerberoastable Accounts
│   └── Service Discovery
│
├── 1.9 ACL Enumeration
│   ├── GenericAll
│   ├── GenericWrite
│   ├── WriteDACL
│   ├── WriteOwner
│   └── ForceChangePassword
│
├── 1.10 GPO Enumeration
│   ├── Startup Scripts
│   ├── Scheduled Tasks
│   ├── Registry Preferences
│   └── GPP Passwords
│
├── 1.11 Trust Enumeration
│   ├── Forest Trusts
│   ├── External Trusts
│   └── SID Filtering
│
├── 1.12 ADCS Enumeration
│   ├── Certificate Authorities
│   ├── Vulnerable Templates
│   ├── Enrollment Rights
│   └── ESC Paths
│
└── 1.13 BloodHound Enumeration
    ├── Collection Methods
    ├── OPSEC Collection
    ├── Path Analysis
    └── Attack Path Prioritization


2. Initial Foothold
│
├── 2.1 Password Spraying
├── 2.2 Credential Stuffing
├── 2.3 Default Credentials
├── 2.4 VPN Access
├── 2.5 RDP Access
├── 2.6 WinRM Access
├── 2.7 MSSQL Access
├── 2.8 SMB Access
├── 2.9 Webshell Pivoting
└── 2.10 Phishing


3. Credential Access
│
├── 3.1 Kerberoasting
├── 3.2 AS-REP Roasting
├── 3.3 DCSync
├── 3.4 LSASS Dumping
├── 3.5 SAM Extraction
├── 3.6 DPAPI Abuse
├── 3.7 NTDS Extraction
├── 3.8 Cached Credentials
├── 3.9 Token Impersonation
├── 3.10 Pass-the-Hash
├── 3.11 Pass-the-Ticket
├── 3.12 Overpass-the-Hash
└── 3.13 Secrets Discovery


4. Local Privilege Escalation
│
├── 4.1 Service Misconfigurations
├── 4.2 Unquoted Service Paths
├── 4.3 DLL Hijacking
├── 4.4 Scheduled Tasks
├── 4.5 AlwaysInstallElevated
├── 4.6 SeImpersonatePrivilege
├── 4.7 Driver Exploitation
├── 4.8 Weak Registry Permissions
├── 4.9 Credential Reuse
└── 4.10 UAC Bypass


5. Lateral Movement
│
├── 5.1 PsExec
├── 5.2 SMBExec
├── 5.3 WMI
├── 5.4 WinRM
├── 5.5 RDP
├── 5.6 DCOM
├── 5.7 Scheduled Tasks
├── 5.8 Remote Services
├── 5.9 MSSQL Pivoting
└── 5.10 SOCKS / Proxy Pivoting


6. Active Directory Privilege Escalation
│
├── 6.1 GenericAll Abuse
├── 6.2 GenericWrite Abuse
├── 6.3 WriteDACL Abuse
├── 6.4 WriteOwner Abuse
├── 6.5 ForceChangePassword
├── 6.6 Shadow Credentials
├── 6.7 RBCD
├── 6.8 Constrained Delegation
├── 6.9 Unconstrained Delegation
├── 6.10 Resource-Based Delegation
├── 6.11 GPO Abuse
├── 6.12 AdminSDHolder
├── 6.13 SIDHistory Abuse
├── 6.14 ExtraSIDs
├── 6.15 Exchange Abuse
├── 6.16 SCCM Abuse
└── 6.17 DNSAdmins Abuse


7. Domain Dominance
│
├── 7.1 DCSync
├── 7.2 DCShadow
├── 7.3 Golden Tickets
├── 7.4 Silver Tickets
├── 7.5 Skeleton Key
├── 7.6 KRBTGT Extraction
├── 7.7 NTDS.dit Extraction
└── 7.8 Forest Compromise


8. ADCS Attacks
│
├── 8.1 ESC1
├── 8.2 ESC2
├── 8.3 ESC3
├── 8.4 ESC4
├── 8.5 ESC6
├── 8.6 ESC8
├── 8.7 Golden Certificates
├── 8.8 Certificate Persistence
├── 8.9 NTLM Relay to ADCS
└── 8.10 Enrollment Agent Abuse


9. Persistence
│
├── 9.1 Golden Tickets
├── 9.2 Silver Tickets
├── 9.3 Shadow Credentials
├── 9.4 Malicious GPOs
├── 9.5 Startup Scripts
├── 9.6 Scheduled Tasks
├── 9.7 Service Persistence
├── 9.8 WMI Event Subscription
├── 9.9 AdminSDHolder Backdoor
└── 9.10 Skeleton Key


10. Defense Evasion
│
├── 10.1 AMSI Bypass
├── 10.2 ETW Bypass
├── 10.3 PowerShell Obfuscation
├── 10.4 In-Memory Execution
├── 10.5 LOLBAS
├── 10.6 AV Evasion
├── 10.7 EDR Evasion
├── 10.8 Traffic Shaping
└── 10.9 Living Off The Land


11. Command and Control
│
├── 11.1 Sliver
├── 11.2 Cobalt Strike
├── 11.3 Mythic
├── 11.4 Havoc
├── 11.5 SOCKS Proxies
├── 11.6 Redirectors
├── 11.7 Malleable Profiles
└── 11.8 Named Pipe Pivoting


12. MSSQL Attacks
│
├── 12.1 MSSQL Enumeration
├── 12.2 xp_cmdshell
├── 12.3 Linked Servers
├── 12.4 UNC Path Injection
├── 12.5 Service Account Abuse
└── 12.6 MSSQL Lateral Movement


13. Linux to AD
│
├── 13.1 SMB Enumeration
├── 13.2 LDAP Enumeration
├── 13.3 Kerberos Enumeration
├── 13.4 BloodHound from Linux
├── 13.5 Impacket Toolkit
└── 13.6 NTLM Relay


14. Cloud / Hybrid AD
│
├── 14.1 Entra ID Enumeration
├── 14.2 AAD Connect
├── 14.3 PTA Abuse
├── 14.4 Cloud Sync Abuse
├── 14.5 Hybrid Escalation
└── 14.6 Token Abuse


15. OPSEC
│
├── 15.1 BloodHound OPSEC
├── 15.2 Kerberoasting OPSEC
├── 15.3 PowerShell Logging
├── 15.4 Defender Detections
├── 15.5 Event IDs
├── 15.6 LSASS Protection
├── 15.7 Beacon Placement
└── 15.8 Network Visibility


16. Attack Paths
│
├── 16.1 Low Priv User → DA
├── 16.2 MSSQL → DA
├── 16.3 GenericWrite → DA
├── 16.4 ADCS → DA
├── 16.5 Local Admin → DA
├── 16.6 Delegation → DA
├── 16.7 NTLM Relay → DA
└── 16.8 Multi-Forest Compromise


17. Tooling
│
├── 17.1 PowerView
├── 17.2 BloodHound
├── 17.3 SharpHound
├── 17.4 Rubeus
├── 17.5 Certipy
├── 17.6 Impacket
├── 17.7 NetExec
├── 17.8 Mimikatz
├── 17.9 Seatbelt
├── 17.10 Sliver
└── 17.11 CrackMapExec


18. Cheat Sheets
│
├── 18.1 PowerShell
├── 18.2 Linux Commands
├── 18.3 Impacket Cheatsheet
├── 18.4 Kerberos Cheatsheet
├── 18.5 ADCS Cheatsheet
├── 18.6 BloodHound Queries
└── 18.7 One-Liners
```
