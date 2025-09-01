<a id="readme-top" name="readme-top"></a>

<!-- ------------------------------
* TITLE, DESCRIPTION & CONTACT
------------------------------ -->
# 💻 webserv

<table>
    <tr>
        <th align="left" width="3333px">Score</th>
        <th align="left" width="3333px">Duration</th>
        <th align="left" width="3333px">Peer(s)</th>
    </tr>
    <tr>
        <td>✅ <b>125</b> / 100</td>
        <td>🕓 6 week(s)</td>
        <td>👷🏻 Yes</td>
    </tr>
</table>

<br>

This team project is a program designed to replicate the behavior of an **HTTP/1.1 web server**, using nginx as a reference. It is able to **listen to several ports** (defined by a complete configuration file) via sockets and **serve different websites**. It supports **GET**, **POST** and **DELETE** methods, manages **cookies/sessions** and **file uploads** on its own, and can delegate to CGIs when necessary.

[**⛓️ FILE : SUBJECT**](en.subject.pdf)


<br>

<!-- ------------------------------
* TABLE OF CONTENTS
------------------------------ -->
## 📋 Table of contents

- [**👀 Overview**](#readme-overview)
- [**📦 Requirements**](#readme-requirements)
- [**💾 Installation**](#readme-installation)
- [**⚡️ Usage**](#readme-usage)
- [**🛠️ Makefile**](#readme-makefile)
- [**👷🏻 Collaborators**](#readme-collaborators)
- [**📄 License**](#readme-license)

<a id="readme-overview" name="readme-overview"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* OVERVIEW
------------------------------ -->
## 👀 Overview

### Features

- Complies with [RFC 2616](https://datatracker.ietf.org/doc/html/rfc2616).
- Supports listening on multiple addresses/ports.
- Supports multiple websites.
- Supports GET, POST and DELETE methods (others are considered GET).
- Returns relevant HTTP error status.
- Returns relevant default HTTP error page to clients.
- Supports custom error file definition.
- Supports directory indexing.
- Supports simplified regexp (`*?`) for routes location.
- Supports simplified regexp (`*?`) for `server_name` directive.
- Supports aliasing for routes.
- Supports rooting for routes.
- Supports default index files.
- Supports redirections.
- Supports CGI.
- Supports file upload on its own.
- Supports session/cookie management on its own.

### Configuration

<table>
    <tr>
        <th align="left" width="500px">Directive</th>
        <th align="left" width="9500px">Description</th>
    </tr>
    <tr><td><code>listen</code></td><td>Address and port on which the server block will listen.</td></tr>
    <tr><td><code>server_name</code></td><td>Server name for which the server block should respond.</td></tr>
    <tr><td><code>max_body_size</code></td><td>Character limit accepted in the body of a request by the server block.</td></tr>
    <tr><td><code>errors_page</code></td><td>Defines an error page for one or many HTTP status for the server block.</td></tr>
    <tr><td><code>methods</code></td><td>HTTP methods accepted by the route.</td></tr>
    <tr><td><code>root</code></td><td>Route root.</td></tr>
    <tr><td><code>alias</code></td><td>Route alias.</td></tr>
    <tr><td><code>index</code></td><td>Default index file for the route.</td></tr>
    <tr><td><code>listing</code></td><td>Enable/Disable directory indexing.</td></tr>
    <tr><td><code>cgi_pass</code></td><td>CGI executable used by the route.</td></tr>
    <tr><td><code>upload_to</code></td><td>Directory in which uploaded files are stored.</td></tr>
    <tr><td><code>rewrite</code></td><td>Url to which the route redirects.</td></tr>
</table>

<a id="readme-requirements" name="readme-requirements"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* REQUIREMENTS
------------------------------ -->
## 📦 Requirements

```
sudo apt -y install clang-12 make
```

<a id="readme-installation" name="readme-installation"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* INSTALLATION
------------------------------ -->
## 💾 Installation

**1. Get the project**

```
git clone https://github.com/rilemko/42-webserv.git
```

<br>

**2. Build the project**

```
make -j$(nproc)
```

> **🔵 Info:** The `webserv` executable will be created in the project root directory.

<br>

**3. Run the project**

```
./webserv [config_file]
```

<a id="readme-usage" name="readme-usage"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* USAGE
------------------------------ -->
## ⚡️ Usage

**Configuration:**

```txt
{
    listen          0.0.0.0:3000
    server_name     *.example1.com *.example2.com
    max_body_size   2000000

    errors_page     400 403 404 www/errors/40x.html
    errors_page     500 www/errors/50x.html

    location / {
        methods         GET POST
        listing         off
        root            www
        index           index.php
        cgi_pass        /usr/bin/php-cgi
    }

    location *.php {
        methods         GET POST
        root            www/cgibin
        cgi_pass        /usr/bin/php-cgi
    }

    location *.jpg {
        methods         GET
        root            www/assets
    }

    location /static {
        methods         GET
        listing         on
        alias           www/static
    }

    location /upload {
        methods         POST DELETE
        listing         off
        alias           www/cgibin
        index           upload.php
        cgi_pass        /usr/bin/php-cgi
        upload_to       www/upload
    }

    location /redirection {
        rewrite         301 https://google.fr
    }
}
```

<a id="readme-makefile" name="readme-makefile"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* MAKEFILE
------------------------------ -->
## 🛠️ Makefile

<table>
    <tr>
        <th align="left" width="500px">Rule</th>
        <th align="left" width="9500px">Action</th>
    </tr>
    <tr><td><code>all</code></td><td>Compile.</td></tr>
    <tr><td><code>clean</code></td><td>Delete all generated object files</td></tr>
    <tr><td><code>fclean</code></td><td>Apply <code>clean</code>, <code>lclean</code>and delete the executable.</td></tr>
    <tr><td><code>lclean</code></td><td>Apply <code>fclean</code> to all used libraries.</td></tr>
    <tr><td><code>re</code></td><td>Apply <code>fclean</code> and force recompile.</td></tr>
</table>

[**⛓️ FILE : MAKEFILE**](Makefile)

<a id="readme-collaborators" name="readme-collaborators"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* COLLABORATORS
------------------------------ -->
## 👷🏻 Collaborators

Maxime U. - [@LeMagicienToz](https://github.com/LeMagicienToz)\
Raphaël D. - [@rdmrdm3](https://github.com/rdmrdm3)

<a id="readme-license" name="readme-license"></a>
<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>

<!-- ------------------------------
* LICENSE
------------------------------ -->
## 📄 License

This project is licensed under the terms of the **MIT** license.

[**⛓️ FILE : LICENSE**](LICENSE.md)

<p align="right"><b><a href="#readme-top">TOP ⬆️</a></b></p>
