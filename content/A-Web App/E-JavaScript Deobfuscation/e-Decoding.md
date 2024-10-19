---
title: e-Decoding
draft: false
tags:
  - decoding
  - xxd
---
## Base64

**Base64** usually used to reduce the use of special characters.

The length of Base64 hash to be in a multiple of **4**.

If the resulting output is only 3 characters long, for example, an extra `=` is added as padding, and so on.

```shell-session
jadu101@htb[/htb]$ echo aHR0cHM6Ly93d3cuaGFja3RoZWJveC5ldS8K | base64 -d

https://www.hackthebox.eu/
```

## Hex

**Hex** encodes each character into its hex order in the ASCII table. 

For example, `a` is `61` in hex, `b` is `62`, `c` is `63`, and so on.

If the hash is consisted of `0-9` and `a-f` then it could be hex encoded. 

To encode in hex:

```shell-session
jadu101@htb[/htb]$ echo https://www.hackthebox.eu/ | xxd -p

68747470733a2f2f7777772e6861636b746865626f782e65752f0a
```

To decode in hex:

```shell-session
jadu101@htb[/htb]$ echo 68747470733a2f2f7777772e6861636b746865626f782e65752f0a | xxd -p -r

https://www.hackthebox.eu/
```


## Caesar/Rot13

Caesar cipher shifts each letter by a fixed number.

 Most common of which is `rot13`, which shifts each character 13 times forward.

To rot13 encode:

```shell-session
jadu101@htb[/htb]$ echo https://www.hackthebox.eu/ | tr 'A-Za-z' 'N-ZA-Mn-za-m'

uggcf://jjj.unpxgurobk.rh/
```

To rot13 decode:

```shell-session
jadu101@htb[/htb]$ echo uggcf://jjj.unpxgurobk.rh/ | tr 'A-Za-z' 'N-ZA-Mn-za-m'

https://www.hackthebox.eu/
```

Online tool is available such as [rot13](https://rot13.com/).

## Online Tool

 [Cipher Identifier](https://www.boxentriq.com/code-breaking/cipher-identifier).

