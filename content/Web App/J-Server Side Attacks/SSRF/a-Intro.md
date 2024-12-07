---
title: a-intro
draft: false
tags:
  - ssrf
---
SSRF occurs when a web app fetches resources from a remote location based on the user input such as URL.

If the web app relies on a user-supplied URL scheme or protocol, an attacker can perform more influencing attack.

- `http://` and `https://`: Fetches content use HTTP/s. Attacker might use this to bypass WAFs, access restricted endpoints, or access endpoints in the internal network.
- `file://`: Reads a file from the local file system. Attacker might use this to read local files (LFI).
- `gopher://`:Sends arbitrary bytes to the specified address. Attacker might use this to send HTTP POST requests with arbitrary payloads or communicate with other services such as SMTP server or database.
