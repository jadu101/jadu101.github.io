---
title: Tunneling and Pivoting
draft: false
tags:
  - pivoting
  - tunneling
  - chisel
---
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

![](https://i.imgur.com/rYmyDm5.png)

You can see that **192.168.122.4** and **192.168.122.5** is live above.

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

## Tunneling/Pivoting
### Chisel
There are many ways for tunneling but my favorite way is **chisel**.

[Here](https://0xdf.gitlab.io/2020/08/10/tunneling-with-chisel-and-ssf-update.html)is a good chisel cheatsheet created by 0xdf. 

Let's say you are on the compromised system and you found 192.168.122.4 host live and connected to your compromised system. It has port 80 open so I would have to tunnel to access that port 80 running on 192.168.122.4.

1. You first upload **chisel** to the compromised system using wget or curl.

![](https://i.imgur.com/bw7gY2r.png)

2. Now on your local machine, start Chisel server listening on whatever port you prefer (In this case 9000).

![](https://i.imgur.com/8uPSoY6.png)

3. Now on the compromised system, tunnel target machine's port to local machine's listener.

![](https://i.imgur.com/cOCJDW7.png)

4. Now on your local machine, you can access port 80 of 192.168.122.4 as if you are accessing service running locally. 

## References
- https://0xdf.gitlab.io/2019/01/28/pwk-notes-tunneling-update1.html
- https://0xdf.gitlab.io/2020/08/10/tunneling-with-chisel-and-ssf-update.html
- https://jadu101.github.io/Hackthebox%F0%9F%93%A6/HTB-Vault
- https://viperone.gitbook.io/pentest-everything/everything/pivoting-and-portforwarding