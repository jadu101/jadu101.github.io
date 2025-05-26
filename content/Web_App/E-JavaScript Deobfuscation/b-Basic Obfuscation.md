---
title: b-Basic Obfuscation
draft: false
tags:
  - packer
---
We first have to understand how code is obfuscated.

## Obfuscation

Obfuscation is used to make a script more difficult to read by humans but allows it so function the same. 

This is usually done through automatic obfuscation tool. 

## Use Case

One common reason on obfuscating code it to prevent it from getting reused or copied without the developer's permission. 

Another reason is to make the code more secure when dealing with authentication or encryption.

Attackers often obfuscate malicious scripts to prevent intrusion detection and prevention systems from detecting their scripts.

## Basic Obfuscation

Let's try obfuscating JavaScript code the below:

```javascript
console.log('CarabinerSecurity Rocks');
```

We should first check on how the code will be printed using [JSConsole](https://jsconsole.com/).

We can see that the code above prints as:

```
CarabinerSecurity Rocks
```

### Minification

Common way of reducing the readability of a snippet of JS code while keeping it's full functionality is JavaScript minification.

We can have entire code in a single line. 

We can use tools such as [javascript-minifier](https://javascript-minifier.com/) to minify out JavaScript code. 

Usually, minified JavaScript code is saved with the extension `.min.js`.

### Packing JS Code

Now let's obfuscate our single line of code to make it more obscure and difficult to read. 

We can use [BeautifyTools](http://beautifytools.com/javascript-obfuscator.php) to obfuscate our code.

```javascript
console.log('CarabinerSecurity Rocks');
```

Above code will be obfuscated as such:

```javascript
eval(function(p,a,c,k,e,d){e=function(c){return c};if(!''.replace(/^/,String)){while(c--){d[c]=k[c]||c}k=[function(e){return d[e]}];e=function(){return'\\w+'};c=1};while(c--){if(k[c]){p=p.replace(new RegExp('\\b'+e(c)+'\\b','g'),k[c])}}return p}('0.1(\'2 3\');',4,4,'console|log|CarabinerSecurity|Rocks'.split('|'),0,{}))
```

`packer` obfuscation tool usually attempted to convert all words and symbols of the ode into a list or a dictionary and then refer them using the `(p, a, c, k, e, d)` function to rebuild the original code during execution. 

Although this does great job reducing code's readability, it can still reveal some of its functionality and this is why we need a better obfuscating way.

