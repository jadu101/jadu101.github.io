---
title: c-static analysis
draft: false
tags:
  - android
  - mobile
  - apktool
---


```cmd
C:\Users\secsh\Downloads\Android-InsecureBankv2>apktool d InsecureBankv2.apk -o InsecureBankv2_apktooled
I: Using Apktool 2.12.1 on InsecureBankv2.apk with 8 threads
I: Baksmaling classes.dex...
I: Loading resource table...
I: Decoding file-resources...
I: Loading resource table from file: C:\Users\secsh\AppData\Local\apktool\framework\1.apk
I: Decoding values */* XMLs...
I: Decoding AndroidManifest.xml with resources...
I: Copying original files...
I: Copying unknown files...
```

```cmd
C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled>dir
 Volume in drive C has no label.
 Volume Serial Number is 2ACD-1208

 Directory of C:\Users\secsh\Downloads\Android-InsecureBankv2\InsecureBankv2_apktooled

02/08/2026  11:05 AM    <DIR>          .
02/08/2026  11:04 AM    <DIR>          ..
02/08/2026  11:05 AM             4,162 AndroidManifest.xml
02/08/2026  11:05 AM               257 apktool.yml
02/08/2026  11:05 AM    <DIR>          original
02/08/2026  11:05 AM    <DIR>          res
02/08/2026  11:04 AM    <DIR>          smali
               2 File(s)          4,419 bytes
               5 Dir(s)  108,264,226,816 bytes free
```

실습 기준, 바로 터지는 것들:

AndroidManifest.xml

exported Activity / Service

res/values/strings.xml

hardcoded secrets

smali/.../LoginActivity.smali

인증 로직

smali/.../RequestDispatcher.smali

API 호출

assets/

hidden data

# Find Hardcoded Strings

could be found in resources/strings.xml and xmls.xml

also in activity source code

threat vector:
- login bypass
- url exposed
- API keys exposed
- Firebase URLs (firebase.io)

<img width="228" alt="image" src="https://github.com/user-attachments/assets/4319f0f1-99c8-46d6-b9db-030e755bb7ab" />

seach keywords:
- api
- password
- username
- firebase.io
- SQL
- key
- ClientID
- ClientSecret
- http://
- https://

<img width="567" alt="image" src="https://github.com/user-attachments/assets/df1332bf-4ea6-48ad-bedc-dca018074d19" />

