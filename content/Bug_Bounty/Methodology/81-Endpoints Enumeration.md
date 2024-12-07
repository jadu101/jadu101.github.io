---
title: 8-Endpoints Enumeration
draft: false
tags:
---




### JWT Hunting


```
cat all_urls.txt| grep "eyJ" 
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101195406.png)

### UUID

A UUID (Universally Unique Identifier) is a 128-bit unique identifier used for resources like user accounts or records. Extracting UUIDs during bug hunting helps identify sensitive resources, which can lead to vulnerabilities like IDOR (Insecure Direct Object Reference) or access control flaws. Finding UUIDs can also expose hidden or deprecated endpoints for further analysis.

```
grep -Eo '[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[1-5][0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-fA-F]{12}' all_urls.txt | sort -u 
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/bbh/methodology/Pasted image 20241101200228.png)


### Any suspicious keyword/path/number

```
grep -Eo '([a-zA-Z0-9_-]{20,})' all_urls.txt
```

### SSN(Social Security Number)

```
grep -Eo '\b[0-9]{3}-[0-9]{2}-[0-9]{4}\b' all_urls.txt
```

### Creditcard Numbers

```
grep -Eo '\b[0-9]{13,16}\b' all_urls.txt
```

### Potential SessionIDs and cookies

```
grep -Eo '[a-zA-Z0-9]{32,}' all_urls.txt
```

### Tokens + Secrets

```
cat wayback_domain.com.txt | grep "token"  
cat wayback_domain.com.txt | grep "token="  
cat wayback_domain.com.txt | grep "code"  
cat wayback_domain.com.txt | grep "code="  
cat wayback_domain.com.txt | grep "secret"  
cat wayback_domain.com.txt | grep "secret="
```

### Others

```
cat wayback_domain.com.txt | grep "admin"  
cat wayback_domain.com.txt | grep "pass"  
cat wayback_domain.com.txt | grep "pwd"  
cat wayback_domain.com.txt | grep "passwd"  
cat wayback_domain.com.txt | grep "password"  
  
cat wayback_domain.com.txt | grep "phone"  
cat wayback_domain.com.txt | grep "mobile"  
cat wayback_domain.com.txt | grep "number"  
  
cat wayback_domain.com.txt | grep "mail"
```

### Private IP Addresses

Identifying private IP addresses is essential for uncovering hidden internal services that could be vulnerable to exploitation. It can reveal potential security misconfigurations that expose sensitive data or systems to unauthorized access. Furthermore, this information assists in mapping out the internal network.

```
grep -Eo '((10|172\.(1[6-9]|2[0-9]|3[0-1])|192\.168)\.[0-9]{1,3}\.[0-9]{1,3})' wayback_domain.com.txt
```

### IPv4

```
grep -Eo '([0-9]{1,3}\.){3}[0-9]{1,3}' wayback_domain.com.txt
```

### IPv6

```
grep -Eo '([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}' wayback_domain.com.txt
```

### Payment

```
grep "payment" wayback_domain.com.txt  
grep "order" wayback_domain.com.txt  
grep "orderid" wayback_domain.com.txt  
grep "payid" wayback_domain.com.txt  
grep "invoice" wayback_domain.com.txt  
grep "pay" wayback_domain.com.txt
```

### API Endpoint

```
grep "/api/" wayback_domain.com.txt  
grep "api." wayback_domain.com.txt  
grep "api" wayback_domain.com.txt  
grep "/graphql" wayback_domain.com.txt  
grep "graphql" wayback_domain.com.txt  
  
  
# when new API versions are released, developers forget to remove previous ones  
# so we go back to previous versions and then exploit them first as more  
# chance to get bug :)  
grep "/v1/" wayback_domain.com.txt  
grep "/v2/" wayback_domain.com.txt  
grep "/v3/" wayback_domain.com.txt  
grep "/v4/" wayback_domain.com.txt  
grep "/v5/" wayback_domain.com.txt
```


### Authentication & Authorization

```
cat wayback_domain.com.txt | grep "sso"  
cat wayback_domain.com.txt | grep "/sso"  
cat wayback_domain.com.txt | grep "saml"  
cat wayback_domain.com.txt | grep "/saml"  
cat wayback_domain.com.txt | grep "oauth"  
cat wayback_domain.com.txt | grep "/oauth"  
cat wayback_domain.com.txt | grep "auth"  
cat wayback_domain.com.txt | grep "/auth"  
cat wayback_domain.com.txt | grep "callback"  
cat wayback_domain.com.txt | grep "/callback"
```

Try to identify endpoints related to SSO, SAML, OAuth, and authentication because they are critical for managing user identities and access control. These endpoints are often complex and can be misconfigured, leading to vulnerabilities such as unauthorized access or privilege escalation. Specifically, misconfigured SSO or OAuth providers can expose sensitive data and create open redirect vulnerabilities, allowing attackers to redirect users to malicious sites. By examining these endpoints, bug hunters can identify and exploit these weaknesses, ensuring robust authentication and authorization mechanisms are implemented to enhance overall application security.

### Information Disclosure via Exposed Files

```
grep -Eo 'https?://[^ ]+\.(env|yaml|yml|json|xml|log|sql|ini|bak|conf|config|db|dbf|tar|gz|backup|swp|old|key|pem|crt|pfx|pdf|xlsx|xls|ppt|pptx)' wayback_domain.com.txt
```