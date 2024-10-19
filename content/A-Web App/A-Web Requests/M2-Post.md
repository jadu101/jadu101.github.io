---
title: M2-POST
draft: false
tags:
  - post
---
`POST` is used when web apps need to transfer files or move the user parameters from the URL.

`GET` places user parameters within the URL. `POST` places user parameters within the HTTP Request body.

By placing user parameters within the HTTP Request body, there are benefits.

- Lack of logging. POST request sometimes transfer large files so it would be stupid to have log all files as part of the requested URL.
- Less Encoding Requirement. POST request places data in body which can accept binary data. The only characters that need to be encoded are those that are used to separate parameters.
- More data can be sent. URL can't be too long (below 2000 characters).

## Login Forms

PHP login forms typically uses POST requests

```shell-session
jadu101@htb[/htb]$ curl -X POST -d 'username=admin&password=admin' http://<SERVER_IP>:<PORT>/

...SNIP...
        <em>Type a city name and hit <strong>Enter</strong></em>
...SNIP...
```

## Authenticated Cookies

After we successfully authenticate, we are given a cookie value so our browser can persist authentication.

```shell-session
jadu101@htb[/htb]$ curl -b 'PHPSESSID=c1nsa6op7vtk7kdis7bcnbadf1' http://<SERVER_IP>:<PORT>/

...SNIP...
        <em>Type a city name and hit <strong>Enter</strong></em>
...SNIP...
```

We can set cookie headers instead as such:

```bash
curl -H 'Cookie: PHPSESSID=c1nsa6op7vtk7kdis7bcnbadf1' http://<SERVER_IP>:<PORT>/
```

```shell-session
jadu101@htb[/htb]$ curl -X POST -d '{"search":"london"}' -b 'PHPSESSID=c1nsa6op7vtk7kdis7bcnbadf1' -H 'Content-Type: application/json' http://<SERVER_IP>:<PORT>/search.php
["London (UK)"]
```





