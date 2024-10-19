---
title: Password Link over HTTP
draft: false
tags:
  - p4
---
It is a P4 category bug:

- Sensitive Data Exposure
- Weak Password Reset Implementation
- Password Reset Token Sent Over HTTP

## How to Find

1. Create user account.
2. Sign in and sign out from your account.
3. At the login page click on `forgot password`.
4. Click on `request reset link`.
5. Right click on `click to reset` and click `copy link address`.
6. Paste it to new tab and if it is sent over HTTP, we have a bug

## References
- https://freedium.cfd/https://medium.com/@loyalonlytoday/finding-a-easy-p4-e32c47c8b54a



