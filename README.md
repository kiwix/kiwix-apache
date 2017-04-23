# Apache Module for Kiwix & ZIM files
This is the home of what's currently a proof-of-concept (p-o-c) experiment to create a module for Apache2.4 Web Servers that serves content from ZIM files.

The aim is to provide similar functionality to [`kiwix-serve`](https://github.com/kiwix/kiwix-tools/tree/master/src/server "Kiwix Server source code") where practical. 
## Prerequisites
- `zimlib` available as a Debian package: `sudo apt-get install libzim-dev`
- Apache Server development environment: `sudo apt-get install apache2-dev`
- `kiwix-build` must have been compiled locally. Follow instructions from https://github.com/kiwix/kiwix-build/

## Building the code
The build is currently performed by a crude, limited bash script `build.sh`. The script requires `sudo` to install the module.
For now, You'll have to manually edit the build.sh script to change the path where your `kiwix-build` has been compiled.

## Configuring Apache to enable the module
The configuration process varies by platform. Debian provides tools to enable and disable modules, for other platforms hand-editing of the Apache Server configuration may be necessary.

### On Debian
Modules are included at startup of Apache. They're loaded from `/etc/apache2/mods-enabled/`. These are configured using the debian command `sudo a2enmod`. 

A sample module that suits the current limited capabilities is the following. You can create a file /etc/apache2/mods-available/kiwix.load with this content, then run `sudo a2enmod kiwix` :

```apache
LoadModule kiwix_module      /usr/lib/apache2/modules/mod_kiwix.so
<Location /kiwix>
    SetHandler kiwix
</Location>
```

### Configuring the module for the ZIM file and location
This module uses a configuration section to specify the path and ZIM filename. 
For now it's only been tested in a known frequently-used location on Apache running on Debian and Ubuntu : /var/www/html. 
The file is called `kiwix.conf` and a sample is available in the `sample-configs/` folder in this repo.

Here's an example of the contents (which can be listed when the module is loaded using the `./config` page, see below).

```apache
ZimFile "wiktionary_fr_all_2016-11.zim"
ZimPath "/var/www/html/"
```
You can copy this file to the relevant folder using `sudo` (as the config folder is owned by `root`) e.g.

`sudo cp ./sample-configs/kiwix.conf /etc/apache2/conf-available/kiwix.conf`

Similarly you need `sudo` to edit this file.
`sudo vi /etc/apache2/conf-available/kiwix.conf`

Enable the additional config file. On Debian/Ubuntu use `sudo a2enconf kiwix`. (To disable the config e.g. if the server refuses to start, use `sudo a2disconf kiwix`).

And finally, you'll need to have the relevant ZIM file in the specified location where `root` has access to it. Read access seems to be enough. 
 
## Loading the new module
Run `sudo service apache2 restart`

## Testing / using the new module
The main root URL to serve Kiwix content is `[URL]/kiwix/`. The service will also respond to `[URL]/kiwix/config` that lists the config of the Kiwix module e.g.

```apache
ZimFile "wiktionary_fr_all_2016-11.zim"
ZimPath "/var/www/html/"
```

and `[URL]/kiwix/status`
which doesn't do much yet, but should at least give you some confidence the module is loaded provided it says:
`I'm OK, thank you, and you?` 

The module is curerntly designed to serve content from a single ZIM file, and serves content from the zim file configured in the config. Visit http://localhost/kiwix/ to see it in action. However, first provide a zim file in one of the document folders of the Apache Server (this might not matter - TBD), on the test machine `/var/www/html/`. A small zim file such as Ray Charles http://download.kiwix.org/zim/wikipedia/wikipedia_en_ray_charles_2013-03.zim will do.

## Known limitations
Here are our known limitations, some will be removed or addressed as we enhance the code.

- Not designed for multi-use or repeated-use
- Incomplete logging to the configured Apache error log e.g. `/var/log/apache2/error.log`
- No way to test the functionality except interactively and manually
- The build process creates a non-portable library (according the the output of the build)
- Needs `kiwixlib` to be built on the same machine.

## Acknowledgements
Thank you to various people who provided help both directly and indirectly.
- For demonstrating how to compile and build an Apache module in `C++`: http://stackoverflow.com/questions/42605138/how-can-i-write-an-apache-module-in-c
- Fellow Kiwix contributors for helping integrate `kiwixlib` into the code and for what else they're about to help with :)
- A special thanks to @kelson for allowing reuse of various code from other kiwix projects so we can still release this with the Apache 2.0 license.
