---
title: 1-Find Seeds
draft: false
tags:
---

# Finding Seeds
## Basic Flow

Before getting started with Subdomain enumeration, we have to have as much seeds as possible to expand our attack surface.

We can achieve this through:

- Acquisition Recon - Crunchbase
- ASN Enumeration - Hurricane Electric Internet Service
- BuiltWith - Relationship Profile
- Google Dork - Copy right information

## Acquisition Recon
## WhoisXML

`https://tools.whoisxmlapi.com/reverse-whois-search` -> Not very accurate.

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241112115018.png" alt="alt text" width="500" />





### crunchbase

Let's search up `World Health Organization` on Crunchbase:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029203901.png" alt="alt text" width="500" />


Probably because this is non-profit organization, I don't see any acquisitions:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029204118.png" alt="alt text" width="500" />


For other companies such as Tesla, there is acquisition.

Below, we can see 10 of them:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029204058.png" alt="alt text" width="500" />


Click on it to list them:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029204534.png" alt="alt text" width="500" />


Click on each of the acquisition one by one manually and add them to the scope.

For the example acquisition below, I will be adding `www.wifierion.com` to the scope:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029204605.png" alt="alt text" width="500" />



After finishing this step, we should have list of acquisitions ready.

----
## ASN Enumeration

Now let's move on to ASN enumeration.

Through ASN enumeration, we can obtain scope of IP addresses that belongs to the target.

We will use [https://bgp.he.net/](https://bgp.he.net/) for enumeration.

Our search result shows lot of results and we have to guess which ones should be the correct ones:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029205208.png" alt="alt text" width="500" />


Since `AS28810` shows the country as Switzerland, this seems to the correct one. I am not sure about the other ones. 

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029205409.png" alt="alt text" width="500" />


Take a note of the ASN number and the IP address range:

```
158.232.0.0/16
195.190.29.0/24
```

----
## Builtwith

Let's go to builtwith.com and check on companies related to `who.int`.

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029205648.png" alt="alt text" width="500" />


Going to `Relationship` tab, we can see bunch of Google Tags that is being used in `who.int`:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029205728.png" alt="alt text" width="500" />


I see some subdomains such as `data.euro.who.int` on the list. 

Take a note on this and add it to the scope since subdomain enumeration might not detect it:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot from 2024-10-29 20-59-54.png" alt="alt text" width="500" />


I had a quick check on `data.euro.who.int` and it got me redirected to `cisid.euro.who.int` and page seemed juicy at first glance.

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029210301.png" alt="alt text" width="500" />


Take note of interesting assets:

```
https://whopartnership.com/
https://heat-health.org/
http://searo.who.int/
euro.who.int
data.euro.who.int
who-dev2-prgsdev-com
who-dev1-prgsdev-com
https://hlh.who.int/
open-who.int
```

> During my subdomain enumeration, they didn't catch this subdomain so it proves that we should always check on builtwith.com relationships. 

----
## Favicon Hash

What is favicon hash?

> favicon hash is a unique identifier generated from the favicon of a website. It is used to represent a website and usually displayed in the browser's address bar or tab.

### Find Favicon

When visiting the target website, open developer tool and go to network tab.

`favicon.ico` should be there.

e.g

```
https://s.yimg.com/rz/l/favicon.ico
```

Copy the URL for later use.

### Calculate Favicon Hash

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

### Search Asset
#### Shodan

On Shodan, search for the hash:

`http.favicon.hash:1523284567`

Click on any of the IP in the result and we will be able to see all subdomains and ASNs.

Check all the IPs one by one and hope on finding some of the hidden subdomains, IPs, and ports. 

Now let's copy all the discovered assets and run `nuclei` on it. 


To copy results from shodan as a text:

1. Click on more on the right corner down:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/enumeration/favicon-hash/s1.png)

2. Choose IP:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/enumeration/favicon-hash/s2.png)

3. Copy


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/enumeration/favicon-hash/s3.png)


### Automation

Tools that will help with finding favicon and find assets:

> These scripts will help you find favicon hashes which you can use in shodan to widened your attack surface

- https://github.com/Mr-P-D/Favicon-Hash-For-Shodan.io 
- https://github.com/devanshbatham/FavFreak
- https://github.com/edoardottt/favirecon
- https://github.com/eremit4/favihunter

---
## Google Dork
### CopyRight

Lastly, do Copyright dorking on Google as such:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029210540.png" alt="alt text" width="500" />



It didn't find nothing new, but it did found some juicy webpages as below:

`https://apps.who.int/healthinfo/systems/surveydata/index.php/auth/forgot_password`

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029210555.png" alt="alt text" width="600" />


## Result

On `seeds.txt`, I saved all the seeds discovered from `who.int`:

<img src="https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241029232547.png" alt="alt text" width="300" />



