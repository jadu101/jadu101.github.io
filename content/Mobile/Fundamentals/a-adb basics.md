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

<img width="441" alt="image" src="https://github.com/user-attachments/assets/7477e55e-e1d4-4f3f-91e9-0101544e2f4e" />


# Using adb
To start the adb server: `adb start-server`

<img width="300" alt="image" src="https://github.com/user-attachments/assets/13ab970d-d9d4-45b0-98e2-041135c20465" />

The following adb command executes the whoami command inside the emulated device and prints the result to the terminal.

```cmd
PS C:\WINDOWS\system32> adb shell whoami
shell
```

We can now start executing Linux commands inside the emulator.

```cmd
PS C:\WINDOWS\system32> adb shell
emu64xa:/ $ ls
adb_keys        cache        debug_ramdisk    linkerconfig  oem          second_stage_resources  system_ext
apex            config       dev              metadata      postinstall  storage                 tmp
bin             d            etc              mnt           proc         sys                     vendor
bootstrap-apex  data         init             odm           product      system                  vendor_dlkm
bugreports      data_mirror  init.environ.rc  odm_dlkm      sdcard       system_dlkm
```

## Installing Apps and Transferring Files

To install apks: `adb install myapp.apk `

To push files from the host to the device, we can execute the following command.

```cmd
PS C:\Users\secsh\Downloads> adb push .\test.txt /sdcard/Download
.\test.txt: 1 file pushed, 0 skipped. 0.0 MB/s (16 bytes in 0.001s)
PS C:\Users\secsh\Downloads> adb shell ls -l /sdcard/Download/
total 8
-rw-rw---- 1 u0_a214 media_rw 16 2026-02-07 13:14 test.txt
```

Similarly, we pull files from the device to the host by executing the following command: `adb pull /sdcard/Download/myapp.apk .`.







