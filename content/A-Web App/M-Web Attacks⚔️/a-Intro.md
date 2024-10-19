---
title: a-Intro
draft: false
tags:
---
## HTTP Verb Tampering

An HTTP Verb Tampering attack exploits web servers that accept many HTTP verbs and methods.

This can be exploited by sending malicious requests using unexpected methods, which may lead to bypassing the web application's authorization mechanism or even bypassing its security controls against other web attacks.

## Insecure Direct Object References (IDOR)

IDOR is among the most common web vulnerabilities and can lead to accessing data that should not be accessible by attackers.

As web applications store users' files and information, they may use sequential numbers or user IDs to identify each item. Suppose the web application lacks a robust access control mechanism and exposes direct references to files and resources. In that case, we may access other users' files and information by simply guessing or calculating their file IDs.

## XML External Entity (XXE) Injection

Many web applications process XML data as part of their functionality. 

Suppose a web application utilizes outdated XML libraries to parse and process XML input data from the front-end user. In that case, it may be possible to send malicious XML data to disclose local files stored on the back-end server.
