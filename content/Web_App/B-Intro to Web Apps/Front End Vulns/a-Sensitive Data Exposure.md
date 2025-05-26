---
title: a-Sensitive Data Exposure
draft: false
tags:
---
Majority of web app pentesting is focused on back end components but it is also important to test front end components.

**Sensitive Data Exposure** - Availability of sensitive data in clear-text to end user. 

They are usually found in the `source code`.

One of the first thing we do when assessing a web app is to review the source code.

## Example

```html
<form action="action_page.php" method="post">

    <div class="container">
        <label for="uname"><b>Username</b></label>
        <input type="text" required>

        <label for="psw"><b>Password</b></label>
        <input type="password" required>

        <!-- TODO: remove test credentials test:test -->

        <button type="submit">Login</button>
    </div>
</form>

</html>
```

