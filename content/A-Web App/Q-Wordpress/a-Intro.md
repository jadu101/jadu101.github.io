---
title: a-intro
draft: false
tags:
---
## Overview

[WordPress](https://wordpress.org/) is the most popular open source Content Management System (CMS), powering nearly one-third of all websites in the world. It can be used for multiple purposes, such as hosting blogs, forums, e-commerce, project management, document management, and much more.

 It has a large library of extensions called themes and plugins, both free and paid, that can be added to enhance the website.
 
WordPress is written in PHP and usually runs on Apache with MySQL as the backend.

## CMS

A CMS is a powerful tool that helps build a website without the need to code everything from scratch (or even know how to write code at all).

A CMS is made up of two key components:

- A Content Management Application (CMA) - the interface used to add and manage content.
- A Content Delivery Application (CDA) - the backend that takes the input entered into the CMA and assembles the code into a working, visually appealing website.

## WordPress Structure

For this module, we will focus on a default WordPress installation on an Ubuntu Linux web server.

WordPress requires a fully installed and configured LAMP stack (Linux operating system, Apache HTTP Server, MySQL database, and the PHP programming language) before installation on a Linux host. After installation, all WordPress supporting files and directories will be accessible in the webroot located at `/var/www/html`.

Below is the file structure:

```shell-session
jadu101@htb[/htb]$ tree -L 1 /var/www/html
.
├── index.php
├── license.txt
├── readme.html
├── wp-activate.php
├── wp-admin
├── wp-blog-header.php
├── wp-comments-post.php
├── wp-config.php
├── wp-config-sample.php
├── wp-content
├── wp-cron.php
├── wp-includes
├── wp-links-opml.php
├── wp-load.php
├── wp-login.php
├── wp-mail.php
├── wp-settings.php
├── wp-signup.php
├── wp-trackback.php
└── xmlrpc.php
```

- `wp-admin` folder contains the login page for administrator access and the backend dashboard. Once a user has logged in, they can make changes to the site based on their assigned permissions. The login page can be located at one of the following paths:
    
    - `/wp-admin/login.php`
    - `/wp-admin/wp-login.php`
    - `/login.php`
    - `/wp-login.php`

- `xmlrpc.php` is a file representing a feature of WordPress that enables data to be transmitted with HTTP acting as the transport mechanism and XML as the encoding mechanism. This type of communication has been replaced by the WordPress [REST API](https://developer.wordpress.org/rest-api/reference).

- The `wp-config.php` file contains information required by WordPress to connect to the database, such as the database name, database host, username and password, authentication keys and salts, and the database table prefix. This configuration file can also be used to activate DEBUG mode, which can useful in troubleshooting.

- The `wp-config.php` file contains information required by WordPress to connect to the database, such as the database name, database host, username and password, authentication keys and salts, and the database table prefix. This configuration file can also be used to activate DEBUG mode, which can useful in troubleshooting.

## WordPress User Roles

There are five types of users in a standard WordPress installation.

|Role|Description|
|---|---|
|Administrator|This user has access to administrative features within the website. This includes adding and deleting users and posts, as well as editing source code.|
|Editor|An editor can publish and manage posts, including the posts of other users.|
|Author|Authors can publish and manage their own posts.|
|Contributor|These users can write and manage their own posts but cannot publish them.|
|Subscriber|These are normal users who can browse posts and edit their profiles.|

Gaining access as an administrator is usually needed to obtain code execution on the server. However, editors and authors might have access to certain vulnerable plugins that normal users do not.

