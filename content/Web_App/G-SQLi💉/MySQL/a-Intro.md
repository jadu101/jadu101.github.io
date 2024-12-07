---
title: a-Intro
draft: false
tags:
  - mysql
---
SQL can be used to perform the following actions:

- Retrieve data
- Update data
- Delete data
- Create new tables and databases
- Add / remove users
- Assign permissions to these users

## Command Line

`mysql` utility can be used to interact with MySQL/MariaDB database.

`-u` - username

`-p` - password, should be passed empty so that password is prompted later.


### Create Database


```shell-session
mysql> CREATE DATABASE users;

Query OK, 1 row affected (0.02 sec)
```


To list databases and choose a database:

```shell-session
mysql> SHOW DATABASES;

+--------------------+
| Database           |
+--------------------+
| information_schema |
| mysql              |
| performance_schema |
| sys                |
| users              |
+--------------------+

mysql> USE users;

Database changed
```


### Tables

e.g Create a table named `logins` to store user data, using `CREATE TABLE` SQL query:

```sql
CREATE TABLE logins (
    id INT,
    username VARCHAR(100),
    password VARCHAR(100),
    date_of_joining DATETIME
    );
```


To list tables:

```shell-session
mysql> SHOW TABLES;

+-----------------+
| Tables_in_users |
+-----------------+
| logins          |
+-----------------+
1 row in set (0.00 sec)
```

`DESCRIBE`  - Used to list the table structure with its fields and data types:

```shell-session
mysql> DESCRIBE logins;

+-----------------+--------------+
| Field           | Type         |
+-----------------+--------------+
| id              | int          |
| username        | varchar(100) |
| password        | varchar(100) |
| date_of_joining | date         |
+-----------------+--------------+
4 rows in set (0.00 sec)
```

#### Table Properties

`AUTO_INCREMENT` - automatically increments the id by one every time a new item is added to the table:

```sql
    id INT NOT NULL AUTO_INCREMENT,
```

`NOT NULL`  - ensures that a particular column is never left empty.

`UNIQUE` - ensures that the inserted item are always unique.

```sql
    username VARCHAR(100) UNIQUE NOT NULL,
```

`DEFAULT` - Specify the default value:

```sql
    username VARCHAR(100) UNIQUE NOT NULL,
```

`PRIMARY KEY` - Used to uniquely identify each record in the table.

Below, we make the `id` column the `PRIMARY KEY`:

```sql
    PRIMARY KEY (id)
```

