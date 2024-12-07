---
title: c-Front End Back End
draft: false
tags:
---
## Front End

Web browser (client-side).

HTML, CSS, Javascript

Should adapt to any screen size and work within any browser on any device. 

- Visual Concept Web Design
- User Interface (UI) design
- User Experience (UX) design

## Back End

Four main back end components for web applications:

- **Back End Servers** - The hardware and OS that hosts all other components.
- **Web Servers** - Apache, Nginx, IIS
- **Databases** - MySQL, MSSQL
- **Development Frameworks** - Laravel, ASP.NET, Spring

## Secure Front/Back End

**Whitebox Pentesting** - When with source code of front end components, we can perform a code review.

**Blackbox Pentesting** - We do not have access to the source code. 

Top 20 mistakes developers make:

| **No.** | **Mistake**                                        |
| ------- | -------------------------------------------------- |
| `1.`    | Permitting Invalid Data to Enter the Database      |
| `2.`    | Focusing on the System as a Whole                  |
| `3.`    | Establishing Personally Developed Security Methods |
| `4.`    | Treating Security to be Your Last Step             |
| `5.`    | Developing Plain Text Password Storage             |
| `6.`    | Creating Weak Passwords                            |
| `7.`    | Storing Unencrypted Data in the Database           |
| `8.`    | Depending Excessively on the Client Side           |
| `9.`    | Being Too Optimistic                               |
| `10.`   | Permitting Variables via the URL Path Name         |
| `11.`   | Trusting third-party code                          |
| `12.`   | Hard-coding backdoor accounts                      |
| `13.`   | Unverified SQL injections                          |
| `14.`   | Remote file inclusions                             |
| `15.`   | Insecure data handling                             |
| `16.`   | Failing to encrypt data properly                   |
| `17.`   | Not using a secure cryptographic system            |
| `18.`   | Ignoring layer 8                                   |
| `19.`   | Review user actions                                |
| `20.`   | Web Application Firewall misconfigurations         |

Below is OWASP top 10:

|**No.**|**Vulnerability**|
|---|---|
|`1.`|Broken Access Control|
|`2.`|Cryptographic Failures|
|`3.`|Injection|
|`4.`|Insecure Design|
|`5.`|Security Misconfiguration|
|`6.`|Vulnerable and Outdated Components|
|`7.`|Identification and Authentication Failures|
|`8.`|Software and Data Integrity Failures|
|`9.`|Security Logging and Monitoring Failures|
|`10.`|Server-Side Request Forgery (SSRF)|