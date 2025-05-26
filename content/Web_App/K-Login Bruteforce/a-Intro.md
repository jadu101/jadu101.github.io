---
title: a-Intro
draft: false
tags:
---
The HTTP specification provides two parallel authentication mechanisms:

1. `Basic HTTP AUTH` is used to authenticate the user to the HTTP server.
    
2. `Proxy Server Authentication` is used to authenticate the user to an intermediate proxy server.

There are several types of password attacks, such as:

|**Password Attack Type**|
|---|
|`Dictionary attack`|
|`Brute force`|
|`Traffic interception`|
|`Man In the Middle`|
|`Key Logging`|
|`Social engineering`|

## Brute Force Attack

Does not depend on a wordlist of common passwords but it works by trying all possible character combinations for the length we specified.

Relying completely on brute force attacks is not ideal, and this is especially true for brute-forcing attacks that take place over the network, like in `hydra`.

## Dictionary Attack

A `Dictionary Attack` tries to guess passwords with the help of lists.

We can find password wordlists in our SecLists in `/opt/useful/SecLists/Passwords/`, and username wordlists in `/opt/useful/SecLists/Usernames/`.

## Methods of Brute Force Attacks

There are many methodologies to carry a Login Brute Force attacks:

| **Attack**                 | **Description**                                                                                                             |
| -------------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| Online Brute Force Attack  | Attacking a live application over the network, like HTTP, HTTPs, SSH, FTP, and others                                       |
| Offline Brute Force Attack | Also known as Offline Password Cracking, where you attempt to crack a hash of an encrypted password.                        |
| Reverse Brute Force Attack | Also known as username brute-forcing, where you try a single common password with a list of usernames on a certain service. |
| Hybrid Brute Force Attack  | Attacking a user by creating a customized password wordlist, built using known intelligence about the user or the service.  |