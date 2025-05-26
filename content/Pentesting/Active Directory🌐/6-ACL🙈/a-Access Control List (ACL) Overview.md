---
title: a-ACL Overview
draft: false
tags:
  - acl
---
For security reasons, not all users and computers in AD can access all objects and files. 

These permissions are controlled through **ACLs**.

Misconfiguration to an ACL can leak permissions to other objects that do not need it.

## Overview

ACL is a list the contains set of ACEs.

ACE is an individual entry in an ACL.

ACLs - List that define:

- Who has access to which asset/resource
- Level of access they are provisioned

Settings are called Access Control Entries (ACEs). 

Each ACE maps back to a user, group or process. 

It defines the rights granted to that principal.

Every object has an ACL but can have multiple ACEs because multiple security principals can access objects in AD. 

ACLs can also be used for auditing access within AD.

### Types of ACL

1. **Discretionary Access Control List (DACL)**: Specifies the permissions that users and groups have to an object. It controls access to the object by allowing or denying permissions.
2. **System Access Control List (SACL)**: Specifies the types of operations that are audited for a user or group. It is used to track access to an object for security auditing purposes.
#### DACL

Discretionary Access Control List (DACL)
- Defines which security principals are granted or denied access to an object.
- 
DACLs are made up of ACEs that either allow or deny access. 

When someone attempts to access on object, system will check the DACL for the level of access that is permitted. 

If DACL **doesn't exist** for an object, all who tries to attempt the object are granted with **full rights**. 

If DACL **exists**, but doesn't have any ACE entries specifying specific security settings, the system will **deny** access to all users, groups, or processes attempting to access it.

#### SACL

System Access Control Lists (SACL)

Allow administrators to log access attempts made to secured objects. 

## ACEs

There are three main types of ACEs that can be applied to all securable objects in AD:

| **ACE**              | **Description**                                                                                                                                                            |
| -------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Access denied ACE`  | Used within a DACL to show that a user or group is explicitly denied access to an object                                                                                   |
| `Access allowed ACE` | Used within a DACL to show that a user or group is explicitly granted access to an object                                                                                  |
| `System audit ACE`   | Used within a SACL to generate audit logs when a user or group attempts to access an object. It records whether access was granted or not and what type of access occurred |

Each ACE is made up the following four components:

| `Security Identifier (SID)` | SID of the user/group that has access to the object.                      |
| --------------------------- | ------------------------------------------------------------------------- |
| `Flag`                      | A flag denoting the type of ACE (Access denied, allowed, or system audit) |
| `Set of flags`              | A set of flags that specify whether or not child containers/objects       |
| `Access Mask`               | 32-bit value that defines the rights granted to an object                 |

## Why are ACEs Important

Could be used for:

- Further access
- Establish persistence

Many organizations are unaware of ACEs.

They cannot be detected by vulnerability scanning tool.

Example AD object security permissions:

- `ForceChangePassword` abused with `Set-DomainUserPassword`
- `Add Members` abused with `Add-DomainGroupMember`
- `GenericAll` abused with `Set-DomainUserPassword` or `Add-DomainGroupMember`
- `GenericWrite` abused with `Set-DomainObject`
- `WriteOwner` abused with `Set-DomainObjectOwner`
- `WriteDACL` abused with `Add-DomainObjectACL`
- `AllExtendedRights` abused with `Set-DomainUserPassword` or `Add-DomainGroupMember`
- `Addself` abused with `Add-DomainGroupMember`

## Moving On

We will cover four specific ACEs to highlight ACL attacks. 

### ForceChangePassword

Gives attacker to reset a user's password without knowing their password. 

### GenericWrite

Gives attacker to right to write to any non-protected attribute on an object. 

With GenericWrite on user, attacker can assign a suer with SPN and perform Kerberoasting.

With GenericWrite on group, we could add attacker to a given group.

With GenericWrite on computer object, resource based delegation attack can be performed.

### AddSelf

Shows security groups that a user can add themselves to.

### GenericAll

Grants us full control over a target object. 

If this is granted over a user or a group.

- Modify group membership
- Force change a password
- Perform Kerberoasting

If this granted over computer objects and LAPS:

- Read the LAPS password and gain admin access.
