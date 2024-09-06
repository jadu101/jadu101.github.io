---
title: M1-GET
draft: false
tags:
  - get
---
`GET` request is the default request to obtain the remote resource hosted at that URL.

> Using browser devtool's Network tab to understand how a web app interacts with its backend is an essential exercise for any web app assessment or bug bounty exercise.


## HTTP Basic Auth

Let's say there is a web app with `basic HTTP authentication` running on it.

If we try yo access the page with `cURL`, we won't be able to, without credentials.

```shell-session
jadu101@htb[/htb]$ curl -i http://<SERVER_IP>:<PORT>/
HTTP/1.1 401 Authorization Required
Date: Mon, 21 Feb 2022 13:11:46 GMT
Server: Apache/2.4.41 (Ubuntu)
Cache-Control: no-cache, must-revalidate, max-age=0
WWW-Authenticate: Basic realm="Access denied"
Content-Length: 13
Content-Type: text/html; charset=UTF-8

Access denied
```

We can provide credentials through cURL using `-u` flag:

```shell-session
jadu101@htb[/htb]$ curl -u admin:admin http://<SERVER_IP>:<PORT>/

<!DOCTYPE html>
<html lang="en">

<head>
...SNIP...
```

We can authenticate without using `-u` flag by directly providing `basic HTTP auth` credentials as such:

```shell-session
jadu101@htb[/htb]$ curl http://admin:admin@<SERVER_IP>:<PORT>/

<!DOCTYPE html>
<html lang="en">

<head>
...SNIP...
```


## HTTP Authorization Header

If we add `-v` flag to the same web app being used above, we can see the line `Authorization: Basic YWRtaW46YWRtaW4=`.

`YWRtaW46YWRtaW4=` decodes as `admin:admin`

```
yoon@yoon-XH695R:/var/www/html$ echo "YWRtaW46YWRtaW4=" | base64 -d
admin:admin
```

Instead of providing credentials, we can just specify authorization header as such and it would still work:

```
jadu101@htb[/htb]$ curl -H 'Authorization: Basic YWRtaW46YWRtaW4=' http://<SERVER_IP>:<PORT>/

<!DOCTYPE html
<html lang="en">

<head>
...SNIP...
```

## GET Parameters

