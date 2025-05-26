---
title: b-Blacklist Filters
draft: false
tags:
  - blacklist
---
When web app validates the type only on the front-end, we can easily bypass by intercepting and modifying the traffic using Burp Suite.

Most web apps will have type validation control on the back-end server but this can still be bypassed.

## Blacklisting Extensions

Let's say we get `Extension not allowed` error when uploading `.php` file.

There are generally two common forms of validating a file extension on the back-end:

1. Testing against a `blacklist` of types
2. Testing against a `whitelist` of types

e.g Code below checks if the uploaded file extension is `PHP`

```php
$fileName = basename($_FILES["uploadFile"]["name"]);
$extension = pathinfo($fileName, PATHINFO_EXTENSION);
$blacklist = array('php', 'php7', 'phps');

if (in_array($extension, $blacklist)) {
    echo "File type not allowed";
    die();
}
```

## Fuzzing Extensions

We should fuzz extensions to find out which of them passes the filter.

We can use SecLists's [Web Extensions](https://github.com/danielmiessler/SecLists/blob/master/Discovery/Web-Content/web-extensions.txt) along with Burp Suite to do this.


