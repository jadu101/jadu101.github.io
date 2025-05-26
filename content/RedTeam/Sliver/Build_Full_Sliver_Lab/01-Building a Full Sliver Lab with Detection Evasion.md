---
title: 01-Building a Full Sliver Lab with Detection Evasion
draft: false
tags:
  - redteam
  - sliver
  - c2
  - active-directory
---
## Goal

Test Sliver in realistic AD environment.

## Setup

- Install Sliver Server on VPS/local machine
- Create lab with:
	- Windows Domain Controller
	- 1-2 Windows 10/11 Clients
	- Linux machine (optional)
- Use tools like DetectionLab or manually configure with Sysmon, ELK, or Velociraptor

**Try**:
- Deliver Sliver payload via:
	- Malicious macro document
	- HTA file or shortcut (.lnk)
- Test commands: 
	- Privilege Escalation
	- Lateral Movement
	- File Exfiltration
	- Observer Detection Logs



























	- 