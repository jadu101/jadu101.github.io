---
title: c-Bruteforce Password Reset Tokens
draft: false
tags:
  - ffuf
---
Many web applications implement a password-recovery functionality if a user forgets their password. This password-recovery functionality typically relies on a one-time reset token, which is transmitted to the user, for instance, via SMS or E-Mail. The user can then authenticate using this token, enabling them to reset their password and access their account.

## Identify Weak Reset Tokens

To identify weak reset tokens, we typically need to create an account on the target web application, request a password reset token, and then analyze it.

e.g We have received the following password reset e-mail:

```
Hello,

We have received a request to reset the password associated with your account. To proceed with resetting your password, please follow the instructions below:

1. Click on the following link to reset your password: Click

2. If the above link doesn't work, copy and paste the following URL into your web browser: http://weak_reset.htb/reset_password.php?token=7351

Please note that this link will expire in 24 hours, so please complete the password reset process as soon as possible. If you did not request a password reset, please disregard this e-mail.

Thank you.
```

As we can see, the password reset link contains the reset token in the GET-parameter `token`. In this example, the token is `7351`. Given that the token consists of only a 4-digit number, there can be only `10,000` possible values. This allows us to hijack users' accounts by requesting a password reset and then brute-forcing the token.

## Attack Weak Reset Tokens

We can use `ffuf `to bruteforce all possible reset tokens. 

We will first create a wordlist from `0000` to `9999`:

```shell-session
jadu101@htb[/htb]$ seq -w 0 9999 > tokens.txt
```

Assuming that there are users currently in the process of resetting their passwords, we can try to brute-force all active reset tokens. If we want to target a specific user, we should send a password reset request for that user first to create a reset token. We can then specify the wordlist in `ffuf` to brute-force all active reset-tokens:

```shell-session
jadu101@htb[/htb]$ ffuf -w ./tokens.txt -u http://weak_reset.htb/reset_password.php?token=FUZZ -fr "The provided token is invalid"

<SNIP>

[Status: 200, Size: 2667, Words: 538, Lines: 90, Duration: 1ms]
    * FUZZ: 6182
```

