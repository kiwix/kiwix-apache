#Apache Module for Kiwix & ZIM files
This is the home of what's currently a proof-of-concept (p-o-c) experiment to create a module for Apache 2 Web Servers that serves content from ZIM files.

The aim is to provide similar functionality to `kiwix-serve` where practical. 
## Prerequisites
- `zimlib` available as a Debian package
- Apache Server development environment

## Building the code
The build is currently performed by a crude, limited bash script `build.sh`. The script requires `sudo` to install the module.

## Configuring Apache to enable the module
The configuration process varies by platform. Debian provides tools to enable and disable modules, for other platforms hand-editing of the Apache Server configuration may be necessary.

### On Debian
Modules are included at startup of Apache. They're loaded from `/etc/apache2/mods-enabled/`. These are configured using the debian command `sudo a2enmod`. 

A sample module that suits the current limited capabilities is

```apache
LoadModule example_module      /usr/lib/apache2/modules/mod_example.so
<Location /example>
    SetHandler example
</Location>
```

## Loading the new module
Run `sudo service apache restart`

## Testing / using the new module
At the time of writing, the module is simply configured to `[URL]/example` and lists files in a known zim file called `wikipedia.zim`. Visit `http://localhost/example` to see it in action. However, first provide a zim file in one of the document folders of the Apache Server (this might not matter - TBD), on the test machine `/var/www/html/`. A small zim file such as Ray Charles http://download.kiwix.org/zim/wikipedia/wikipedia_en_ray_charles_2013-03.zim will do.

## Known limitations
Here are our known limitations, some will be removed or addressed as we enhance the code.

- Hardcoded behaviour and ZIM file
- Not designed for multi-use or repeated-use
- No logging and no integration with Apache logging or error reporting
- No way to test the functionality except interactively and manually
- The build process creates a non-portable library (according the the output of the build)
