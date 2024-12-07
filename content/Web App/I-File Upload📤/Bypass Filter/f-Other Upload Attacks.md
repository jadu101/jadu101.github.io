---
title: f-Other Upload Attacks
draft: false
tags:
---
## Injection in File Name

We can use a malicious string for the uploaded file name.

e.g Name file as `file$(whoami).jpg` or `file'whoami'.jpg` or `file.jpg|whoami`. Web app will attempt to move the uploaded file with an OS command such as `mv file /dir` and `whoami` command might get executed.

e.g Use XSS payload in the file name, such as `<script>alert(window.origin);</script>`

e.g Use SQL query in the file name such as `file';select+sleep(5);--.jpg`

## Upload Directory Disclosure

In some cases (feedback form or a submission form), we may not have access to the link of our uploaded file and may not know the uploads directory.

In such cases, we have to directory bruteforce to figure out the directory.

We can also force an error message to reveal information for further exploitation.

## Windows-specific Attacks

We can use reserved characters such as (`|`, `<`, `>`, `*`, or `?`), which are usually reserved for special uses like wildcards. 

If these characters are not sanitized properly, it might cause an error and reveal the upload directory.

We can also use Windows reserved names for the upload file name such as `CON`, `COM1`, `LPT1`, or `NUL`.

Finally, we may utilize the Windows [8.3 Filename Convention](https://en.wikipedia.org/wiki/8.3_filename) to overwrite existing files or refer to files that do not exist. Older versions of Windows were limited to a short length for file names, so they used a Tilde character (`~`) to complete the file name, which we can use to our advantage.

For example, to refer to a file called (`hackthebox.txt`) we can use (`HAC~1.TXT`) or (`HAC~2.TXT`), where the digit represents the order of the matching files that start with (`HAC`). As Windows still supports this convention, we can write a file called (e.g. `WEB~.CONF`) to overwrite the `web.conf` file.

