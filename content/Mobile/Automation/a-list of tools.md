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

## Methodology

Prepare APK
-  Always verify the APK signature with apksigner to make sure you're testing the legitimate production version and not a modified one.

### Static Analysis

#### apktool
analyze AndroidManifest.xml
what to look for
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
search for secrets

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
```

#### jadx gui

jadxgui
Navigate to interesting classes:

    com.app.util.ApiClient (API calls)
    com.app.auth.LoginActivity (authentication)
    com.app.storage.DatabaseHelper (data storage)

Look for

```
// Bad: Hardcoded credentials
String apiKey = "sk_live_abc123xyz789";

// Bad: Weak encryption
String password = Base64.encode(userPassword);  // Base64 is NOT encryption!

// Bad: Insecure storage
SharedPreferences prefs = context.getSharedPreferences("user_data", MODE_WORLD_READABLE);
prefs.edit().putString("password", userPassword).commit();
```

#### apk2url
- extract all URLs and endpoints hidden in the decompiled code.

#### MobSF 

### Dynamic Analysis
Burp Suite
Frida
Objection
drozer

