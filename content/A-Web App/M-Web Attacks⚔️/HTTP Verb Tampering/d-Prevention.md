---
title: d-Prevention
draft: false
tags:
---
## Insecure Configuration

Following is an example of vulnerable configuration for an Apache server:

```xml
<Directory "/var/www/html/admin">
    AuthType Basic
    AuthName "Admin Panel"
    AuthUserFile /etc/apache2/.htpasswd
    <Limit GET>
        Require valid-user
    </Limit>
</Directory>
```

`<Limit GET>` keyword is being used.

Therefore, page will be accessible through `POST` requests. 

Following is an example from Tomcat web server (`web.xml`):

```xml
<security-constraint>
    <web-resource-collection>
        <url-pattern>/admin/*</url-pattern>
        <http-method>GET</http-method>
    </web-resource-collection>
    <auth-constraint>
        <role-name>admin</role-name>
    </auth-constraint>
</security-constraint>
```

Following is an example from ASP.NET web server (`web.config`):

```xml
<system.web>
    <authorization>
        <allow verbs="GET" roles="admin">
            <deny verbs="GET" users="*">
        </deny>
        </allow>
    </authorization>
</system.web>
```

If we want to specify a single method, we can use safe keywords, like `LimitExcept` in Apache, `http-method-omission` in Tomcat, and `add`/`remove` in ASP.NET, which cover all verbs except the specified ones.

Finally, to avoid similar attacks, we should generally `consider disabling/denying all HEAD requests` unless specifically required by the web application.

## Insecure Coding

To identify this vulnerability in the code, we need to find inconsistencies in the use of HTTP parameters across functions, as in some instances, this may lead to unprotected functionalities and filters.

Let's consider the following `PHP` code from our `File Manager` exercise:

```php
if (isset($_REQUEST['filename'])) {
    if (!preg_match('/[^A-Za-z0-9. _-]/', $_POST['filename'])) {
        system("touch " . $_REQUEST['filename']);
    } else {
        echo "Malicious Request Denied!";
    }
}
```

We see that the `preg_match` filter only checks for special characters in `POST` parameters with `$_POST['filename']`. However, the final `system` command uses the `$_REQUEST['filename']` variable, which covers both `GET` and `POST` parameters. So, in the previous section, when we were sending our malicious input through a `GET` request, it did not get stopped by the `preg_match` function, as the `POST` parameters were empty and hence did not contain any special characters. Once we reach the `system` function, however, it used any parameters found in the request, and our `GET` parameters were used in the command, eventually leading to Command Injection.

