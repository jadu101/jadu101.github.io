---
title: a-Intro
draft: false
tags:
---
XSS targets the web browser, so it is client side attack.

Server side attacks target the web server.

Below topics will be covered:

- Server-Side Request Forgery (SSRF)
- Server-Side Template Injection (SSTI)
- Server-Side Includes (SSI) Injection
- eXtensible Stylesheet Language Transformations (XSLT) Server-Side Injection

## SSRF


**SSRF** is a vulnerability which attacker can manipulate a web application into sending unauthorized requests from the server. 

This often occurs when an application makes HTTP requests to other servers based on the user input.

Successful exploitation can enable an attacker to access internal systems, bypass firewalls, and retrieve sensitive information.

## SSTI

**SSTI** is a vulnerability where an attacker can inject a template code which can lead to risks such as data leakage and even a RCE.

Web apps sometimes utilize templating engines and server-side templates to generate responses such as HTML content dynamically.

This generation is often based on the user input and it enables the web app to respond to user input dynamically.

## SSI

**SSI** can be used to generate HTML responses dynamically. 

SSI directives instruct the webserver to include additional content dynamically. 

These directives are embedded into HTML files. For instance, SSI can be used to include content that is present in all HTML pages, such as headers or footers. When an attacker can inject commands into the SSI directives, [Server-Side Includes (SSI) Injection](https://owasp.org/www-community/attacks/Server-Side_Includes_(SSI)_Injection) can occur. SSI injection can lead to data leakage or even remote code execution.

## XSLT Server-Side Injection

**XSLT Server-Side injection** happens when an attacker can manipulate XSLT transformations performed on the server.

**XSLT** is a language which is used to transform XML documents into other formats such as HTML, and it is commonly emplyed in web apps to generate content dynamically.

Attacker can exploit this to execute arbitrary code on the server. 