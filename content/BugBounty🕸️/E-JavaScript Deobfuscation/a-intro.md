---
title: a-intro
draft: false
tags:
---
Code deobfuscation is important to learn if you want to become good in code analysis and reverse engineering. 

We often encounter obfuscated code that is hiding certain functionalities, like malware that uses obfuscated JS code to retrieve its main payload. 

Following topics will be covered:

- Locating JavaScript code
- Intro to Code Obfuscation
- How to Deobfuscate JavaScript code
- How to decode encoded messages
- Basic Code Analysis
- Sending basic HTTP requests

## JavaScript

`JavaScript` can be internally written between `<script>` elements or written into a separate `.js` file and referenced within the `HTML` code.

```html
<script src="secret.js"></script>
```

