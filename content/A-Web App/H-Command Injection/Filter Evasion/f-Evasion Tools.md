---
title: f-Evasion Tools
draft: false
tags:
  - bashfuscator
  - dosfuscation
---
If we are dealing with advanced security tools, using automated obfuscation tools might be the best way.

## Linux - Bashfuscator

Installation:

```shell-session
jadu101@htb[/htb]$ git clone https://github.com/Bashfuscator/Bashfuscator
jadu101@htb[/htb]$ cd Bashfuscator
jadu101@htb[/htb]$ pip3 install setuptools==65
jadu101@htb[/htb]$ python3 setup.py install --user
```

Once we have the tool set up, we can start using it from the `./bashfuscator/bin/` directory.

We can start obfuscating as such:

```shell-session
jadu101@htb[/htb]$ ./bashfuscator -c 'cat /etc/passwd'

[+] Mutators used: Token/ForCode -> Command/Reverse
[+] Payload:
 ${*/+27\[X\(} ...SNIP...  ${*~}   
[+] Payload size: 1664 characters
```

We can set up to produce a shorter and simpler obfuscated command, as follows:

```shell-session
jadu101@htb[/htb]$ ./bashfuscator -c 'cat /etc/passwd' -s 1 -t 1 --no-mangling --layers 1

[+] Mutators used: Token/ForCode
[+] Payload:
eval "$(W0=(w \  t e c p s a \/ d);for Ll in 4 7 2 1 8 3 2 4 8 5 7 6 6 0 9;{ printf %s "${W0[$Ll]}";};)"
[+] Payload size: 104 characters
```


## Windows - DOSfuscation

Installation:

```powershell-session
PS C:\htb> git clone https://github.com/danielbohannon/Invoke-DOSfuscation.git
PS C:\htb> cd Invoke-DOSfuscation
PS C:\htb> Import-Module .\Invoke-DOSfuscation.psd1
PS C:\htb> Invoke-DOSfuscation
Invoke-DOSfuscation> help

HELP MENU :: Available options shown below:
[*]  Tutorial of how to use this tool             TUTORIAL
...SNIP...

Choose one of the below options:
[*] BINARY      Obfuscated binary syntax for cmd.exe & powershell.exe
[*] ENCODING    Environment variable encoding
[*] PAYLOAD     Obfuscated payload via DOSfuscation
```

We can start using the tool as such:

```powershell-session
Invoke-DOSfuscation> SET COMMAND type C:\Users\htb-student\Desktop\flag.txt
Invoke-DOSfuscation> encoding
Invoke-DOSfuscation\Encoding> 1

...SNIP...
Result:
typ%TEMP:~-3,-2% %CommonProgramFiles:~17,-11%:\Users\h%TMP:~-13,-12%b-stu%SystemRoot:~-4,-3%ent%TMP:~-19,-18%%ALLUSERSPROFILE:~-4,-3%esktop\flag.%TMP:~-13,-12%xt
```

