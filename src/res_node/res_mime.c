/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2020 by Alexander Tenbusch

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#include <libxml/parser.h>

#include "basics.h"
#include <res_node/res_mime.h>

/* s. http://en.wikipedia.org/wiki/List_of_file_formats_%28alphabetical%29 */
static const char* resMimeTypeStr[] = {
  /* MIME_UNDDEFINED */ "type/undefined",
  /* MIME_UNKNOWN */ "type/unknown",
  /*
    Microsoft Office
  */
  /* MIME_APPLICATION_MSACCESS */ "application/msaccess",
  /* MIME_APPLICATION_MSEXCEL */ "application/msexcel",
  /* MIME_APPLICATION_MSGFILE */ "application/msgfile",
  /* MIME_APPLICATION_MSPOWERPOINT */ "application/mspowerpoint",
  /* MIME_APPLICATION_MSPROJECT */ "application/msproject",
  /* MIME_APPLICATION_MSWORD */ "application/msword",
  /* MIME_APPLICATION_VISIO */ "application/vnd.visio",
  /* MIME_APPLICATION_MSSHORTCUT */ "application/x-ms-shortcut",
  /*
    Microsoft Office XML formats
  */
  /* MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION */ "application/vnd.openxmlformats-officedocument.presentationml.presentation",
  /* MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET */ "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
  /* MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT */ "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
  /* MIME_APPLICATION_VND_MS_VISIO_DRAWING_MAIN_XML_2013 */ "application/vnd.ms-visio.drawing.main+xml",
  /* MIME_APPLICATION_VND_MICROSOFT_PORTABLE_EXECUTABLE */ "application/vnd.microsoft.portable-executable",
  /*
    Open Office https://www.openoffice.org/framework/documentation/mimetypes/mimetypes.html
  */
  /* MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT */ "application/vnd.oasis.opendocument.text",
  /* MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET */ "application/vnd.oasis.opendocument.spreadsheet",
  /* MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION */ "application/vnd.oasis.opendocument.presentation",
  /* MIME_APPLICATION_VND_SUN_XML_WRITER */ "application/vnd.sun.xml.writer",
  /* MIME_APPLICATION_VND_SUN_XML_CALC */ "application/vnd.sun.xml.calc",
  /* MIME_APPLICATION_VND_SUN_XML_IMPRESS */ "application/vnd.sun.xml.impress",
  /* MIME_APPLICATION_VND_STARDIVISION_WRITER */ "application/vnd.stardivision.writer",
  /* MIME_APPLICATION_VND_STARDIVISION_CALC */ "application/vnd.stardivision.calc",
  /* MIME_APPLICATION_VND_STARDIVISION_IMPRESS */ "application/vnd.stardivision.impress",
  /*
    DASSAULT CAD formats
  */
  /* MIME_APPLICATION_VND_DASSAULT_CATIA_V4_2D */ "application/vnd.dassault.catia.v4.2d",
  /* MIME_APPLICATION_VND_DASSAULT_CATIA_V4_3D */ "application/vnd.dassault.catia.v4.3d",
  /* MIME_APPLICATION_VND_DASSAULT_CATIA_V5_2D */ "application/vnd.dassault.catia.v5.2d",
  /* MIME_APPLICATION_VND_DASSAULT_CATIA_V5_3D */ "application/vnd.dassault.catia.v5.3d",
  /*
    PTC CAD formats
  */
  /* MIME_APPLICATION_VND_PTC_CAD_3D */ "application/vnd.ptc.cad.3d",
  /* MIME_APPLICATION_VND_PTC_CAD_2D */ "application/vnd.ptc.cad.2d",
  /* MIME_APPLICATION_VND_PTC_CAD_CONFIGURATION */ "application/vnd.ptc.cad.configuration",
  /* MIME_APPLICATION_VND_PTC_CAD_XYZ */ "application/vnd.ptc.cad.XYZ",
  /*
    GARMIN formats
  */
  /* MIME_APPLICATION_VND_GARMIN_FIT */  "application/vnd.garmin.fit",
  /* MIME_APPLICATION_VND_GARMIN_FITX */ "application/vnd.garmin.fit+xml",
  /* MIME_APPLICATION_VND_GARMIN_TCX */  "application/vnd.garmin.tcx+xml",
  /*
    Application formats
  */
  /* MIME_APPLICATION_CAD_2D */ "application/cad+2d",
  /* MIME_APPLICATION_CAD_3D */ "application/cad+3d",
  /* MIME_APPLICATION_SLA */ "application/sla",
  /* MIME_APPLICATION_COMMAND */ "application/command",
  /* MIME_APPLICATION_CXP_XML */ "application/cxp+xml",
  /* MIME_APPLICATION_MMAP_XML */ "application/mmap+xml",
  /* MIME_APPLICATION_MM_XML */ "application/mm+xml",
  /* MIME_APPLICATION_X_BYTECODE_ELISP */ "application/x-bytecode.elisp",
  /* MIME_APPLICATION_JSON */ "application/json",
  /* MIME_APPLICATION_X_JAVASCRIPT */ "application/x-javascript",
  /* MIME_APPLICATION_X_JAVA_VM */ "application/x-java-vm",
  /* MIME_APPLICATION_XMIND_XML */ "application/xmind+xml",
  /* MIME_APPLICATION_XMMAP_XML */ "application/xmmap+xml",
  /* MIME_APPLICATION_RDF_XML */ "application/rdf+xml",
#ifdef HAVE_PIE
  /* MIME_APPLICATION_PIE_XML */ "application/pie+xml",
  /* MIME_APPLICATION_PIE_XML_INDEX */ "application/pie+xml+index",
#endif
  /* MIME_APPLICATION_X_SH */ "application/x-sh",
  /* MIME_APPLICATION_XSPF_XML */ "application/xspf+xml",
  /* MIME_APPLICATION_X_SQLITE3 */ "application/x-sqlite3",
  /* MIME_APPLICATION_X_BAT */ "application/x-bat",
  /* MIME_APPLICATION_ZIP */ "application/zip",
  /* MIME_APPLICATION_GZIP */ "application/gzip",
  /* MIME_APPLICATION_X_BZIP */ "application/x-bzip",
  /* MIME_APPLICATION_X_TAR */ "application/x-tar",
  /* MIME_APPLICATION_X_ISO9660_IMAGE */ "application/x-iso9660-image",
  /* MIME_APPLICATION_PDF */ "application/pdf",
  /* MIME_APPLICATION_POSTSCRIPT */ "application/postscript",
  /*
    Audio formats
  */
  /* MIME_AUDIO_MP3 */ "audio/mp3",
  /* MIME_AUDIO_OGG */ "audio/ogg",
  /* MIME_AUDIO_WAV */ "audio/wav",
  /*
    Binary formats
  */
  /* MIME_BINARY_DLL */ "binary/dll",
  /* MIME_BINARY_EXE */ "binary/exe",
  /* MIME_BINARY_OBJECT */ "binary/object",
  /*
    Image formats
  */
  /* MIME_IMAGE_GIF */ "image/gif",
  /* MIME_IMAGE_JPEG */ "image/jpeg",
  /* MIME_IMAGE_PNG */ "image/png",
  /* MIME_IMAGE_TIFF */ "image/tiff",
  /* MIME_IMAGE_WMF */ "image/x-wmf",
  /*
    Filesystem elements
  */
  /* MIME_INODE_DIRECTORY */ "inode/directory",
  /* MIME_INODE_SYMLINK */ "inode/symlink",
  /*
    Text formats
  */
  /* MIME_TEXT_C */ "text/c",
  /* MIME_TEXT_CALENDAR */ "text/calendar",
  /* MIME_TEXT_CSS */ "text/css",
  /* MIME_TEXT_CSV */ "text/csv",
  /* MIME_TEXT_HTML */ "text/html",
  /* MIME_TEXT_PLAIN */ "text/plain",
  /* MIME_TEXT_PLAIN_CALENDAR */ "text/plain+calendar",
  /* MIME_TEXT_MARKDOWN */ "text/markdown",
  /* MIME_TEXT_VCARD */ "text/vcard",
  /* MIME_TEXT_XML */ "text/xml",
  /* MIME_TEXT_X_SCRIPT_ELISP */ "text/x-script.elisp",
  /* MIME_TEXT_X_SCRIPT_PERL */ "text/x-script.perl",
  /* MIME_TEXT_X_SCRIPT_TCL */ "text/x-script.tcl",
  /* MIME_TEXT_XSL */ "text/xsl",
  /*
    Video formats
  */
  /* MIME_VIDEO_AVI */ "video/avi",
  /* MIME_VIDEO_MP4 */ "video/mp4",
  /* MIME_APPLICATION_X_BACKUP_EDITOR */ "application/editor+backup",
  /*
    pseudo types for error markup
  */
  /* MIME_ERROR_FOPEN */ "error/fopen",
  /* MIME_ERROR_PARSE */ "error/parse",
  /* MIME_ERROR_UNKNOWN */ "error/unknown",
  NULL
};


/*! \return a pointer to string of iArg or NULL in case of errors
*/
char *
resMimeGetTypeStr(int iArg)
{
  if (iArg >= MIME_UNKNOWN && iArg < MIME_END) {
    return (char *) resMimeTypeStr[iArg];
  }
  return NULL;
}
/* end of resMimeGetTypeStr() */


/*! \return the MIME type index according to pchArg or MIME_UNDEFINED in case of errors
*/
RN_MIME_TYPE
resMimeGetType(const char *pchArg)
{
  int i;

  if (pchArg) {
    for (i = MIME_END - 1;; i--) {
      if (i == MIME_UNKNOWN) {
	/* try to detect some generic formats */
	if (strstr(pchArg, "xml")) {
	  return MIME_TEXT_XML;
	}
	else if (strstr(pchArg, "html")) {
	  return MIME_TEXT_HTML;
	}
	else {
	  return MIME_UNKNOWN;
	}
      }
      else if (strcmp(resMimeTypeStr[i], pchArg)==0) {
	return (RN_MIME_TYPE)i;
      }
    }
  }
  return MIME_UNDEFINED;
}
/* end of resMimeGetType() */


/*! \return TRUE if iMimeType is a XML format
*/
BOOL_T
resMimeIsXml(int iMimeType)
{
  return (iMimeType == MIME_APPLICATION_CXP_XML
      || iMimeType == MIME_APPLICATION_MM_XML
      || iMimeType == MIME_APPLICATION_XMMAP_XML
      || iMimeType == MIME_APPLICATION_RDF_XML
#ifdef HAVE_PIE
      || iMimeType == MIME_APPLICATION_PIE_XML
      || iMimeType == MIME_APPLICATION_PIE_XML_INDEX
#endif
#ifdef HAVE_ZLIB
      || iMimeType == MIME_APPLICATION_MMAP_XML
#ifndef HAVE_LIBARCHIVE
      /* */
      || iMimeType ==  MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT
      || iMimeType ==  MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET
      || iMimeType ==  MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT
#endif
#endif
      || iMimeType == MIME_APPLICATION_VND_GARMIN_FITX
      || iMimeType == MIME_APPLICATION_VND_GARMIN_TCX
      || iMimeType == MIME_APPLICATION_XSPF_XML
      || iMimeType == MIME_TEXT_XML
      || iMimeType == MIME_TEXT_XSL);
} /* end of resMimeIsXml() */


/*! \return TRUE if iMimeType is an archive format
*/
BOOL_T
resMimeIsDatabase(int iMimeType)
{
  return (iMimeType == MIME_APPLICATION_X_SQLITE3
      );
} /* end of resMimeIsDatabase() */


/*! \return TRUE if iMimeType is a picture/image format
*/
BOOL_T
resMimeIsPicture(int iMimeType)
{
  return (iMimeType == MIME_IMAGE_GIF
	  || iMimeType == MIME_IMAGE_JPEG
	  || iMimeType == MIME_IMAGE_PNG
	  || iMimeType == MIME_IMAGE_TIFF
	  || iMimeType == MIME_IMAGE_WMF
	  );
} /* end of resMimeIsPicture() */


/*! \return TRUE if iMimeType is an archive format
*/
BOOL_T
resMimeIsArchive(int iMimeType)
{
  return (iMimeType == MIME_APPLICATION_ZIP
    || iMimeType ==  MIME_APPLICATION_GZIP
    || iMimeType ==  MIME_APPLICATION_X_BZIP
    || iMimeType ==  MIME_APPLICATION_X_TAR
    || iMimeType ==  MIME_APPLICATION_X_ISO9660_IMAGE
    // || iMimeType == MIME_APPLICATION_MMAP_XML
    || iMimeType == MIME_APPLICATION_XMIND_XML
#ifdef HAVE_LIBARCHIVE
    || iMimeType ==  MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION
    || iMimeType ==  MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET
    || iMimeType ==  MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT
    || iMimeType ==  MIME_APPLICATION_VND_MS_VISIO_DRAWING_MAIN_XML_2013
    || iMimeType ==  MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT
    || iMimeType ==  MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET
    || iMimeType ==  MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION
    || iMimeType ==  MIME_APPLICATION_VND_SUN_XML_WRITER
    || iMimeType ==  MIME_APPLICATION_VND_SUN_XML_CALC
    || iMimeType ==  MIME_APPLICATION_VND_SUN_XML_IMPRESS
    || iMimeType ==  MIME_APPLICATION_VND_STARDIVISION_WRITER
    || iMimeType ==  MIME_APPLICATION_VND_STARDIVISION_CALC
    || iMimeType ==  MIME_APPLICATION_VND_STARDIVISION_IMPRESS
#endif
    );
} /* end of resMimeIsArchive() */


/*! \return TRUE if iMimeType is a plain text format
*/
BOOL_T
resMimeIsPlain(int iMimeType)
{
  return (iMimeType == MIME_TEXT_PLAIN
    || iMimeType == MIME_TEXT_C
    || iMimeType == MIME_TEXT_CALENDAR
    || iMimeType == MIME_TEXT_CSS
    || iMimeType == MIME_TEXT_CSV
    || iMimeType == MIME_TEXT_PLAIN_CALENDAR
#ifdef WITH_MARKDOWN
    || iMimeType == MIME_TEXT_MARKDOWN
#endif
    || iMimeType == MIME_TEXT_VCARD
    || iMimeType == MIME_TEXT_X_SCRIPT_TCL
    || iMimeType == MIME_TEXT_X_SCRIPT_PERL
    );
} /* end of resMimeIsPlain() */


/*! Read and sets the file MIME type of this context.

  \param prnArg the context
 */
RN_MIME_TYPE
resMimeGetTypeFromExt(const xmlChar *pucArg)
{
  RN_MIME_TYPE eMimeTypeResult = MIME_UNDEFINED;

  if (pucArg == NULL || xmlStrlen(pucArg) < 1) {
    /*  */
  }
  else if (pucArg[xmlStrlen(pucArg) - 1] == '~') {
    eMimeTypeResult = MIME_APPLICATION_X_BACKUP_EDITOR;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"txt") == 0) {
    eMimeTypeResult = MIME_TEXT_PLAIN;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"md") == 0) {
    eMimeTypeResult = MIME_TEXT_MARKDOWN;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"csv") == 0) {
    eMimeTypeResult = MIME_TEXT_CSV;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"cal") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"gcal") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"tdv") == 0) {
    eMimeTypeResult = MIME_TEXT_PLAIN_CALENDAR;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"pie") == 0) {
#ifdef HAVE_PIE
    eMimeTypeResult = MIME_APPLICATION_PIE_XML;
#else
    eMimeTypeResult = MIME_TEXT_XML;
#endif
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"rdf") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"rss") == 0) {
    eMimeTypeResult = MIME_APPLICATION_RDF_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"ics") == 0) {
    eMimeTypeResult = MIME_TEXT_CALENDAR;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"vcf") == 0) {
    eMimeTypeResult = MIME_TEXT_VCARD;
  }
  /* Microsoft Office
   */
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mdb") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSACCESS;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xls") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSEXCEL;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"ppt") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSPOWERPOINT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mpp") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSPROJECT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"doc") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"dot") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSWORD;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"vsd") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VISIO;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"msg") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSGFILE;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"lnk") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MSSHORTCUT;
  }
  /* Microsoft Office XML formats
   */
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xlsx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"pptx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"docx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"vsdx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_MS_VISIO_DRAWING_MAIN_XML_2013;
  }
  /* https://www.openoffice.org/framework/documentation/mimetypes/mimetypes.html
   */
  else if (xmlStrcasecmp(pucArg, BAD_CAST"odt") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"ods") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"odp") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sxw") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_SUN_XML_WRITER;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sxc") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_SUN_XML_CALC;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sxi") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_SUN_XML_IMPRESS;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sdw") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_STARDIVISION_WRITER;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sdc") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_STARDIVISION_CALC;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sdd") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_STARDIVISION_IMPRESS;
  }
  /*
  */
  else if (xmlStrcasecmp(pucArg, BAD_CAST"zip") == 0) {
    eMimeTypeResult = MIME_APPLICATION_ZIP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"gz") == 0) {
    eMimeTypeResult = MIME_APPLICATION_GZIP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"bz") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_BZIP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"bz2") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_BZIP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"tar") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_TAR;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"iso") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_ISO9660_IMAGE;
  }
  /*
  */
  else if (xmlStrcasecmp(pucArg, BAD_CAST"pdf") == 0) {
    eMimeTypeResult = MIME_APPLICATION_PDF;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"ps") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"eps") == 0) {
    eMimeTypeResult = MIME_APPLICATION_POSTSCRIPT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"cxp") == 0) {
    eMimeTypeResult = MIME_APPLICATION_CXP_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xspf") == 0) {
    eMimeTypeResult = MIME_APPLICATION_XSPF_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xsl") == 0) {
    eMimeTypeResult = MIME_TEXT_XSL;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xml") == 0) {
    eMimeTypeResult = MIME_TEXT_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mm") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MM_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mmap") == 0) {
    eMimeTypeResult = MIME_APPLICATION_MMAP_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xmmap") == 0) {
    eMimeTypeResult = MIME_APPLICATION_XMMAP_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"xmind") == 0) {
    eMimeTypeResult = MIME_APPLICATION_XMIND_XML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"css") == 0) {
    eMimeTypeResult = MIME_TEXT_CSS;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"html") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"htm") == 0) {
    eMimeTypeResult = MIME_TEXT_HTML;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sqlite") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"db") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"db3") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_SQLITE3;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mp3") == 0) {
    eMimeTypeResult = MIME_AUDIO_MP3;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"ogg") == 0) {
    eMimeTypeResult = MIME_AUDIO_OGG;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"wav") == 0) {
    eMimeTypeResult = MIME_AUDIO_WAV;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"jpg") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"jpeg") == 0) {
    eMimeTypeResult = MIME_IMAGE_JPEG;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"gif") == 0) {
    eMimeTypeResult = MIME_IMAGE_GIF;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"tif") == 0) {
    eMimeTypeResult = MIME_IMAGE_TIFF;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"wim") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"swm") == 0) {
    eMimeTypeResult = MIME_IMAGE_WMF;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"png") == 0) {
    eMimeTypeResult = MIME_IMAGE_PNG;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"pro") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"dtl") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"sup") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"win") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_PTC_CAD_CONFIGURATION;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"prt") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"asm") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"mfg") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"gph") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_PTC_CAD_3D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"fit") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_GARMIN_FIT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"fitx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_GARMIN_FITX;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"tcx") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_GARMIN_TCX;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"drw") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"lay") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"sec") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"frm") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"sym") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"tbl") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_PTC_CAD_2D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"catpart") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"CATProduct") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_DASSAULT_CATIA_V5_3D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"catdrawing") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_DASSAULT_CATIA_V5_2D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"model") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_DASSAULT_CATIA_V4_3D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"drawing") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_DASSAULT_CATIA_V4_2D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"dwg") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"dxf") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"hp") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"plt") == 0
    ) {
    eMimeTypeResult = MIME_APPLICATION_CAD_2D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"cgr") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"igs") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"stp") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"vda") == 0) {
    eMimeTypeResult = MIME_APPLICATION_CAD_3D;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"stl") == 0) {
    eMimeTypeResult = MIME_APPLICATION_SLA;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"bat") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"cmd") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_BAT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"sh") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_SH;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"mp4") == 0) {
    eMimeTypeResult = MIME_VIDEO_MP4;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"avi") == 0) {
    eMimeTypeResult = MIME_VIDEO_AVI;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"dot") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"log") == 0) {
    eMimeTypeResult = MIME_TEXT_PLAIN;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"dll") == 0) {
    eMimeTypeResult = MIME_BINARY_DLL;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"exe") == 0) {
    eMimeTypeResult = MIME_APPLICATION_VND_MICROSOFT_PORTABLE_EXECUTABLE;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"o") == 0) {
    eMimeTypeResult = MIME_BINARY_OBJECT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"obj") == 0) {
    eMimeTypeResult = MIME_BINARY_OBJECT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"c") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"cpp") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"h") == 0
    || xmlStrcasecmp(pucArg, BAD_CAST"hpp") == 0) {
    eMimeTypeResult = MIME_TEXT_C;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"tcl") == 0) {
    eMimeTypeResult = MIME_TEXT_X_SCRIPT_TCL;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"json") == 0) {
    eMimeTypeResult = MIME_APPLICATION_JSON;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"js") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_JAVASCRIPT;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"el") == 0) {
    eMimeTypeResult = MIME_TEXT_X_SCRIPT_ELISP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"elc") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_BYTECODE_ELISP;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"pl") == 0 || xmlStrcasecmp(pucArg, BAD_CAST"pm") == 0) {
    eMimeTypeResult = MIME_TEXT_X_SCRIPT_PERL;
  }
  else if (xmlStrcasecmp(pucArg, BAD_CAST"jar") == 0) {
    eMimeTypeResult = MIME_APPLICATION_X_JAVA_VM;
  }
  else {
    /* no mime informations */
    eMimeTypeResult = MIME_UNKNOWN;
  }
  return eMimeTypeResult;
}
/* end of resMimeGetTypeFromExt() */


#ifdef TESTCODE
#include "test/test_res_mime.c"
#endif

