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
















