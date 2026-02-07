---
title: b-Installing InsecureBankV2
draft: false
tags:
  - android
  - mobile
  - jadx
  - androidinsecure
---

# InsecureBankV2 Installation

All of the steps could be found at [here](https://github.com/dineshshetty/Android-InsecureBankv2).

Assuming you already have AVD installed, firt download all the requirements using Python2.7:

`C:\Python27\python.exe -m pip install -r requirements.txt`

Navigate to the AndroLabServer folder in the downloaded InsecureBankv2 project
source code and use the below command to run the HTTP server on the default port
8888:

```cmd
C:\Users\secsh\Downloads\Android-InsecureBankv2\AndroLabServer>C:\Python27\python.exe app.py
The server is hosted on port: 8888
```

Now have the AVD setup properly:

<img width="335" alt="image" src="https://github.com/user-attachments/assets/c99b62d4-ab12-4b8e-8bc9-283ac4c1f6e0" />

Installing via the command `adb install InsecureBankv2.apk`, we can now view the app through the emulator:

<img width="316" alt="image" src="https://github.com/user-attachments/assets/735c5d3b-9c04-49ad-ad8d-744edbbba710" />

Signin using the following credentials: `jack/Jack@123$`

<img width="300" alt="image" src="https://github.com/user-attachments/assets/901b3b72-2198-4fc5-bca2-512126281e81" />

