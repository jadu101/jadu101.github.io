---
title: a-adb basics
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


## Interactions

To list all the applications installed:

```cmd
emu64xa:/ # pm list packages
package:com.android.companiondevicemanager.auto_generated_characteristics_rro
package:com.android.systemui.auto_generated_rro_vendor__
package:com.google.android.providers.media.module
```

or

```cmd
emu64xa:/ # ls -l /data/data
total 2056
drwx------  4 system         system         4096 2026-01-29 12:48 android
drwx------  4 u0_a95         u0_a95         4096 2026-01-29 12:48 android.auto_generated_characteristics_rro
drwx------  4 u0_a96         u0_a96         4096 2026-01-29 12:48 android.auto_generated_rro_product__
```

To look for specific application:

```cmd
emu64xa:/ # pm list packages | grep hack
package:com.hackthebox.myapp
```

To find out where the app is installed at:

```cmd
emu64xa:/ # pm path com.hackthebox.myapp
package:/data/app/~~T4SGxhALvs-sdPMp5uX1nw==/com.hackthebox.myapp-D6UwOMlfwXrxsjvC_nCuOQ==/base.apk
```

We can pull the app from virtual device and analyze it on our host machine:

```cmd
C:\Users\secsh\Downloads>mkdir APKFolder
C:\Users\secsh\Downloads>adb pull /data/app/~~T4SGxhALvs-sdPMp5uX1nw==/com.hackthebox.myapp-D6UwOMlfwXrxsjvC_nCuOQ==/base.apk testapp_pulled.apk
/data/app/~~T4SGxhALvs-sdPMp5uX1nw==/com.hackthebox.myapp-D6UwOMlfwXrxsjvC_n...OQ==/base.apk: 1 file pulled, 0 skipped. 50.1 MB/s (4969680 bytes in 0.095s)
```

Loading the pulled app through `jadx`, we are ready for static analysis:

<img width="690" alt="image" src="https://github.com/user-attachments/assets/c7fe2045-e371-4e0f-8935-3c8d0744836f" />

## Using AndroidStudio for Interaction

Using Android Studio for uploading files might be easier to some people:

<img width="394" alt="image" src="https://github.com/user-attachments/assets/0db8bae2-ca14-44aa-8493-e0456fab8339" />


<img width="284" alt="image" src="https://github.com/user-attachments/assets/ca8a75db-98bb-44b4-80e2-8426c4040cf8" />





