
Speicherung <main.txt 15.5.2015 20:40:46 developer>

# A (C)onfigurable (X)ml (PROC)essor: “cxproc”

## AUTHOR and HOMEPAGE

Copyright (C) 2006,2007,2008,2013,2020 by Alexander Tenbusch <https://github.com/raxdne/cxproc>

## DESCRIPTION

Cxproc is a configurable processor for XML-XSL fans. Its a portable frontend for the GNOME libraries libxml2 and libxslt under GNU GENERAL PUBLIC LICENSE Version 3 (s. file LICENSE in source code).

It helps to develop more simple stylesheets

Main features
1) configurable frontend for XSL transformations “parse XML once, XSL   transforms often” (via caching of DOMs)
1) simple substitutions inside configurations
   - fixed strings,
   - date and time related format strings (like date command).
   - command line arguments,
   - environment variables,
   - HTTP/CGI data,


1) XML configurable make processor for XML, XSL, XHTML, Plain text
   - Plain text to XML processor (Emacs outline mode, line mode and a    tabulator separated mode)
   - XML Calendar generator
   - XML Directory listings (levels of verbosity)
   - Petrinet (path searching and graph imgaes via Graphviz)
   - Rudimentary image processing (via linked ImageMagick libs on Linux)
   - MySQL query results into XML (Draft on Linux)





Some Minor features are
- multithreading on demand (eg. multiple synchronous downloads)
- transforming from data → instructions → data → ... (like self   generating instructions)
- recursive search for files using the directory tree (like $PATH or   kpathsea lib)
- simple “system” calls for messages, directory change or external scripts
- configurable log levels for developing of complex configurations
- a map with word counts, buzzwords for web search and semantic web (OWL?)



## Advantages
- With the multi step approach its easy to develop smaller, less   complex XML stylesheets.
- There is a good performance with libxml2 and other libraries in C.
- Configurations are usable on command line, scripts, file manager menus and   webservers (Apache/CGI tested only yet).
- not as quick as command line, but more flexible and portable with typical tasks
- good for prototyping of complete websites and stylesheets
- Its a modular extensible approach with cxproc modules as core and   applications in contrib directory.
- on top of this tool its easy to develop own XML/XSL based applications
- usable on GNU/Linux and MS Windows

