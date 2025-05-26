---
title: a-intro
draft: false
tags:
  - lfi
---
## Local File Inclusion (LFI)

We find LFIs most at template engines. 

To have the web app look the same when navigating, templating engine is used to display a page that shows the common static parts like `header`, `navigation bar`, and `footer`.

This is why we often see a parameter like `/index.php?page=about`, where `index.php` sets static content (e.g. header/footer).

LFI can lead to

- Source code disclosure
- Sensitive data exposure
- RCE

## Examples of Vulnerable Code

### PHP

In `PHP`, we may use the `include()` function to load a local or a remote file as we load a page.

```php
if (isset($_GET['language'])) {
    include($_GET['language']);
}
```

We see that the `language` parameter is directly passed to the `include()` function. So, any path we pass in the `language` parameter will be loaded on the page, including any local files on the back-end server.

Functions such as `include_once()`, `require()`, `require_once()`, `file_get_contents()` can have the same vulnerability.


### NodeJS

The following is a basic example of how a GET parameter `language` is used to control what data is written to a page:

```javascript
if(req.query.language) {
    fs.readFile(path.join(__dirname, req.query.language), function (err, data) {
        res.write(data);
    });
}
```

As we can see, whatever parameter passed from the URL gets used by the `readfile` function, which then writes the file content in the HTTP response.

### Java

he following examples show how web applications for a Java web server may include local files based on the specified parameter, using the `include` function:

  
```jsp
<c:if test="${not empty param.language}">
    <jsp:include file="<%= request.getParameter('language') %>" />
</c:if>
```

### .NET


```cs
@if (!string.IsNullOrEmpty(HttpContext.Request.Query['language'])) {
    <% Response.WriteFile("<% HttpContext.Request.Query['language'] %>"); %> 
}
```




## Read Execute

The most important thing to keep in mind is that `some of the above functions only read the content of the specified files, while others also execute the specified files`. Furthermore, some of them allow specifying remote URLs, while others only work with files local to the back-end server:

|**Function**|**Read Content**|**Execute**|**Remote URL**|
|---|:-:|:-:|:-:|
|**PHP**||||
|`include()`/`include_once()`|✅|✅|✅|
|`require()`/`require_once()`|✅|✅|❌|
|`file_get_contents()`|✅|❌|✅|
|`fopen()`/`file()`|✅|❌|❌|
|**NodeJS**||||
|`fs.readFile()`|✅|❌|❌|
|`fs.sendFile()`|✅|❌|❌|
|`res.render()`|✅|✅|❌|
|**Java**||||
|`include`|✅|❌|❌|
|`import`|✅|✅|✅|
|**.NET**||||
|`@Html.Partial()`|✅|❌|❌|
|`@Html.RemotePartial()`|✅|❌|✅|
|`Response.WriteFile()`|✅|❌|❌|
|`include`|✅|✅|✅|