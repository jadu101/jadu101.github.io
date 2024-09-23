---
title: b-Identify SSTI
draft: false
tags:
  - ssti-identify
---
## Identify SSTI

The most effective way is to inject special characters with semantic meaning in template engines and observe the web application's behavior.

```
${{<%[%'"}}%\.
```

Since the above test string should almost certainly violate the template syntax, it should result in an error if the web application is vulnerable to SSTI.

## Identify Template Engine

There are SSTI charts out there that usually starts with injecting `${7*7}`.

Follow the flow chart and try out different payloads until you find out the template engine.

