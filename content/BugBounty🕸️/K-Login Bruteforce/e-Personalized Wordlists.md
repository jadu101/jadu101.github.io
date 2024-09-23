---
title: e-Personalized Wordlists
draft: false
tags:
  - cupp
---
## CUPP

Many tools can create a custom password wordlist based on certain information. The tool we will be using is `cupp`.

We run it in interactive mode by specifying the `-i` argument, and answer the questions, as follows:

```shell-session
jadu101@htb[/htb]$ cupp -i

___________
   cupp.py!                 # Common
      \                     # User
       \   ,__,             # Passwords
        \  (oo)____         # Profiler
           (__)    )\
              ||--|| *      [ Muris Kurgas | j0rgan@remote-exploit.org ]
                            [ Mebus | https://github.com/Mebus/]


[+] Insert the information about the victim to make a dictionary
[+] If you don't know all the info, just hit enter when asked! ;)

> First Name: William
> Surname: Gates
> Nickname: Bill
> Birthdate (DDMMYYYY): 28101955

> Partners) name: Melinda
> Partners) nickname: Ann
> Partners) birthdate (DDMMYYYY): 15081964

> Child's name: Jennifer
> Child's nickname: Jenn
> Child's birthdate (DDMMYYYY): 26041996

> Pet's name: Nila
> Company name: Microsoft

> Do you want to add some key words about the victim? Y/[N]: Phoebe,Rory
> Do you want to add special chars at the end of words? Y/[N]: y
> Do you want to add some random numbers at the end of words? Y/[N]:y
> Leet mode? (i.e. leet = 1337) Y/[N]: y

[+] Now making a dictionary...
[+] Sorting list and removing duplicates...
[+] Saving dictionary to william.txt, counting 43368 words.
[+] Now load your pistolero with william.txt and shoot! Good luck!
```


## Password Policy


Let's say the victim's password must meet the following conditions:

1. 8 characters or longer
2. contains special characters
3. contains numbers

Let's use the commands below to remove any passwords that doesn't meet these conditions:

```bash
sed -ri '/^.{,7}$/d' william.txt            # remove shorter than 8
sed -ri '/[!-/:-@\[-`\{-~]+/!d' william.txt # remove no special chars
sed -ri '/[0-9]+/!d' william.txt            # remove no numbers
```

## Mangling

We never know how our target thinks when creating their password, and so our safest option is to add as many alterations and permutations as possible.

Many great tools do word mangling and case permutation quickly and easily, like [rsmangler](https://github.com/digininja/RSMangler) or [The Mentalist](https://github.com/sc0tfree/mentalist.git). These tools have many other options, which can make any small wordlist reach millions of lines long.

## Custom Username Wordlist

For example, the person's username could be `b.gates` or `gates` or `bill`, and many other potential variations.

One tool we can use is [Username Anarchy](https://github.com/urbanadventurer/username-anarchy), which we can clone from GitHub, as follows:

```bash
./username-anarchy Bill Gates > bill.txt
```

