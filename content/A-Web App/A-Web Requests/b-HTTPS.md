---
title: b-Hypertest Transfer Protocol Secure (HTTPS)
draft: false
tags:
  - https
  - cURL
---
One drawbacks of HTTP is that all data is transferred in clear-text.

Anyone between the source and destination can perform a MiTM attack to view transferred data.

**HTTPS** encrypts the communication so attacker cannot extract the data out of it even the attacker intercept the traffic.

Data going through HTTPS protocol is encrypted but attacker may still reveal the visited URL if the target contacted a clear-text DNS server. This is why it is recommended to utilize encrypted DNS server (such as 8.8.8.8 or 1.1.1.1) or a VPN service.

How does HTTP connection made?

HTTPS request starts with client (browser) sending a `client hello`packet, giving information about itself. After that, server relies with `server hello` packet, followed by a `key exchange` to exchange SSL certificates. Client verifies the key/certificate and sends back one of its own, which initiates an encrypted handshake to confirm whether the encryption is and transfer are working correctly. Once the handshake completes, normal HTTP communication is continued which is encrypted after that. 

> **HTTP downgrade attack** (downgrades HTTPS to HTTP): Attacker sets up a MITM proxy to transfer all traffic through the attacker's host without the user's knowledge. But most modern browsers, server, and web apps is protected against this attack. 


## cURL for HTTPS

cURL automatically handle all HTPS communications standards and perform secure handshake and then encrypt/decrypt data automatically. 

Sometimes, encountered with a website with an invalid SSL certificate or an outdated one, cURL by default wouldn't proceed with the communication to protect against the earlier mentioned MITM attack.

```
yoon@yoon-XH695R:~$ curl https://carabinersec.com

curl: (60) SSL certificate problem: Invalid certificate chain
More details here: https://curl.haxx.se/docs/sslcerts.html
...SNIP...
```

Most modern web applications also does this by showing a invalid SSL certification warning.

Sometimes, when testing local web app or a web app built for testing purposes, valid SSL certificate might not be implemented yet.

We can use `-k` flag to skip the certificate check.

```
yoon@yoon-XH695R:~$ curl -k https://carabinersec.com

<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
<html><head>
...SNIP...
```

