---
title: c-HTTP Requests and Responses
draft: false
tags:
  - cURL
  - devtool
---
**HTTP Request** -> cURL/browser -> URL, path, parameters

**HTTP Response** -> web server -> Response code, resource data

## HTTP Request

Take a look at the example HTTP request below:

```
GET /services/login.html HTTP/1.1
```

|           |                     |                                                                                                                       |
| --------- | ------------------- | --------------------------------------------------------------------------------------------------------------------- |
| `Method`  | `GET`               | The HTTP method or verb, which specifies the type of action to perform.                                               |
| `Path`    | `/users/login.html` | The path to the resource being accessed. This field can also be suffixed with a query string (e.g. `?username=user`). |
| `Version` | `HTTP/1.1`          | The third and final field is used to denote the HTTP version.                                                         |

> HTTP version 1.x sends requests in a clear-text. HTTP version 2.x sends requests as binary data in a dictionary form.

Next set of lines in a request contains information such as HTTP header value pairs, like`host`, `User-Agent`,`Cookie` and many more. Headers are used to specify various attributes of a request. 

## HTTP Response

Take a look at the example HTTP Response below:

```
HTTP/1.1 200 OK
```

Here, HTTP version is 1.1 and HTTP response code is 200. 

After the first line, response lists its headers and response body.

## cURL

Using `cURL`, we can preview both HTTP request and the full HTTP response.

We can use `-v` verbose flag to view full HTTP request and response.

```
yoon@yoon-XH695R:~$ curl -v https://jadu101.github.io
* Host jadu101.github.io:443 was resolved.
* IPv6: 2606:50c0:8001::153, 2606:50c0:8003::153, 2606:50c0:8000::153, 2606:50c0:8002::153
* IPv4: 185.199.109.153, 185.199.111.153, 185.199.110.153, 185.199.108.153
*   Trying 185.199.109.153:443...
* Connected to jadu101.github.io (185.199.109.153) port 443
* ALPN: curl offers h2,http/1.1
* TLSv1.3 (OUT), TLS handshake, Client hello (1):
*  CAfile: /etc/ssl/certs/ca-certificates.crt
*  CApath: /etc/ssl/certs
```

## Browser DevTools

Browser devtools were build so that developers can test their web apps.

When we visit a website, browser sends multiple web requests and handles multiple HTTP responses to render the final view we see in the browser window.

