---
title: Password Policy Enumeration
draft: false
tags:
  - password-spray
  - password-policy
---
Let's say we retrieved password/hash from LLMNR/NBT-NS poisoning. 

Now it's time for us to spray this password/hash for further access. 

During password spraying, we always have to be cautious with account lockout. 

There are several ways to pull the domain password policy.

We will learn about ways to enumerate password policy from both Linux and Windows, with or without credentials.

## Linux - w Creds

With valid domain credentials, password policy can be obtained through CME or rpcclient.

Using CrackMapExec:

```shell-session
crackmapexec smb 172.16.5.5 -u avazquez -p Password123 --pass-pol
```

## Linux - wo Creds - SMB NULL Sessions

Without the credentials, we may be able to obtain the password policy using SMB NULL Session or LDAP anonymous bind.

SMB NULL session allows an unauthenticated attacker to retrieve information from the domain.

We can use rpcclient to check DC for SMB NULL session access.

Once connected, issue commands like `querydominfo` to confirm NULL session access:

```shell-session
jadu101@htb[/htb]$ rpcclient -U "" -N 172.16.5.5

rpcclient $> querydominfo
Domain:		INLANEFREIGHT
Server:		
Comment:	
Total Users:	3650
Total Groups:	0
Total Aliases:	37
```

To query password policy:

```shell-sessions
rpcclient $> getdompwinfo
min_password_length: 8
password_properties: 0x00000001
	DOMAIN_PASSWORD_COMPLEX
```

We can enumerate password policy using enum4linux as well:

```shell-session
jadu101@htb[/htb]$ enum4linux -P 172.16.5.5
```

Also, using enum4linux-ng, which is same as enum4linux but with data export function:

```shell-session
jadu101@htb[/htb]$ enum4linux-ng -P 172.16.5.5 -oA ilfreight
```

### Windows - NULL Sessions

Use below command to establish a null session from a windows machine and confirm we can perform more of this type of attack:

```cmd-session
C:\htb> net use \\DC01\ipc$ "" /u:""
The command completed successfully.
```

If we have username/password, we can try to authenticate:

```cmd-session
C:\htb> net use \\DC01\ipc$ "" /u:guest
```

## Linux - wo Creds - LDAP Anonymous Bind

LDAP anonymous bind allows unauthenticated attacker to retrieve information from the domain. 

With LDAP anonymous bind configured, we can use such tools to obtain password policy:

- windapsearch.py
- ldapsearch
- ad-ldapdomaindump.py

Using ldapsearch:

```shell-session
jadu101@htb[/htb]$ ldapsearch -h 172.16.5.5 -x -b "DC=INLANEFREIGHT,DC=LOCAL" -s sub "*" | grep -m 1 -B 10 pwdHistoryLength

forceLogoff: -9223372036854775808
lockoutDuration: -18000000000
lockOutObservationWindow: -18000000000
lockoutThreshold: 5
maxPwdAge: -9223372036854775808
minPwdAge: -864000000000
minPwdLength: 8
modifiedCountAtLastProm: 0
nextRid: 1002
pwdProperties: 1
pwdHistoryLength: 24
```

We can see that minimum password length is 8, lockout threshold is 5 and password complexity is set as 1.

## Windows - w Creds


If we can authenticate to domain from windows host, we use built-in windows binary such as **net.exe** to retrieve password policy.

We could also use tools such as:

- PowerView
- CME
- SharpMapExec
- SharpView

### net.exe

```cmd-session
C:\htb> net accounts

Force user logoff how long after time expires?:       Never
Minimum password age (days):                          1
Maximum password age (days):                          Unlimited
Minimum password length:                              8
Length of password history maintained:                24
Lockout threshold:                                    5
Lockout duration (minutes):                           30
Lockout observation window (minutes):                 30
Computer role:                                        SERVER
The command completed successfully.
```

### PowerView

```powershell-session
PS C:\htb> import-module .\PowerView.ps1
PS C:\htb> Get-DomainPolicy

Unicode        : @{Unicode=yes}
SystemAccess   : @{MinimumPasswordAge=1; MaximumPasswordAge=-1; MinimumPasswordLength=8; PasswordComplexity=1;
                 PasswordHistorySize=24; LockoutBadCount=5; ResetLockoutCount=30; LockoutDuration=30;
                 RequireLogonToChangePassword=0; ForceLogoffWhenHourExpire=0; ClearTextPassword=0;
                 LSAAnonymousNameLookup=0}
KerberosPolicy : @{MaxTicketAge=10; MaxRenewAge=7; MaxServiceAge=600; MaxClockSkew=5; TicketValidateClient=1}
Version        : @{signature="$CHICAGO$"; Revision=1}
RegistryValues : @{MACHINE\System\CurrentControlSet\Control\Lsa\NoLMHash=System.Object[]}
Path           : \\INLANEFREIGHT.LOCAL\sysvol\INLANEFREIGHT.LOCAL\Policies\{31B2F340-016D-11D2-945F-00C04FB984F9}\MACHI
                 NE\Microsoft\Windows NT\SecEdit\GptTmpl.inf
GPOName        : {31B2F340-016D-11D2-945F-00C04FB984F9}
GPODisplayName : Default Domain Policy
```



## Strategy based on Password Policy

So using the various methods above, we can obtain password policy. 

- Minimum password length = 8
- Account lockout threshold = 5
- Lockout duration = 30 minutes
- Accounts unlock automatically
- Password complexity enabled = User must choose a password with 3/4 of the following: uppercase, lowercase letter. number, special character.

Below is the default password policy:

|Policy|Default Value|
|---|---|
|Enforce password history|24 days|
|Maximum password age|42 days|
|Minimum password age|1 day|
|Minimum password length|7|
|Password must meet complexity requirements|Enabled|
|Store passwords using reversible encryption|Disabled|
|Account lockout duration|Not set|
|Account lockout threshold|0|
|Reset account lockout counter after|Not set|

## Moving On

Now we obtained password policy. 

With the list of usernames, we go for password spraying attack.

