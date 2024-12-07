---
title: Port 389 - LDAP
draft: false
tags:
  - ldap
  - ldapsearch
  - awk
---
## ldapsearch

Query base naming contexts:

`ldapsearch -H ldap://10.10.10.169 -x -s base namingcontexts`

![](https://i.imgur.com/t5rqcmh.png)

Try binding to specific base:

`ldapsearch -H ldap://10.10.10.169 -x -b "DC=megabank,DC=local"`

![](https://i.imgur.com/BfBMIML.png)

With creds, attempt on bind:

`ldapsearch -H ldap://active.htb -D 'SVC_TGS@active.htb' -w 'GPPstillStandingStrong2k18' -b "DC=active,DC=htb" -vv | sudo tee ldapsearch-result-svc_tgs`


## Analyze Output

If the Bind Output is too long, analyze the data as such:

`cat xb-bind.txt | awk '{print $1}' | sort | uniq -c | sort -nr > xb-bind-sorted.txt`

You can also grep **sAMAccountName**:

`cat xb-bind.txt| grep -i 'samaccountname' | awk '{print $2}'`

