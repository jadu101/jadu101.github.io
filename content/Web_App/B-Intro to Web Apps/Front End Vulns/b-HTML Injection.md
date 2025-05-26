---
title: b-HTML Injection
draft: false
tags:
---
Some user input doesn't make it to the back end and in some cases they are completely processed and rendered on the front end. 

**HTML Injection** - Unfiltered user input is displayed on the page.

This can be done through retrieving previously submitted code, like retrieving a user comment from the back end database, or directly displaying unfiltered user input through JavaScript on the front end. 

Examples are listed below:

- Inject malicious HTML code that will trick users into logging in while actually sending their credentials to malicious server.
- Inject new HTML code to change the web page's appearance, insert malicious ads, etc. 

## Example

```html
<script>
        function inputFunction() {
            var input = prompt("Please enter your name", "");

            if (input != null) {
                document.getElementById("output").innerHTML = "Your name is " + input;
            }
        }
    </script>
```


Page takes user input and directly displays it without any validation.

```html
<style> body { background-image: url('https://academy.hackthebox.com/images/logo.svg'); } </style>
```

Injecting sample HTML code, we will have the web page's background image changed.


