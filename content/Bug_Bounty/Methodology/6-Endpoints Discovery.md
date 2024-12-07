---
title: 6-Endpoints Discovery
draft: false
tags:
---
## Finding URLs

Let's find URLs from live hosts using `waybackurls`, `gau`, and `katana`.

We will feed the find URLs to automated tools such as `gf` to test for XSS and stuff. 

### waybackurls

`httprobe_result.txt` contains the list of live hosts that is verified through `httprobe`.

 I feed this to `waybackurls` to find URLs:

```
cat subs.txt | /home/carabiner1/go/bin/waybackurls| sudo /home/carabiner1/go/bin/anew waybackurls.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031165033.png)

`waybackurls` found 13543 URLs:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot from 2024-10-31 16-47-49.png)

### gau

Now let's use `gau` to collect more URLs:

```
cat subs.txt | /home/carabiner1/go/bin/gau| /home/carabiner1/go/bin/anew gau.txt 
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031170041.png)

`gau` somehow just froze. Some people say only using `katana` is also fine.

### katana

> Based on my experience, `katana` is the fastest. 

**Make sure before doing this Burp Bounty and AutoRepeater is set up for SSRF and Blind XSS!**

Now let's use `katana` to collect more URLs:

```
while read subdomain; do /home/carabiner1/go/bin/katana -proxy http://127.0.0.1:8080 -jc -aff -u "$subdomain" | sudo /home/carabiner1/go/bin/anew katana.txt; done < subs.txt
```

> I am also having katana go through the burp proxy so that I can log more information on Burp Suite Targets. This also eases on testing SSRF and XSS automatically. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031170511.png)

I stopped it running in the middle of the scan and yet it still discovered 50003 URLs:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031210342.png)

## Sort It

Let's sort for unique URLs:

```
cat *.txt | sort -u | sudo tee all_urls.txt
```

Now we have 63528 unique URLs to be tested:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031210522.png)

