---
title: b-WSDL
draft: false
tags:
  - wsdl
---
WSDL stands for Web Service Description Language. WSDL is an XML-based file exposed by web services that informs clients of the provided services/methods, including where they reside and the method-calling convention.

WSDL shouldn't be publicly accessible.

However, through directory/parameter fuzzing, we may reveal the location and content of WSDL file.

## Exploitation

Suppose we are assessing a SOAP service residing in `http://<TARGET IP>:3002`. We have not been informed of a WSDL file.
### Identify WSDL

Let's start with basic directory fuzzing against the web service:

```shell-session
jadu101@htb[/htb]$ dirb http://<TARGET IP>:3002

-----------------
DIRB v2.22    
By The Dark Raver
-----------------

START_TIME: Fri Mar 25 11:53:09 2022
URL_BASE: http://<TARGET IP>:3002/
WORDLIST_FILES: /usr/share/dirb/wordlists/common.txt

-----------------

GENERATED WORDS: 4612                                                          

---- Scanning URL: http://<TARGET IP>:3002/ ----
+ http://<TARGET IP>:3002/wsdl (CODE:200|SIZE:0)                            
                                                                               
-----------------
END_TIME: Fri Mar 25 11:53:24 2022
DOWNLOADED: 4612 - FOUND: 1
```

It looks like `http://<TARGET IP>:3002/wsdl` exists. Let us inspect its content as follows.

```shell-session
jadu101@htb[/htb]$ curl http://<TARGET IP>:3002/wsdl 
```

Unfortunately, the response is empty.

Maybe there is a parameter that will provide us with access to the SOAP web serivce's WSDL file.

Let's fuzz parameter with `ffuf` and [burp-parameter-names.txt](https://github.com/danielmiessler/SecLists/blob/master/Discovery/Web-Content/burp-parameter-names.txt) list (`-fs 0_ filters out empty responses (size = 0) and _-mc 200_ matches _HTTP 200_ responses).

```shell-session
jadu101@htb[/htb]$ ffuf -w "/home/htb-acxxxxx/Desktop/Useful Repos/SecLists/Discovery/Web-Content/burp-parameter-names.txt" -u 'http://<TARGET IP>:3002/wsdl?FUZZ' -fs 0 -mc 200

        /'___\  /'___\           /'___\       
       /\ \__/ /\ \__/  __  __  /\ \__/       
       \ \ ,__\\ \ ,__\/\ \/\ \ \ \ ,__\      
        \ \ \_/ \ \ \_/\ \ \_\ \ \ \ \_/      
         \ \_\   \ \_\  \ \____/  \ \_\       
          \/_/    \/_/   \/___/    \/_/       

       v1.3.1 Kali Exclusive <3
________________________________________________

 :: Method           : GET
 :: URL              : http://<TARGET IP>:3002/wsdl?FUZZ
 :: Wordlist         : FUZZ: /home/htb-acxxxxx/Desktop/Useful Repos/SecLists/Discovery/Web-Content/burp-parameter-names.txt
 :: Follow redirects : false
 :: Calibration      : false
 :: Timeout          : 10
 :: Threads          : 40
 :: Matcher          : Response status: 200
 :: Filter           : Response size: 0
________________________________________________

:: Progress: [40/2588] :: Job [1/1] :: 0 req/sec :: Duration: [0:00:00] :: Error
:: Progress: [537/2588] :: Job [1/1] :: 0 req/sec :: Duration: [0:00:00] :: Erro
wsdl [Status: 200, Size: 4461, Words: 967, Lines: 186]
```

It looks like _wsdl_ is a valid parameter. Let us now issue a request for `http://<TARGET IP>:3002/wsdl?wsdl`

```shell-session
jadu101@htb[/htb]$ curl http://<TARGET IP>:3002/wsdl?wsdl 

<?xml version="1.0" encoding="UTF-8"?>
<wsdl:definitions targetNamespace="http://tempuri.org/"
	xmlns:s="http://www.w3.org/2001/XMLSchema"
	xmlns:soap12="http://schemas.xmlsoap.org/wsdl/soap12/"
	xmlns:http="http://schemas.xmlsoap.org/wsdl/http/"
	xmlns:mime="http://schemas.xmlsoap.org/wsdl/mime/"
<SNIP>
```

We successfully identified the SOAP service's WSDL file.

> **Note**: WSDL files can be found in many forms, such as `/example.wsdl`, `?wsdl`, `/example.disco`, `?disco` etc. [DISCO](https://docs.microsoft.com/en-us/archive/msdn-magazine/2002/february/xml-files-publishing-and-discovering-web-services-with-disco-and-uddi) is a Microsoft technology for publishing and discovering Web Services.

