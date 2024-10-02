---
title: e-XSS
draft: false
tags:
  - xss
---
Cross-Site Scripting (XSS) vulnerabilities affect web applications and APIs alike. An XSS vulnerability may allow an attacker to execute arbitrary JavaScript code within the target's browser and result in complete web application compromise if chained together with other vulnerabilities.

> Suppose we are having a better look at the API of the previous section, `http://<TARGET IP>:3000/api/download`.

Let us first interact with it through the browser by requesting the below.

```URL
http://<TARGET IP>:3000/api/download/test_value
```

And we get a result saying:

```
test_value not found!
```


`test_value` is reflected in the response.

Let us see what happens when we enter a payload such as the below (instead of _test_value_).

```javascript
<script>alert(document.domain)</script>
```

It looks like the application is encoding the submitted payload:

```
Cannot GET /api/download/%3Cscript%3Ealert(document.domain)%3C/script%3E
```

We can try URL-encoding our payload once and submitting it again, as follows.

```javascript
%3Cscript%3Ealert%28document.domain%29%3C%2Fscript%3E
```

Now our submitted JavaScript payload is evaluated successfully. The API endpoint is vulnerable to XSS!

