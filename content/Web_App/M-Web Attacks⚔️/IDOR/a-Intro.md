---
title: a-Intro
draft: false
tags:
  - idor
---
`Insecure Direct Object References (IDOR)` vulnerabilities are among the most common web vulnerabilities and can significantly impact the vulnerable web application.

e.g

Let's say user request to access a file they downloaded (`download.php?file_id=123`). By using `download.php?file_id=124` instead, user can access different files. 

## What Makes IDOR

Many web applications restrict users from accessing resources by restricting them from accessing the pages, functions, and APIs that can retrieve these resources. However, what would happen if a user somehow got access to these pages (e.g., through a shared/guessed link)?

Would they still be able to access the same resources by simply having the link to access them? If the web application did not have an access control system on the back-end that compares the user's authentication to the resource's access list, they might be able to.

The main takeaway is that `an IDOR vulnerability mainly exists due to the lack of an access control on the back-end`. If a user had direct references to objects in a web application that lacks access control, it would be possible for attackers to view or modify other users' data.

## Impact of IDOR

Attacker might be able to access private files and resources of other users.

Attacker might even be able to modify or delete other user's data.

Attacker might also be able to elevate user privilege.

