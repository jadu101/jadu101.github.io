---
title: d-drozer
draft: false
tags:
  - android
  - mobile
  - drozer
---

https://github.com/ReversecLabs/drozer

`pip install C:\Users\secsh\Downloads\drozer-3.1.0-py3-none-any.whl`

<img width="341" alt="image" src="https://github.com/user-attachments/assets/4e46ba6c-8b87-47b6-acb3-d52638b22ae6" />

<img width="409" alt="image" src="https://github.com/user-attachments/assets/a52e05d8-c1ea-4996-9b98-7014863d9a32" />


<img width="200" height="168" alt="image" src="https://github.com/user-attachments/assets/c6a2467f-961f-42ae-ae20-63a719bfc533" />

<img width="339" height="549" alt="image" src="https://github.com/user-attachments/assets/1a4026ab-3c04-43ab-97a9-3cce00fcf5b8" />

<img width="378" height="766" alt="image" src="https://github.com/user-attachments/assets/ed932ea3-4c82-4c56-93d8-b09a68eb5be7" />

This sets up a port forwarding between your PC and the emulator.

On your PC: connecting to localhost:31415 is forwarded to emulator-5554:31415.

This allows Drozer (on your PC) to talk to the agent (on the emulator) without needing an IP address.

<img width="617" height="441" alt="image" src="https://github.com/user-attachments/assets/2d0724ce-ed71-4391-bcba-e45bde592f5a" />

 run app.package.info -a com.android.insecurebankv2

<img width="755" height="515" alt="image" src="https://github.com/user-attachments/assets/9754027a-59de-4b56-aff8-99bd65d6a79f" />

 run app.package.attacksurface com.android.insecurebankv2

<img width="548" height="149" alt="image" src="https://github.com/user-attachments/assets/912f3575-a431-4c89-8b0c-337df7ba291c" />

5 activities exported → These are screens (Activities) in the app that other apps could potentially start without restriction. Exported activities are often attack vectors.

1 broadcast receiver exported → A receiver that can listen to system or custom intents from other apps. Could be abused if not protected.

1 content provider exported → A data-sharing component that other apps can query or modify. Needs proper permissions.

0 services exported → No background services are exported, so less risk there.

is debuggable → The app is marked as debuggable, which makes it easier to attack (you can attach debuggers, access private data, etc.).

run app.activity.info -a com.android.insecurebankv2

<img width="509" height="250" alt="image" src="https://github.com/user-attachments/assets/43e93f19-c216-4e1e-bcdc-ef0b742aaa7f" />


Each line shows an activity name and its required permission.

Permission: null → No permission is required, so any other app could potentially start these activities.

Activities like DoTransfer or ViewStatement are especially sensitive because they could allow money transfers or data viewing without authentication if exploited.


<img width="340" height="488" alt="image" src="https://github.com/user-attachments/assets/a0cb9ce1-a369-49dc-bfc4-91cff0d3ef82" />

```
dz> run app.activity.start --component com.android.insecurebankv2 com.android.insecurebankv2.PostLogin
Attempting to run shell module
```

<img width="343" height="461" alt="image" src="https://github.com/user-attachments/assets/374ab243-d4aa-45e8-9278-1852c9db74df" />

