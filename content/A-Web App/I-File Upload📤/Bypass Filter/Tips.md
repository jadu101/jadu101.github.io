---
title: Tips
draft: false
tags:
---
- Identify blacklisting
- Identify whitelisting
- Fuzz for allowed extensions
- Fuzz for double & reverse extensions
- Try injecting MIME, but I prefer to just use actually image file and inject php code inside of it.
- If we don't know the uploaded directory, use SVG XXE to find out.



PHP Server
    
    ```powershell
    .php
    .php3
    .php4
    .php5
    .php7
    
    # Less known PHP extensions
    .pht
    .phps
    .phar
    .phpt
    .pgif
    .phtml
    .phtm
    .inc
    ```