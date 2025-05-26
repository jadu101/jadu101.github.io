---
title: f-Mitigation
draft: false
tags:
---
## Input Sanitization

Here is an example of a vulnerable php code that is used for authentication:

```php
<SNIP>
  $username = $_POST['username'];
  $password = $_POST['password'];

  $query = "SELECT * FROM logins WHERE username='". $username. "' AND password = '" . $password . "';" ;
  echo "Executing query: " . $query . "<br /><br />";

  if (!mysqli_query($conn ,$query))
  {
          die('Error: ' . mysqli_error($conn));
  }

  $result = mysqli_query($conn, $query);
  $row = mysqli_fetch_array($result);
<SNIP>
```

Above script takes `username` and `password` directly from the POST request and passes it to the query directly. 

We can use libraries such as `mysql_real_escape_string()` to sanitize user input, rendering injected queries.

This function escapes characters such as `'` and `"`.

```php
<SNIP>
$username = mysqli_real_escape_string($conn, $_POST['username']);
$password = mysqli_real_escape_string($conn, $_POST['password']);

$query = "SELECT * FROM logins WHERE username='". $username. "' AND password = '" . $password . "';" ;
echo "Executing query: " . $query . "<br /><br />";
<SNIP>
```

## Input Validation

We can validated the input based on the data used to query to ensure it matched the expected form such as `@email.com`.

Below is a vulnerable code:

```php
<?php
if (isset($_GET["port_code"])) {
	$q = "Select * from ports where port_code ilike '%" . $_GET["port_code"] . "%'";
	$result = pg_query($conn,$q);
    
	if (!$result)
	{
   		die("</table></div><p style='font-size: 15px;'>" . pg_last_error($conn). "</p>");
	}
<SNIP>
?>
```

`port_code` parameter is being used in the query directly. 

Since `port_code` parameter can only consist of letters or spaces, we can restrict user input as such:

```php
<SNIP>
$pattern = "/^[A-Za-z\s]+$/";
$code = $_GET["port_code"];

if(!preg_match($pattern, $code)) {
  die("</table></div><p style='font-size: 15px;'>Invalid input! Please try again.</p>");
}

$q = "Select * from ports where port_code ilike '%" . $code . "%'";
<SNIP>
```

Above code used `preg_match()` function to check if the input matched the given pattern or not.

## User Privileges

We have to ensure that the user querying the database only has minimum permissions.

Superusers and users with administrative privileges should never be used with web applications.

## Firewall

**WAF** could be used to detect malicious input and reject any HTTP requests containing them.

## Parameterized Queries

Parameterized queries contain placeholders for the input data, which is then escaped and passed on by the driver:

```php
<SNIP>
  $username = $_POST['username'];
  $password = $_POST['password'];

  $query = "SELECT * FROM logins WHERE username=? AND password = ?" ;
  $stmt = mysqli_prepare($conn, $query);
  mysqli_stmt_bind_param($stmt, 'ss', $username, $password);
  mysqli_stmt_execute($stmt);
  $result = mysqli_stmt_get_result($stmt);

  $row = mysqli_fetch_array($result);
  mysqli_stmt_close($stmt);
<SNIP>
```



