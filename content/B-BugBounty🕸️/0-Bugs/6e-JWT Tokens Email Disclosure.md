---
title: JWT Tokens Email Disclosure
draft: false
tags:
  - jwt
  - waybackurls
  - gau
  - anew
  - waymore
---
Let's see how to find emails using JWT tokens.

## Subdomain Enum

First, collect all subdomains.

Below, I used `assetfinder` to do it. 

```
┌──(carabiner1㉿carabiner)-[~]
└─$ assetfinder --subs-only carabinersec.com        
<SNIP>
```

## Wayback URLs
### waybackurls

First, use `go` to install [waybackurls](https://github.com/tomnomnom/waybackurls):

```
┌──(carabiner1㉿carabiner)-[~]
└─$ /usr/local/go/bin/go install github.com/tomnomnom/waybackurls@latest

go: downloading github.com/tomnomnom/waybackurls v0.1.0
```

> If go install is not working, try changing the proxy: `export GOPROXY=https://goproxy.io`

Next, download `anew`:

```    
┌──(carabiner1㉿carabiner)-[~]
└─$ /usr/local/go/bin/go install -v github.com/tomnomnom/anew@latest 
go: downloading github.com/tomnomnom/anew v0.1.1
github.com/tomnomnom/anew
```

Now you should have `anew` and `waybackurls` on your system:

```
┌──(carabiner1㉿carabiner)-[~/go/bin]
└─$ ls
anew  waybackurls
```

Below command will take the list of subdomains and retrieve historical URLs from the Wayback Machine associated with those subdomains using `waybackurls`, and then appends only the new, unique URLs to the file `wayback_urls`:

```
┌──(root㉿carabiner)-[/home/carabiner1/Documents/bbh/who]
└─# cat who-subs | /home/carabiner1/go/bin/waybackurls | /home/carabiner1/go/bin/anew wayback_urls
```

We can see that it collected 22033 URLs:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ wc -l wayback_urls     
22033 wayback_urls
```
### gau

We can achieve the same with [gau](https://github.com/lc/gau)

- `gau` is used to retrieve URLs that have been indexed by various online services (not just the Wayback Machine).

First, install it:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ /usr/local/go/bin/go install github.com/lc/gau/v2/cmd/gau@latest

go: downloading github.com/lc/gau/v2 v2.2.3
go: downloading github.com/lc/gau v1.2.0
go: downloading github.com/sirupsen/logrus v1.8.1
```

Collect historical URLs:

```
┌──(root㉿carabiner)-[/home/carabiner1/Documents/bbh/who]
└─# cat who-subs | /home/carabiner1/go/bin/gau | /home/carabiner1/go/bin/anew gau_urls
```

`gau` collected 13026 lines:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ wc -l gau_urls                  
13026 gau_urls
```

### waymore

We can achieve the same with [waymore](https://github.com/xnl-h4ck3r/waymore).

First, install it:

```
pip install waymore
```

Collect historical URLs:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ cat who-subs | ~/.local/bin/waymore | sudo ~/go/bin/anew waymore_urls
IMPORTANT: It looks like you may be passing a subdomain. If you want ALL subs for a domain, then pass the domain only. It will be a LOT quicker, and you won't miss anything. NEVER pass a file of subdomains if you want everything, just the domains.

WARNING: Cannot find file "config.yml", so using default values
```

`waymore` collected 39992 historical URLs:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ wc -l waymore_urls              
39992 waymore_urls
```
## Search for JWT Tokens

Now, go through the historical URLs downloaded and look for JWT Tokens:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ cat *urls | grep "=eyJ"
https://<SNIP>?mkt_tok=eyJpIjoiT0RKaE9XWmpaak0xTUROaSIsInQiOiJ1SEhjUU9ZTE9Ia3VBUU9LbEo2d2Y0VW5nektJc2o4bTZXVkQzQnpub21hdmg0R005eENjTFFMeHh0b2NPMU1pV3dKV1JyTVFnR2hudEhldXRCRUdCeVVEK2Nob2VxTjNnVnZCK3ZzVGd5VkxtREtCQnZPZUUwakJvdUE0NUI1ZiJ9
https://<SNIP>?mkt_tok=eyJpIjoiWWpsbU1tRTVPV0prWW1NeiIsInQiOiJGTXNuUFBVeFVUdmJMc0ZVK0dySFprUkxQQ0MxTDFHdEx0ZU83SFJDa1hRbzVmNkptXC9UWGpMaEFDWVZySjZudjdONkU0R3ptUWR3b0p0UG1mS3BQWUhKVkN5YUxkYnM1UnNrNkRHRys0OHFjdk5hQzJ0NmxmYjhJc0lCXC9hWnhsIn0=
https://<SNIP>?mkt_tok=eyJpIjoiWWpsbU1tRTVPV0prWW1NeiIsInQiOiJGTXNuUFBVeFVUdmJMc0ZVK0dySFprUkxQQ0MxTDFHdEx0ZU83SFJDa1hRbzVmNkptXC9UWGpMaEFDWVZySjZudjdONkU0R3ptUWR3b0p0UG1mS3BQWUhKVkN5YUxkYnM1UnNrNkRHRys0OHFjdk5hQzJ0NmxmYjhJc0lCXC9hWnhsIn0=
https://<SNIP>?mkt_tok=eyJpIjoiT0RKaE9XWmpaak0xTUROaSIsInQiOiJ1SEhjUU9ZTE9Ia3VBUU9LbEo2d2Y0VW5nektJc2o4bTZXVkQzQnpub21hdmg0R005eENjTFFMeHh0b2NPMU1pV3dKV1JyTVFnR2hudEhldXRCRUdCeVVEK2Nob2VxTjNnVnZCK3ZzVGd5VkxtREtCQnZPZUUwakJvdUE0NUI1ZiJ9
```

We can also look for info such as email addresses:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who]
└─$ cat *urls | grep "gmail.com"
```
## jwt.io

After we collect JWT tokens, we can decode it to see the content of it using [jwt.io](https://jwt.io)

If we can see sensitive information such as below, it is a valid bug. Report it.

```
{
  "sub": "1234567890",
  "name": "John Doe",
  "iat": 1516239022
}
```
## References
- https://medium.com/@musab_alharany/10-ways-to-exploit-json-web-token-jwt-ac5f4efbc41b


