
# TODO

TODO: remove "#ifdef EXPERIMENTAL"

## Documentation

build presentation from `doc/*.*` using <impress.js>

## Features

shell environment

    <xsl:thread>
	<cxp:system execute="dir c:\temp">
		<!-- set shell environment -->
		<cxp:env name="TEMP" select="c:\temp"/>
	</cxp:system>
    </xsl:thread>

parser for source codes C, LaTeX etc detect TODO|BUG|TEST etc in comments of procedure context

## GUI

s. Gtkdialog, GTK2-window for log messages and confirmations (á la ‚dialog‘ utility)

- <http://jplugins.directory/ferromenu/?show=web>
- <http://www.gtk.org/download/win32_tutorial.php>
- Tinyfiledialogs <http://sourceforge.net/projects/tinyfiledialogs/>
- wxwidgets <http://docs.wxwidgets.org/trunk/index.html>
- <http://www.fltk.org/index.php>
- <http://iup.sourceforge.net/>
- <http://cpptk.sourceforge.net/index.html>

## STL/librp

- integrate [admesh](https://github.com/admesh)
- output of
  - Meta info (Bounding-Box, Facets, ...)
  - XML based formats ([X3D](http://xml.coverpages.org/vrml-X3D.html) )
  - other formats (VRML, GnuPlot, PovRay)
- processing, slicing using libadmesh and librp
  - Slices as SVG
  - runtime diagram

## Backends

- Filesystem
- ZIP file
- URL
  - http
  - Web server
  - Application server
- VCS
  - Subversion
  - Git
- Document DB
  - MongoDB <http://en.wikipedia.org/wiki/MongoDB>
  -	MySQL/MariaDB

## CGI

TODO: check security ++

- access control in CGI mode
- HTTP Status message if access denied

## Security

- Limitations or Sandbox
  - use Unicode functions to avoid MAX_PATH on Windows
  - <cxp:make readonly="no|yes|write|unlink|create"> "rcwd"
  - <cxp:make network="no">
  - prevent system() in cgi_mode (or readonly_mode)
- <https://www.tu-braunschweig.de/Medien-DB/sec/pubs/2016-ccs.pdf>
- Docker

## Building

TODO: statically linked binaries for ARMv7, x64_linux, x64_windows?

TODO: detail information in binaries

TODO: pack library frontends in bin/ xmllint, xsltproc, zip, curl ...

TODO: Binary without Microsoft Runtime DLL (at all requiered libs??)

VC++

- Side-by-side assembly <https://en.wikipedia.org/wiki/Side-by-side_assembly>
        <https://www.codeguru.com/cpp/article.php/c18071/Simplify-Visual-C-Application-Deployment-with-Manifest-Files.htm>
        <https://docs.microsoft.com/de-de/cpp/build/understanding-manifest-generation-for-c-cpp-programs?view=vs-2019>

  - target_link_options(cxproc-cgi     PUBLIC /MANIFEST /ManifestFile:${CXPROC_SRC_DIR}/cxproc.manifest)

  - ADD_CUSTOM_COMMAND( 	TARGET cxproc-cgi 	POST_BUILD 	COMMAND "mt.exe" -manifest \"${CXPROC_SRC_DIR}/cxproc.manifest\" -inputresource:\"$<TARGET_FILE:cxproc-cgi>\"\;\#1 -outputresource:\"$<TARGET_FILE:cxproc-cgi>\"\;\#1 	COMMENT "Adding display aware manifest..."  ) 

MacOS?

TODO: test LLVM

TODO: Package Information for pkg-config in cxproc.pc.in (s. xml2-config)

## Code quality

- compiler warnings C, C++
- splint
- memcheck
- VC++ code analysis
- Valgrind/memcheck (s. ctest)

## Profiling

TODO: gprof and valgrind/cachegrind (KCachegrind)

## XML

TODO: Update of RELAX NG schemas (translation by [trang](http://www.thaiopensource.com/relaxng/trang-manual.html) )

## Testing

test coverage by gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>

Test with text files from Gutenberg Project <https://www.gutenberg.org/>
        <https://www.gutenberg.org/files/3176/3176-0.txt>

## Deployment

TODO: CMake packaging

Packages

- deb
- rpm
- tar.gz
- zip
- exe
  - 7zip
  - NSI

Hosting

- Github
- Web server

## Scripting

TODO: JSON output from database, dir etc

Javascript

- Duktape
- node.js + V8
- REST based server
- Frontend

Tcl

- as package (no embedded Tcl interpreter)
- Tcl bindings "Tcxproc"

Python

use cases

- XML creation (from plain text)
- plain XML modifications before parsing
- regexp tasks in XML/XSL
- preprocessor-like embedding in XML/XHTML, JS, CSS
- JSON generator and tests
- automatic tests

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

### sqlite

Module "xmlsqlio" (similar to "xmlzipio")

- transparent SQL to XML access
- xmlRead("sql:///tmp/abc.db3:SELECT * FROM table;")

### plain

HTML to UTF-8 Plain text Renderer `<cxp:plain><cxp:html name="abc.html"/></cxp:plain>` like "w3m -dump"

### pie

#### pie_csv

markup

       #begin_of_csv
       #import()
       <import type="csv" sep=",">A;B;C</import>
       <import name="a.csv" type="csv" sep=","/>
       <file name="a.csv" verbosity="4"/>

parse from UTF-8 memory buffer

examples

- <https://github.com/dinedal/textql>
- harelba.github.io > Q <http://harelba.github.io/q/>
- boyet.com > Articles > Csvparser <http://www.boyet.com/articles/csvparser.html>
- <https://stackoverflow.com/questions/12911299/read-csv-file-in-c>
- <https://github.com/JamesRamm/csv_parser>

libraries

- <https://github.com/rgamble/libcsv>

#### pie_calendar

#### pie_text

append XML attributes to text markup

    ** TEST @assignee="abc" @date="1234567"
    paragraph kajdsklj @style="def"

- additional generic attributes as „[class="abc" cluster="cad" effort="1"]“

    JSON-like? „* Section json="[class:'abc', cluster:'cad', effort:1]"“ → ‘section class="abc" cluster="cad" effort="1"/‚
    JSON-like? „json="{Section [class:'abc', cluster:'cad', effort:1]}"“ → ‘section class="abc" cluster="cad" effort="1"/‚

embedded scripts in Texts

    #begin_of_script_viewable

decoding of RFC1738 URL with Umlaute to UTF-8

#### pie_timezone

test sunrise sunset with Shanghai, Berlin, Boston

calculate different time zone values

#### markdown

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

## References

see REST principles

caching (eid derived from mtime and size -> problems with imports)

„XProc: An XML Pipeline Language“ <http://www.w3.org/TR/xproc/>

http://fuse.sourceforge.net/ dynamic generation of files in a pseudo filesystem
