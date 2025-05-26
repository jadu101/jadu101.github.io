---
title: a-Via Direct Access
draft: false
tags:
---
## Direct Access

The most straightforward way of bypassing authentication checks is to request the protected resource directly from an unauthenticated context. An unauthenticated attacker can access protected information if the web application does not properly verify that the request is authenticated.

Let us assume a web application uses the following snippet of PHP code to verify whether a user is authenticated:

```php
if(!$_SESSION['active']) {
	header("Location: index.php");
}
```

This code redirects the user to `/index.php` if the session is not active/unauthenticated.

However, the PHP script does not stop execution, resulting in protected information within the page being sent in the response body. So if we send something like `GET /admin.php`, we'd be able to see the page through Burp Suite.

But if we try to access `/admin.php` through a browser, it won't work since the browser follows the redirect.



