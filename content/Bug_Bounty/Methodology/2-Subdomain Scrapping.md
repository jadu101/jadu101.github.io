---
title: 2-Subdomain Scrapping
draft: false
tags:
---

# Subdomain Scrapping
## Sudomain Enumeration

Now that we have the seeds ready, let's get subdomains out of them.

We will use the following tools to do so:

- Subfinder
- Assetfinder
- crt.sh
- amass
- Subdominator

### Subfinder

I will first use `subfinder` to recursively go through all seed domains on the list and save it to `subfinder_recurse_on_seeds.txt` inside the `subs` folder:

```
subfinder -dL seeds.txt -all -recursive > subfinder.txt
subfinder -d example.com -all -recursive > subfinder.txt
```

- `-all`: Uses all available sources for subdomain discovery. Subfinder integrates with multiple data sources such as ThreatCrowd, VirusTotal, Censys, etc., and using `-all` ensures you're casting a wide net.

![[Pasted image 20241029233003.png]]

`subfinder` with recursive mode found 383 subdomains from `seeds.txt`:

![[Pasted image 20241029233559.png]]

### Assetfinder

`assetfinder` usually doesn't discover a lot but it sometimes discover unique ones so it is worth using it. 

Since `assetfinder` only takes one domain input at a time, I use following `while` loop that will read seed domain in `seeds.txt` line by line and feed it to `assetfinder`:

```
while read -r line; do assetfinder --subs-only "$line" >> assetfinder.txt; done < seeds.txt
assetfinder --subs-only example.com >> assetfinder.txt
```

![[Pasted image 20241029233626.png]]

`assetfinder` only found 18 subdomains from `seeds.txt`:

![[Screenshot from 2024-10-29 23-41-44.png]]

### Amass

Now let's use `amass` to find subdomains from `seeds.txt`:

```
sudo amass enum -df seeds.txt -o amass_default.txt
```


![[Pasted image 20241029235303.png]]

Default `amass` scan found 2509 subdomains from `seeds.txt`:

![[Pasted image 20241030012752.png]]

We will run active scan as well:

```
sudo amass enum -active -df seeds.txt -o amass_active.txt
```

![[Pasted image 20241030104929.png]]

We can see that active scan found much more:

![[Pasted image 20241030131045.png]]

To get a list of FQDNs, use [amass_beautifier.sh](https://github.com/Asbawy/amassbeautifier/blob/main/amassbeautifier.sh) and then:

```
amass enum -d example.com | grep -Eo '([a-zA-Z0-9.-])+\.com \(FQDN\)' | awk '{print $1}' | anew out.txt
```

To retrieve both subdomains and IP addresses, use:

```
amass enum -d example.com | tee >(grep -Eo '([a-zA-Z0-9.-])+\.com | awk '{print $1}') >(grep -Eo '([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)' | awk '{print $1}')
```
####  OAM-Tools

Tools: https://medium.com/@ZishanSec/amass-v4-x-x-a-comprehensive-step-by-step-usage-guide-29124cdd871a

With **oam_subs**, you can efficiently analyze and interpret the collected data to enhance your security assessments.

```
go install -v github.com/owasp-amass/oam-tools/cmd/oam_subs@master
```
## Result

Now we have four files containing subdomains from `amass`, `assetfinder`, and `subfinder`

![[Pasted image 20241030132355.png]]

I will use the command below to sort for unique assets only:

```
cat amass.txt subfinder.txt gobuster_subs.txt other.txt | anew all-subs.txt
```



