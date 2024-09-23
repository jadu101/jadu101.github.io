---
title: b-Further Session Attacks
draft: false
tags:
---
## Session Fixation

A web application vulnerable to session fixation does not assign a new session token after a successful authentication. If an attacker can coerce the victim into using a session token chosen by the attacker, session fixation enables an attacker to steal the victim's session and access their account.

1. An attacker obtains a valid session token by authenticating to the web application. For instance, let us assume the session token is `a1b2c3d4e5f6`. Afterward, the attacker invalidates their session by logging out.
    
2. The attacker tricks the victim to use the known session token by sending the following link: `http://vulnerable.htb/?sid=a1b2c3d4e5f6`. When the victim clicks this link, the web application sets the `session` cookie to the provided value, i.e., the response looks like this:
```http
HTTP/1.1 200 OK
[...]
Set-Cookie: session=a1b2c3d4e5f6
[...]
```

3. The victim authenticates to the vulnerable web application. The victim's browser already stores the attacker-provided session cookie, so it is sent along with the login request. The victim uses the attacker-provided session token since the web application does not assign a new one.
    
4. Since the attacker knows the victim's session token `a1b2c3d4e5f6`, they can hijack the victim's session.

## Improper Session Timeout


A web application must define a proper [Session Timeout](https://owasp.org/www-community/Session_Timeout) for a session token.

For instance, a web application dealing with sensitive health data should probably set a session timeout in the range of minutes. In contrast, a social media web application might set a session timeout of multiple hours.
