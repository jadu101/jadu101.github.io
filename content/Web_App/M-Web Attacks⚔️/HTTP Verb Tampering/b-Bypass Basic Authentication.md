---
title: b-Bypass Basic Authentication
draft: false
tags:
---
HTTP Verb Tampering vulnerability is pretty simple. We just need to try alternate HTTP methods to see how they are handled.

HTTP Verb Tampering vuln from insecure server configuration can be identified easily using automated tools but insecure coding is not identified from tools because it requires active testing.

First type of HTTP Verb Tampering vulnerability is usually caused from Insecure Web Server Configurations and exploiting this, we can bypass HTTP Basic Authentication prompt on certain pages.

## Identify

> e.g Basic File Manager Web Application. We can add new files by typing their names and hitting enter. We can delete all files by clicking on `Reset` button but this is restricted for authenticated users only.


Let's try to bypass the above restriction.

We first should identify which pages are restricted by this authentication. 

Let's say `/admin/reset.php` page is being restricted. It is either the `/admin` directory or only `/admin/reset.php` is restricted.

We can confirm this by visiting the `/admin` directory, and we do indeed get prompted to log in again. This means that the full `/admin` directory is restricted.

## Exploit

To exploit the page, we need to identify the HTTP request method being used:

```Burp
GET /admin/reset.php
```

Page is using `GET` request.  

Let's try sending `POST` request and see whether the web page allows `POST` requests. (Right click on Burp and select `Change Request Method`).

Once we send in the modified traffic, we still get prompted to log in, meaning web server configurations do cover both `GET` and `POST`.

Let's see if the server accepts `HEAD` requests:

```shell-session
jadu101@htb[/htb]$ curl -i -X OPTIONS http://SERVER_IP:PORT/

HTTP/1.1 200 OK
Date: 
Server: Apache/2.4.41 (Ubuntu)
Allow: POST,OPTIONS,HEAD,GET
Content-Length: 0
Content-Type: httpd/unix-directory
```

As we can see, the response shows `Allow: POST,OPTIONS,HEAD,GET`, which means that the web server indeed accepts `HEAD` requests, which is the default configuration for many web servers.

Now let's intercept the `reset` traffic again this time use `HEAD` request to see how the web server handles it. 

Luckily, with using `HEAD`, we are not prompted to login and all the files actually got deleted. 

We successfully triggered the `Reset` functionality without having admin access or any credentials. 



