---
title: b-Session Fixation
draft: false
tags:
---
Session Fixation occurs when an attacker can fixate a (valid) session identifier. As you can imagine, the attacker will then have to trick the victim into logging into the application using the aforementioned session identifier. If the victim does so, the attacker can proceed to a Session Hijacking attack (since the session identifier is already known).

- **Step 1: Obtain valid session identifier** - Create a account to obtain session identifier. Some web apps assign session identifiers just from browsing.
- **Step 2: Fixate valid session identifier** - Step 1 is expected behavior but can turn into a session fixation if:
	- The assigned session identifier pre-login remains the same post-login `and`
	- Session identifiers (such as cookies) are being accepted from _URL Query Strings_ or _Post Data_ and propagated to the application
	- If, for example, a session-related parameter is included in the URL (and not on the cookie header) and any specified value eventually becomes a session identifier, then the attacker can fixate a session.
- **Step 3: Trick victim into establishing a session using above mentioned session identifier** - All the attacker has to do is craft a URL and lure the victim into visiting it. If the victim does so, the web application will then assign this session identifier to the victim.

## Exploit
### P1: Session Fixation Identification

Navigate to the target web app.

We came across a URL of the below format:

`http://oredirect.htb.net/?redirect_uri=/complete.html&token=<RANDOM TOKEN VALUE>`

Using Web Developer Tools (Shift+Ctrl+I in the case of Firefox), notice that the application uses a session cookie named `PHPSESSID` and that the cookie's value is the same as the `token` parameter's value on the URL.

We are probably dealing with a session fixation vulnerability.

### P2: Session Fixation Exploit Attempt

On private windows, go to `http://oredirect.htb.net/?redirect_uri=/complete.html&token=IControlThisCookie`.

Using Web Developer Tools (Shift+Ctrl+I in the case of Firefox), notice that the `PHPSESSID` cookie's value is `IControlThisCookie`.

**Confirmed**: We are dealing with a Session Fixation vulnerability.

Attacker can send a URL similar to above to the victim and if the victim logs into the application, we can hijack the session.

> **Note**: Another way of identifying this is via blindly putting the session identifier name and value in the URL and then refreshing.

For example, suppose we are looking into `http://insecure.exampleapp.com/login` for session fixation bugs, and the session identifier being used is a cookie named `PHPSESSID`. To test for session fixation, we could try the following `http://insecure.exampleapp.com/login?PHPSESSID=AttackerSpecifiedCookieValue` and see if the specified cookie value is propagated to the application (as we did in this section's lab exercise).

