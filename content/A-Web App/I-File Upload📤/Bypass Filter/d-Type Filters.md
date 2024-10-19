---
title: d-Type Filters
draft: false
tags:
  - mime
  - content-type
  - file-signature
  - magic-byte
---
Most modern web servers and web apps test the content of the uploaded file.

Content filters usually specify single category such as images, videos, documents.

There are two common methods for validating the file content:

- Content-Type Header
- File Content

## Content-Type

Below is how a PHP web app can test for Content-Type header to validate the file type:

```php
$type = $_FILES['uploadFile']['type'];

if (!in_array($type, array('image/jpg', 'image/jpeg', 'image/png', 'image/gif'))) {
    echo "Only images are allowed";
    die();
}
```

We can fuzz Content-Type header with SecLists's `/usr/share/seclists/Miscellaneous/Web/content-type.txt` and Burp Intruder.

e.g We can change the Content-Type of PHP code as such:

```PHP
Content-Type: image/jpg
```

> **Note:** A file upload HTTP request has two Content-Type headers, one for the attached file (at the bottom), and one for the full request (at the top). We usually need to modify the file's Content-Type header, but in some cases the request will only contain the main Content-Type header (e.g. if the uploaded content was sent as `POST` data), in which case we will need to modify the main Content-Type header.


## MIME-Type

More common type of tile content validation is testing the uploaded file's `MIME-Type`. 

**MIME** - Multipurpose Internet Mail Extensions - Determines the type of file through its general format and bytes structure.

This is usually done by inspecting the first few bytes of the file's content, which contains the File Signature or Magic Bytes.

e.g `GIF87a` or `GIF89a` - Indicates it is a GIF image.

e.g File starting with a plain-text is usually considered a Text file.

If we change the first byte of any file to the GIF magic byte, its MIME type would be changed to a GIF image.

e.g

```shell-session
jadu101@htb[/htb]$ echo "this is a text file" > text.jpg 
jadu101@htb[/htb]$ file text.jpg 
text.jpg: ASCII text
```



> Best way is to modify both the content type and MIME type.


