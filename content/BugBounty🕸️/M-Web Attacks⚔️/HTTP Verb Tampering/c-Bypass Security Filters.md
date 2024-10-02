---
title: c-Bypass Security Filters
draft: false
tags:
---
The other and more common type of HTTP Verb Tampering vulnerability is caused by `Insecure Coding` errors made during the development of the web application, which lead to web application not covering all HTTP methods in certain functionalities.

This is usually found in security filters that detect malicious requests.

> **e.g** Security Filter is being used to detect injection vulnerability and only checks for injections in `POST` parameter (`$_POST['parameter']`), it may be possible to bypass it by simply changing request method to `GET`.


## Identify

Let's continue with the example of `File Manager` from `b-Bypass Basic Authentication`.

If we try to create a new file name with special character such as `test;`, we get an error saying `Malicious Request Denied`.

This means the web app is using a security filter on the back-end is blocking injection attempts. If no matter what we do, we cannot bypass the filter, we may try an HTTP Verb Tampering attack to see if we can bypass the security filter.

## Exploit

Let's intercept the creating new file traffic and use `Change Request Method` to change it to another method:

```Burp
GET /index.php?filename=test%3B
```

This time we didn't get error message and the file was successfully created (`test`).

Let's try command injection to tell if we have successfully bypassed the security filter.

We will use the following file name in our attack `file1; touch file2;`.

Once we send the above file name as `GET` request, we can see that this time both `file1` and `file2` was created, meaning command injection works. 



