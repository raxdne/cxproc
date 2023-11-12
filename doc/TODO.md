
# TODO

## v2.0 #v20

TODO: remove legacy code

TODO: fix testing code #v20

TODO: review code internal tags

TODO: update Windows libraries and vcpkg ✔

TODO: prepare as a package for `vcpkg` #v20

## Documentation

TODO: update descriptions in Doxygen format #v20

s. `./option/*/TODO.md`

TODO: update program description #v20

## Features

TODO: remove legacy element names #v20 ✔

REQ: element `<skip>` #v20 ✔

REQ: shell environment

    <xsl:thread>
	  <cxp:system execute="dir %TEMP%">
		<cxp:env name="TEMP" select="c:\temp"/>
	  </cxp:system>
    </xsl:thread>

REQ: parser for source codes C, LaTeX etc
- detect TODO|BUG|TEST etc in comments of procedure context

REQ: Dialog for log messages and confirmations (á la ‚dialog‘ utility, s. Gtkdialog, GTK2-window)
- Tinyfiledialogs <http://sourceforge.net/projects/tinyfiledialogs/>
- wxwidgets <http://docs.wxwidgets.org/trunk/index.html>
- <http://www.fltk.org/index.php>
- <http://iup.sourceforge.net/>
- <http://cpptk.sourceforge.net/index.html>

REQ: additional Backends
- ZIP file
- URL
  - http
  - Web server
  - Application server
- VCS
  - Subversion
  - Git
- Document DB
  - SQLite
  - MongoDB <http://en.wikipedia.org/wiki/MongoDB>
  -	MySQL/MariaDB

REQ: use environment variable `LANG` for localization of calendar, articles etc ✔

DONE: cleanup use of cxpXslParamProcess(), params required? (script, XSL)

DONE: cleanup AJAX code (not required on server side)

#### calendar

TEST: sunrise sunset with Shanghai, Berlin, Boston ✘

REQ: calculate different time zone values

REQ: handle timezone in date node
- TODO: code refactoring using libc database (s. https://en.wikipedia.org/wiki/Tz_database)

### plain

REQ: HTML to UTF-8 Plain text Renderer `<cxp:plain><cxp:html name="abc.html"/></cxp:plain>` like "w3m -dump" ✘

## Security

TEST: check security #v20 ++
- access control in CGI mode
- HTTP Status message if access denied

Limitations or Sandbox
- use Unicode functions to avoid MAX_PATH on Windows
- prevent system() in cgi_mode (or readonly_mode)
- Docker

REQ: flags for permissions

	<cxp:make readonly="no|yes|write|unlink|create"> "rcwd"
	<cxp:make network="no">

## Profiling

TODO: gprof and valgrind/cachegrind (KCachegrind) #v20

## Building

DONE: Binary without Microsoft Runtime DLL (at all requiered libs??)

TODO: statically linked binaries for ARMv7, x64_linux, x64_windows (using `vcpkg`) ✘

TODO: provide a Flatpack #v20

TODO: detail information in binaries #v20

TODO: MacOS binaries

## XML

TODO: Update of RELAX NG schemas (translation by [trang](http://www.thaiopensource.com/relaxng/trang-manual.html) ) #v20

## Testing

test coverage by gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>

Test with text files from Gutenberg Project <https://www.gutenberg.org/>
        <https://www.gutenberg.org/files/3176/3176-0.txt>

## Deployment

DONE: CMake packaging (tar.gz, zip) #v13

TODO: CMake packaging (deb, rpm, NSI) #v20

## Scripting

TODO: JSON output from database, dir etc

Javascript

- Duktape
- node.js + V8
- REST based server

REQ: Tcl bindings "Tcxproc"

REQ: Python bindings
- lxml
- http://xmlsoft.org/xslt/python.html

## Web server

Apache HTTPD ✔

IIS

HTTPS

nginx

node.js

## cxproc-httpd (Application Server) ???

Reverse Proxy

    <https://en.wikipedia.org/wiki/Reverse_proxy>
    <https://www.gnu.org/software/libmicrohttpd/>
    Apache
        <https://httpd.apache.org/docs/2.4/howto/reverse_proxy.html>
    IIS
        <https://www.iis.net/learn/extensions/configuring-application-request-routing-arr/creating-a-forward-proxy-using-application-request-routing>
        <http://stackoverflow.com/questions/4739669/how-to-set-proxy-settings-for-iis-processes>
        <https://blogs.msdn.microsoft.com/friis/2016/08/25/setup-iis-with-url-rewrite-as-a-reverse-proxy-for-real-world-apps/>
        <https://www.iis.net/learn/extensions/url-rewrite-module/reverse-proxy-rule-template>
        <https://www.iis.net/learn/extensions/url-rewrite-module/modifying-http-response-headers>

Authentification

REST Principles

## Examples

### exifmover

TEST: on Linux and Windows

### DiskUsage

TEST: on Linux and Windows

### myclean.cxp

TEST: on Linux and Windows

## Misc

### gallery

- add a "move/copy Picture to" option
- sorting of large picture collections
- button "move to trash"
- thumbnails?
- to fix

### audioserver

- encoding of XML special chars in path names
- playlist formats xspf, m3u
- playlist from Archive file

### STL/librp ✘

- integrate [admesh](https://github.com/admesh)
- output of
  - Meta info (Bounding-Box, Facets, ...)
  - XML based formats ([X3D](http://xml.coverpages.org/vrml-X3D.html) )
  - other formats (VRML, GnuPlot, PovRay)
- processing, slicing using libadmesh and librp
  - Slices as SVG
  - runtime diagram

## References

see REST principles

„XProc: An XML Pipeline Language“ <http://www.w3.org/TR/xproc/>
- https://github.com/topics/xproc
- <https://transpect.github.io/index.html>

The [JasperReports Library](http://community.jaspersoft.com/project/jasperreports-library) is the world's most popular open source reporting engine.

[XMLStarlet command line XML toolkit](https://sourceforge.net/projects/xmlstar/)

