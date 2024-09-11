---
title: a-intro
draft: false
tags:
---
Most web apps use a database on the back-end. 

When user input is used to construct the query to the database, malicious user can trick the query into being used for something other than what the original programmer intended, providing attacker the access to query the database using an attack known as SQL Injection.


## Impact

- Retrieve logins and passwords or credit card information.
- Subvert the intended web application logic. (e.g Bypass login)
- Access features that are locked to specific users.
- Read and write files directly on the back-end server.


## Databases

- Relational Database
- Non-Relational Database

### Relational Databases

Relational Database is the most common type of database.

It uses schema, a template to dictate the data structure stored in the database.

### Non-relational Databases

NoSQL database doesn't use tables, rows, and columns or prime keys.

NoSQL database stores data using various storage models, depending on the type of data stored. 

They are very scalable and flexible. So when dealing with datasets that are not very well defined and structured, NoSQL is the best choice.

Four common storage models are:

- Key-Value - Stores data in JSON or XML
- Document-Based
- Wide-Column
- Graph


