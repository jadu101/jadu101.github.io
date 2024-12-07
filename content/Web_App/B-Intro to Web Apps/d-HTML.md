---
title: d-HTML
draft: false
tags:
  - html
---
Most dominant component of the front end of web app is **HTML**.

## HTML Structure

```shell-session
document
 - html
   -- head
      --- title
   -- body
      --- h1
      --- p
```

## URL Encoding

For a browser to properly display a page's content, it has to know the charset in use. 

In URLs, browsers can only use ASCII encoding.

All other characters outside of ASCII character set have to be URL encoded.

Below are some examples of it:

|Character|Encoding|
|---|---|
|space|%20|
|!|%21|
|"|%22|
|#|%23|
|$|%24|

