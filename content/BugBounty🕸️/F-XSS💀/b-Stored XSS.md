---
title: b-Stored XSS
draft: false
tags:
  - stored-xss
---
**Stored XSS (Persistent XSS)** - Inject payload gets stored in the back-end database and retrieve upon visiting the page.

Stored XSS is the most critical type and affects wider audience. It is also harder to remove. 

Example:

```html
<script>alert(window.origin)</script>
<script>alert(document.cookie)</script>
```

If `alert()` tag is blocked, use something like `<plaintext>`.

`<script>print()</script>` this also works.

