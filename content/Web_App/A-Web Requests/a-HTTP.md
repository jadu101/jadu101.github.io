---
title: a-HyperText Transfer Protocol(HTTP)
draft: false
tags:
  - http
  - cURL
---
Most internet communications are made with web requests through HTTP protocol.

HTTP communication is consisted of **client** and **server**.

Default port: **80**

## URL

When we visit a website, we use **Fully Qualified Domain Name(FQDN)** as a **Uniform Resource Locator (URL)**, like `jadu101.github.io`.

We access resource over HTTP through **URL** as such:

`http://admin:password@carabinersec.com:80/login.php?login=true#status`

Let's break down above URL:

- Scheme -> http:// -> Identify protocol being accessed by the client.
- User Info -> admin:password@ 
- Host -> carabinersec.com -> Resource location
- Port -> :80
- Path -> /login.php -> Resource being accessed. Could be file or a folder
- Query String -> ?login=true -> Query string starts with `?`. `login` is a parameter. `true` is a value. `&` is used for multiple parameters.
- Fragments -> `#status` -> They are processed by the browsers on the client-side to locate sections within the primary resource.

When a user enters the URL (carabinersec.com), DNS server resolves the domain and gets the ip of carabinersec.com. Once the browser gets the IP address linked the requested domain, it sends a `GET` request to the default HTTP port. Web server receives the request and processes it. 

## cURL

`cURL` is a command line tool that supports HTTP along with many other protocols. 

We send basic HTTP request as such:

```
yoon@yoon-XH695R:~$ curl www.google.com
<!doctype html><html itemscope="" itemtype="http://schema.org/WebPage" lang="ko"><head><meta content="text/html; charset=UTF-8" http-equiv="Content-Type"><meta content="/images/branding/googleg/1x/googleg_standard_color_128dp.png" itemprop="image"><title>Google</title><script nonce="3xYE5vRzZsI254P6YyzqPg">(functio
```

`cURL` doesn't render HTML/JavaScript/CSS code unlike web browser, making it faster and more convenient than a web browser.

Using `-O` `-o` flag, we can download a page or a file into a file.

`-O` flag will automatically name the file as the remote file name:

```
yoon@yoon-XH695R:~$ curl -O carabinersec.com/index.html
yoon@yoon-XH695R:~$ ls
index.html
```

One more thing, `-s` silents the status. 

