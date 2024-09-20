---
title: a-Discovery and Scan
draft: false
tags:
  - pivoting
  - tunneling
  - chisel
---

**Lateral Movement**:

Technique for further access to additional hosts, applications, and services within a network environment. 

**Pivoting**:

Accessing hosts to cross network boundaries you would not usually have access to. 

**Tunneling**:

Shuttling traffic in/out of a network where there is a chance of our traffic being detected. 


**Port Forwarding**: 

Technique that allows attacker to redirect a communication request from one port to another. 




## Plot

Let's say you finally compromised the machine. You have shell connection but it seems like you need to pivot to another machine in different network. In this note, I will cover following topics:

- Enumeration for Pivoting
- Host discovery
- Port Scan
- Tunneling/Pivoting

## Enumeration for Pivoting
Following contents mainly comes from [my writeup](https://jadu101.github.io/Hackthebox%F0%9F%93%A6/HTB-Vault#pivoting-daveubuntu-to-davedns)for HTB-Vault.

### ifconfig
First thing you want to do is to run `ifconfig`:

In this case, you see **virbr0** interface, which is the network interface that I need to pivot into later. 

![](https://i.imgur.com/Vf9Xv28.png)

### /etc/hosts
**/etc/hosts** file might also include information about internally connected network:

![](https://i.imgur.com/80R0Y3G.png)


## Host discovery

Once you identified new network that you need to pivot into, perform host discovery to find live hosts:

In this case, I performed host discovery for all the hosts within **192.168.122.1/24**

`time for i in $(seq 1 254); do (ping -c 1 192.168.122.${i} | grep "bytes from" &); done`

or 

`for i in {1..254} ;do (ping -c 1 172.16.5.$i | grep "bytes from" &) ;done`


![](https://i.imgur.com/rYmyDm5.png)

You can see that **192.168.122.4** and **192.168.122.5** is live above.


If on Windows CMD:

`for /L %i in (1 1 254) do ping 172.16.5.%i -n 1 -w 100 | find "Reply"`

If on Windows Powershell:

`1..254 | % {"172.16.5.$($_): $(Test-Connection -count 1 -comp 172.15.5.$($_) -quiet)"}`

## Port Scan
For port scanning, you can upload binary version of nmap to the system to run nmap on the compromised system but I somehow found just running **nc** scan more comfortable.

You can download static binary version of nmap from [here](https://github.com/andrew-d/static-binaries/blob/master/binaries/linux/x86_64/nmap).

`wget https://github.com/andrew-d/static-binaries/blob/master/binaries/linux/x86_64/nmap?raw=true -O nmap-static

**For Full Port Scan:**

`time for i in $(seq 1 65535); do (nc -zvn 192.168.122.5 ${i} 2>&1 | grep -v "Connection refused" &); done`

**For Ports 1-100 Scan:**

`nc -zv 192.168.122.4 1-100`

`time for i in $(seq 1 100); do (nc -zvn 192.168.122.5 ${i} 2>&1 | grep -v "Connection refused" &); done`

You can see here that port 22 is open in 192.168.122.4:

![](https://i.imgur.com/hth0q73.png)

If netcat is not found on system, try this way:


```bash
for port in {1..65535}; do echo > /dev/tcp/172.19.0.1/$port && echo "$port open"; done 2>/dev/null           
```

![](https://i.imgur.com/fiQtWb0.png)


## Things to Document and Track

- DNS records, network device backups, and DHCP configurations
- Full and current application inventory
- A list of all enterprise hosts and their location
- Users who have elevated permissions
- A list of any dual-homed hosts (More than one network interface)
- Keeping a visual network diagram of your environment

## Diagram

- [Netbrain](https://www.netbraintech.com/)
- [diagrams.net](https://app.diagrams.net/)


## References
- https://0xdf.gitlab.io/2019/01/28/pwk-notes-tunneling-update1.html
- https://0xdf.gitlab.io/2020/08/10/tunneling-with-chisel-and-ssf-update.html
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/HTB-Vault
- https://viperone.gitbook.io/pentest-everything/everything/pivoting-and-portforwarding