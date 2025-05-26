---
title: c-PHP Filters
draft: false
tags:
  - lfi
  - ffuf
---
If we identify an LFI vulnerability in PHP web applications, then we can utilize different [PHP Wrappers](https://www.php.net/manual/en/wrappers.php.php) to be able to extend our LFI exploitation, and even potentially reach remote code execution.

Let's say we want to read `config.php` using LFI. However, since the web app is running PHP, if we request to read `config.php`, it might get executed, instead of us reading the source file. 

In case like above, we have to base64 encode it to get the output in encoded format, instead of having it executed and rendered.
## Input Filters

To use PHP wrapper streams, we can use the `php://` scheme in our string, and we can access the PHP filter wrapper with `php://filter/`.

`filter` wrapper mainly requires parameter `resource` and `read`.

- `resource` - Required for filter wrappers, we can specify the stream we would like to apply the filter on, such as local file.
- `read` - can apply different filters on the input resource. We can use it to specify which filter we want to apply on our resource.

There are four different types of filters available:

- [String Filters](https://www.php.net/manual/en/filters.string.php)
- [Conversion Filters](https://www.php.net/manual/en/filters.convert.php)
- [Compression Filters](https://www.php.net/manual/en/filters.compression.php)
- [Encryption Filters](https://www.php.net/manual/en/filters.encryption.php)

Filter that is useful for LFI attack is `convert.base64-encode` filter, which is under `Conversion Filters`.

## Fuzzing for PHP files

We first have to fuzz and find out different available PHP pages:

```shell-session
jadu101@htb[/htb]$ ffuf -w /opt/useful/SecLists/Discovery/Web-Content/directory-list-2.3-small.txt:FUZZ -u http://<SERVER_IP>:<PORT>/FUZZ.php

...SNIP...

index                   [Status: 200, Size: 2652, Words: 690, Lines: 64]
config                  [Status: 302, Size: 0, Words: 1, Lines: 1]
```

Above we found `index.php` and `config.php`.

`config.php` seems interesting. We want to read it.

## Standard PHP Inclusion

If we use `base64` php filter, we can get the file in base64 encoded format, instead of having it being executed and rendered.

## Source Code Disclosure

Now that we have a potential PHP files we want to read, let's read it using `base64` filter. 

We can do this by specifying `convert.base64-encode` for the `read` parameter and `config` for the `resource` parameter as such:

```url
php://filter/read=convert.base64-encode/resource=config
```

On a full LFI URL, it would look as such:

```
http://server.com/index.php?language=php://filter/read=convert.base64-encode/resource=config
```

> **Note:** We intentionally left the resource file at the end of our string, as the `.php` extension is automatically appended to the end of our input string, which would make the resource we specified be `config.php`.

> Always read the index and understand the code: `php://filter/read=convert.base64-encode/resource=index`
