---
title: b-Prevention
draft: false
tags:
---
## System Commands

Avoid using functions that execute system commands.

## Input Validation

Input validation should be done both on front-end and on the back-end.

In `PHP`, we can use something like `filter_var` to filter for a variety of standard formats, like emails, URLs, and even IPs.

```php
if (filter_var($_GET['ip'], FILTER_VALIDATE_IP)) {
    // call function
} else {
    // deny request
}
```

## Input Sanitization

Always remove non-necessary special characters from the user input.

e.g We can use `preg_replace` to remove any special characters from the user input:

```php
$ip = preg_replace('/[^A-Za-z0-9.]/', '', $_GET['ip']);
```


## Server Configuration

- Use the web server's built-in Web Application Firewall (e.g., in Apache `mod_security`), in addition to an external WAF (e.g. `Cloudflare`, `Fortinet`, `Imperva`..)
    
- Abide by the [Principle of Least Privilege (PoLP)](https://en.wikipedia.org/wiki/Principle_of_least_privilege) by running the web server as a low privileged user (e.g. `www-data`)
    
- Prevent certain functions from being executed by the web server (e.g., in PHP `disable_functions=system,...`)
    
- Limit the scope accessible by the web application to its folder (e.g. in PHP `open_basedir = '/var/www/html'`)
    
- Reject double-encoded requests and non-ASCII characters in URLs
    
- Avoid the use of sensitive/outdated libraries and modules (e.g. [PHP CGI](https://www.php.net/manual/en/install.unix.commandline.php))
