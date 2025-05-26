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

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/RedTeam/Sliver/BuildSliverLab/01-vm-setup.png)