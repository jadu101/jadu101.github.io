---
title: adb basics
draft: false
tags:
  - android
  - mobile
  - adb
---

# Installing adb

```cmd
C:\> Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
C:\> iex (New-Object System.Net.WebClient).DownloadString('https://get.scoop.sh')
C:\> scoop bucket add extras
C:\> scoop install adb
```

On Windows, the resulting platform tools including adb.exe are located at `C:\Users\<username>\AppData\Local\Android\Sdk\platform-tools`.

```cmd
PS C:\Users\Owner\AppData\Local\Android\Sdk\platform-tools> ls


    Directory: C:\Users\Owner\AppData\Local\Android\Sdk\platform-tools


Mode                 LastWriteTime         Length Name
----                 -------------         ------ ----
-a----         5/17/2024  12:24 PM        5857056 adb.exe
-a----         5/17/2024  12:24 PM         108320 AdbWinApi.dll
-a----         5/17/2024  12:24 PM          73504 AdbWinUsbApi.dll
-a----         5/17/2024  12:24 PM         439072 etc1tool.exe
-a----         5/17/2024  12:24 PM        1807136 fastboot.exe
-a----         5/17/2024  12:24 PM          54560 hprof-conv.exe
-a----         5/17/2024  12:24 PM         242128 libwinpthread-1.dll
-a----         5/17/2024  12:24 PM         477472 make_f2fs.exe
-a----         5/17/2024  12:24 PM         477472 make_f2fs_casefold.exe
-a----         5/17/2024  12:24 PM           1157 mke2fs.conf
-a----         5/17/2024  12:24 PM         754464 mke2fs.exe
-a----         5/17/2024  12:24 PM        1110529 NOTICE.txt
-a----         5/17/2024  12:24 PM          18404 package.xml
-a----         5/17/2024  12:24 PM             38 source.properties
-a----         5/17/2024  12:24 PM        2838304 sqlite3.exe```
```

To add adb to PATH environment variable, check out the following from [this article](https://theflutterist.medium.com/setting-up-adb-path-on-windows-android-tips-5b5cdaa9084b):

<img width="441" height="800" alt="image" src="https://github.com/user-attachments/assets/7477e55e-e1d4-4f3f-91e9-0101544e2f4e" />


# Using adb
