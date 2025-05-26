---
title: a-Intro
draft: false
tags:
  - ssti
---
## Template Engines

Template engines are often used by web apps to generate dynamic responses.

It combines pre-defined templates with dynamically generated data and is often used by web applications to generate dynamic responses.

An everyday use case for template engines is a website with shared headers and footers for all pages.


Popular examples of template engines are [Jinja](https://jinja.palletsprojects.com/en/3.1.x/) and [Twig](https://twig.symfony.com/).

## Templating

Template engines usually require two inputs:

- Set of values to be inserted into the template.
- Template.

```jinja2
Hello {{ name }}!
```

Above contains a single variable called `name` and it is replaced with a dynamic value during rendering.

If we provide the variable `name="peter"` to the rendering function, the template engine will generate `Hello peter!`.

```jinja2
{% for name in names %}
Hello {{ name }}!
{% endfor %}
```

For instance, if we pass the function with a list such as `names=["vautia", "21y4d", "Pedant"]`, the template engine will generate the following string:

```
Hello vautia!
Hello 21y4d!
Hello Pedant!
```

## SSTI

Server-Side Template Injection (SSTI) occurs when an attacker can inject templating code into a template that is later rendered by the server.

Using this, attacker can potentially execute the code during the rendering process, enabling an attacker to compromise the server.

