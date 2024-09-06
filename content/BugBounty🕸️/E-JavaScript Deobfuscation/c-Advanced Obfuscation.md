---
title: c-Advanced Obfuscation
draft: false
tags:
---
From earlier's `Basic Obfuscation`, the code still contains strings in cleartext, which may reveal its original functionality.

## Obfuscator

Let's use  [https://obfuscator.io](https://obfuscator.io/) to perform a better obfuscation. 

Go to  [https://obfuscator.io](https://obfuscator.io/) and change `String Array Encoding` to `Base64`.

Let's try obfuscating the code below:

```javascript
console.log('CarabinerSecurity Rocks');
```

Code above is obfuscated as the code below:

```javascript
function _0x1638(){var _0x442f3f=['odG1ntjYswzny2i','q2fYywjPBMvYu2vJDxjPDhKGuM9JA3m','oduYnJG0DuXiyufn','mJGZmJG2n3bVrNj4vG','nJK4mtaWmgXQwwPKAW','mJCWnZu4oe5brM5Iyq','mJyZmMv3svH0wa','mta5mZm5ogPMEePMzG','mJaWmty0ogney1bwyG'];_0x1638=function(){return _0x442f3f;};return _0x1638();}function _0x224f(_0x47f66c,_0x50e070){var _0x163843=_0x1638();return _0x224f=function(_0x224f29,_0x4033bd){_0x224f29=_0x224f29-0x121;var _0x2ea47f=_0x163843[_0x224f29];if(_0x224f['PQxtfT']===undefined){var _0x5d93fc=function(_0x408347){var _0x5476c0='abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/=';var _0x3e0730='',_0x4f72df='';for(var _0x46748c=0x0,_0x5ea5f9,_0x3ae8b8,_0xc91609=0x0;_0x3ae8b8=_0x408347['charAt'](_0xc91609++);~_0x3ae8b8&&(_0x5ea5f9=_0x46748c%0x4?_0x5ea5f9*0x40+_0x3ae8b8:_0x3ae8b8,_0x46748c++%0x4)?_0x3e0730+=String['fromCharCode'](0xff&_0x5ea5f9>>(-0x2*_0x46748c&0x6)):0x0){_0x3ae8b8=_0x5476c0['indexOf'](_0x3ae8b8);}for(var _0xe3c238=0x0,_0xb09fea=_0x3e0730['length'];_0xe3c238<_0xb09fea;_0xe3c238++){_0x4f72df+='%'+('00'+_0x3e0730['charCodeAt'](_0xe3c238)['toString'](0x10))['slice'](-0x2);}return decodeURIComponent(_0x4f72df);};_0x224f['kVHUhW']=_0x5d93fc,_0x47f66c=arguments,_0x224f['PQxtfT']=!![];}var _0x1a8f83=_0x163843[0x0],_0x429ddf=_0x224f29+_0x1a8f83,_0x439c35=_0x47f66c[_0x429ddf];return!_0x439c35?(_0x2ea47f=_0x224f['kVHUhW'](_0x2ea47f),_0x47f66c[_0x429ddf]=_0x2ea47f):_0x2ea47f=_0x439c35,_0x2ea47f;},_0x224f(_0x47f66c,_0x50e070);}var _0x3876fb=_0x224f;(function(_0x2cf212,_0x1388e3){var _0x395173=_0x224f,_0x492665=_0x2cf212();while(!![]){try{var _0x575128=parseInt(_0x395173(0x125))/0x1+parseInt(_0x395173(0x129))/0x2+parseInt(_0x395173(0x121))/0x3+parseInt(_0x395173(0x123))/0x4+parseInt(_0x395173(0x122))/0x5+parseInt(_0x395173(0x126))/0x6+-parseInt(_0x395173(0x124))/0x7*(parseInt(_0x395173(0x127))/0x8);if(_0x575128===_0x1388e3)break;else _0x492665['push'](_0x492665['shift']());}catch(_0x24d143){_0x492665['push'](_0x492665['shift']());}}}(_0x1638,0xad0b6),console['log'](_0x3876fb(0x128)));
```

Now the code is even harder to read!

## More Obfuscation


Let's use the following obfuscation tools to obfuscate the code below:

```javascript
console.log('CarabinerSecurity Rocks');
```


> Note that below obfuscators usually make code execution/compliation very slow so it is not recommended to be used unless for obvious reason like bypassing web filters or restrictions. 
### JSF

We can use  [JSF](http://www.jsfuck.com/) to further obfuscate.

```
[][(![]+[])[+[]]+(![]+[])[!+[]+!+[]]+(![]+[])[+!+[]]+(!![]+[])[+[]]][([]<SNIP>
```

### JJ Encode

[JJ Encode](https://utf-8.jp/public/jjencode.html)

```
$=~[];$={___:++$,$$$$:(![]+"")[$],__$:++$,$_$_:(![]+"")[$],_$_:++$,$_$$:({}+"")[$],$$_$:($[$]+"")[$],_$$:++$,$$$_:(!""+"")
<SNIP>
```


### AA Encode

[AA Encode](https://utf-8.jp/public/aaencode.html)

```
ﾟωﾟﾉ= /｀ｍ´）ﾉ ~┻━┻   //*´∇｀*/ ['_']; o=(ﾟｰﾟ)  =_=3; c=(ﾟΘﾟ) =(ﾟｰﾟ)-(ﾟｰﾟ); (ﾟДﾟ) =(ﾟΘﾟ)= (o^_^o)/ (o^_^o);(ﾟДﾟ)={ﾟΘﾟ: '_' ,ﾟωﾟﾉ : 
```