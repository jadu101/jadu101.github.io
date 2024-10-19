---
title: a-Fuff
draft: false
tags:
  - ffuf
---
## Basic Methodology


1. Fuzz for subdomains.
2. Fuzz for extensions in each subdomains.
3. Fuzz for directories in each subdomains with proper extensions.
4. Fuzz for parameters on the found directories using both `POST` and `GET` method.
5. Fuzz for values in parameters. 


## Directory Fuzzing

`-w` - wordlists

`-u` - URL

```shell-session
┌──(carabiner1㉿carabiner)-[~]
└─$ ffuf -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-small.txt:FUZZ -u http://94.237.58.173:39024/FUZZ
```

## Extension Fuzzing

If the server is running on `apache`, extension could be `.php`.

If the server is running on `IIS`, extension could be `.asp` or `.aspx`.

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/Web-Content/web-extensions.txt:FUZZ -u http://SERVER_IP:PORT/blog/indexFUZZ
```

## Page Fuzzing

Remember to remove the copyright at the top of the wordlist since it can cause errror.

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/Web-Content/directory-list-2.3-small.txt:FUZZ -u http://SERVER_IP:PORT/blog/FUZZ.php
```

## Recursive Fuzzing


We can enable recursive scanning with the `-recursion` flag.

We can specify the recursion depth with `-recursion-depth` flag. 

Depth `1`  will only fuzz the main directory and their direct sub directories (like `/login/user`)

We can specify extension using `-e .php`.

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/Web-Content/directory-list-2.3-small.txt:FUZZ -u http://SERVER_IP:PORT/FUZZ -recursion -recursion-depth 1 -e .php -v
```

## Subdomain Fuzzing

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/DNS/subdomains-top1million-5000.txt:FUZZ -u https://FUZZ.inlanefreight.com/

www                     [Status: 200, Size: 22266, Words: 2903, Lines: 316, Duration: 583ms]
my                      [Status: 301, Size: 0, Words: 1, Lines: 1, Duration: 558ms]
support                 [Status: 301, Size: 0, Words: 1, Lines: 1, Duration: 566ms]
customer                [Status: 301, Size: 0, Words: 1, Lines: 1, Duration: 557ms]
```


## Vhost Fuzzing

Vhost is different from subdomain that it is basically a subdomain served on the same server and has the same IP. This way single IP could server two or more different websites.

In many cases, some websites would have subdomains but they won't make it public so we cannot find them in public DNS records. This is why we have to bruteforce it. 

We can fuzz for HTTP headers to scan for VHosts. 

Let's use `Host:` header with `-H` flag.

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/DNS/subdomains-top1million-5000.txt:FUZZ -u http://academy.htb:PORT/ -H 'Host: FUZZ.academy.htb'
```

But this way, we cannot tell which VHost is a valid, existing one. We need filtering process. 

### Filter Result

We can usually filter bases on the **Response Size** since we know the size of the incorrect results. 

Below command will filter out results with the response size of 900:

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/DNS/subdomains-top1million-5000.txt:FUZZ -u http://academy.htb:PORT/ -H 'Host: FUZZ.academy.htb' -fs 900
```


We can also filter based on the word count as such:
```
ffuf -u http://10.10.11.23 -c -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt -H 'Host: FUZZ.permx.htb' -fw 18
```


## Parameter Fuzzing

Fuzzing parameters sometimes expose unpublished parameters that are publicly accessible. These parameters tend to be less secure. 

### GET Request Fuzzing

Let's try fuzzing `GET` requests which are usually passed right after the URL with a `?` symbol such as:

- `http://carabinersec.com:PORT/admin/admin.php?param1=key`


We can fuzz for parameters as such:

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/Web-Content/burp-parameter-names.txt:FUZZ -u http://admin.academy.htb:PORT/admin/admin.php?FUZZ=key -fs xxx
```

### POST Request Fuzzing

Unlike `GET` request, `POST` requests are passed in the `data` field within the HTTP request.

To fuzz `data` field with ffuf, we can use `-d` flag. 

We also have to add `-X POST` to send `POST` requests. 

```shell-session
ffuf -w /opt/useful/SecLists/Discovery/Web-Content/burp-parameter-names.txt:FUZZ -u http://admin.academy.htb:PORT/admin/admin.php -X POST -d 'FUZZ=key' -H 'Content-Type: application/x-www-form-urlencoded' -fs xxx
```

### Value Fuzzing

Let's say from above's request fuzzing, we have a working parameter. We now have to fuzz the correct value that would return the content we want.

Let's learn about how to fuzz for parameter values. 

Below is an example which fuzz numbers into `id` parameter in `POST` request:

```shell-session
ffuf -w ids.txt:FUZZ -u http://admin.academy.htb:PORT/admin/admin.php -X POST -d 'id=FUZZ' -H 'Content-Type: application/x-www-form-urlencoded' -fs xxx
```


