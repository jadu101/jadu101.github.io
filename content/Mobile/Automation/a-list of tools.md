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

apktool
apk2url
- extract all URLs and endpoints hidden in the decompiled code.

jadxgui
- Search globally for keywords like "api_key", "secret", "password", "token" and "firebase".

MobSF 
drozer

