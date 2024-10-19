---
title: a-My Methodology
draft: false
tags:
  - bbh
---
My bug bounty methodology and how I approach a target.

## Choose Target
### Program Launch Date

It is better to go for programs which are relatively young. 

Old programs are more likely the most of the easy bugs would have already been found unless there are new assets.

### Program Responsiveness

If the program is taking too long to resolve security issue, this mean it's more likely to get a duplicate. 

Below are shorter the better:

- Time to first response
- Time to tirage

You can also see the percentage of the reports which have met those response metrics. If it is above 90%, I’d probably accept the invitation if the rest of the metrics is ok.

### Program Scope

Beginners should prefer bigger scopes such as wildcard domains over a single web application.

### Bug Bounty Rewards

BBP provides rewards while VDP doesn't. 

For beginners who is just pursuing to learn, not get rewarded, VDP might be better since it is less competitive.


## Enumeration
### Subdomain Enumeration
#### assetfinder

I like to start subdomain enumeration with Tomnomnom’s [](https://github.com/tomnomnom/assetfinder)[assetfinder](https://github.com/tomnomnom/assetfinder) tool. 

`assetfinder` is very fast and accurate.

```
assetfinder --subs-only domain.name
```

I usually avoid bruteforcing since it takes too much time.

### Individual Assets
#### httpprobe

ow that we have identified list of assets, let's filter them using Tomnomnom’s [httprobe](https://github.com/tomnomnom/httprobe).

I'm usually interested only with port 80 and 443:

```
cat domains | httprobe
```

If the program is new, I can use `Shodan` or use `rustscan` to perform a portscan to see if the web app is using any non-standard open ports.

#### aquatone

I run [aquatone](https://github.com/michenriksen/aquatone) to screenshot the list of live web applications. 

We can have a good overview of different web application categories and technologies.

### Choose Target

By this point, we will have some web applications to choose from.

I like to choose a target that is a little different from others. 

> For example, if all web applications implement a centralized Single Sign-on authentication mechanism, I would look for any directly accessible asset.

If I spot something familiar, such as user interface (monitoring tool, CMS), I would target them first. 

Also, if the web application is disclosing the name and the version of the software being used, I search for exploits online and get a issue bug.

For the other custom-made web applications, I will generally choose the one whose user interface deviates from the common company’s theme. If I don’t find one, I might repeat my previous steps with deeper enumeration. For instance, I would take the subdomains I found earlier and combine them with the name of the company to generate a custom wordlist. Then, I’d use tools like [OWASP amass](https://github.com/OWASP/Amass) and brute force the subdomains using the wordlist I constructed.

## Mapping

Now that we have chose what web application to target, let's actually engage with it. 

### Look Around

Open up a web browser and use the web app as the normal user. 

Try signup feature, click on every link, visit every tab, fill up every form. 

Try your best to understand the business features and make note of interesting ones. 

### Understand Architecture
  
This is where I revise my Burp traffic to answer specific questions.

How is authentication working? OAuth flow? CSRF protection? IDOR? SQLi? API? Framework?

Have a clear idea of architecture and the defense mechanism.

### JavaScript Enumeration

JS files power the client-side of the web application and we may find hidden endpoints, cross-site scripting, and broken access control vulnerability from it.






## References 
- https://thehackerish.com/my-bug-bounty-methodology-and-how-i-approach-a-target/
- https://thehackerish.com/the-top-9-bug-bounty-resources-to-stay-up-to-date/
