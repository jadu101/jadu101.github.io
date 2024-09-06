---
title: h-Development and APIs
draft: false
tags:
---
Creating a modern web app from scratch is very hard. This is why most of the popular web apps are developed using web frameworks.

Some of the common ones are:

- **Laravel (PHP)** - Usually for startups and small companies.
- **Express (Node.JS)** - Paypal, Yahoo, Uber, IBM
- **Django (Python)** - Google, Youtube, Instagram, Mozilla
- **Rails (Ruby)** - Github, Hulu, Twitch, Airbnb

## APIs

An important aspect of back end application development is the use of Web APIs and HTTP request parameters to connect the front end and the back end. 

We use APIs to connect front end with back end and ask for certain tasks to be carried out.

## Query Parameters

Default method of sending argument is `GET` and `POST`.

Below is an example:

```http
POST /search.php HTTP/1.1
...SNIP...

item=apples
```



## Web APIs

**API (Application Programming Interface)** - Specifies how application can interact with other applications.

Example:

Twitter's API, allows us to retrieve the latest Tweets from a certain account in XML or JSON formats, and even allows us to send a Tweet if authenticated.

Developers have to develop APIs on the back end of the web app using the API standards such as `SOAP` or `REST`.

## SOAP

**SOAP (Simple Objects Access)**  - Shares data through `XML`, where the request it made in `XML` through HTTP request and the response is also returned in `XML`.

Below is example SOAP message:

```xml
<?xml version="1.0"?>

<soap:Envelope
xmlns:soap="http://www.example.com/soap/soap/"
soap:encodingStyle="http://www.w3.org/soap/soap-encoding">

<soap:Header>
</soap:Header>

<soap:Body>
  <soap:Fault>
  </soap:Fault>
</soap:Body>

</soap:Envelope>
```

`SOAP` is useful for transferring structured data or even binary data. 

It is also useful for sharing/changing the current state of web page. 

However, `SOAP` is difficult to use for beginners or require long and complicated requests even for small queries. This is why `READ` API standard is more useful. 

## REST

**REST (Representational State Transfer)** - Shares data through the URL path. Usually returns the output in JSON format.

```json
{
  "100001": {
    "date": "01-01-2021",
    "content": "Welcome to this web application."
  },
  "100002": {
    "date": "02-01-2021",
    "content": "This is the first post on this web app."
  },
  "100003": {
    "date": "02-01-2021",
    "content": "Reminder: Tomorrow is the ..."
  }
}
```

