---
title: c-XSS
draft: false
tags:
  - xss
---
HTML injection can often be used to also perform Cross-Site Scripting (XSS) attacks by injecting JavaScript code to be executed on the client-side.

When we can execute code on the victim's machine, we can potentially gain access to the victim's account or even their machine.

There are three main types of XSS. 

| Type            | Description                                                                                                               |
| --------------- | ------------------------------------------------------------------------------------------------------------------------- |
| `Reflected XSS` | User input is displayed on the page after processing. Search result or error message is an example.                       |
| `Stored XSS`    | User input is stored in the back end database and then displayed when retrieved. Posts or comments is an example of this. |
| `DOM XSS`       | User input is directly shown in the browser and is written to an HTML DOM object. (Vulnerable username or page title)     |
Example XSS payload:

```javascript
#"><img src=/ onerror=alert(document.cookie)>
```

Above payload will access the HTML document tree and retrieve cookie object's value. When the browser process our input, it will be considered a new DOM and our payload will be executed.

