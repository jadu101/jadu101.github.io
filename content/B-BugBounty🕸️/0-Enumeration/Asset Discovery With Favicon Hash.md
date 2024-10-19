---
title: Asset Discovery With Favicon Hash
draft: false
tags:
  - favicon
  - shodan
---
What is favicon hash?

> favicon hash is a unique identifier generated from the favicon of a website. It is used to represent a website and usually displayed in the browser's address bar or tab.

## Find Favicon

When visiting the target website, open developer tool and go to network tab.

`favicon.ico` should be there.

e.g

```
https://s.yimg.com/rz/l/favicon.ico
```

Copy the URL for later use.

## Calculate Favicon Hash

Use the script below to calculate the favicon hash:

```python
import mmh3 
import requests 
import codecs

response = requests.get('https://in.search.yahoo.com/favicon.ico')
favicon = codecs.encode(response.content,"base64") 
hash = mmh3.hash(favicon) 
print(hash)
```

Above script will give result such as `1623284567`

## Search Asset
### Shodan

On Shodan, search for the hash:

`http.favicon.hash:1523284567`

Click on any of the IP in the result and we will be able to see all subdomains and ASNs.

Check all the IPs one by one and hope on finding some of the hidden subdomains, IPs, and ports. 

Now let's copy all the discovered assets and run `nuclei` on it. 


To copy results from shodan as a text:

1. Click on more on the right corner down:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/enumeration/favicon-hash/s1.png)

2. Choose IP:

![[../../../Images/bbh/enumeration/favicon-hash/s2.png]]

3. Copy

![[../../../Images/bbh/enumeration/favicon-hash/s3.png]]


## Automation

Tools that will help with finding favicon and find assets:

> These scripts will help you find favicon hashes which you can use in shodan to widened your attack surface

- https://github.com/Mr-P-D/Favicon-Hash-For-Shodan.io 
- https://github.com/devanshbatham/FavFreak
- https://github.com/edoardottt/favirecon
- https://github.com/eremit4/favihunter


