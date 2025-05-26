---
title: Interactive Shell
draft: false
tags:
---
Most of the time we can use Python to spawn interactive shell. But sometimes Python might not be available. 

Most Linux system will have `/bin/sh` or `/bin/bash.



## /bin/sh -i

```shell-session
/bin/sh -i
sh: no job control in this shell
sh-4.2$
```

## Perl

```shell-session
perl —e 'exec "/bin/sh";'
```

or

```shell-session
perl: exec "/bin/sh";
```

## Ruby

```shell-session
ruby: exec "/bin/sh"
```


## Lua

```shell-session
lua: os.execute('/bin/sh')
```

## AWK

```shell-session
awk 'BEGIN {system("/bin/sh")}'
```

## Find

Find is a command but can also be used to spawn interactive shell.

```shell-session
find / -name nameoffile -exec /bin/awk 'BEGIN {system("/bin/sh")}' \;
```

Or

```shell-session
find . -exec /bin/sh \; -quit
```

## VIM

Vim is a text editor.

```shell-session
vim -c ':!/bin/sh'
```

Vim escape:

```shell-session
vim
:set shell=/bin/sh
:shell
```

