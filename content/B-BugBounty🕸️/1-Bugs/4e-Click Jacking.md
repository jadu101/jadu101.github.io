---
title: Click Jacking
draft: false
tags:
---
Click-Jacking is also known as UI Redressing.

> a dangerous technique for deceiving a user into clicking on something other from what they think they’re clicking on, potentially revealing private information or allowing others to take control of their computer while clicking on seemingly harmless objects, such as web sites.

Using [clickjacker.io](https://clickjacker.io), we can easily get the website tested. 

## Mitigation

The solution to prevent against Clickjacking attacks is pretty straight-forward, just use the **_X-Frame-Options_** header. More you can find here: [https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Frame-Options](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/X-Frame-Options)

