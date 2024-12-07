---
title: 4-List Hosts
draft: false
tags:
---
# Live Hosts

I see lot of people using `httpx` using for identifying live hosts these days but I prefer `httprobe` since when using `httpx`, I see bunch of IP addresses that I don't know where it is coming from. `httpx` has advantage that it can look for different ports other than 80 and 443 but modern web applications would rarely have them open.

I usually first look for 403 pages and see if I can bypass it.

Then I check on other status codes such as 404, 400 and check for subdomain takeover.

After that, I check for installation pages such as Apache, IIS and run fuzzing on it. 
## httprobe

```
cat subs_final.txt | httprobe > httprobe_result.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-10-30%2016-31-58.png)

`httprobe` identified 1960 assets being live.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot%20from%202024-10-30%2016-43-42.png)
## httpx - Sort for By Status Code

```
while read -r url; do
    status_code=$(curl -o /dev/null -s -w "%{http_code}" "$url")
    echo "$url - $status_code"
done < httprobe_result.txt > status_codes.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241030172036.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241031110434.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241031110520.png)
## aquatone

We can use aquatone to take screenshots of the host. 