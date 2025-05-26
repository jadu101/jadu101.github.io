---
title: b-SQLi Basics
draft: false
tags:
---
Once DBMS is installed and set up on the back-end server, web app can start utilizing it to store and retrieve data.

e.g With a PHP web app, we can connect to our database as follows:

```php
$conn = new mysqli("localhost", "root", "password", "users");
$query = "select * from logins";
$result = $conn->query($query);
```

Below PHP code will print all returned results of the SQL query in new lines:

```php
while($row = $result->fetch_assoc() ){
	echo $row["name"]."<br>";
}
```

e.g User uses the search function to search for other users, their search input is passed to the web app, which uses the input to search within the databases:

```php
$searchInput =  $_POST['findUser'];
$query = "select * from logins where username like '%$searchInput'";
$result = $conn->query($query);
```

In the above examples, web app accepts user input and pass it directly to the SQL query without sanitization.

## SQL Injection

SQL injection occurs when user-input is inputted into the SQL query string without properly sanitizing or filtering the input.

Take a look at the example below:

```php
$searchInput =  $_POST['findUser'];
$query = "select * from logins where username like '%$searchInput'";
$result = $conn->query($query);
```


If we input `admin`, it becomes `'%admin'`. 

If we input `1'; DROP TABLE users;`, the search input would be:

```php
'%1'; DROP TABLE users;'
```

So, the final SQL query executed would be as follows:

```sql
select * from logins where username like '%1'; DROP TABLE users;'
```

## Types of SQLi

- Union Based - Specify exact location (e.g column), query will direct the output to  be printed there. 
- Error Based - Used when we get `PHP` or `SQL` errors in the front-end, we may intentionally cause an SQL error that will return the output of the query.
- Blind SQL Injection - We may not get the output printed, we retrieve output character by character.
	- Boolean Based - Use SQL conditional Statement to control whether the page returns any output at all.
	- Time Based -Use SQL conditional statements that delay the page response if the conditional statement returns `true` using `Sleep()` function.



