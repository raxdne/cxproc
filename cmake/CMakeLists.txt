#
# for testing of *.cmake only
#

cmake_minimum_required(VERSION 3.16)

project(cmakeTests)

# local *.cmake

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
FIND_PACKAGE( PCRE2 )
FIND_PACKAGE( Duktape )
FIND_PACKAGE( Exif )
FIND_PACKAGE( Id3Tag )
FIND_PACKAGE( LibOGG )
FIND_PACKAGE( LibVorbis )

# provided by cmake

FIND_PACKAGE( BZip2 )
FIND_PACKAGE( CURL )
FIND_PACKAGE( LibArchive )
FIND_PACKAGE( LibLZMA )
FIND_PACKAGE( LibXml2 )
FIND_PACKAGE( LibXslt )
FIND_PACKAGE( SQLite3 )
FIND_PACKAGE( ZLIB )
