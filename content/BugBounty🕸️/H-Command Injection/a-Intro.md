---
title: a-intro
draft: false
tags:
---
**Command Injection** is one of the most critical types of vulnerability and it might lead to entire system compromise.

There are different types of injections found in web apps:

|Injection|Description|
|---|---|
|OS Command Injection|Occurs when user input is directly used as part of an OS command.|
|Code Injection|Occurs when user input is directly within a function that evaluates code.|
|SQL Injections|Occurs when user input is directly used as part of an SQL query.|
|Cross-Site Scripting/HTML Injection|Occurs when exact user input is displayed on a web page.|

## OS Command Injections

The user input must directly or indirectly go into a web query that executes system commands. 

### PHP Example

**PHP** may use `exec`, `system`, `shell_exec`, `passthru`, or `popen` functions to execute commands directly on the back-end server.

Example code that is vulnerable to command injection:

```php
<?php
if (isset($_GET['filename'])) {
    system("touch /tmp/" . $_GET['filename'] . ".pdf");
}
?>
```

### NodeJS Example

**NodeJS** may use `child_process.exec` or `child_process.spawn` to execute commands.

Example code that is vulnerable to command injection:

```javascript
app.get("/createfile", function(req, res){
    child_process.exec(`touch /tmp/${req.query.filename}.txt`);
})
```

