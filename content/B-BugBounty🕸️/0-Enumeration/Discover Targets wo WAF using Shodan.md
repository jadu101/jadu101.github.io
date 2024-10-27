---
title: Discover Targets wo WAF using Shodan
draft: false
tags:
  - shodan
  - waf
---
Often times **WAF** blocks attackers from injecting payloads such as for XSS and SQL injection. 

Bypassing WAF is hard. We can instead look for targets that is not protected via WAF.

Check out `7e-Discover Origin IP Address` post for details.

Basically, dork out your target's name , hostname or organisation on shodan, to get all it's publicly accessible ip addresses.

Check them out one by one to see how they look and if you are able to find an IP address that displays the home page of your target application, we have the source IP address, which is clear of WAFs and proxies. 

Now go ahead and bruteforce, inject, fuzz.

