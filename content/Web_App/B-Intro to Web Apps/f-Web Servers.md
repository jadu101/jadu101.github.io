---
title: f-Web Servers
draft: false
tags:
  - web-server
---
**Web Server** - Runs on the back end server, handles all the HTTP traffic from the client-side browser.

Usually runs on TCP port `80` or `443`.

## Response codes

| Code                        | Description                                                                                                         |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| **Successful responses**    |                                                                                                                     |
| `200 OK`                    | The request has succeeded                                                                                           |
| **Redirection messages**    |                                                                                                                     |
| `301 Moved Permanently`     | The URL of the requested resource has been changed permanently                                                      |
| `302 Found`                 | The URL of the requested resource has been changed temporarily                                                      |
| **Client error responses**  |                                                                                                                     |
| `400 Bad Request`           | The server could not understand the request due to invalid syntax                                                   |
| `401 Unauthorized`          | Unauthenticated attempt to access page                                                                              |
| `403 Forbidden`             | The client does not have access rights to the content                                                               |
| `404 Not Found`             | The server can not find the requested resource                                                                      |
| `405 Method Not Allowed`    | The request method is known by the server but has been disabled and cannot be used                                  |
| `408 Request Timeout`       | This response is sent on an idle connection by some servers, even without any previous request by the client        |
| **Server error responses**  |                                                                                                                     |
| `500 Internal Server Error` | The server has encountered a situation it doesn't know how to handle                                                |
| `502 Bad Gateway`           | The server, while working as a gateway to get a response needed to handle the request, received an invalid response |
| `504 Gateway Timeout`       | The server is acting as a gateway and cannot get a response in time                                                 |

## Apache

`Apache` or `httpd` is the most common one. 

Hosts more than 40% of all internet websites. 

## NGINX

Hosts around 30% of all internet websites.

Focuses on serving many concurrent web requests with relatively low memory and CPU load by using an async architecture. 

NGINX is considered very reliable for top companies worldwide.

Companies such as Google, Facebook, Twitter, Cisco, Intel, Netflix, and Hackthebox uses NGINX.


## IIS

**IIS (Internet Information Services)** hosts around 15% of all internet websites. 

IIS is usually used to host web applications developed for the Microsoft .NET framework.

IIS is well optimized for Active Directory integration and includes features like `Windows Auth` for authenticating users using Active Directory.

Companies such as Microsoft, Office365, Skype, Stack Overflow, and Dell uses IIS.

