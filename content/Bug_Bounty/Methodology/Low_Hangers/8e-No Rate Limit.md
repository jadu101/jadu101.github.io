---
title: No Rate Limit
draft: false
tags:
---
What is rate limiting?

> Rate limiting refers to the practice of controlling the number of requests that can be made to a service over a defined period. For instance, an API might allow only 100 requests per minute from a single user or IP address. If this limit is exceeded, the service will typically return an error message indicating that the rate limit has been surpassed.


## Discover

Let's see there is a feature for inviting teammate on a web app.

Users can send invitation to team member's email.

Go to any temporary online email service and use it to receive the invitation later. 

1. Intercept the traffic when the invitation is being sent. 
2. Sent the traffic to Intruder.
3. Choose attacker type : `Sniper`
4. Go to Payloads tab and select `Null payloads`  for payload type, under Payload sets.
5. Start Attack
6. After finishing the attack, if only 200 status code is shown, it is vulnerable. 
7. Check email inbox, it must have 100+ invitations coming in.

## Impact

- Emailing bombing
- Resource Wastage
- Denial of Service

## Mitigation

- Rate limit function
- CAPTCHA