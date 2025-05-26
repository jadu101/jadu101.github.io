---
title: d-Misconfigurations
draft: false
tags:
  - misconfigurations
  - active-directory
---
## Exchange Related Group Membership

**Microsoft Exchange Server**: Email, calendaring, contact, scheduling and collaboration platform. 

Exchange is granted lot of privileges within the domain.

**Exchange Windows Permissions Group** is not a protected group but members are granted the ability to `write a DACL` to the domain object. (**DACL**: Control list that lists users and groups allowed or denied access to ab object).

If member can write to DACL, this can give user a DCSync privilege. (**DCSync**: Pretends to be DC and get user credentials from another DC)

Attacker can add accounts to **Exchange Windows Permissions** group by leveraging a DACL misconfiguration or by leveraging a compromised account that is a member of the Account Operators Group.

It is pretty common to find user accounts that is a member of **Exchange Windows Permissions Group**.

**Organization Management** exchange group is also a very powerful group. 

Members of this group can access the mailboxes of all domain users.

Members of this group also has full control of **Microsoft Exchange Security Groups**, which contains the group **Exchange Windows Permissions**.

Compromising Exchange Server often means Domain Admin privilege. 

Dumping credentials in Exchange Server memory will provide attacker cleartext credentials or NTLM hashes. 


### 1-PrivExchange Attack

PrivExchange Attack is caused from flaw in Exchange Server's `PushSubscription` feature.

With flaw in `PushSubscription` feature, any domain user with a mailbox can force the exchange server to authenticate to any host provided by the client over HTTP.

Exchange service runs as SYSTEM by default.

Attacker can use this flaw to relay LDAP and dump the domain NTDS database.

This attack will allow attacker directly to Domain Admin with any authenticated domain user account.


### 2-Printer Bug

Printer Bug is a flaw in MS-RPRN protocol
