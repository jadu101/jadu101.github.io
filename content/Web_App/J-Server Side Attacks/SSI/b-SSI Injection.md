---
title: b-SSI Injection
draft: false
tags:
  - ssi
---
## Exploitation

Let's say there is a web form that is asking for our name and we can submit the name.

If we enter our name, we are redirected to `/page.shtml`, which displays some general information: `Hi Peter!`.

We can guess that the page supports SSI based on the file extension. If our username is inserted into the page without prior sanitization, it might be vulnerable to SSI injection.

Let us confirm this by providing a username of `<!--#printenv -->`. 

Let us confirm that we can execute arbitrary commands using the `exec` directive by providing the following username: `<!--#exec cmd="id" -->`: