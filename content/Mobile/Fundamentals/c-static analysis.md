---
title: c-static analysis
draft: false
tags:
  - android
  - mobile
  - apktool
  - dex2jar
  - jadxgui
---

## apktool

`apktool` will provide raw smali code.

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

## dex2jar -> jadx gui

``` 
C:\Users\secsh\Downloads\Android_Pentests\dex-tools-v2.4\dex-tools-v2.4>d2j-dex2jar.bat -f InsecureBankv2.apk
dex2jar InsecureBankv2.apk -> .\InsecureBankv2-dex2jar.jar

C:\Users\secsh\Downloads\Android_Pentests\dex-tools-v2.4\dex-tools-v2.4>dir *jar
 Volume in drive C has no label.
 Volume Serial Number is 2ACD-1208

 Directory of C:\Users\secsh\Downloads\Android_Pentests\dex-tools-v2.4\dex-tools-v2.4

02/15/2026  03:39 PM         6,944,741 InsecureBankv2-dex2jar.jar
               1 File(s)      6,944,741 bytes
               0 Dir(s)  122,810,023,936 bytes free
```


## Explore File System

`shared_prefs`, `databases`

```
generic_x86:/data/data/com.android.insecurebankv2 # ls -l
total 28
drwxrwx--x 2 u0_a77 u0_a77       4096 2026-02-15 16:36 app_textures
drwx------ 3 u0_a77 u0_a77       4096 2026-02-15 16:36 app_webview
drwxrws--x 3 u0_a77 u0_a77_cache 4096 2026-02-15 16:36 cache
drwxrws--x 2 u0_a77 u0_a77_cache 4096 2026-02-15 14:27 code_cache
drwxrwx--x 2 u0_a77 u0_a77       4096 2026-02-15 14:27 databases
drwxrwx--x 2 u0_a77 u0_a77       4096 2026-02-15 16:36 files
drwxrwx--x 2 u0_a77 u0_a77       4096 2026-02-15 16:36 shared_prefs
```

```
generic_x86:/data/data/com.android.insecurebankv2/databases # ls -l
total 20
-rw-rw---- 1 u0_a77 u0_a77 20480 2026-02-15 14:27 mydb
-rw-rw---- 1 u0_a77 u0_a77     0 2026-02-15 14:27 mydb-journal
```

```
generic_x86:/data/data/com.android.insecurebankv2/databases # sqlite3 mydb
SQLite version 3.18.2 2017-07-21 07:56:09
Enter ".help" for usage hints.
sqlite> .tables
android_metadata  names
sqlite> select * from android_metadata
   ...> ;
en_US
```


# LogCat Info Leaks

search password, login etc on logcat
