---
title: a-intro
draft: false
tags:
  - ssi
---
## SSI Injection

SSI injection occurs when an attacker can inject SSI directives into a file that is subsequently served by the web server, resulting in the execution of the injected SSI directives.

For instance, when the web application contains a vulnerable file upload vulnerability that enables an attacker to upload a file containing malicious SSI directives into the web root directory. Additionally, attackers might be able to inject SSI directives if a web application writes user input to a file in the web root directory.

**Server-Side Includes (SSI)** is used to create dynamic content on HTML pages.

Typical file extensions include `.shtml`, `.shtm`, and `.stm`.

However, web servers can be configured to support SSI directives in arbitrary file extensions. As such, we cannot conclusively conclude whether SSI is used only from the file extension.

## SSI Directives

SSI utilizes `directives` to add dynamically generated content to a static HTML page. These directives consist of the following components:

- `name`: the directive's name
- `parameter name`: one or more parameters
- `value`: one or more parameter values

```ssi
<!--#name param1="value1" param2="value" -->
```


Below are some common SSI directives.
### printenv

This directive prints environment variables. It does not take any variables

```ssi
<!--#printenv -->
```

### config

This directive changes the SSI configuration by specifying corresponding parameters. For instance, it can be used to change the error message using the `errmsg` parameter:

```ssi
<!--#config errmsg="Error!" -->
```

### echo

This directive prints the value of any variable given in the `var` parameter. Multiple variables can be printed by specifying multiple `var` parameters. For instance, the following variables are supported:

- `DOCUMENT_NAME`: the current file's name
- `DOCUMENT_URI`: the current file's URI
- `LAST_MODIFIED`: timestamp of the last modification of the current file
- `DATE_LOCAL`: local server time


```ssi
<!--#echo var="DOCUMENT_NAME" var="DATE_LOCAL" -->
```

### exec

This directive executes the command given in the `cmd` parameter:


```ssi
<!--#exec cmd="whoami" -->
```

### include

This directive includes the file specified in the `virtual` parameter. It only allows for the inclusion of files in the web root directory.


```ssi
<!--#include virtual="index.html" -->
```

