---
title: 5-Burp Suite Set Up
draft: false
tags:
---
Before getting started with endpoints discovery, let's set up Burp Suite properly so that our endpoints scan can go through Burp Suite and during so, we can automatically look for bugs. 

I use the following extensions
- CORS
- SSRF Everywhere
- 403 Bypasser
- Param Miner

## Settings
### Out of Scope

```
^(firefox\.com|google\.com|mozilla\.com|mozilla\.org)$
```
<img width="1127" height="551" alt="image" src="https://github.com/user-attachments/assets/e0db6d7a-916a-49a6-934c-43950017c328" />

Check `is in the scope`:

<img width="764" height="339" alt="image" src="https://github.com/user-attachments/assets/42dc71f6-a20a-48fe-ac26-e3960e55e4f4" />


### REST API

First, check on `service running` and `Allow access without API Keys`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241106151850.png)
### Tasks

Let's create a new resource pool as below:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241106152009.png)

## Setting Scope

Filter setting - show only in-scope items

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241106152408.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241106153850.png)

Use advanced scope control to use regex

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241106154015.png)





# Burp Pro Extensions

Now let's look at some Burp Suite pro extensions.

We can use `burp bounty` and `autorepeater` to hunt for XSS and SSRF.

**This step should come before `Find Enpoints` and so that as `katana` searches for endpoints, it goes through Burp Suite and Burp Suite extensions hunts for XSS and SSRF**

## Blind XSS
### Burp Bounty

Create a profile as below:

- Payload Position: Append
- Insertion point type: All

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-11-04%2014-09-23.png)

Name issue so we can later see it from the dashboard:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-11-04%2014-09-41.png)

### Payloads

https://github.com/jadu101/blind_xss_payload_generator/blob/main/collaborator_blind_xss_payloads.txt


- https://github.com/lauritzh/blind-xss-payloads
- https://github.com/rocketscientist911/webpayloads/blob/master/blindxsspayloads.txt

Above two payloads were used to create [blind xss payload generator](https://github.com/jadu101/blind_xss_payload_generator).

Run the bash script and input your Burp Collaborator URL:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-11-04%2014-29-17.png)

201 blind XSS payloads have been created:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241104142957.png)

Now load it to the payload section of the profile:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241104143023.png)

Now, as we freely go through the website, blind XSS payload is going to be tested. 

### Scan

We can run scan as well to make sure we go through all endpoints. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241104145158.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-11-04%2014-52-22.png)


# AutoRepeater
## SSRF
 - https://xelkomy.medium.com/found-ssrf-and-lfi-in-just-10-minutes-of-using-burp-492fddef3f3e

On `AutoRepeater`, create following regex and set the value as the picture below:

```
https?:\/\/\S+
```



![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241110155046.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241110155104.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241110155116.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241110155135.png)



The Above regex is what I have used to get the SSRF, with those steps like below.

 Later, when we use tools such as Katana by projectdiscvery and use the proxy option and add the burp proxy URL by the default, it will be http://127.0.0.1:8080

4 — Open the subdomain manually by the browser and visit every login page and any page you have seen on the target website you have.

5 — Open the burp again and you will see the catch by the burp suite auto repeater extension, if there are any parameters to take an input like `[http://google.com](http://google.com/)` it will change to your collab URL you have been replaced with in the autorepeater and if that no response send to your collab you will see that the autorepeater have saved the tested URL in it until you close the burp suite.

After there is a hit, try escalating to LFI.


## Autorize

> Autorize — For Each Request you do, it will send an equal request But with changed cookies of the session or any additional header used for authorization.


**USER A — Admin**

**USER B — Normal User**

Browse the Web App with **User A** and Add the cookie of **User B** in the autorize so that it can browse with the User B account automatically.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241105135816.png)



Turn ON the Autorize and surf the WebApp with User A and requests with start flowing through autorize

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241105135835.png)

> Just browse through the WebApp and try each and every Functionality which has Admin privileges and can't be accessed by Normal User, if you get 200 Ok on that request it's an IDOR Vulnerability. So Auditing the Request is upto you.

