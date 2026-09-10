


```bash
./reconftw.sh -d unomaha.edu -s -o ~/reconftw/Recon/unomaha.edu
```

<img width="1574" height="791" alt="image" src="https://github.com/user-attachments/assets/fa87e7a0-7ae9-4549-96c2-1ef44a9e11f3" />


Using `tree -h .`

<img width="679" height="681" alt="image" src="https://github.com/user-attachments/assets/fd454b7a-dffa-4929-a3cd-743306e732ab" />

Now rending for live hosts: 

```bash
httpx -l subdomains/subdomains.txt -p 80,443,8000,8080,8443,9000,9443 -status-code -title -o webs/webs_uncommon_ports.txt
```


# Another way

chaos -d target.com -o chaos.txtsubfinder -d target.com -all -recursive -o passive.txtcurl -s "https://crt.sh/?q=%.target.com&output=json" | jq -r '.[].name_value' | sed 's/\*\.//g' >> passive.txt

cat chaos.txt passive.txt | sort -u | dnsx -a -cname -resp -silent -o resolved.txt

cat resolved.txt | dnsgen - | puredns resolve --resolvers resolvers.txt --write permuted.txt

httpx -l resolved.txt -silent | getJS --complete | grep -oE '([a-zA-Z0-9-]+\.)+target\.com'

ffuf -w wordlist.txt -H "Host: FUZZ.target.com" -u https://<shared-ip>/ -mc 200,301,302,403
