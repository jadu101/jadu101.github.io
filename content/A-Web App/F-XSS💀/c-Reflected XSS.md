---
title: c-Reflected XSS
draft: false
tags:
  - reflected-xss
---
There are two types of **Non-Persistent XSS**:

- Reflected XSS - Gets processed by the back-end server.
- DOM-based XSS - Gets completly processed on the client-side and never reached the back-end server.

**Reflected XSS** - Input reaches the back-end server and gets returned to us without being filtered or sanitized. (e.g error messages, confirmation messages)

But if the XSS vulnerability is non-persistent, how would we target victims with it?

> We can send the target user containing our payload and hope they click on it.


