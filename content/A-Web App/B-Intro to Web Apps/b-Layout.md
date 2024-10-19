---
title: b-Layout
draft: false
tags:
---
**Web Application Infrastructure** - Structure of required components, such as database. 

**Web Application Components** - UI/UX, Client, and Server

**Web Application Architecture** - Architecture compromises all the relationships between the various web application components.

## Web App Infrastructure

Web app can use many different infrastructure setups (models).

- Client-Server
- One Server (Risky Design, all eggs in one basket)
- Many Servers - One database
- Many Servers - Many database


## Web App Components

- Client
- Server
	- Webserver
	- Web Application Logic
	- Database
- Services (Microservices)
	- 3rd Party Integrations
	- Web Application Integrations
- Functions (Serverless)

> **Microservices** - e.g Registration, search, payments
> Communication between microservices is `stateless`. They can be written in different languages and still interact.

> **Serverless** - AWS, GCP, Azure
> Serverless apps run in stateless computing containers such as docker. 



## Web App Architecture

- **Presentation Layer** - UI Process components that enable communication with the application and the system. Can be accessed via browser.
- **Application Layer** - Ensures all client requests are correctly processed.
- **Data Layer** - Determine exactly where the required data is stored and can be accessed.



- 