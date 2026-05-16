---
title: API Testing
draft: false
tags:
  - api
  - web
---
# API Testing

## 1-API Recon

First we need to identify API endpoints. Such as: `/api/books`

After idntifying the API endpoint, figure out:
- Input data API processes (XML? JSON?)
- Types of requests (HTTP Method? Media Formats?)
- Rate Limits and authentication mechanisms

## 2-API Documentation

API Documentation could be both human readable and machine readable. 

```
/api
/swagger/index.html
/openapi.json
```

**Make sure you investigate the basepath manually**: `/api`, `/api/swagger`, `/api/swagger/v1`

## 3-Identify & Interact API Endpoints

Couple ways to identify API Endpoints: `Burp Crawl`, `manual investigation`, and `JavaScript Files (JS Link Finder BApp)`.

After identifying API endpoint -> 

1. **Identify Supported HTTP Methods**: Use Intruder HTTP Verbs List.
2. **Identify Supported Content Types**: modify `Content-Type` header using `Content type converter BApp` (XML <-> JSON)
3. **Identify more hidden endpoints using Intruder**

## 4-Find Hidden Parameters

Sometimes there are API parameters that are not documented. 

How to find those parameters?: `Intruder`, `Param miner BApp`, and `Content Discovery Tool`.

## 5-Mass Assignment Vulnerability

This can inadvertently create hidden parameters.

Mass assignment creates parameters from object fields and you can often identify these hidden parameters by manually examining objects returned by the API.

For example, below JSON return might indicate that hidden `id` and `isAdmin` parameters are bound to internal user object:

```
{
    "id": 123,
    "name": "John Doe",
    "email": "john@example.com",
    "isAdmin": "false"
}
```

To test whether the parameter works, try adding it to the request as such:

```
{
    "username": "wiener",
    "email": "wiener@example.com",
    "isAdmin": false,
}
```

You can also try giving it a invalid parameter value as such:

```
{
    "username": "wiener",
    "email": "wiener@example.com",
    "isAdmin": "foo",
}
```

----

## 6-Server-side parameter pollution

There are intenal APIs that shouldn't be accessible. SSPP occurs when website embeds user input in a server-side request to an internal API without adequate encoding. 

To test for SSPP: place query syntax characters like `#`, `&`, and `=` in your input and observe how the application responds. 

For example: `GET /userSearch?name=peter%23foo&back=/home`

For example, if the response returns the user peter, the server-side query may have been truncated. If an Invalid name error message is returned, the application may have treated foo as part of the username. This suggests that the server-side request may not have been truncated. 

Another example: `GET /userSearch?name=peter%26foo=xyz&back=/home`

You can also try adding second valid parameter: `GET /userSearch?name=peter%26email=foo&back=/home`

You could try to override the original parameter. Do this by injecting a second parameter with the same name: `GET /userSearch?name=peter%26name=carlos&back=/home`

    - PHP parses the last parameter only. This would result in a user search for carlos.
    - ASP.NET combines both parameters. This would result in a user search for peter,carlos, which might result in an Invalid username error message.
    - Node.js / express parses the first parameter only. This would result in a user search for peter, giving an unchanged result.

If you're able to override the original parameter, you may be able to conduct an exploit. For example, you could add name=administrator to the request. This may enable you to log in as the administrator user. 

For SSPP on **REST PATH**:

RESTful API places parameter names and values in the URL path like `/api/users/123`.

If there's an request as such: `GET /edit_profile.php?name=peter`, try manipulating as `GET /edit_profile.php?name=peter%2f..%2fadmin`.



















