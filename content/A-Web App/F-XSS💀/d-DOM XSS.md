---
title: d-DOM XSS
draft: false
tags:
  - dom-xss
---
**DOM-based XSS** is completely processed on the client-side through JavaScript. 

It occurs when JavaScript is used to change the page source through the `Document Object Model (DOM)`.

We can check for DOM based processes using Browser dev tool.

Make a request of some kind and check on Dev tool's `Network` page, if there is a DOM based process, there won't be anything.

## Source and Sink

To better understand DOM-base XSS, we need to understand `Source` and `Sink` of the object displayed on the page. 

**Source** - JavaScript object the takes the user input, it can be any input parameter like a URL parameter or an input field.

**Sink** - Function that writes the user input to a DOM object on the page. 

If the Sink function doesn't properly sanitize the user input, it would be vulnerable to an XSS attack.

 Some of the commonly used JavaScript functions to write to DOM objects are:

- `document.write()`
- `DOM.innerHTML`
- `DOM.outerHTML`

Furthermore, some of the `jQuery` library functions that write to DOM objects are:

- `add()`
- `after()`
- `append()`

Example:

```javascript
document.getElementById("todo").innerHTML = "<b>Next Task:</b> " + decodeURIComponent(task);
```

## DOM Attacks

`innerHTML` function does not allow the use of `<script>` tags within it as a security feature. 

Instead, we can use payload such as:

```html
<img src="" onerror=alert(window.origin)>
<img src="" onerror=alert(document.cookie)>
```



