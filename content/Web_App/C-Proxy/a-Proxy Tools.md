---
title: a-Proxy Tools
draft: false
tags:
  - proxychain
---
We should be able to know how to intercept web request made by command-line tools and thick client applications. 

## Proxychains

We have to edit `/etc/proxychains.conf` to use Proxychains.

```shell-session
#socks4         127.0.0.1 9050
http 127.0.0.1 8080
```

Now `cURL` traffic goes through localhost port 8080 which goes into Burp Suite:

```shell-session
jadu101@htb[/htb]$ proxychains curl http://SERVER_IP:PORT

ProxyChains-3.1 (http://proxychains.sf.net)
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <title>Ping IP</title>
    <link rel="stylesheet" href="./style.css">
</head>
...SNIP...
</html>    
```

## Nmap

We can send Nmap traffic to localhost port 8080 using `--proxies` flag:

```shell-session
jadu101@htb[/htb]$ nmap --proxies http://127.0.0.1:8080 SERVER_IP -pPORT -Pn -sC

Starting Nmap 7.91 ( https://nmap.org )
Nmap scan report for SERVER_IP
Host is up (0.11s latency).

PORT      STATE SERVICE
PORT/tcp open  unknown

Nmap done: 1 IP address (1 host up) scanned in 0.49 seconds
```


## Metasploit

Using `msfconsole`, we can use the `set PROXIES` flag:

```shell-session
jadu101@htb[/htb]$ msfconsole

msf6 > use auxiliary/scanner/http/robots_txt
msf6 auxiliary(scanner/http/robots_txt) > set PROXIES HTTP:127.0.0.1:8080

PROXIES => HTTP:127.0.0.1:8080


msf6 auxiliary(scanner/http/robots_txt) > set RHOST SERVER_IP

RHOST => SERVER_IP
```

