---
title: Subdomain Takeover
draft: false
tags:
---
Let's say your company is called `Carabiner Security` and you are using S3 bucket for your app's static files, called `static-stuff`. 

Obviously, you don't want the domain name for your bucket to be like `static-stuff.s3.amazonaws.com` so you register a CNAME (DNS Alias)) that says `static.carabinersec.com` is an alias for `static-stuff.s3.amazonaws.com`.

A few years pass and you forgot about old stuff you developed in the past so you deleted `static-stuff` S3 bucket. 

However, you forgot to delete the CNAME entry that connects `static.carabinersec.com` with `static-stuff.s3.amazonaws.com`.

Attacker can create a new S3 bucket and uses the `static-stuff`.

Since `static.carabinersec.com` is still an alias for `static-stuff.s3.amazonaws.com`, attacker now can take over `static.carabinersec.com` and control it's content.

## Exploitation

So the attacker now owns `static.carabinersec.com`. Now what?

### Phishing

Create phishing page and try social engineering.

### Exploit CORS Misconfigurations

Say there is `ai.carabinersec.com` that allow CORS requests from all subdomains of `*.carabinersec.com`.

Now that you control a subdomain of `carabinersec.com`, you can host content on `static.carabinersec.com` that sends cross-origin requests to `ai.carabinersec.com`and stores the HTTP responses.

You then social engineer a user of `ai.carabinersec.com` to visit `static.carabinersec.com`.

Now, you can then send requests in the user’s name to `ai.carabinersec.com`.

### Bypass CSP for XSS

Say you find XSS but can’t exploit it because of the CSP, but the **CSP trusts all scripts from subdomains of** `***.carabinersec.com**`.

You can see what’s next right? Just host the XSS payload on `static.carabinersec.com` then.

## How to Find?

How do we find subdomain takeover vulnerability?

### S1: Find Subdomains

Find subdomains:

mail.test.com  
static.test.com  
cool-app.test.com

### S2: Find CNAMES

It is unlikely we can get our hands on e.g `mail.test.com` unless we can hack the page itself or like the DNS server. 

This is why we have to check if there are some CNAMEs (aliases) for all identified subdomains.

```
dig +short CNAME mail.test.com
```


Let's say that we found a CNAME for `static.test.com`, and it’s an S3 bucket (`static-test.s3.amazonaws.com`).

### S3: Find Abandoned Domains

Next, we need to check if any of the identified CNAMEs have been abandoned, i.e. **no longer used**.

Good indicators are:

- 404 HTTP response status
- DNS errors that indicate that the domain is non-existent

Let's say we checked on identified `static-test.s3.amazonaws.com` and it doesn't exist anymore. 

### S4: Check Takeover

This is the hardest part. 

We can only take over `static.test.com` if we can register a new S3 bucket and control its domain name. 

If we can't do above, we won't be able to register `static-test.s3.amazonaws.com`. 

But how do we know if we can register a new resource with the same name?

https://github.com/EdOverflow/can-i-take-over-xyz


### S5: Subdomain Takeover


Once we confirm we can take it over, the final step is taking over the subdomain. This means for example creating a new **S3 bucket with the same name** as a previously deleted bucket.


