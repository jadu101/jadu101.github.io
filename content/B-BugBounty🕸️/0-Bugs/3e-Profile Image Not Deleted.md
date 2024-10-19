---
title: Profile Image Not Deleted
draft: false
tags:
---
This is an easy bug you can find on profile picture field.

## Identify

1. Upload image as profile picture.
2. Open that image in new tab. We can either right click and open or look for upload directory on source code.
3. Now remove the profile picture.
4. Refresh the link that originally showed the profile picture. 
5. If the link is still accessible, it is confirmed vulnerable. 

## Impact

If the picture is containing any sensitive information, attacker can steel that information using the link.