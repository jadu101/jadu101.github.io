---
title: b-Via Parameter Modification
draft: false
tags:
---
An authentication implementation can be flawed if it depends on the presence or value of an HTTP parameter, introducing authentication vulnerabilities.

This type of vulnerability is closely related to authorization issues such as `Insecure Direct Object Reference (IDOR)` vulnerabilities.

## Parameter Modification

Let us take a look at our target web application. This time, we are provided with credentials for the user `htb-stdnt`. After logging in, we are redirected to `/admin.php?user_id=183`.

In some cases, we can bypass authentication entirely by accessing the URL `/admin.php?user_id=183` directly: