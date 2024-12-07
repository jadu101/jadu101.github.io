---
title: f-XSS Prevention
draft: false
tags:
  - xss-prevention
---
XSS vulnerability is mainly linked to two parts of the web app:

- `Source` - such as user input field
- `Sink` - displays the input data

## Front-end

Sanitize and validate the user input on the front-end using JavaScript.

### Input Validation

e.g email field won't accept input if the email format is invalid:

```javascript
function validateEmail(email) {
    const re = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
    return re.test($("#login input[name=email]").val());
}
```

### Input Sanitization

We should always ensure that we do not allow any input with JavaScript code in it.

We can utilize the [DOMPurify](https://github.com/cure53/DOMPurify) JavaScript library, as follows:

```javascript
<script type="text/javascript" src="dist/purify.min.js"></script>
let clean = DOMPurify.sanitize( dirty );
```

This will escape any special characters with a `\`. 

### Direct Input

Never let user input directly with certain HTML tags:

1. JavaScript code `<script></script>`
2. CSS Style Code `<style></style>`
3. Tag/Attribute Fields `<div name='INPUT'></div>`
4. HTML Comments `<!-- -->`

We should avoid using JavaScript functions that allow changing raw text of HTML fields like:

- `DOM.innerHTML`
- `DOM.outerHTML`
- `document.write()`
- `document.writeln()`
- `document.domain`

And the following jQuery functions:

- `html()`
- `parseHTML()`
- `add()`
- `append()`
- `prepend()`
- `after()`
- `insertAfter()`
- `before()`
- `insertBefore()`
- `replaceAll()`
- `replaceWith()`
## Back-end
### Input Validation


An example of E-Mail validation on a PHP back-end is the following:

```php
if (filter_var($_GET['email'], FILTER_VALIDATE_EMAIL)) {
    // do task
} else {
    // reject input - do not display it
}
```

### Input Sanitization

For example, for a PHP back-end, we can use the `addslashes` function to sanitize user input by escaping special characters with a backslash:

```php
addslashes($_GET['email'])
```

For a NodeJS back-end, we can also use the [DOMPurify](https://github.com/cure53/DOMPurify) library as we did with the front-end, as follows:

```javascript
import DOMPurify from 'dompurify';
var clean = DOMPurify.sanitize(dirty);
```

### Output HTML Encoding

We have to encode any special characters into their HTML codes, which is helpful if we need to display the entire user input without introducing XSS vulnerability.

