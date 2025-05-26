---
title: c-Obtain Session Identifier wo User Interaction
draft: false
tags:
  - wireshark
---
There are two category for an attacker to obtain session identifiers:

1. Session ID-obtaining attacks without user interaction
2. Session ID-obtaining attacks requiring user interaction

In this section, we will talk about Session ID obtaining without user interaction.

## Obtaining Session Identifiers via Traffic Sniffing

This type of information will be more challenging and, most of the time, impossible to obtain if HTTP traffic is encrypted through SSL or IPsec.

Obtaining session identifiers through traffic sniffing requires:

- The attacker must be positioned on the same local network as the victim
- Unencrypted HTTP traffic

We can use Wireshark for packer sniffing.

### Simulate Attacker

Navigate to the target web app and notice that the application uses a cookie named `auth-session` most probably as a session identifier.

Now fire up Wireshark to start sniffing traffic on the local network as follows.

```shell-session
[!bash!]$ sudo -E wireshark
```

Right-click "tun0" and then click "Start capture"

### Simulate Victim

Navigate to `http://xss.htb.net` through a `New Private Window` and login to the application using the credentials below:

- Email: heavycat106
- Password: rocknrol

This is an account that we created to look into the application!

### Obtain Victim's Cookie

Inside Wireshark, first, apply a filter to see only HTTP traffic.

Inside Wireshark, first, apply a filter to see only HTTP traffic.

Inside Wireshark, first, apply a filter to see only HTTP traffic.

Select `String` on the third drop-down menu and specify `auth-session` on the field next to it. Finally, click `Find`. Wireshark will present you with the packets that include an `auth-session` string.

The cookie can be copied by right-clicking on a row that contains it, then clicking on `Copy` and finally clicking `Value`.

### Hijack Victim Session

Back to the browser window using which you first browsed the application (not the Private Window), open Web Developer Tools, navigate to _storage_, and change your current cookie's value to the one you obtained through Wireshark (remember to remove the `auth-session=` part).

## Obtaining Session Identifiers Post-Exploitation (Web Server Access)

During the post-exploitation phase, session identifiers and session data can be retrieved from either a web server's disk or memory. Of course, an attacker who has compromised a web server can do more than obtain session data and session identifiers. That said, an attacker may not want to continue issuing commands that increase the chances of getting caught.

### PHP

Let us look at where PHP session identifiers are usually stored.

```shell-session
[!bash!]$ locate php.ini
[!bash!]$ cat /etc/php/7.4/cli/php.ini | grep 'session.save_path'
[!bash!]$ cat /etc/php/7.4/apache2/php.ini | grep 'session.save_path'
```

In our default configuration case it's `/var/lib/php/sessions`. Now, please note a victim has to be authenticated for us to view their session identifier. The files an attacker will search for use the name convention `sess_<sessionID>`.

### Java

In our default configuration case it's `/var/lib/php/sessions`. Now, please note a victim has to be authenticated for us to view their session identifier. The files an attacker will search for use the name convention `sess_<sessionID>`.

### .NET

Finally, let us look at where .NET session identifiers are stored.

Session data can be found in:

- The application worker process (aspnet_wp.exe) - This is the case in the _InProc Session mode_
- StateServer (A Windows Service residing on IIS or a separate server) - This is the case in the _OutProc Session mode_
- An SQL Server

## Obtaining Session Identifiers Post-Exploitation (Database Access)


```sql
show databases;
use project;
show tables;
select * from users;
```

Here we can see the users' passwords are hashed. We could spend time trying to crack these; however, there is also a "all_sessions" table. Let us extract data from that table.

```sql
select * from all_sessions;
select * from all_sessions where id=3;
```

