---
title: PHP Info-Information Disclosure
draft: false
tags:
---
PHP info page disclosure is one of the easy bug.

## Fuzz

Google dork to fuzz:

```
site:*.Redacted.com intitle:"phpinfo()" | intext:"(php.ini)"
```

Or using directory fuzzing , e.g ”gobuster, dirsearch, fuff”:

```
dirsearch -u https://[*].[Redacted].com/ - max-rate=10
```

## Exploit

Vulnerable page might be something like:

```
https://[*].[Redacted].gov/upload/test.php
```