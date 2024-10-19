---
title: a-Identify Filters
draft: false
tags:
  - commandi
  - filters
---
Developers might secure their web app by 
- blacklisting characters and words on the back-end 
- WAF

Let's say there is a `Host Checker` application where user can input an IP address and the back-end server will execute `ping -c 1 USER_INPUT`.

So normally, we would be able to execute commands by injecting payload such as `127.0.0.1; whoami` but this time, we will consider this web app having a filter running.
## Filter/WAF Detection

Let's say when we try the payload such as `127.0.0.1; whoami`, using operators such as `;`, `&&`, `||`; we get the error message saying `invalid input`.

```bash
127.0.0.1; whoami
```

Above is the payload we sent:

1. A semi-colon character `;`
2. A space character
3. A `whoami` command

Web App detected a blacklisted character or a command from the above payload.

Let's see how to bypass it.

## Blacklisted Characters

A PHP code that blacklist characters might look something like below:

```php
$blacklist = ['&', '|', ';', ...SNIP...];
foreach ($blacklist as $character) {
    if (strpos($_POST['ip'], $character) !== false) {
        echo "Invalid input";
    }
}
```

We should try to identify which character caused the denied request. 

### Identify Blacklisted Character

We should try command such as:

- 127.0.0.1;
- 127.0.0.1'
- 127.0.0.1&&
- 127.0.0.1%0a

Keep on going until there is no error.