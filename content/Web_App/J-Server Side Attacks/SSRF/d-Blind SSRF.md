---
title: d-Blind SSRF
draft: false
tags:
  - ssrf
  - blind-ssrf
---
Sometimes we cannot see the response of SSRF vulnerability and this is called blind SSRF.

## Identify Blind SSRF

Continuing from `c-Exploit SSRF`, let's say we confirmed SSRF by sending traffic to netcat listener:

```shell-session
jadu101@htb[/htb]$ nc -lnvp 8000

listening on [any] 8000 ...
connect to [172.17.0.1] from (UNKNOWN) [172.17.0.2] 32928
GET /index.php HTTP/1.1
Host: 172.17.0.1:8000
Accept: */*
```

However, if we attempt to point the web app to itself (`dateserver=http://127.0.0.1/idnex.php&date=2024-01-01`), we can see that the response (`Data is unavailable. Please choose a different data`) does not contain the HTML response of the request.

This way we know there is a blind SSRF vulnerability.

## Exploit Blind SSRF

Exploiting blind SSRF is generally very limited compared to non-blind SSRF vulnerability.

However, we still might be able to conduct a local port scan, provided the response differs for open and closed ports.

In this case, the web application responds with `Something went wrong!` for closed ports.

However, if a port is open and responds with a valid HTTP response, we get a different error message: `Data is unavailable. Please choose a different data`

We can do the same with identifying existing files on the system.

e.g `dateserver=file///etc/passwd&date=2024-01-01` - `Data is unavailable. Please choose a different data`

e.g `dateserver=file///etc/passwd&date=2024-01-01` - `Something went wrong!`




