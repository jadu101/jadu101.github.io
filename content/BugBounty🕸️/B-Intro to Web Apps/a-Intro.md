---
title: a-intro
draft: false
tags:
---
Web application is a interactive applications that run on web browsers. 

**Front End** - What the user sees (Client Side)

**Back End** - Web app source code (server, database)

Example of web apps are `Gmail`, `Amazon`, and `Google Docs`.


## Web App vs Website

Then what is the difference between a web app and a website?

`Web 1.0 (Website)` - Static. If were to change the website's content, developers have to edit the code manually.

`Web 2.0 (Web App)` - Dynamic. Content is based on user interaction. Fully functional and can perform various functionalities for the end-user.


## Web App vs Native OS App

Unlike native OS apps, web apps can run in a browser on any OS.

Web apps got **Version Unity**, meaning all users use the same version and the same web app until the web app is updated. 

Web apps can be updated in a single location (webserver) without developing different builds for each platform.

Native OS apps are more **capable** since they have a deeper integration to the OS.

## Web App Distribution

Some common open source web apps:

- WordPress
- OpenCart
- Joomla

Some common closed source web apps:

- Wix
- Shopify
- DotNetNuke

## Security Risks of Web Apps

Web apps are accessible from any country by everyone with an internet connection.

Web apps run on servers that may host other sensitive information.

One of the most common methodology is `OWASP Web Seucurity Testing Guide`.

Pentesters usually start with the front-end (HTML, CSS, JavaScript) and look for vulnerabilities such as `Sensitive Data Exposure` and `XSS`.

Next, pentesters review web app's core functionality and interaction between the browser and the webserver. 

Pentester should assess web app from both unauthenticated and authenticated perspective.

## Attacking Web Apps

Every company has a web app within their external perimeter. 

Nowadays, it is not common to find a bug from a web app through public exploits.

Attack such as file upload sometimes lead to code execution. 

SQL injection is still quite prevalent. We often find it on web apps that use AD for authentication.

When web app vulnerability is chained, it could be very damaging:

`SQL Injection` - Obtain AD usernames and perform password spray with it.

`File Includion` - Read source code to find hidden page or directory, which leads to additional functionality

`Unrestricted File Upload` - Upload malicious code instead of uploading a profile picture.

`IDOR` - Combined with broken access control, can be used to access another user's files or functionality. 

`Broken Access Control` - `username=bjones&password=Welcome1&email=bjones@inlanefreight.local&roleid=3`, change roleid to 1 and we have admin access






