---
title: HTB-Runner
draft: false
tags:
  - htb
  - linux
  - medium
  - knockpy
  - cve-2023-42793
  - teamcity
  - chisel
  - portrainer
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/Runner.png)

## Information Gathering
### Rustscan

Rustscan finds SSH, HTTP, and port 8000 open:

```bash
┌──(yoon㉿kali)-[~/Documents/htb/runner]
└─$ rustscan --addresses 10.10.11.13 --range 1-65535
<snip>
Host is up, received syn-ack (0.40s latency).
Scanned at 2024-05-22 03:17:04 EDT for 0s

PORT     STATE SERVICE  REASON
22/tcp   open  ssh      syn-ack
80/tcp   open  http     syn-ack
8000/tcp open  http-alt syn-ack

Read data files from: /usr/bin/../share/nmap
Nmap done: 1 IP address (1 host up) scanned in 0.87 seconds
```

### Nmap

Let's better enumerate port 80 and 8000:

`sudo sudo nmap -sVC -p 80,8000 10.10.11.13`

```bash
PORT     STATE SERVICE     VERSION
80/tcp   open  http        nginx 1.18.0 (Ubuntu)
|_http-title: Runner - CI/CD Specialists
|_http-server-header: nginx/1.18.0 (Ubuntu)
8000/tcp open  nagios-nsca Nagios NSCA
|_http-title: Site doesn't have a title (text/plain; charset=utf-8).
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

**nagios-nsca** is running on port 8000.

> Nagios NSCA (Nagios Service Check Acceptor) is a component used in Nagios, a popular open-source monitoring system. NSCA facilitates the communication between remote hosts and the central Nagios server. Here are the key points about Nagios NSCA.

## Enumeration
### HTTP - TCP 80

After adding **runner.htb** to `/etc/hosts`, we can access the website:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image.png)

Let's enumerate subdomains using **knockpy**:

`knockpy runner.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-5.png)

We will add **teamcity.runner.htb** to `/etc/hosts` as well. 

### Nagios - TCP 8000

We tried accessing port 8000 through browser but nothing was found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-1.png)

Feroxbuster found two paths, `/version` and `/health`:

`sudo feroxbuster -u http://runner.htb:8000/ -n -w /usr/share/seclists/Discovery/Web-Content/directory-list-2.3-medium.txt -C 404`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-4.png)

Below is the screenshot of /version:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-2.png)

Below is the screenshot of /health:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-3.png)

Since we found nothing intriguing, let's move on. 


## CVE-2023-42793

Opening **teamcity.runner.htb**, we see a login page for TeamCity Version 2023.05.3:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-7.png)

> TeamCity is a continuous integration (CI) and continuous deployment (CD) server developed by JetBrains. It is designed to support the automated building, testing, and deployment of software projects. TeamCity integrates with version control systems, builds tools, testing frameworks, and deployment tools, facilitating efficient and reliable software development and delivery processes.

Searching for known exploit, we discovered [CVE-2023-42793](https://www.exploit-db.com/exploits/51884).

Running the exploit script will create an Admin account for us to login:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-6.png)

Using the created crednetials, we can login:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-8.png)

## Shell as John

Let's explore the dashboard. 

On user management tab, we see several interesting sub-menus:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-9.png)

Going to **Users**, we get a list of users:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-10.png)


### Backup

Under server administration tab, there is a **Backup** menu:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-11.png)

Backups are always interesting, let's take a look into it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-12.png)

In backup section, we will create a backup file and download it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-13.png)

Remembering the username from earlier, we will search for **angry-admin**:

`grep -ir 'angry-admin' *`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-14.png)

**users** file inside database_dump seems to be containing password hash for angry-admin. 

**users** file contains password hash for other users as well: 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-15.png)

| USERNAME       | PASSWORD                                                        | NAME    | EMAIL                   |
| -------------- | --------------------------------------------------------------- | ------- | ----------------------- |
| admin          | $2a$07$neV5T/BlEDiMQUs.gM1p4uYl8xl8kvNUo4/8Aja2sAWHAQLWqufye     | John    | john@runner.htb         |
| matthew        | $2a$07$q.m8WQP8niXODv55lJVovOmxGtg6K/YPHbD48/JQsdGLulmeVo.Em     | Matthew | matthew@runner.htb      |
| city_adminjmp9 | $2a$07$4QQVn7iv3g5Oz8xWQbo8de4M6/cMSOb6YQmIp0i3a.z/VN2L124Ym     |         | angry-admin@funnybunny.org |


### Hash Cracking

Let's try cracking discovered hashes with hashcat:

`hashcat -m 3200 -a 0 hash ~/Downloads/rockyou.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/runner-h.png)


We managed to crack password for matthew (piper123), but failed to crack for other user's hashes.

We tried SSH login as user matthew and with the cracked password but it won't work. 

### SSH

Exploring the backup more, we discovered **id_rsa** file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-59.png)

Discovered id_rsa key works for user john:

`ssh -i id_rsa john@10.10.11.13`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-17.png)

Now we have ssh connection as john. 

## Privesc: john to root

Earlier, we managed to crack password for user john. This must be useful somewhere.

Keeping this in mind, let's look for internally open ports:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-60.png)

There are many ports open internally and port 9000 looks interesting. 

### Chisel

Let's port forward port 9000 back to us. 

We will first transfer chisel over to the target machine:

`scp -i id_rsa /opt/chisel/chisel_linux john@10.10.11.13:/home/john/chisel_linux`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-19.png)

Now, let's start chisel client for port 9000:

`./chisel_linux client 10.10.14.13:9001 R:9000:127.0.0.1:9000`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-21.png)

Chisel server running on local kali machine detects incoming connection:

`chisel server -p 9001 --reverse`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-20.png)

### Portrainer

We can now access port 9000 from our local browser.

Website shows a portrainer login portal:

`http://127.0.0.1:9000`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-22.png)

Using the password hash cracked earlier (matthew:piper123), we can log in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-23.png)

Searching for privilege escalation regarding portrainer, we discovered [this article](https://rioasmara.com/2021/08/15/use-portainer-for-privilege-escalation/).

We will follow the article to escalate our privilege to root. 

Currently, we see two images available:

`/docker/images`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-24.png)

We will copy image ID of one of them.

With the Image ID copied on our clipboard, let's move on to creating new volume.

We will give it a name **Root** and set Driver Options as the below:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-49.png)

By setting device path as `/`, we should be able to access root folder later.

Now let's create a container. 

We will give it a name **Pwned** and copy-paste in the image ID we copied earlier:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-56.png)

Scroll down and go to advanced setting.

We will set up Console to be **Interactive & TTY**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-57.png)

For Volumes, we will set it the path to be `/mnt/root` and use the volme we created earlier:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-58.png)

After deploying, we can see our container created:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-54.png)

Open on created container and there will be Console menu:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-55.png)

Cliking on Console, we should be able to execute commands as the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-50.png)

Going to `/mnt/root/root`, we can read root.txt:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/runner/image-53.png)

## References
- https://www.exploit-db.com/exploits/51884
- https://rioasmara.com/2021/08/15/use-portainer-for-privilege-escalation/