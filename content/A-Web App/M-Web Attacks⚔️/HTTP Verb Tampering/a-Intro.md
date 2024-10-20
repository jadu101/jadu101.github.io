---
title: a-intro
draft: false
tags:
---
The `HTTP` protocol works by accepting various HTTP methods as `verbs` at the beginning of an HTTP request.

Some web apps are configured to accept certain HTTP methods for their various functionalities and perform a particular action based on the type of the request.

`GET` and `POST` is the most common but we can send different methods and see how the web server handles it.

If the web server configuration is not restricted to only accept the HTTP methods required by the web server, web app is not developed to handle other types of HTTP requests and we may be able to exploit this.

## HTTP Verb Tampering

HTTP has [9 different verbs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods) that can be accepted as HTTP methods by web servers.

|Verb|Description|
|---|---|
|`HEAD`|Identical to a GET request, but its response only contains the `headers`, without the response body|
|`PUT`|Writes the request payload to the specified location|
|`DELETE`|Deletes the resource at the specified location|
|`OPTIONS`|Shows different options accepted by a web server, like accepted HTTP verbs|
|`PATCH`|Apply partial modifications to the resource at the specified location|

Some of the verbs can be very powerful.

We can do something like writing (`PUT`) or deleting (`DELETE`) files to the webroot directory on the back-end server.

## Insecure Configurations

Imagine following configuration is used to require authentication on a particular web page:

```xml
<Limit GET POST>
    Require valid-user
</Limit>
```

An attacker may still use a different HTTP method (like `HEAD`) to bypass this authentication mechanism altogether.


## Insecure Coding

Insecure coding sometimes causes the other type of HTTP verb tampering vulnerability.

 For example, if a web page was found to be vulnerable to a SQL Injection vulnerability, and the back-end developer mitigated the SQL Injection vulnerability by the following applying input sanitization filters:


```php
$pattern = "/^[A-Za-z\s]+$/";

if(preg_match($pattern, $_GET["code"])) {
    $query = "Select * from ports where port_code like '%" . $_REQUEST["code"] . "%'";
    ...SNIP...
}
```

Above, sanitization filter is only being tested on the `GET` parameter.

In this case, an attacker may use a `POST` request to perform SQL injection, in which case the `GET` parameters would be empty (will not include any bad characters). The request would pass the security filter, which would make the function still vulnerable to SQL Injection.

