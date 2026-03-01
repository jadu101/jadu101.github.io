---
title: a-list of tools
draft: false
tags:
  - android
  - mobile
---

drozer

MobSF
Quark Engine
Frida (Objection)
Medusa

## Static Analysis


    Introduction
    Disassembling the APK
    Understanding Smali
    Reading Hardcoded Strings
    Bad Cryptography Implementation
    Reversing Hybrid Apps
    Reading Obfuscated Code
    Deobfuscating Code
    Reversing Shared Objects
    Reversing DLL Files
    Authentication Bypass
    Modifying Game Apps
    License Verification Bypass
    Root Detection Bypass
    Skills Assessment



## Dynamic Analysis


    Introduction
    Enumerating Local Storage
    Exported Activities
    Insecure Logging
    Pending Intents
    Exploiting WebViews
    Insecure Library Load Through Deep Linking
    Hooking Java Methods
    Altering Method Values
    Hooking Native Methods
    Bypassing Detection Mechanisms
    Authentication Token Manipulation
    Intercepting API Calls
    IDOR Attack
    SSL/TLS Certificate Pinning Bypass
    Skills Assessment


<img width="1755" height="851" alt="image" src="https://github.com/user-attachments/assets/75ca362b-13e7-4f79-8664-159b2c058dfd" />

Automation Tool -> Static on Insecurebank -> Dynamic on InsecureBank -> Hackerone VDP?

# Methodology
Prepare APK
-  Always verify the APK signature with apksigner to make sure you're testing the legitimate production version and not a modified one.

# Static Analysis

## apktool

`apktool` will give `smali` codes: `apktool d InsecureBankv2.apk -o InsecureBankv2_apktool`

Below are some that should be manually looked into:

    - AndroidManifest.xml
    - exported Activity / Service
    - res/values/strings.xml
    - hardcoded secrets
    - smali/…/LoginActivity.smali
    - smali/…/RequestDispatcher.smali

On `AndroidManifest.xml`, check for the followings:

```
<!-- Dangerous permissions -->
<uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.INTERNET"/>

<!-- Exported components (can be accessed by other apps) -->
<activity 
    android:name=".AdminActivity"
    android:exported="true">  ← VULNERABLE!
</activity>

<!-- Backup allowed (app data can be backed up) -->
<application
    android:allowBackup="true"  ← POTENTIAL ISSUE
    android:debuggable="true">  ← HUGE VULNERABILITY IF IN PRODUCTION
```

Search for interesting strings as well:

```
# Search for API keys
grep -r "api_key" .
grep -r "API_KEY" .
grep -r "apiKey" .

# Search for AWS credentials
grep -r "AKIA" .  # AWS Access Key pattern
grep -r "aws_secret" .

# Search for Firebase
grep -r "firebaseio.com" .

# Search for passwords
grep -r "password" .
grep -r "pwd" .

# Search for tokens
grep -r "token" .
grep -r "bearer" .

## Search for internet related
grep -r "http" .
grep -r "https" .
```

## jadx gui


Navigate to interesting classes:

  - com.app.util.ApiClient (API calls)
  - com.app.auth.LoginActivity (authentication)
  - com.app.storage.DatabaseHelper (data storage)

Search for:

```
// Bad: Hardcoded credentials
String apiKey = "sk_live_abc123xyz789";

// Bad: Weak encryption
String password = Base64.encode(userPassword);  // Base64 is NOT encryption!

// Bad: Insecure storage
SharedPreferences prefs = context.getSharedPreferences("user_data", MODE_WORLD_READABLE);
prefs.edit().putString("password", userPassword).commit();
```

## apk2url

- extract all URLs and endpoints hidden in the decompiled code.

This works only on Linux

## Nuclei

Download templates: `https://github.com/optiv/mobile-nuclei-templates`

`C:\Users\secsh\Downloads\Android_Pentests\nuclei_3.7.0_windows_amd64\nuclei.exe -target C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled -t C:\Users\secsh\Downloads\Android_Pentests\mobile-nuclei-templates-main -file`

```cmd
C:\Users\secsh\Downloads\Android-InsecureBankv2>C:\Users\secsh\Downloads\Android_Pentests\nuclei_3.7.0_windows_amd64\nuclei.exe -target C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled -t C:\Users\secsh\Downloads\Android_Pentests\mobile-nuclei-templates-main -file

                     __     _
   ____  __  _______/ /__  (_)
  / __ \/ / / / ___/ / _ \/ /
 / / / / /_/ / /__/ /  __/ /
/_/ /_/\__,_/\___/_/\___/_/   v3.7.0

                projectdiscovery.io

[ERR] Could not read nuclei-ignore file: open C:\Users\secsh\AppData\Roaming\nuclei\.nuclei-ignore: The system cannot find the file specified.
[INF] Current nuclei version: v3.7.0 (outdated)
[INF] Current nuclei-templates version:  (latest)
[INF] New templates added in latest release: 0
[INF] Templates loaded for current scan: 42
[WRN] Loading 42 unsigned templates for scan. Use with caution.
[INF] Targets loaded for current scan: 1
[android-debug-enabled] [file] [low] C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled\AndroidManifest.xml
[adb-backup-enabled] [file] [low] C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled\AndroidManifest.xml
```

## MobSF 

127.0.0.1:8000

<img width="1137" height="904" alt="image" src="https://github.com/user-attachments/assets/5b1e8d22-b9e8-4417-9905-8f3b77c1d5aa" />


# Dynamic Analysis
Burp Suite
## Frida

```
# pipx 설치
sudo apt install pipx -y
pipx ensurepath
# 터미널 재실행 후
pipx install frida-tools
```

Objection
drozer

resource 
