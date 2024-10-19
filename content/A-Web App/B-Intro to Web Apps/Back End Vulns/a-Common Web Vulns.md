---
title: a-Common Web Vulns
draft: false
tags:
---
## Broken Authentication / Access Control

**Broken Authentication** - Allows attacker to bypass authentication functions. 

**Broken Access Control** - Allows attackers to access pages and features they should not have access to.


## Malicious File Upload

If the web app has a file upload feature and does not properly check and validate uploaded files, attackers can upload a malicious script. 

## Command Injection

Many web apps execute local OS commands to perform certain processes. 

If not properly filtered and sanitized, attackers may be able to inject another command to be executed alongside the originally intended command. 

## SQL Injection

Web app executes a SQL query including a value taken from user-supplied input.

```php
$query = "select * from users where name like '%$searchInput%'";
```

