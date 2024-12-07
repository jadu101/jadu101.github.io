---
title: e-XSS Discovery
draft: false
tags:
---
##  Automated

Automated tools (like [Nessus](https://www.tenable.com/products/nessus), [Burp Pro](https://portswigger.net/burp/pro), or [ZAP](https://www.zaproxy.org/)).

- Passive Scan - Reviews client side code for potential DOM-based vulnerabilities.
- Active Scan - Sends various types of payloads to attempt to trigger an XSS through payload injection.

Some of the common open-source tools are [XSS Strike](https://github.com/s0md3v/XSStrike), [Brute XSS](https://github.com/rajeshmajumdar/BruteXSS), and [XSSer](https://github.com/epsylon/xsser).


## Manual

We can find huge lists of XSS payloads online, like the one on [PayloadAllTheThings](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/XSS%20Injection/README.md) or the one in [PayloadBox](https://github.com/payloadbox/xss-payload-list).

We can copy paste and run the payload one by one but this is very inefficient.

Furthermore, most of the payload will not work because these payloads are designed for many different specific cases.

## Code Review

The most reliable method of detecting XSS vulnerabilities is manual code review.

We are unlikely to find any XSS vulnerabilities through payload lists or XSS tools for the more common web applications. 

This is because the developers of such web applications likely run their application through vulnerability assessment tools and then patch any identified vulnerabilities before release.