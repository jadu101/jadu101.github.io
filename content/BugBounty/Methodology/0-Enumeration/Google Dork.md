---
title: Google Dork
draft: false
tags:
  - google-dork
  - waybackurls
  - web-archive
---
Let's learn how to scrapte URLs using Google Dorks. 

We can find juicy `.php` URLs.

## S1: Google Dork

When used properly, we can dig up ton of information on the target website.

To discover `.php` pages:

```
site:*.vulnweb.com ext:php  
site:*.vulnweb.com filetype:php
```

## S2: Python Automation

[dork.py](https://github.com/schooldropout1337/dork?source=post_page-----527a420ecfc9--------------------------------)

```
python dork.py -d 'site:*.vulnweb.com ext:php'
```

## S3: Web Archives

```
http://web.archive.org/cdx/search/cdx?url=vulnweb.com/*&output=text&fl=original&collapse=urlkey&from=
```

This pulls a list of historical URLs from `vulnweb.com` on the Wayback Machine. Never underestimate the power of old content. It might show something the admin forgot to patch or remove.

## S4: Waybackurls

```
waybackurls vulnweb.com | grep '\.php'
```