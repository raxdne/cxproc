
# TODO

## Documentation

TODO: build presentation from `doc/*.*` using <impress.js>

TODO: update descriptions in Doxygen format #v13

## Features

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

## Security

TEST: check security #v13 ++
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

TODO: gprof and valgrind/cachegrind (KCachegrind) #v13

## Building

DONE: Binary without Microsoft Runtime DLL (at all requiered libs??)

TODO: statically linked binaries for ARMv7, x64_linux, x64_windows  #v13

TODO: provide a Flatpack

TODO: detail information in binaries #v13

TODO: MacOS binaries

## XML

TODO: Update of RELAX NG schemas (translation by [trang](http://www.thaiopensource.com/relaxng/trang-manual.html) ) #v13 ++

## Testing

test coverage by gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>

Test with text files from Gutenberg Project <https://www.gutenberg.org/>
        <https://www.gutenberg.org/files/3176/3176-0.txt>

## Deployment

TODO: CMake packaging (deb, rpm, tar.gz, zip, 7zip, NSI) #v13

## Scripting

TODO: JSON output from database, dir etc

Javascript

- Duktape
- node.js + V8
- REST based server

REQ: Tcl bindings "Tcxproc"

REQ: Python bindings

## Web server

IIS

HTTPS

nginx

node.js

## cxproc-httpd (Application Server)

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

## option

### pie

#### pie_csv

markup

       #begin_of_csv
       #import()
       <import type="csv" sep=",">A;B;C</import>
       <import name="a.csv" type="csv" sep=","/>
       <file name="a.csv" verbosity="4"/>

parse from UTF-8 memory buffer

#### pie_calendar

#### pie_text

REQ: append XML attributes to text markup
- `** TEST @assignee="abc" @date="1234567"`
- additional generic attributes as `[class="abc" cluster="cad" effort="1"]`
- JSON-like? `* Section json="[class:'abc', cluster:'cad', effort:1]"`

decoding of RFC1738 URL with Umlaute to UTF-8

TODO: change markup #v13

    <pre></pre>
    <script display="yes"></script>

#### pie_timezone

TEST: sunrise sunset with Shanghai, Berlin, Boston

REQ: calculate different time zone values

### sqlite

REQ: transparent SQL to XML access  `xmlRead("sql:///tmp/abc.db3:SELECT * FROM table;")` ("xmlsqlio", similar to "xmlzipio")

### plain

REQ: HTML to UTF-8 Plain text Renderer `<cxp:plain><cxp:html name="abc.html"/></cxp:plain>` like "w3m -dump"

## Misc

### markdown

<https://daringfireball.net/projects/markdown/basics>

Markdown parser <https://css-tricks.com/choosing-right-markdown-parser/>

### image

ImageMagick

- use „VisualMagick configuration tool“ for VC++ Project setup on Windows
- Magic Scripting Language <http://www.linux-nantes.org/~fmonnier/ocaml/ml-conjure/documentation.html>
- Conjure <http://support.cs.nott.ac.uk/help/docs/image/ImageMagick/www/conjure.html>

- additional image processing instructions as elements
- embed comments into image files

    <cxp:image to="result.png">
      <cxp:image name="source.jpg" cache="yes"/>
      <cxp:scale="0.5"/>
      <cxp:orientation="auto"/>
      <cxp:comment>This is a Test!</cxp:comment>
      <cxp:scale="0.5"/>
      <cxp:scale="300"/>
    </cxp:image>

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

### STL/librp

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

- s. <https://transpect.github.io/index.html>

The [JasperReports Library](http://community.jaspersoft.com/project/jasperreports-library) is the world's most popular open source reporting engine.

[XMLStarlet command line XML toolkit](https://sourceforge.net/projects/xmlstar/)

### CSV

<https://github.com/dinedal/textql>

https://github.com/harelba/q

<https://stackoverflow.com/questions/12911299/read-csv-file-in-c>

<https://github.com/JamesRamm/csv_parser>

<https://github.com/rgamble/libcsv>


