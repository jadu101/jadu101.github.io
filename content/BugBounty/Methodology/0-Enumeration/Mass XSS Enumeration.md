---
title: Mass XSS Enumeration
draft: false
tags:
  - xss
  - mass-xss
  - httpx
  - amass
  - assetfinder
  - subdomainator
  - sublist3r
  - crt-sh
  - subfinder
---
## Choose Program

First, find bug bounty program using Google Dorking. It is better to not use programs from H1 or bugcrowd because they are too competitive. `inurl /bug bounty`

## Subdomain Enumeration

Use different methods to get a list of subdomains:

- Subdominiator
`sudo subdominator -d who.int -o who_r1.txt`

- assetfinder
`assetfinder --subs-only who.int`

- crt.sh
`curl -s "https://crt.sh/?q=who.int&output=json" | jq -r '.[].name_value' | sed 's/\*\.//g' | sort -u | grep -Po '(\w+\.\w+\.\w+)$' | sudo ~/go/bin/anew crt-who.int`

- sublist3r
`python3 sublist3r.py -d who.int -v -t 5 -o who-int-subliste3r.txt`

- amass
`amass enum -d evil.com -o evil_amass.txt`

`amass enum -active -d evil.com -o evil.active_scan.txt`
``

Let's check how many subs we have in total:

`cat * | wc -l`

Now, we will sort only the unique ones:

`cat * | sort -u | tee sorted_subs.txt`

Lastly, we will recursive subdomain enumerate on them once again:

`subfinder -dL sorted_subs.txt -all -recursive -o evil_sbfdr_all.txt`

Now that we have full list of subdomains, we will use `httpx` to check for alive domains:

`cat evil_sbfdr_all.txt | httpx -ports 80,443,8009,8080,8081,8090,8180,8443 -sc -cl -title -t 100 -fr -nc | tee evil.alive.txt`

## Find URLs

Let's find URLs. 

Using waybackurls:

```
cat evil.alive.txt | waybackurls | tee wbk.urls.txt
```

Using gau:

```
cat evil.alive.txt | gau -o gau_urls.txt
```

Using Katana:

```
cat evil.alive.txt |  ~/go/bin/katana -d 5 -ps -pss waybackarchive,commoncrawl,alienvault | tee ktna_urls.txt
```

## Find XSS Parameters



