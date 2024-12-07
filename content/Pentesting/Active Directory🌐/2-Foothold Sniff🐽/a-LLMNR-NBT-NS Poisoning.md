---
title: a-LLMNR/NBT-NS Poisoning - Responder
draft: false
tags:
  - llmnr
  - nbt-ns
  - responder
---
Let's say we have completed initial enumeration on the domain and we have the following information:

- basic user and group info
- Open hosts and their roles
- Services running on hosts
- Naming scheme on the domain

Now it is time for:

- **Network Poisoning**
- **Password Spraying**

Using the methods below, we can gather credentials and gain initial foothold during an assessment.

- Man-in-the-Middle Attack on Link-Local Multicast Name Resolution (LLMNR)
- NetBIOS Name Service (NBT-NS)


## LLMNR & NBT-NS Primer

They are components of Microsoft Windows that is used for host identification when DNS fails. 

*LLMNR?*

If a machine tries to resolve a host but DNS fails, machine will talk to others on local network for the correct host address via LLMNR.

It uses port **5355** over udp natively.

*NBT-NS?*

If LLMNR fails, NBT-NS is used. 

NBT-NS identifies systems on a local network by NetBIOS name. 

It uses port 137 over UDP

【How this works】

When LLMNR/NBT-NS is used for name resolution, ANY host on the network can reply to their request. 

Responder can be used to poison these requests. 

Attacker spoof an authoritative name resolution source by responding to LLMNR and NBT-NS traffic. 

So attacker is acting as if we know the answer for the requesting host.

If the requested host requires name resolution or authentication actions, we can capture NetNTLM hash. 

We can use this hash for many purpose such as SMB relay attack.

【Tools that can do this】

|**Tool**|**Description**|
|---|---|
|[Responder](https://github.com/lgandx/Responder)|Responder is a purpose-built tool to poison LLMNR, NBT-NS, and MDNS, with many different functions.|
|[Inveigh](https://github.com/Kevin-Robertson/Inveigh)|Inveigh is a cross-platform MITM platform that can be used for spoofing and poisoning attacks.|
|[Metasploit](https://www.metasploit.com/)|Metasploit has several built-in scanners and spoofing modules made to deal with poisoning attacks.|

## Responder

`-A`: Analyze mode. This option allows you to see NBT-NS, BROWSER, LLMNR requests without responding.

`-w`: Starts WPAD rogue proxy server. Very effective. Captures all HTTP requests by any user.

`-f`: Will attempt to fingerprint the remote host OS and version.

`-v`: Verbosity


Responder will listen and answer any requests it sees on wire 

If successful, hash would be captured and written to `/usr/share/responder/logs` directory.


### Default Run

```bash
sudo responder -I ens224 
```

### Hashcat

```shell-session
hashcat -m 5600 hash /usr/share/wordlists/rockyou.txt 
```


## Moving On

So far,

1. External Recon
2. Internal Recon - Host Discovery, Service Enumeration, Mapping, Kerbrute, naming scheme
3. Network Poisoning -  Attempt to gain credentials

Hopefully, by this point, we gained as much data as possible. 

We should attempt to crack as many hashes we can.

Next, we will go ahead to spray the obtained password.

