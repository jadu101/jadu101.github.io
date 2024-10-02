---
title: a-LFI
draft: false
tags:
  - lfi
---
## Basics

> Let's say there is a web app which it allows us to change language to either English or Spanish

```
/index.php?language=es.php
```

Changing `es.php` to `/etc/passwd`, we can read the file.

## Path Traversal

If the code like below, we can use absolute path such as `/etc/passwd`:

```php
include($_GET['language']);
```

If the code is like below, we need path traversal:

```php
include("./languages/" . $_GET['language']);
```

(e.g. `../../../../etc/passwd`)

## Filename Prefix

In our previous example, we used the `language` parameter after the directory, so we could traverse the path to read the `passwd` file. On some occasions, our input may be appended after a different string. For example, it may be used with a prefix to get the full filename, like the following example:

```php
include("lang_" . $_GET['language']);
```

We can prefix a `/` before our payload, and this should consider the prefix as a directory, and then we should bypass the filename and be able to traverse directories:

`/../../../../../etc/passwd`

## Appended Extensions

Another very common example is when an extension is appended to the `language` parameter, as follows:

```php
include($_GET['language'] . ".php");
```

There are several techniques that we can use to bypass this, and we will discuss them in upcoming sections.

## Second-Order Attacks

This occurs because many web application functionalities may be insecurely pulling files from the back-end server based on user-controlled parameters.

For example, a web application may allow us to download our avatar through a URL like (`/profile/$username/avatar.png`). If we craft a malicious LFI username (e.g. `../../../etc/passwd`), then it may be possible to change the file being pulled to another local file on the server and grab it instead of our avatar.

