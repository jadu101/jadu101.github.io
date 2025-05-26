---
title: a-Session Hijacking
draft: false
tags:
---
An attacker can obtain a victim's session identifier using several methods, with the most common being:

- Passive Traffic Sniffing
- Cross-Site Scripting (XSS)
- Browser history or log-diving
- Read access to a database containing session information

## Identify Session Identifier

Let's say we logged in to a web app and used Web Developer Tool to check on Storage.

We noticed that cookie named `auth-session` is being used for session identifier.

Double click on it to copy.

## Simulate an Attacker

Now, suppose that you are the attacker and you somehow got access to the `auth-session` cookie's value for the user "Julie Rogers".

Go to `New Private Window` and replace the current `auth-session` cookie's value with the one you copied in Part 1. 

Upon reload, we can now log-in as Julie Rogers.

