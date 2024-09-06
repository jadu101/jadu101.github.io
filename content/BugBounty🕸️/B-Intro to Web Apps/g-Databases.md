---
title: g-Databases
draft: false
tags:
  - database
---
Web application use back end databases so to store content and information related to the web app.

## Relational (SQL)

Relational (SQL) databases store their data in tables, rows, and columns. 

Each table has unique keys and they link tables together and create relationships between tables.

Using relational database, it is very and quick and easy to retrieve all data about a certain element from all databases.

Some of the famous Relational databases are MySQL, MSSQL, Oracle, PostgreSQL.

## Non-relational (NoSQL)

non-relational doesn't use tables, rows, columns, primary keys, relationships, or schemas.

NoSQL stores data using various storage models depending on the type of the data stored. 

Because there is no defined structure for the database, NoSQL databases are very scalable and flexible. 

Some of the most common NoSQL databases include MongoDB, ElasticSearch, ApacheCassandra.

4-common storage models for NoSQL databases:

- Key-Value
- Document-Based
- Wide-Column
- Graph

### Key-Value

Key-Value model usually stores data in JSON or XML. 

It has a key for each pair and stores all of its data as its value. 


```json
{
  "100001": {
    "date": "01-01-2021",
    "content": "Welcome to this web application."
  },
  "100002": {
    "date": "02-01-2021",
    "content": "This is the first post on this web app."
  },
  "100003": {
    "date": "02-01-2021",
    "content": "Reminder: Tomorrow is the ..."
  }
}
```


### Document-Based

Document-Based model stores data in complex JSON objects and each object has certain meta-data while storing the rest of the data similarly to the Key-Value model.



## Use in Web Apps

Database has to be installed and set up on the back end server. 

Let's say there's a PHP web app, once MySQL is up and running, we connect to the database server as such:

```php
$conn = new mysqli("localhost", "user", "pass");
```


We create a new database with:

```php
$sql = "CREATE DATABASE database1";
$conn->query($sql)
```

Connect to the new database and start using MySQL database through MySQL syntax:

```php
$conn = new mysqli("localhost", "user", "pass", "database1");
$query = "select * from table_1";
$result = $conn->query($query);
```

