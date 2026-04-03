#
# for testing of *.cmake only
#

cmake_minimum_required(VERSION 3.20)

project(cmakeTests)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/Modules" "${PROJECT_SOURCE_DIR}/Options")

include(helper)

# local *.cmake

#FIND_PACKAGE( LocalZLIB )
#FIND_PACKAGE( PCRE2 )
#FIND_PACKAGE( Duktape )
#FIND_PACKAGE( Exif )

# FIND_PACKAGE( Id3Tag )
# FIND_PACKAGE( LibOGG )
# FIND_PACKAGE( LibVorbis )

# # provided by cmake

#FIND_PACKAGE( LocalBZip2 )
#FIND_PACKAGE( LocalLibLZMA )
#FIND_PACKAGE( LocalCURL )

# FIND_PACKAGE( LibArchive )

#FIND_PACKAGE( LibXml2 )
FIND_PACKAGE( LocalLibXml2 )
# FIND_PACKAGE( LibXslt )
#FIND_PACKAGE( LocalLibXslt )
# FIND_PACKAGE( SQLite3 )
