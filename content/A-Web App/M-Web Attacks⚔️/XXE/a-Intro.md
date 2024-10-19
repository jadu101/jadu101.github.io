---
title: a-intro
draft: false
tags:
  - xxe
---
`XML External Entity (XXE) Injection` vulnerabilities occur when XML data is taken from a user-controlled input without properly sanitizing or safely parsing it, which may allow us to use XML features to perform malicious actions.

## XML

`Extensible Markup Language (XML)` is a common markup language (similar to HTML and SGML) designed for flexible transfer and storage of data and documents in various types of applications.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<email>
  <date>01-01-2022</date>
  <time>10:00 am UTC</time>
  <sender>john@inlanefreight.com</sender>
  <recipients>
    <to>HR@inlanefreight.com</to>
    <cc>
        <to>billing@inlanefreight.com</to>
        <to>payslips@inlanefreight.com</to>
    </cc>
  </recipients>
  <body>
  Hello,
      Kindly share with me the invoice for the payment made on January 1, 2022.
  Regards,
  John
  </body> 
</email>
```


## XML DTD

`XML Document Type Definition (DTD)` allows the validation of an XML document against a pre-defined document structure. The pre-defined document structure can be defined in the document itself or in an external file. The following is an example DTD for the XML document we saw earlier:

```xml
<!DOCTYPE email [
  <!ELEMENT email (date, time, sender, recipients, body)>
  <!ELEMENT recipients (to, cc?)>
  <!ELEMENT cc (to*)>
  <!ELEMENT date (#PCDATA)>
  <!ELEMENT time (#PCDATA)>
  <!ELEMENT sender (#PCDATA)>
  <!ELEMENT to  (#PCDATA)>
  <!ELEMENT body (#PCDATA)>
]>
```


As we can see, the DTD is declaring the root `email` element with the `ELEMENT` type declaration and then denoting its child elements. After that, each of the child elements is also declared, where some of them also have child elements.

The above DTD can be placed within the XML document itself, right after the `XML Declaration` in the first line. Otherwise, it can be stored in an external file (e.g. `email.dtd`), and then referenced within the XML document with the `SYSTEM` keyword, as follows:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE email SYSTEM "email.dtd">
```

## XML Entities

We can define custom entities in XML DTDs to allow refactoring of variables and reduce repetitive data.

This can be done with the use of the `ENTITY` keyword:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE email [
  <!ENTITY company "Inlane Freight">
]>
```

We can reference XML entity as such:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE email [
  <!ENTITY company SYSTEM "http://localhost/company.txt">
  <!ENTITY signature SYSTEM "file:///var/www/html/signature.txt">
]>
```

- **`SYSTEM`**: It specifies that the entity is an external reference. It can point to a remote URL (like `http://localhost/company.txt`) or a local file path (like `file:///var/www/html/signature.txt`).
    
- **`company` and `signature`**: These are the names of the external entities. Wherever these entities are used in the XML document, their value will be replaced with the contents of the specified external resource (in this case, a URL or file).

