---
title: e-Weak Bruteforce Protection
draft: false
tags:
  - rate-limits
  - captcha
---
Among the common types of brute-force protection mechanisms are rate limits and CAPTCHAs.

## Rate Limits

Controls the rate of incoming request to a system or API.

Its primary purpose is to prevent:
- System being overwhelmed
- System down
- Bruteforce attack

It limits the number of requests allowed within a specified time frame.

Rate limits usually replies on IP address but in a real world scenario, obtaining attacker's IP address might not be very easy.

Thus, some rate limits rely on HTTP headers such as `X-Forwarded-For` to obtain the actual source IP address.

However, this causes an issue as an attacker can set arbitrary HTTP headers in request, bypassing the rate limit entirely. This enables an attacker to conduct a brute-force attack by randomizing the `X-Forwarded-For` header in each HTTP request to avoid the rate limit.

## CAPTCHAs

A `Completely Automated Public Turing test to tell Computers and Humans Apart (CAPTCHA)` is a security measure to prevent bots from submitting requests.

Tools and browser extensions to solve CAPTCHAs automatically are rising. Many open-source CAPTCHA solvers can be found. In particular, the rise of AI-driven tools provides CAPTCHA-solving capabilities by utilizing powerful image recognition or voice recognition machine learning models.


 