---
title: d-Using Comments
draft: false
tags:
---
Let's learn how to use comments to subvert the logic of more advanced SQL queries to bypass the authentication.

## Comments

There are two types of comments with MySQL:

- `--`
- `#`
- `/**/`

`--` needs a space after it for the comment to work. This is why payloads often looks like `-- -`.

## Auth Bypass

Let's try injecting `admin'--` as our username. Final query will look like:

```sql
SELECT * FROM logins WHERE username='admin'-- ' AND password = 'something';
```

Now the username is `admin` and the remainder of the query is not ignored.

