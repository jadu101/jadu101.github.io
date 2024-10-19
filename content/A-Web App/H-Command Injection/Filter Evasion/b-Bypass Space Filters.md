---
title: b-Bypass Space Filters
draft: false
tags:
  - new-line
  - space
---
Let's say there is a `Host Checker` application where user can input an IP address and the back-end server will execute `ping -c 1 USER_INPUT`.

So normally, we would be able to execute commands by injecting payload such as `127.0.0.1; whoami` but this time, we will consider this web app having a filter running.

Most of the injection operators are blacklisted but the new-line character is not blacklisted (`%0a`).



## Bypass Blacklisted Spaced

So new-line operator (`%0a`) is not black listed.

Below payload will be passed:

```
127.0.0.1%0a
```

However, when we try `127.0.0.1%0a+whoami`, we will still get an error, meaning there is another filters to bypass.

So there are two possibilities:

- `whoami` command was blacklisted. But most likely not. It is just a command.
- `+`(space) was blacklisted. Most Likely yes since most of the time spaces are blacklisted.

### Using Tabs

Using tabs (`%09`) instead of spaces is a technique that may work.

We can use payload such as:

`127.0.0.1%0a%09`

### Using $IFS

Using the (`$IFS`) Linux Environment Variable may also work since its default value is a space and a tab.

Payload should look like:

`127.0.0.1%0a${IFS}`

### Using Brace Expansion

We can use the `Bash Brace Expansion` feature, which automatically adds spaces between arguments wrapped between braces, as follows:

```shell-session
jadu101@htb[/htb]$ {ls,-la}

total 0
drwxr-xr-x 1 21y4d 21y4d   0 Jul 13 07:37 .
drwxr-xr-x 1 21y4d 21y4d   0 Jul 13 13:01 ..
```

Our payload should look like:

`127.0.0.1%0a{ls,-la}`

