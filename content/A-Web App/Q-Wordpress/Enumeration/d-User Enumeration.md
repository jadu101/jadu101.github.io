---
title: d-User Enumeration
draft: false
tags:
---
## Uncover User ID

The first method is reviewing posts to uncover the ID assigned to the user and their corresponding username.

The `admin` user is usually assigned the user ID `1`. We can confirm this by specifying the user ID for the `author` parameter in the URL.

`http://blog.inlanefreight.com/?author=1`

We can find for non-existing user as such:

```shell-session
jadu101@htb[/htb]$ curl -s -I http://blog.inlanefreight.com/?author=100

HTTP/1.1 404 Not Found
Date: Wed, 13 May 2020 20:47:14 GMT
Server: Apache/2.4.29 (Ubuntu)
Expires: Wed, 11 Jan 1984 05:00:00 GMT
Cache-Control: no-cache, must-revalidate, max-age=0
Link: <http://blog.inlanefreight.com/index.php/wp-json/>; rel="https://api.w.org/"
Transfer-Encoding: chunked
Content-Type: text/html; charset=UTF-8
```

## JSON Endpoint

The second method requires interaction with the `JSON` endpoint, which allows us to obtain a list of users. This was changed in WordPress core after version 4.7.1, and later versions only show whether a user is configured or not. Before this release, all users who had published a post were shown by default.

```shell-session
jadu101@htb[/htb]$ curl http://blog.inlanefreight.com/wp-json/wp/v2/users | jq

[
  {
    "id": 1,
    "name": "admin",
    "url": "",
    "description": "",
    "link": "http://blog.inlanefreight.com/index.php/author/admin/",
    <SNIP>
  },
  {
    "id": 2,
    "name": "ch4p",
    "url": "",
    "description": "",
    "link": "http://blog.inlanefreight.com/index.php/author/ch4p/",
    <SNIP>
  },
<SNIP>
```

