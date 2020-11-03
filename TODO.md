
# TODO

TODO: remove "#ifdef EXPERIMENTAL"

code analysis
    VC
    Callgrind/Kcachegrind
    Valgrind/memcheck (s. ctest)

TODO: CMake packaging

## Features

    configuration files for default values
    Module "xmlsqlio" (similar to "xmlzipio")
        transparent SQL to XML access
        xmlRead("sql:///tmp/abc.db3:SELECT * FROM table;")
    image
        ImageMagick
            exifmover.cxp
                Test
                Windows + Linux
            use „VisualMagick configuration tool“ for VC++ Project setup on Windows
            Magic Scripting Language <http://www.linux-nantes.org/~fmonnier/ocaml/ml-conjure/documentation.html>
                Conjure <http://support.cs.nott.ac.uk/help/docs/image/ImageMagick/www/conjure.html>
        additional image processing instructions as elements
        embed comments into image files

    <cxp:image to="result.png">
      <cxp:image name="source.jpg" cache="yes"/>
      <cxp:scale="0.5"/>
      <cxp:orientation="auto"/>
      <cxp:comment>This is a Test!</cxp:comment>
      <cxp:scale="0.5"/>
      <cxp:scale="300"/>
    </cxp:image>

HTML to UTF-8 Plain text Renderer
    cxp:plain
        cxp:html
            cxp:xml
            cxp:xsl
    like "w3m -dump"

gallery
    add a "move/copy Picture to" option
    sorting of large picture collections
    button "move to trash"
    thumbnails?
    to fix

audioserver
    encoding of XML special chars in path names
    playlist formats
        PC xspf, else m3u
    playlist from Archive file

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

## STL/librp

    integrate admesh <http://www.varlog.com/admesh-htm>
    info as XML
    output XML based formats
        X3D <http://xml.coverpages.org/vrml-X3D.html>
    processing by cxp
    slicing
    mit libadmesh und librp
        Slicen als SVG, als Bauzeit-Diagramm
        Projektion auf Ebene als SVG
        Meta-Info (Bounding-Box, Facets, ...)
        Facetten in XML-Schreibweise zum Konvertieren in andere Formate (VRML, GnuPlot, PovRay)

## Backends

    Filesystem
    ZIP file
    URL
        http
            Web server
            Application server
        ftp
    VCS
        Subversion
        Git
    Document DB
        MongoDB <http://en.wikipedia.org/wiki/MongoDB>
	MySQL/MariaDB

## CGI

TODO: check security ++

- access control in CGI mode

- HTTP Status message if access denied

## Security

    Limitations or Sandbox
        use Unicode functions to avoid MAX_PATH on Windows
        <cxp:make readonly="no|yes|write|unlink|create"> "rcwd"
        <cxp:make network="no">
        prevent system() in cgi_mode (or readonly_mode)
    <https://www.tu-braunschweig.de/Medien-DB/sec/pubs/2016-ccs.pdf>
    Docker

## building

TODO: statically linked binaries for 

- ARMv7

- x64_linux

- x64_windows

TODO: detail information in binaries

TODO: pack library frontends in bin/ xmllint, xsltproc, zip, curl ...

TODO: Binary without Microsoft Runtime DLL (at all requiered libs??)

VC++
    Side-by-side assembly <https://en.wikipedia.org/wiki/Side-by-side_assembly>
        <https://www.codeguru.com/cpp/article.php/c18071/Simplify-Visual-C-Application-Deployment-with-Manifest-Files.htm>
        <https://docs.microsoft.com/de-de/cpp/build/understanding-manifest-generation-for-c-cpp-programs?view=vs-2019>

    target_link_options(cxproc-cgi     PUBLIC /MANIFEST /ManifestFile:${CXPROC_SRC_DIR}/cxproc.manifest)

    ADD_CUSTOM_COMMAND( 	TARGET cxproc-cgi 	POST_BUILD 	COMMAND "mt.exe" -manifest \"${CXPROC_SRC_DIR}/cxproc.manifest\" -inputresource:\"$<TARGET_FILE:cxproc-cgi>\"\;\#1 -outputresource:\"$<TARGET_FILE:cxproc-cgi>\"\;\#1 	COMMENT "Adding display aware manifest..."  ) 

    32 Bit
        dynamic linking
            DLL updates
                debug version of DLL
                Multithreading /MT
            delayed linking
                pcre3.dll
                zlib1.dll
                libsqlite3.dll
                libxslt.dll
        static linking
    64 Bit
        dynamic linking
        static linking
GCC

    Debian
        32 Bit
            install DTD files to /usr/share/sgml/dtd/
            automatic query of package configuration (like „libpng-config –prefix“)
                s. xml2-config
                Package Information for pkg-config in cxproc.pc.in
            Build valid ".index.pie"
        64 Bit
            dynamically linked
            statically linked
    Redhat
        64 Bit
            dynamically linked
            statically linked
    MinGW/Eclipse
        32 Bit
        64 Bit
        use "MozillaBuildSetup-Latest.exe"
    MacOS

TODO: test LLVM

TODO: Package Information for pkg-config in cxproc.pc.in (s. xml2-config)

## Quality

    compiler
        Warnings
            C
            C++
    splint
    memcheck
    command line
    gcov
        test coverage by gcov <https://gcc.gnu.org/onlinedocs/gcc/Gcov.html>
    VC++ code analysis

## Profiling

gprof and valgrind/cachegrind (KCachegrind)

## XML

TODO: Update of RELAX NG schemas (translation by [trang](http://www.thaiopensource.com/relaxng/trang-manual.html) )

## Testing

    Test with text files from Gutenberg Project <https://www.gutenberg.org/>
        <https://www.gutenberg.org/files/3176/3176-0.txt>

## Deployment

    Packages
        deb
        rpm
        tar.gz
        zip
        exe
            7zip
            NSI

    Hosting
        Github
        tenbusch.info <http://www.tenbusch.info/cxproc/>

## Scripting

    Javascript
        node.js + V8
            REST based server
        Frontend
    Tcl
        as package (no embedded Tcl interpreter)
        Tcl bindings "Tcxproc"
    use cases
        XML creation (from plain text)
        plain XML modifications before parsing
        regexp tasks in XML/XSL
        preprocessor-like embedding in XML/XHTML, JS, CSS
        JSON generator and tests
        automatic tests

### Ducktape

    new classes
        Cxproc
            Cxproc.new();
            Cxproc.parseXML("abc.xml");
            Cxproc.transform("def.xsl").variable("str_test","AAAA");
            var strXML = Cxproc.serialize();
            Cxproc.run()
            map method calls to an internal cxp:make
        File
            var objInput = File.new("test.txt").search("yes");
            print objInput.getContent();
        Locator
            getNameNormalized()
            getNameNative()
            Mime
        Env
            print Env.getValue('TMP');
            Env.setValue('TMP','c:\tmp');
        Cgi
            print Cgi.getValue('path');
        Date
        XML
        Plain
        Xhtml
        Pathtable
        Database
        Image
    example
        Interpret script source
        iterations
        print Env.getValue('TMP');
Env.setValue('TMP','c:\tmp');

        Transform XML source
            var strCgiPath = Cgi.getValue('path');
if (strCgiPath == undef) {
  //
} else {
  var cxpT = new Cxproc();

  cxpT.parseXML(strCgiPath);
  cxpT.transform("def.xsl").variable("str_test","AAAA");

  print cxpT.serialize();
}

        output of plain text file
            var objInput = File.new("test.txt").search("yes");
print objInput.getContent();

    Test
        test created file content of single tests
        string based processing

## Web server

    IIS
    HTTPS
    nginx
    node.js

## cxproc-httpd (ApplServer)

    Reverse Proxy
        <https://en.wikipedia.org/wiki/Reverse_proxy>
        <https://www.gnu.org/software/libmicrohttpd/>
        Apache
            <https://httpd.apache.org/docs/2.4/howto/reverse_proxy.html>
        IIS
            <https://www.iis.net/learn/extensions/configuring-application-request-routing-arr/creating-a-forward-proxy-using-application-request-routing>
            stackoverflow.com > Questions > 4739669 > How-to-set-proxy-settings-for-iis-processes <http://stackoverflow.com/questions/4739669/how-to-set-proxy-settings-for-iis-processes>
            <https://blogs.msdn.microsoft.com/friis/2016/08/25/setup-iis-with-url-rewrite-as-a-reverse-proxy-for-real-world-apps/>
            <https://www.iis.net/learn/extensions/url-rewrite-module/reverse-proxy-rule-template>
            <https://www.iis.net/learn/extensions/url-rewrite-module/modifying-http-response-headers>
        Authentification
        REST Principles
        New URL Scheme
            all "/cxproc/exe" reroute to Application server
            CGI compatible!
        CXP_ROOT and CXP_PATH
            for Application Server only
        Tests
            Authorization or localhost only
            GET
            POST
            XMLHttpRequest

## option

### pie

    remove XML-only features
        @effort
        @assignee
        ...

    autodetection
        line or paragraph input
        encoding
            BOM
        input language
            pattern matching /\bund\b/ or /\band\b/
            pieDetectLanguage()
                GERMAN
                FRENCH
                LANG_C
            re_tags
                de
                en
        dates containing offsets, ranges etc
            "Test\Calendars\TestCalendar.txt"
            "Test\Calendars\TestCalendar.pie"

    define an id markup for referencing
        §ID1
        anchor for linking
        local and global anchors: |#abc|TEST|

    pie_csv
        requirements
            process a leading „sep=“ in CSV content
            autodetect separator
                sep=
                statistics
            respect double and single quotes and remove them
            processing of headers
            markup
                #begin_of_csv
                #import()
                <import type="csv" sep=",">A;B;C</import>
                <import name="a.csv" type="csv" sep=","/>
                <file name="a.csv" verbosity="4"/>
            parse from UTF-8 memory buffer
            multithreading
        examples
            <https://github.com/dinedal/textql>
            harelba.github.io > Q <http://harelba.github.io/q/>
            boyet.com > Articles > Csvparser <http://www.boyet.com/articles/csvparser.html>
            <https://stackoverflow.com/questions/12911299/read-csv-file-in-c>
            <https://github.com/JamesRamm/csv_parser>
        libraries
            <https://github.com/rgamble/libcsv>

    pie_calendar
        sunrise/sunset as ‘subst string="%SS" now="sunrise"/‚ depending on XML context calendar/year/month/day and calendar/@timezone
        BUG: sunrise="67:20" sunset="75:07"
        BUG: test-calendar-003-002: calendar from Freemind map
        display of database entries with one date formatted column
        counter for occurance of a event (2009*w7mon.7x23)
        comma separated dates of diffent types (2008*w47mon,20081128,2008*w47) ??
        Birthday in Days „20170719 %D days old“
            „20170719 script="Date() - Date(1989-11-12T11:00:00Z)" days old“
            OR Excel sheet?
        cxp:calendar year="2014"
            cxp:col
                cxp:pie
                cxp:xml
                    cxp:file name="abc.xml" verbosity="3"
                cxp:xml
                    cxp:dir name="def" verbosity="3" depth="3"
                cxp:xml
                    cxp:query
            cxp:col
            cxp:col
            cxp:col
        NameOfMonth in English, German ... (in calendar definition?)
        set done or close attributes of tasks in calendar and todo table
        processing of date and time values
        doesnt work: task date="20101202,03,201101"
        validate in calendar import

    pie_text
        append XML attributes to text markup
            ** TEST @assignee="abc" @date="1234567"
            paragraph kajdsklj @style="def"
            additional generic attributes as „[class="abc" cluster="cad" effort="1"]“
                JSON-like? „* Section json="[class:'abc', cluster:'cad', effort:1]"“ → ‘section class="abc" cluster="cad" effort="1"/‚
                    JSON-like? „json="{Section [class:'abc', cluster:'cad', effort:1]}"“ → ‘section class="abc" cluster="cad" effort="1"/‚
        explicit: cite, date
        cite markup [] to <cite></cite>
        regexp based tagging (in text nodes of XML DOM)
             <cxp:tagregexp pattern="[A-Z]+" name="cite" attribute="name" replace="yes"/>

        Encryption of Plain text files.
            <cxp:plain name="test.asc" encrypt="yes">

         → twitter-like (s. microformats.org)
             as a vector
                 message
                    ’htag class="source"‘’/htag‘ message
                message 
                    message ’htag class="target"‘’/htag‘
                message  more 
                    message ’htag‘’/htag‘ more ’htag‘’/htag‘
                    default class
            Display texts
                http://de.wikipedia.org/ → Wikipedia <http://de.wikipedia.org/>
                 → abc.png|ABC| <abc.png>
                 → OSS <#Linux>
                 → John Muller <@you>
        scripting in Texts
            begin_of_script_viewable
        decoding of RFC1738 URL with Umlaute to UTF-8
        Link markup
            <https://www.markdownguide.org/basic-syntax/#links>
            <https://www.mediawiki.org/wiki/Help:Links#External_links>
            page anchors?
        microformats.org > Wiki > Microformats2 <http://microformats.org/wiki/microformats2>

    pie_timezone
        test sunrise sunset with
            Shanghai
            Berlin
            Boston
        calculate different time zone values

    markdown
        <https://daringfireball.net/projects/markdown/basics>
        <https://spec.commonmark.org/0.29/>
            1 Introduction
                1.1 What is Markdown?
                1.2 Why is a spec needed?
                1.3 About this document
            2 Preliminaries
                2.1 Characters and lines
                2.2 Tabs
                2.3 Insecure characters
            3 Blocks and inlines
                3.1 Precedence
                3.2 Container blocks and leaf blocks
            4 Leaf blocks
                4.1 Thematic breaks
                4.2 ATX headings
                4.3 Setext headings
                4.4 Indented code blocks
                4.5 Fenced code blocks
                4.6 HTML blocks
                4.7 Link reference definitions
                4.8 Paragraphs
                4.9 Blank lines
            5 Container blocks
                5.1 Block quotes
                5.2 List items
                5.3 Lists
            6 Inlines
                6.1 Backslash escapes
                6.2 Entity and numeric character references
                6.3 Code spans
                6.4 Emphasis and strong emphasis
                6.5 Links
                6.6 Images
                6.7 Autolinks
                6.8 Raw HTML
                6.9 Hard line breaks
                6.10 Soft line breaks
                6.11 Textual content
            Appendix: A parsing strategy
                Overview
                Phase 1: block structure
                Phase 2: inline structure

        Markdown parser <https://css-tricks.com/choosing-right-markdown-parser/>


## Misc

TODO: JSON output from database, dir etc

## References

    see REST principles
    caching
        eid derived from mtime and size (problems with imports)
    /cxproc/get?path=abc.txt
    portability to different webserver

    https://quabel.com Browser-Editor <https://quabel.com>
    impress.js (nicht mit IE)
    Foundation CSS Framework
    <http://cssdeck.com>
    <https://sites.google.com/site/mynotex/>
    Rednotebook <http://rednotebook.sourceforge.net/>
    Rainlendar <http://www.rainlendar.net/>
    projectmallard.org XML <http://projectmallard.org/>
        Mallard is an XML format designed to make topic-oriented help as simple as possible.
    Tasque <https://wiki.gnome.org/Apps/Tasque>
        is a simple task management app
    "Remember the Milk" <http://www.rememberthemilk.com/>
    Record Editor CSV <http://record-editor.sourceforge.net/>
         is a Programmers Data-file editor
    en.wikipedia.org > Wiki > Comparison of JavaScript-based source code editors <http://en.wikipedia.org/wiki/Comparison_of_JavaScript-based_source_code_editors>
    webodf.org <http://webodf.org/>
    leoeditor.com <http://leoeditor.com/>
    The JasperReports Library is the world's most popular open source reporting engine. <http://community.jaspersoft.com/project/jasperreports-library>
    GUI/Dialog
        jplugins.directory > Ferromenu >  ? ... <http://jplugins.directory/ferromenu/?show=web>
        gtk.org > Download > Win32 tutorial <http://www.gtk.org/download/win32_tutorial.php>
        sourceforge.net > Projects > Tinyfiledialogs <http://sourceforge.net/projects/tinyfiledialogs/>
        docs.wxwidgets.org > Trunk > Index <http://docs.wxwidgets.org/trunk/index.html>
        fltk.org > Index <http://www.fltk.org/index.php>
        iup.sourceforge.net <http://iup.sourceforge.net/>
        cpptk.sourceforge.net > Index <http://cpptk.sourceforge.net/index.html>
    xmlstar.sourceforge.net <http://xmlstar.sourceforge.net/>
    <http://htmltopdf.org/>
    <https://transpect.github.io/index.html>
    <https://www.deepl.com/api-reference.html>
    microformats.org > Wiki > About <http://microformats.org/wiki/about>
    s.  „XProc: An XML Pipeline Language“ <http://www.w3.org/TR/xproc/>

http://fuse.sourceforge.net/
    dynamic generation of files in a pseudo filesystem
