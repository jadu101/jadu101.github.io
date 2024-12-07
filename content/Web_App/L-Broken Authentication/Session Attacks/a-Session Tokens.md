---
title: a-Session Tokens
draft: false
tags:
---
Session tokens are unique identifiers a web application uses to identify a user. More specifically, the session token is tied to the user's session. If an attacker can obtain a valid session token of another user, the attacker can impersonate the user to the web application, thus taking over their session.

## Bruteforce

For instance, consider the following web application that assigns a four-character session token.

We can easily brute-force all possible session tokens and hijack all active sessions.

In real-world scenario, this is unlikely to happen.

In a slightly more common variant, the session token itself provides sufficient length; however, the token consists of hardcoded prepended and appended values, while only a small part of the session token is dynamic to provide randomness.

e.g

```
2c0c58b27c71a2ec5bf2b4b6e892b9f9
2c0c58b27c71a2ec5bf2b4546092b9f9
2c0c58b27c71a2ec5bf2b497f592b9f9
2c0c58b27c71a2ec5bf2b48bcf92b9f9
2c0c58b27c71a2ec5bf2b4735e92b9f9
```

As we can see, all session tokens are very similar. In fact, of the 32 characters, 28 are the same for all five captured sessions.

Another vulnerable example would be an incrementing session identifier. For instance, consider the following capture of successive session tokens:

```
141233
141234
141237
141238
141240
```


## Predictable Session Tokens

The generation of session tokens is not truly random; it can be predicted by an attacker with insight into the session token generation logic.

e.g Cookie is base64 encoded

```shell-session
jadu101@htb[/htb]$ echo -n dXNlcj1odGItc3RkbnQ7cm9sZT11c2Vy | base64 -d

user=htb-stdnt;role=user
```

e.g Cookie is hex encoded. How to create fake cookie as admin:

```shell-session
jadu101@htb[/htb]$ echo -n 'user=htb-stdnt;role=admin' | xxd -p

757365723d6874622d7374646e743b726f6c653d61646d696e
```

