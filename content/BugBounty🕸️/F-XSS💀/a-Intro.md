---
title: a-Intro
draft: false
tags:
  - xss
---
**Cross Site Scripting (XSS)** take advantage of a flaw in user input sanitization to "write" JavaScript code to the page and execute it on the client side, leading to several types of attacks. 

## What is XSS

When web app does not properly sanitize user input, hacker can inject extra JavaScript code in an input field to such as comment/reply.

Once another user views the same page, malicious JavaScript code would be executed. 

XSS doesn't affect the back-end server but only affects the user executing the vulnerability.

## XSS Attacks

Example 1: Make target user unwillingly send their session cookie to the attacker's web server.

Example 2: Make the target's browser execute API calls that lead to a malicious action such as changing user's password. 

## Types of XSS

| Type                             | Description                                                                                                                                                                                                                                  |
| -------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `Stored (Persistent) XSS`        | The most critical type of XSS, which occurs when user input is stored on the back-end database and then displayed upon retrieval (e.g., posts or comments)                                                                                   |
| `Reflected (Non-Persistent) XSS` | Occurs when user input is displayed on the page after being processed by the backend server, but without being stored (e.g., search result or error message)                                                                                 |
| `DOM-based XSS`                  | Another Non-Persistent XSS type that occurs when user input is directly shown in the browser and is completely processed on the client-side, without reaching the back-end server (e.g., through client-side HTTP parameters or anchor tags) |
