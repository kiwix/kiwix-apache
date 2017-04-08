# Apache Module for Kiwix & ZIM files
This is the home of what's currently a proof-of-concept (p-o-c) experiment to create a module for Apache2.4 Web Servers that serves content from ZIM files.

The aim is to provide similar functionality to [`kiwix-serve`](https://github.com/kiwix/kiwix-tools/tree/master/src/server "Kiwix Server source code") where practical. 
## Prerequisites
- `zimlib` available as a Debian package: `sudo apt-get install libzim-dev`
- Apache Server development environment: `sudo apt-get install apache2-dev`

## Building the code
The build is currently performed by a crude, limited bash script `build.sh`. The script requires `sudo` to install the module.

## Configuring Apache to enable the module
The configuration process varies by platform. Debian provides tools to enable and disable modules, for other platforms hand-editing of the Apache Server configuration may be necessary.

### On Debian
Modules are included at startup of Apache. They're loaded from `/etc/apache2/mods-enabled/`. These are configured using the debian command `sudo a2enmod`. 

A sample module that suits the current limited capabilities is

```apache
LoadModule kiwix_module      /usr/lib/apache2/modules/mod_kiwix.so
<Location /kiwix>
    SetHandler kiwix
</Location>
```

## Loading the new module
Run `sudo service apache restart`

## Testing / using the new module
`[URL]/kiwix/` 

The module is curerntly designed to serve content from a single ZIM file, and serves content from a zim file called `wikipedia.zim`. Visit http://localhost/kiwix/ to see it in action. However, first provide a zim file in one of the document folders of the Apache Server (this might not matter - TBD), on the test machine `/var/www/html/`. A small zim file such as Ray Charles http://download.kiwix.org/zim/wikipedia/wikipedia_en_ray_charles_2013-03.zim will do.

## Known limitations
Here are our known limitations, some will be removed or addressed as we enhance the code.

- Hardcoded behaviour and ZIM file
- Not designed for multi-use or repeated-use
- Incomplete logging to the configured Apache error log e.g. `/var/log/apache2/error.log`
- No way to test the functionality except interactively and manually
- The build process creates a non-portable library (according the the output of the build)

## Acknowledgements
Thank you to various people who provided help both directly and indirectly.
- For demonstrating how to compile and build an Apache module in `C++`: http://stackoverflow.com/questions/42605138/how-can-i-write-an-apache-module-in-c
- Fellow Kiwix contributors for helping integrate `kiwixlib` into the code and for what else they're about to help with :)
- A special thanks to @kelson for allowing reuse of various code from other kiwix projects so we can still release this with the Apache 2.0 license.
