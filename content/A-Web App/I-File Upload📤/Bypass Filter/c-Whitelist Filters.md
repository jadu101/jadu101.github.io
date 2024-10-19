---
title: c-Whitelist Filters
draft: false
tags:
---
## Whitelisting Extensions

Whitelisting only accept extension that are on the list.

e.g

We get `Only images are allowed` error when uploading `.phtml` file to the web app.

The following is an example of a file extension whitelist test:

```php
$fileName = basename($_FILES["uploadFile"]["name"]);

if (!preg_match('^.*\.(jpg|jpeg|png|gif)', $fileName)) {
    echo "Only images are allowed";
    die();
}
```

## Double Extensions

We can upload files like `shell.jpg.php`.

However, double extensions still might not work sometimes if the web app is using a strict `regex` pattern as such:

```php
if (!preg_match('/^.*\.(jpg|jpeg|png|gif)$/', $fileName)) { ...SNIP... }
```

## Reverse Double Extension

If double extension wouldn't work, we can try using reverse double extension.

Even if the file upload functionality uses a strict regex pattern, organization  may use the insecure configurations for the web server.

e.g `/etc/apache2/mods-enabled/php7.4.conf` for Apache2 web server may include the following configuration:

```xml
<FilesMatch ".+\.ph(ar|p|tml)">
    SetHandler application/x-httpd-php
</FilesMatch>
```

Above configuration specifies a whitelist with regex pattern that matches .phar, .php, and .phtml. 

We can pass in payload as `shell.php.jpg`.

## Character Injection

We can inject several characters before or after the final extension to cause the web app to misinterpret the filename and execute the uploaded file as a PHP script:

- `%20`
- `%0a`
- `%00`
- `%0d0a`
- `/`
- `.\`
- `.`
- `…`
- `:`

e.g `shell.php%00.jpg` will work with PHP server 5.x

e.g `shell.aspx:.jpg` might work with Windows server.

Using the script below we can generate all permutations of the file name, where the above characters would be injected before and after both `PHP` and `JPG` extensions:

```bash
for char in '%20' '%0a' '%00' '%0d0a' '/' '.\\' '.' '…' ':'; do
    for ext in '.php' '.phps'; do
        echo "shell$char$ext.jpg" >> wordlist.txt
        echo "shell$ext$char.jpg" >> wordlist.txt
        echo "shell.jpg$char$ext" >> wordlist.txt
        echo "shell.jpg$ext$char" >> wordlist.txt
    done
done
```

