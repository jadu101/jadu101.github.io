---
title: a-intro
draft: false
tags:
---
HTTP is a stateless communication and requests are not related to each other.

This means that each request have to carry all needed information for the server to act upon it appropriately.

This is why web apps uses cookies, URL parameters, URL arguments, body arguments for session tracking and management purposes.

## Session Identifier Security

A unique **session identifier** (Session ID) or token is the basis upon which user sessions are generated and distinguished.

An attacker can obtain a session identifier through a multitude of techniques, not all of which include actively attacking the victim. A session identifier can also be:

- Captured through passive traffic/packet sniffing
- Identified in logs
- Predicted
- Brute Forced

A session identifier's security level depends on its:

- `Validity Scope` (a secure session identifier should be valid for one session only)
- `Randomness` (a secure session identifier should be generated through a robust random number/string generation algorithm so that it cannot be predicted)
- `Validity Time` (a secure session identifier should expire after a certain amount of time)

A session identifier's security level also depends on the location where it is stored:

- `URL`: If this is the case, the HTTP _Referer_ header can leak a session identifier to other websites. In addition, browser history will also contain any session identifier stored in the URL.
- `HTML`: If this is the case, the session identifier can be identified in both the browser's cache memory and any intermediate proxies
- `sessionStorage`: SessionStorage is a browser storage feature introduced in HTML5. Session identifiers stored in sessionStorage can be retrieved as long as the tab or the browser is open. In other words, sessionStorage data gets cleared when the _page session_ ends. Note that a page session survives over page reloads and restores.
- `localStorage`: LocalStorage is a browser storage feature introduced in HTML5. Session identifiers stored in localStorage can be retrieved as long as localStorage does not get deleted by the user. This is because data stored within localStorage will not be deleted when the browser process is terminated, with the exception of "private browsing" or "incognito" sessions where data stored within localStorage are deleted by the time the last tab is closed.
## Session Attacks

- **Session Hijacking** - Hijacks the session and uses them to authenticate to the server and impersonate the victim.
- **Session Fixation** - Attacker fixate a session identifier and tricks the victim into logging into application using the aforementioned session identifier. This way attacker can hijack session.
- **XSS** - Focus on user sessions.
- **CSRF** - Cross-Site Request Forgery (CSRF or XSRF) is an attack that forces an end-user to execute inadvertent actions on a web application in which they are currently authenticated. This attack is usually mounted with the help of attacker-crafted web pages that the victim must visit or interact with. These web pages contain malicious requests that essentially inherit the identity and privileges of the victim to perform an undesired function on the victim's behalf.
- **Open Redirects** - Occurs when an attacker can redirect a victim to an attacker-controlled site by abusing a legitimate application's redirection functionality.


```shell-session
jadu101@htb[/htb]$ IP=ENTER SPAWNED TARGET IP HERE
jadu101@htb[/htb]$ printf "%s\t%s\n\n" "$IP" "xss.htb.net csrf.htb.net oredirect.htb.net minilab.htb.net" | sudo tee -a /etc/hosts
```