---
layout: page
title: Present
category: BINARY
type: ctf_cyberseagames2019
desc: Binary 100 Pts.
img-link: /writeups/ctf/cyberseagames2019/images/present_1.png
---

# [BINARY] Present - 100 Pts.
<br />
We were given this file:
<br />

![lol it didn't load](images/present_1.png)
<br />
After extracting the contents, we got present.exe.
We can also see what kind of file type it is. A PE binary but packed with UPX (Ultimate Packer for eXecutables)

![lol it didn't load](images/present_2.png)
<br />

We can unpack this using the UPX tool.

![lol it didn't load](images/present_3.png)
<br />

After unpacking, we can now see that it is a windows PE 32 bit. We can do static analysis against the unpacked PE to investigate what it really does.

![lol it didn't load](images/present_4.png)

![lol it didn't load](images/present_5.png)
<br />

To investigate the binary, we used Cutter (radare2 w/ GUI)

![lol it didn't load](images/present_6.png){:height="100%" width="100%"}

First, we have to find the main. From the image above, it is concluded
that the "main" identified by the Cutter (highlighted)
doesn't really do that's interesting. Therefore, we proceeded finding
other interesting functions

<br />
In the image below, we can see there are two mains:

```C
sym.__main and sym._main
```

![lol it didn't load](images/present_8.png)

Let's see what 

```sym.__main``` 

does... It looks like it only goes to the .ctors of the binary

![lol it didn't load](images/present_9.png)

Let's check ```sym._main```... There's an interesting string that's encoded in base64

![lol it didn't load](images/present_10.png)

<br />
After decoding, we got the flag.


![lol it didn't load](images/present_11.png)

:D :D :D

Thank you for reading!













