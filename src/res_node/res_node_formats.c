/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 2006..2024 by Alexander Tenbusch

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

#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif

#include <libxml/uri.h>
#include <libxml/parser.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node_io.h>

#ifdef HAVE_PIE
#include <pie/pie_text_blocks.h>
#include <ics/ics.h>
#include <vcf/vcf.h>
#endif

#include "dom.h"
#include <cxp/cxp_dtd.h>

#ifdef HAVE_JSON
#include <json/json.h>
#endif
#ifdef HAVE_LIBSQLITE3
#include <database/database.h>
#endif
#ifdef HAVE_LIBID3TAG
#include <audio/audio.h>
#endif
#if defined(HAVE_LIBEXIF)
#include <image/image_exif.h>
#endif

/*!

  \param pndArg a xmlNodePtr to append data
  \param prnArg filesystem name
*/
BOOL_T
resNodeContentToDOM(xmlNodePtr pndArg, resNodePtr prnArg)
{
  if (pndArg) {
    RN_MIME_TYPE iMimeType;
    size_t liSizeContent;

    /*\bug fails when there is no child because of archive errors */
    
    liSizeContent = resNodeGetSize(prnArg);
    iMimeType = resNodeGetMimeType(prnArg);
    switch (iMimeType) {

#ifdef EXPERIMENTAL
    case MIME_INODE_SYMLINK:
      if (resNodeGetChild(prnArg) != NULL) {
	xmlNodePtr pndChild;

	if (resNodeIsDir(resNodeGetChild(prnArg))) {
	  /*! follow link to directory */
	  /*!\bug avoid circular references */
	}
	else if (resNodeIsFile(resNodeGetChild(prnArg)) && resNodeIsReadable(resNodeGetChild(prnArg))) {
	  /*! follow link to file */
	  if ((pndChild = domGetFirstChild(pndArg, BAD_CAST NAME_FILE)) == NULL) {
	    xmlNewChild(pndArg, NULL, BAD_CAST NAME_FILE, NULL);
	  }
	  resNodeContentToDOM(pndChild, resNodeGetChild(prnArg));
	}
	else {
	  xmlNewChild(pndArg, NULL, BAD_CAST NAME_ERROR, BAD_CAST "File link broken");
	}
      }
      break;
#endif
      
    case MIME_EMPTY:
      /* no file content */
    break;

#ifdef HAVE_PIE
    case MIME_APPLICATION_PIE_XML:
    case MIME_TEXT_PLAIN:
#ifdef WITH_MARKDOWN
    case MIME_TEXT_MARKDOWN:
#endif
    case MIME_TEXT_CSV:
      {
	xmlNodePtr pndPie = NULL;

	CompileRegExpDefaults();
	
	if (iMimeType == MIME_APPLICATION_PIE_XML) {
	  xmlDocPtr pdocResult = NULL;

	  pdocResult = resNodeGetContentDoc(prnArg);
	  if (pdocResult != NULL && (pndPie = xmlDocGetRootElement(pdocResult)) != NULL) {
#ifdef EXPERIMENTAL
	    domUnlinkNodeList(pndPie);
#else
	    /*!\todo optimization avoid copy of node list use domUnlinkNodeList(pndRootResult) */
	    pndPie = xmlCopyNodeList(pndPie);
#endif
	  }
	  else {
	    xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST "parse");
	  }
	}
	else {
	  xmlChar *pucContent;

	  pucContent = BAD_CAST resNodeGetContent(prnArg,1024);
	  if (STR_IS_NOT_EMPTY(pucContent)) {

	    pndPie = xmlNewNode(NULL, NAME_PIE);
	    if (pndPie) {
	      rmode_t m;

	      if (iMimeType == MIME_TEXT_PLAIN) {
		m = GetModeByExtension(resNodeGetExtension(prnArg));
	      }
	      else {
		m = GetModeByMimeType(iMimeType);
	      }

	      if (ParsePlainBuffer(pndPie, pucContent, m)) {
#ifdef HAVE_CGI
		xmlSetProp(pndPie->children, BAD_CAST "context", resNodeGetNameShort(prnArg)); /* in CGI mode use short path only */
#else
		xmlSetProp(pndPie->children, BAD_CAST "context", resNodeGetURI(prnArg));
#endif
		SetTypeAttr(pndPie->children,m);
		//domPutNodeString(stderr, BAD_CAST "pndPie", pndPie);
	      }
	    }
	    //xmlNewTextChild(pndPie, NULL, BAD_CAST"meta", pucContent);
	  }
	  else {
	    resNodeSetError(prnArg, rn_error_access, "Cant read from '%s'", resNodeGetNameNormalized(prnArg));
	  }
	}

	if (pndPie) {
	  xmlNodePtr pndMeta;
	  xmlNodePtr pndTags;

	  if ((pndMeta = domGetFirstChild(pndPie, BAD_CAST NAME_META)) == NULL) {
	    pndMeta = xmlNewChild(pndPie, NULL, BAD_CAST NAME_PIE_META, NULL);
	  }
	  pndTags = xmlNewChild(pndMeta, NULL, BAD_CAST NAME_PIE_TAGLIST, NULL);

	  RecognizeIncludes(pndPie);
	  RecognizeSubsts(pndPie);
	  RecognizeImports(pndPie);
	  RecognizeScripts(pndPie);
	  RecognizeFigures(pndPie);
	  RecognizeInlines(pndPie);
	  RecognizeUrls(pndPie);
	  RecognizeSymbols(pndPie, LANG_DEFAULT);
	  RecognizeDates(pndPie, iMimeType);
	  RecognizeTasks(pndPie);
	  RecognizeHashtags(pndPie,NULL, NULL);
	  InheritHashtags(pndPie, pndPie);
	  RecognizeGlobalTags(pndTags, pndPie);
	  CleanListTag(pndTags, FALSE);
	  //domPutNodeString(stderr, BAD_CAST "resNodeContentToDOM(): ", pndPie);
	  xmlAddChild(pndArg, pndPie);
	}
      }
      break;

    case MIME_TEXT_CALENDAR:
      icsParse(pndArg, prnArg);
      break;

    case MIME_TEXT_VCARD:
      vcfParse(pndArg, prnArg);
      break;

#else
    case MIME_TEXT_PLAIN:
    case MIME_TEXT_CSV:
    case MIME_TEXT_VCARD:
#ifdef WITH_MARKDOWN
#else
    case MIME_TEXT_MARKDOWN:
#endif
    {
      xmlChar *pucT = NULL;

      pucT = BAD_CAST resNodeGetContentPtr(prnArg);
      if (STR_IS_NOT_EMPTY(pucT)) {
	xmlChar *pucContent = NULL;

	/*!\bug encoding required */
	pucContent = xmlEncodeEntitiesReentrant(pndArg->doc, pucT);
	xmlAddChild(pndArg,xmlNewText(pucContent));
	xmlFree(pucContent);
      }
      else {
	resNodeSetError(prnArg,rn_error_access, "Cant read from '%s'", resNodeGetNameNormalized(prnArg));
      }
    }
    break;
#endif

    case MIME_APPLICATION_CXP_XML:
    case MIME_APPLICATION_RDF_XML:
    case MIME_APPLICATION_XSD_XML:
    case MIME_APPLICATION_RNG_XML:
    case MIME_APPLICATION_MM_XML:
    case MIME_APPLICATION_XMMAP_XML:
    case MIME_APPLICATION_VND_GARMIN_FITX:
    case MIME_APPLICATION_VND_GARMIN_TCX:
    case MIME_APPLICATION_XSPF_XML:
    case MIME_TEXT_HTML:
    case MIME_TEXT_XML:
    case MIME_TEXT_XSL:
    {
	xmlChar *pucT = NULL;
	xmlDocPtr pdocResult = NULL;

	resNodeGetNameNormalized(prnArg); /* set internal name */

	if ((pdocResult = resNodeGetContentDoc(prnArg)) != NULL) {
	  xmlNodePtr pndRootResult;
	  
	  if ((pndRootResult = xmlDocGetRootElement(pdocResult)) != NULL) {
	    xmlAddChild(pndArg, xmlCopyNode(pndRootResult,1));
	  }
	  else {
	    xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST "parse");
	  }
	}
	else if ((pucT = BAD_CAST resNodeGetContentPtr(prnArg)) != NULL) { /* no DOM result */
	  xmlChar *pucContent = NULL;

	  /*!\bug encoding required */
	  pucContent = xmlEncodeEntitiesReentrant(pndArg->doc, pucT);
	  xmlAddChild(pndArg,xmlNewText(pucContent));
	  xmlFree(pucContent);
	}
	else {
	  /* handle parser errors */
	  xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST "parse");
	}
      }
      break;

    case MIME_APPLICATION_VND_PTC_CAD_2D:
    case MIME_APPLICATION_VND_PTC_CAD_3D:
    case MIME_APPLICATION_VND_PTC_CAD_CONFIGURATION:
    case MIME_APPLICATION_VND_DASSAULT_CATIA_V4_2D:
    case MIME_APPLICATION_VND_DASSAULT_CATIA_V4_3D:
    case MIME_APPLICATION_VND_DASSAULT_CATIA_V5_2D:
    case MIME_APPLICATION_VND_DASSAULT_CATIA_V5_3D:
    {
      /*!\todo add cadParseFile(pndArg,prnArg); */
    }
    break;

    case MIME_APPLICATION_MMAP_XML:
    case MIME_APPLICATION_XMIND_XML:
    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT:
    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET:
    case MIME_APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION:
    case MIME_APPLICATION_VND_SUN_XML_WRITER:
    case MIME_APPLICATION_VND_SUN_XML_CALC:
    case MIME_APPLICATION_VND_SUN_XML_IMPRESS:
    case MIME_APPLICATION_VND_STARDIVISION_WRITER:
    case MIME_APPLICATION_VND_STARDIVISION_CALC:
    case MIME_APPLICATION_VND_STARDIVISION_IMPRESS:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION:
    case MIME_APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET:
    case MIME_APPLICATION_VND_MS_VISIO_DRAWING_MAIN_XML_2013:
    {
#if 1
      xmlNodePtr pndArchive;
      resNodePtr prnI;

      PrintFormatLog(2, "Use document content of file '%s'", resNodeGetNameNormalized(prnArg));
      if (IS_NODE_ARCHIVE(pndArg)) {
	pndArchive = pndArg;
      }
      else {
	pndArchive = xmlNewChild(pndArg, NULL, BAD_CAST NAME_ARCHIVE, NULL);
      }

      for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	xmlNodePtr pndEntry;

	if ((pndEntry = resNodeToDOM(prnI, RN_OUT_MAX))) {
	  xmlAddChild(pndArchive, pndEntry);
	}
      }
#else
      xmlChar *pucT = NULL;
      xmlDocPtr pdocResult = NULL;

      resNodeGetNameNormalized(prnArg); /* set internal name */

      if (prnArg->pdocContent == NULL) {
	xmlNodePtr pndRoot = NULL;

	if (zipDocumentRead(prnArg) && (pndRoot = resNodeToDOM(prnArg, RN_OUT_MAX)) != NULL) {
	  pdocResult = xmlNewDoc(BAD_CAST "1.0");
	  xmlDocSetRootElement(pdocResult, pndRoot);
	}
	else {
	  xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST "parse");
	}
	resNodeSetContentDocEat(prnArg, pdocResult);
      }
      else {
	/* handle parser errors */
	xmlSetProp(pndArg, BAD_CAST "error", BAD_CAST "parse");
      }
#endif
    }
    break;

#ifdef HAVE_LIBARCHIVE
    case MIME_APPLICATION_ZIP:
    case MIME_APPLICATION_GZIP:
    case MIME_APPLICATION_X_BZIP:
    case MIME_APPLICATION_X_TAR:
    case MIME_APPLICATION_X_ISO9660_IMAGE:
    {
	xmlNodePtr pndArchive;
	resNodePtr prnI;
	
	PrintFormatLog(2, "Use archive content of file '%s'", resNodeGetNameNormalized(prnArg));
	if (IS_NODE_ARCHIVE(pndArg)) {
	  pndArchive = pndArg;
	}
	else {
	  pndArchive = xmlNewChild(pndArg, NULL, BAD_CAST NAME_ARCHIVE, NULL);
	}

	for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	  xmlNodePtr pndEntry;

	  if ((pndEntry = resNodeToDOM(prnI, RN_OUT_MAX))) {
	    xmlAddChild(pndArchive, pndEntry);
	  }
	}
      }
      break;
#endif

#ifdef HAVE_JSON
    case MIME_APPLICATION_JSON:
      PrintFormatLog(2, "Use JSON content of file '%s'", resNodeGetNameNormalized(prnArg));
      jsonParseFile(pndArg, prnArg);
      break;
#endif

    case MIME_APPLICATION_SLA:
      // rpParseFile(pndArg,prnArg);
      break;

#ifdef HAVE_LIBSQLITE3
    case MIME_APPLICATION_X_SQLITE3:
      dbDumpContextToNode(pndArg, prnArg, (DB_PROC_DECL | DB_PROC_ENTRIES));
      break;
#endif

#ifdef HAVE_LIBID3TAG
    case MIME_AUDIO_MP3:
      dirParseFileAudioMP3(pndArg, prnArg);
      break;
#endif

#ifdef HAVE_LIBVORBIS
    case MIME_AUDIO_OGG:
      dirParseFileAudioVorbis(pndArg,prnArg);
      break;
#endif

#ifdef HAVE_LIBEXIF
    case MIME_IMAGE_JPEG:
    case MIME_IMAGE_TIFF:
    {
      /* get image information details via libexif */
      imgParseFileExif(pndArg, prnArg);
      /*! no break, due to required default block */
    }
#elif defined HAVE_LIBMAGICK	
    case MIME_IMAGE_GIF:
    case MIME_IMAGE_JPEG:
    case MIME_IMAGE_PNG:
    case MIME_IMAGE_TIFF:
    {
      /* get image information details via ImageMagick */
      imgParseFile(pndArg, prnArg);
      /*! no break, due to required default block */
    }
#endif
      
    default: /* no addtitional file information details */
      {
	xmlChar *pucContent;

	pucContent = resNodeGetContentBase64Eat(prnArg,1024);
	if (pucContent) {
	  xmlNodePtr pndBase64;
	  
	  pndBase64 = xmlNewChild(pndArg,NULL,BAD_CAST NAME_BASE64,NULL);
	  if (pndBase64) {
	    xmlNodePtr pndBase64Text;

	    xmlSetProp(pndBase64,BAD_CAST"type",BAD_CAST resNodeGetMimeTypeStr(prnArg));
	    pndBase64Text = xmlNewText(NULL);
	    if (pndBase64Text) {
	      pndBase64Text->content = pucContent; /* direct use of buffer as node content, to avoid duplication of large buffers */
	      pucContent = NULL;
	      xmlAddChild(pndBase64,pndBase64Text);
	    }
	  }
	  xmlFree(pucContent);
	}
      }
    }
    xmlAddChild(pndArg, xmlNewPI(BAD_CAST "end-of", resNodeGetNameBase(prnArg)));
  }
  else {
    //resNodeSetError(prnArg,1,"Unknown type '%s'",pndArg->name);
  }

  return TRUE;
} /* end of resNodeContentToDOM() */


/*!\todo constructs a resNode out of a file DOM

  \param pndArg pointer to dir/file node
  \return resNodeListPtr
*/
resNodePtr 
resNodeFromDOM(xmlNodePtr pndArg, int iArgOptions)
{
  resNodePtr prnResult = NULL;
  xmlChar *pucT;

/*

check/merge code of dirNodeToResNodeList()

*/
  if (pndArg != NULL && (pucT = domGetPropValuePtr(pndArg, BAD_CAST "name")) != NULL) {
	xmlNodePtr pndT;

    prnResult = resNodeNew();
    resNodeSetNameBase(prnResult, pucT);
    if (xmlStrEqual(pndArg->name, BAD_CAST "file")) {
      resNodeSetType(prnResult, rn_type_file);
    }
    else if (xmlStrEqual(pndArg->name, BAD_CAST "dir")) {
      resNodeSetType(prnResult, rn_type_dir);
    }
    else {
      // resNodeSetType(prnResult,  rn_type_file);
    }
    resNodeSetNameBaseDir(prnResult, domGetPropValuePtr(pndArg, BAD_CAST "prefix"));
    // resNodeSetSize(prnResult,domGetPropValuePtr(pndArg,BAD_CAST"size"));
    resNodeSetMimeType(prnResult, resMimeGetType((const char *)domGetPropValuePtr(pndArg, BAD_CAST "type")));

    if ((pndT = domGetFirstChild(pndArg, BAD_CAST NAME_BASE64)) != NULL && pndT->children != NULL && (pucT = pndT->children->content) != NULL) {
      int ret;
      char *pchT;
      size_t inlen;
      size_t outlen;

      inlen = xmlStrlen(pucT);
      pchT = (char *)xmlMalloc(inlen * sizeof(char *));
      outlen = inlen;

      ret = base64decode((char *)pucT, inlen, BAD_CAST pchT, &outlen);
      if (ret==0) {
	PrintFormatLog(3, "Decoded '%i' byte to '%i'", inlen, outlen);
	prnResult->pContent = pchT;
	resNodeSetSize(prnResult, outlen);
      }
      else {
	// return NULL;
      }
    }
    else if (pndArg->children != NULL) {
	/* dump DOM to pContent */
    }
  }
  else {}
  return prnResult;
} /* end of resNodeFromDOM() */


/*!\todo constructs a resNodeList out of a pie/dir DOM

\param prnArg -- resNode tree to build as DOM
\param iArgOptions bits for options 
\return DOM tree representing prnArg
*/
resNodePtr 
resNodeListFromDOM(xmlNodePtr pndArg, int iArgOptions)
{
  resNodePtr prnResult = NULL;
  xmlNodePtr pndT;

/*

check code of dirNodeToResNodeList()

*/
  for (pndT = pndArg; pndT; pndT = pndT->next) {
    if (prnResult == NULL) {
      prnResult = resNodeFromDOM(pndT, iArgOptions);
    }
    else {
      resNodeAddSibling(prnResult,resNodeFromDOM(pndT, iArgOptions));
    }
  }
  return prnResult;
} /* end of resNodeListFromDOM() */


/*!tests the readability of a file or directory with given name 

  \param pndFile pointer to file node for appending of results
  \param pucArgPath relative or absolute path and name of file
  \return true if the named file is is readable

  set all file stat to pndFile as attributes
*/
xmlNodePtr
resNodeToDOM(resNodePtr prnArg, int iArgOptions)
{
  xmlNodePtr pndResult = NULL;

  if (prnArg) {
    xmlChar *pucT;
    xmlChar mpucOut[BUFFER_LENGTH];
    xmlNodePtr pndT;
    resNodePtr prnI;

    if (resNodeIsDir(prnArg) || resNodeIsDirInArchive(prnArg)) {
      pndT = xmlNewNode(NULL, BAD_CAST NAME_DIR);
      if ((pucT = resNodeGetNameBase(prnArg)) != NULL && xmlStrlen(pucT) > 0 && xmlStrEqual(pucT, BAD_CAST ".") == FALSE) {
	xmlSetProp(pndT, BAD_CAST "name", pucT);
      }
      else {
	xmlSetProp(pndT, BAD_CAST "name", BAD_CAST ".");
      }

      if (resNodeIsRoot(prnArg) == FALSE && resNodeGetParent(prnArg) == NULL && (pucT = resNodeGetNameBaseDir(prnArg)) != NULL && xmlStrlen(pucT) > 0) {
	xmlSetProp(pndT, BAD_CAST "prefix", pucT);
      }
    }
    else if (resNodeIsLink(prnArg)) {
      pndT = xmlNewNode(NULL, BAD_CAST NAME_SYMLINK);
      xmlSetProp(pndT, BAD_CAST "name", resNodeGetNameBase(prnArg));
      if (resNodeGetChild(prnArg)) {
	xmlNodePtr pndTT;

	pndTT = resNodeToDOM(resNodeGetChild(prnArg), RN_OUT_MIN);
#ifdef HAVE_CGI
	pucT = resPathDiffPtr(resNodeGetNameBaseDir(prnArg), resNodeGetNameBaseDir(resNodeGetChild(prnArg)));
	if (STR_IS_NOT_EMPTY(pucT)) {
	  xmlSetProp(pndTT, BAD_CAST "prefix", pucT);
	}
	else {
	  xmlNodeSetName(pndTT, BAD_CAST "error");
	  xmlNodeSetContent(pndTT, BAD_CAST "out of root");
	}
#else
	xmlSetProp(pndTT, BAD_CAST "prefix", resNodeGetNameBaseDir(resNodeGetChild(prnArg)));
#endif
	xmlAddChild(pndT, pndTT);
      }
    }
    else {
      pndT = xmlNewNode(NULL, BAD_CAST NAME_FILE);
      xmlSetProp(pndT, BAD_CAST "name", resNodeGetNameBase(prnArg));
      if (resNodeGetParent(prnArg) == NULL && (pucT = resNodeGetNameBaseDir(prnArg)) != NULL && xmlStrlen(pucT) > 0) {
	xmlSetProp(pndT, BAD_CAST "prefix", pucT);
      }
    }

    if (IS_OUT_TYPE(iArgOptions) && resNodeGetMimeType(prnArg) != MIME_UNDEFINED) { /* MIME type */
      xmlSetProp(pndT, BAD_CAST "type", BAD_CAST resNodeGetMimeTypeStr(prnArg));
    }

    if (IS_OUT_PATH(iArgOptions) && (pucT = resNodeGetExtension(prnArg)) != NULL && xmlStrlen(pucT) > 0) {
      xmlSetProp(pndT, BAD_CAST "ext", pucT);
    }

    /* set kind of error
     */
    switch (resNodeGetError(prnArg)) {
    case rn_error_none:
      break;
    case rn_error_stat:
      xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "stat");
      break;
    case rn_error_name:
      xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "name");
      break;
    case rn_error_owner:
      // xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "owner");
      break;
    case rn_error_memory:
      xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "memory");
      break;
    case rn_error_max_path:
      xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "maxpath");
      break;
    default:
      xmlSetProp(pndT, BAD_CAST "error", BAD_CAST "context");
    }

    if (resNodeIsExist(prnArg)) {

      /* set values from filesystem as file properties
       */
      if (IS_OUT_SIZE(iArgOptions) && (resNodeIsFile(prnArg) || resNodeIsFileInArchive(prnArg) || resNodeIsURL(prnArg))) {
	xmlStrPrintf(mpucOut, BUFFER_LENGTH, "%li", resNodeGetSize(prnArg));
	xmlSetProp(pndT, BAD_CAST "size", mpucOut);
      }

      if (IS_OUT_TIME(iArgOptions)) { /*  */
	long int liMtime;

	liMtime = (long int)resNodeGetMtime(prnArg);
	if (liMtime == 0 || liMtime == 315529200) {
	  /* avoid symbolic dates "1970-01-01T00:00:00" "1980-01-01T00:00:00" */
	}
	else {
	  xmlStrPrintf(mpucOut, BUFFER_LENGTH, "%lu", liMtime);
	  xmlSetProp(pndT, BAD_CAST "mtime", mpucOut);
	  xmlSetProp(pndT, BAD_CAST "mtime2", resNodeGetMtimeStr(prnArg));
	}
      }

      if (IS_OUT_ATTR(iArgOptions)) { /* file system attributes */
	xmlSetProp(pndT, BAD_CAST "read", BAD_CAST(resNodeIsReadable(prnArg) ? "yes" : "no"));
	xmlSetProp(pndT, BAD_CAST "write", BAD_CAST(resNodeIsWriteable(prnArg) ? "yes" : "no"));
	xmlSetProp(pndT, BAD_CAST "execute", BAD_CAST(resNodeIsExecuteable(prnArg) ? "yes" : "no"));
	xmlSetProp(pndT, BAD_CAST "hidden", BAD_CAST(resNodeIsHidden(prnArg) ? "yes" : "no"));
      }

      if (prnArg->ppucProp) {
	size_t i;
	xmlNodePtr pndInfo = NULL;

	pndInfo = xmlNewChild(pndT, NULL, BAD_CAST "info", NULL);
	if (pndInfo) {
	  for (i = 0; prnArg->ppucProp[i]; i += 2) { xmlNewChild(pndInfo, NULL, prnArg->ppucProp[i], prnArg->ppucProp[i + 1]); }
	}
      }
    }

    if (resNodeIsDir(prnArg) || resNodeIsDirInArchive(prnArg)) {
      for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) { xmlAddChild(pndT, resNodeToDOM(prnI, iArgOptions)); }
    }
    else if (resNodeIsZipDocument(prnArg)) {
      xmlNodePtr pndArchive;

      prnArg->eType = rn_type_zip;
      pndArchive = xmlNewChild(pndT, NULL, BAD_CAST "archive", NULL);
      if (pndArchive) {
	if (prnArg->pContent != NULL || prnArg->pdocContent != NULL) {
	  /* use existing buffer content */
	}
	else if (prnArg->children != NULL) {
	  /* use existing descendants */
	}
	else {
	  zipDocumentRead(prnArg, iArgOptions);
	}

	for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) { xmlAddChild(pndArchive, resNodeToDOM(prnI, iArgOptions)); }
      }
    }
#ifdef HAVE_LIBARCHIVE
    else if (resNodeIsArchive(prnArg)) {
      xmlNodePtr pndArchive;

      prnArg->eType = rn_type_archive;
      pndArchive = xmlNewChild(pndT, NULL, BAD_CAST "archive", NULL);
      if (pndArchive) {
	if (arcAppendEntries(prnArg, NULL, IS_OUT_CONTENT(iArgOptions))) {
	  for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) { xmlAddChild(pndArchive, resNodeToDOM(prnI, iArgOptions)); }
	}
      }
    }
#endif
    else if (resNodeIsFileInArchive(prnArg)) {
      if (IS_OUT_CONTENT(iArgOptions) && (resNodeGetContentPtr(prnArg) || resNodeUpdate(prnArg, RN_INFO_CONTENT, NULL, NULL))) {
	resNodeContentToDOM(pndT, prnArg);
      }
      else if (resNodeGetChild(prnArg)) {
	for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) { xmlAddChild(pndT, resNodeToDOM(prnI, iArgOptions)); }
      }
    }
    else if (IS_OUT_CONTENT(iArgOptions) && resNodeIsURL(prnArg)) {
      if (resNodeGetChild(prnArg)) { /* there are updated childs of this URL already */
	for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) { xmlAddChild(pndT, resNodeToDOM(prnI, iArgOptions)); }
      }
      else if (resNodeGetContentPtr(prnArg) || resNodeUpdate(prnArg, RN_INFO_CONTENT, NULL, NULL)) {
	resNodeContentToDOM(pndT, prnArg);
      }
    }
    else if (IS_OUT_CONTENT(iArgOptions) && resNodeIsLink(prnArg)) {
      /*! add link target content */
      resNodeContentToDOM(pndT, prnArg);
    }
    else if (IS_OUT_PATH(iArgOptions) && (resNodeIsShortcut(prnArg) || resNodeGetMimeType(prnArg) == MIME_APPLICATION_CXP_XML)) {
      /*! required for shortcuts, titles and icons */
      resNodeContentToDOM(pndT, prnArg);
    }
    else if (resNodeIsPicture(prnArg)) {
      if (IS_OUT_CONTENT(iArgOptions)) {
	resNodeContentToDOM(pndT, prnArg);
      }
#ifdef HAVE_LIBEXIF
      else if (IS_OPTION_INFO(iArgOptions)) {
	/* get image information details via libexif */
	imgParseFileExif(pndT, prnArg);
      }
#endif
    }
    else if (resNodeIsVideo(prnArg)) {
      // resNodeContentToDOM(pndT, prnArg);
    }
    else if (resNodeIsFile(prnArg) || resNodeIsFileInArchive(prnArg)) {

      if (IS_OUT_CONTENT(iArgOptions) || (IS_OUT_XML(iArgOptions) && resMimeIsXml(resNodeGetMimeType(prnArg)))) {
	if (resNodeUpdate(prnArg, RN_INFO_CONTENT, NULL, NULL)) {
	  resNodeContentToDOM(pndT, prnArg);
	}
      }

      if (IS_OUT_CONTENT(iArgOptions) && resNodeGetNameObject(prnArg) != NULL) {
	xmlSetProp(pndT, BAD_CAST "object", resNodeGetNameObject(prnArg));
      }
    }

    if (IS_OUT_OWNER(iArgOptions) && (pucT = resNodeGetOwner(prnArg)) != NULL) {
      xmlSetProp(pndT, BAD_CAST "owner", pucT);
    }

    /*\todo insert comment text from file */

    pndResult = pndT;
  }

  return pndResult;
} /* end of resNodeToDOM() */


/*! Resource Node List To DOM

\param prnArg -- resNode tree to build as DOM
\param iArgOptions bits for options 
\return DOM tree representing prnArg
*/
xmlNodePtr
resNodeListToDOM(resNodePtr prnArg, int iArgOptions)
{
  xmlNodePtr pndResult = NULL;
  resNodePtr prnT;

  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    if (pndResult == NULL) {
      pndResult = resNodeToDOM(prnT, iArgOptions);
    }
    else {
      xmlAddSibling(pndResult, resNodeToDOM(prnT, iArgOptions));
    }
  }

  return pndResult;
} /* end of resNodeListToDOM() */


/*! Resource Node List To Xml String 

\param prnArg -- resNode tree to build as XML string
\param iArgOptions bits for options 
\return pointer to buffer with XML string
*/
xmlChar *
resNodeListToXmlStr(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  xmlNodePtr pndT = NULL;
  xmlDocPtr pdocT = NULL;

  if (prnArg) {
    if ((pndT = resNodeListToDOM(prnArg, iArgOptions)) != NULL && (pdocT = domDocFromNodeNew(pndT)) != NULL) {
      int iLength = 0;

      xmlDocDumpMemoryEnc(pdocT, &pucResult, &iLength, "UTF-8");
      if (iLength > 0 && pucResult != NULL) {
	/* success */
      }
      else {
	resNodeSetError(prnArg, rn_error_xml, "Error xmlDocDumpMemoryEnc()");
      }
    }
    else {
      resNodeSetError(prnArg, rn_error_xml, "Error resNodeListToXmlStr()");
    }

    xmlFreeNode(pndT);
    xmlFreeDoc(pdocT);
  }
  return pucResult;
} /* end of resNodeListToXmlStr() */


/*!
\todo change to a single line format

  \param prnArg a pointer to a resource node
  \return TRUE if prnArg is initialized
 */
xmlChar *
resNodeToPlain(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    xmlChar mucT[BUFFER_LENGTH];

    switch (resNodeGetType(prnArg)) {

    case rn_type_stdout:
    case rn_type_stderr:
    case rn_type_stdin:
    case rn_type_dir_in_zip:
    case rn_type_file_in_zip:
      /* ignoring */
      break;

    case rn_type_dir:
    case rn_type_archive:
    case rn_type_dir_in_archive:
    case rn_type_file:
    case rn_type_file_in_archive:
    case rn_type_symlink:

      if (IS_OUT_ATTR(iArgOptions)) { /* file system attributes */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%c%c%c%c%c\"" CSV_SEP_STR,
		     (resNodeIsDir(prnArg)    ? 'd'
		      : resNodeIsLink(prnArg) ? 'l'
					      : '-'),
		     resNodeIsReadable(prnArg) ? 'r' : '-', resNodeIsWriteable(prnArg) ? 'w' : '-', resNodeIsExecuteable(prnArg) ? 'x' : '-',
		     resNodeIsHidden(prnArg) ? 'h' : '-');
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_SIZE(iArgOptions)) { /* size + child */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "%lu" CSV_SEP_STR "%lu" CSV_SEP_STR "%lu" CSV_SEP_STR, resNodeGetSize(prnArg), resNodeGetRecursiveSize(prnArg),
		     resNodeGetCountChilds(prnArg));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_TIME(iArgOptions)) { /* name + path */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR "%lu" CSV_SEP_STR, resNodeGetMtimeStr(prnArg), resNodeGetMtimeDiff(prnArg));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, (resNodeGetNameBase(prnArg) != NULL ? resNodeGetNameBase(prnArg) : BAD_CAST ""));
      pucResult = xmlStrcat(pucResult, mucT);

      if (IS_OUT_PATH(iArgOptions)) { /* name + path */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR "=HYPERLINK(\"%s\";\"%s\")" CSV_SEP_STR "\"%s\"" CSV_SEP_STR,
		     (resNodeGetExtension(prnArg) != NULL ? resNodeGetExtension(prnArg) : BAD_CAST ""), resNodeGetNameBaseDir(prnArg),
		     resNodeGetNameBaseDir(prnArg), (resNodeGetNameObject(prnArg) != NULL ? resNodeGetNameObject(prnArg) : BAD_CAST ""));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_OWNER(iArgOptions)) { /* owner */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, (resNodeGetOwner(prnArg) != NULL ? resNodeGetOwner(prnArg) : BAD_CAST "---"));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_TYPE(iArgOptions)) { /* MIME type */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, resNodeGetMimeTypeStr(prnArg));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      pucResult = xmlStrcat(pucResult, BAD_CAST "\n");
      break;

    default:
      /* ignoring */
      break;
    }
  }

  return pucResult;
} /* end of resNodeToPlain() */


/*! Resource Node List To Plain

\param prnArg -- resNode tree to build as plain string
\param iArgOptions bits for options 
\return pointer to buffer with plain output
*/
xmlChar *
resNodeListToPlain(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    resNodePtr prnT;

    for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
      xmlChar* pucT = NULL;
      resNodePtr prnChild;

      if ((prnChild = resNodeGetChild(prnT))) {
	if ((pucT = resNodeListToPlain(prnChild, iArgOptions))) {
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult, pucT);
	    xmlFree(pucT);
	  }
	  else {
	    pucResult = pucT;
	  }
	}
      }

      if ((pucT = resNodeToPlain(prnT, iArgOptions))) {
	if (pucResult) {
	  pucResult = xmlStrcat(pucResult, pucT);
	  xmlFree(pucT);
	}
	else {
	  pucResult = pucT;
	}
      }
    }
  }
  else {
    resNodeSetError(prnArg,rn_error_argv,"Error resNodeListToPlain()");
  }
  return pucResult;
} /* end of resNodeListToPlain() */


/*! Read and sets the file MIME type of this context.

  \param prnArg the context
 */
xmlChar *
resNodeCSVHeaderStr(int iArgOptions)
{
  xmlChar *pucResult = NULL;

  pucResult = xmlStrdup(BAD_CAST "sep=" CSV_SEP_STR "\n");

  if (IS_OUT_ATTR(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"Mode\"" CSV_SEP_STR);
  }

  if (IS_OUT_SIZE(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"Size [Byte]\"" CSV_SEP_STR "\"Recursive Size [Byte]\"" CSV_SEP_STR "\"# Childs\"" CSV_SEP_STR);
  }

  if (IS_OUT_TIME(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"MTime\"" CSV_SEP_STR "\"DiffTime [s]\"" CSV_SEP_STR);
  }

  pucResult = xmlStrcat(pucResult, BAD_CAST "\"Name\"" CSV_SEP_STR);

  if (IS_OUT_PATH(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"Extension\"" CSV_SEP_STR "\"Basedir\"" CSV_SEP_STR "\"Object Name\"" CSV_SEP_STR);
  }

  if (IS_OUT_OWNER(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"Owner\"" CSV_SEP_STR);
  }

  if (IS_OUT_TYPE(iArgOptions)) {
    pucResult = xmlStrcat(pucResult, BAD_CAST "\"MIME\"" CSV_SEP_STR);
  }

  pucResult = xmlStrcat(pucResult, BAD_CAST "\n");

  return pucResult;
} /* end of resNodeCSVHeaderStr() */

/*!
  \param prnArg a pointer to a resource node
  \return pointer to a string buffer containing separated columns
 */
xmlChar *
resNodeToCSV(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    xmlChar mucT[BUFFER_LENGTH];

    switch (resNodeGetType(prnArg)) {

    case rn_type_stdout:
    case rn_type_stderr:
    case rn_type_stdin:
    case rn_type_dir_in_zip:
    case rn_type_file_in_zip:
      /* ignoring */
      break;

    case rn_type_dir:
    case rn_type_archive:
    case rn_type_dir_in_archive:
    case rn_type_file:
    case rn_type_file_in_archive:
    case rn_type_symlink:

      if (IS_OUT_STRUCT(iArgOptions) == FALSE && resNodeGetType(prnArg) != rn_type_dir) {
	/* ignoring all but directories */
	break;
      }

      if (IS_OUT_ATTR(iArgOptions)) { /* file system attributes */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%c%c%c%c%c\"" CSV_SEP_STR,
		     (resNodeIsDir(prnArg)    ? 'd'
		      : resNodeIsLink(prnArg) ? 'l'
					      : '-'),
		     resNodeIsReadable(prnArg) ? 'r' : '-', resNodeIsWriteable(prnArg) ? 'w' : '-', resNodeIsExecuteable(prnArg) ? 'x' : '-',
		     resNodeIsHidden(prnArg) ? 'h' : '-');
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_SIZE(iArgOptions)) { /* size + child */
	if (resNodeGetCountChilds(prnArg) > 0) {
	  xmlStrPrintf(mucT, BUFFER_LENGTH, "%lu" CSV_SEP_STR "%lu" CSV_SEP_STR "%lu" CSV_SEP_STR, resNodeGetSize(prnArg), resNodeGetRecursiveSize(prnArg),
		       resNodeGetCountChilds(prnArg));
	}
	else {
	  xmlStrPrintf(mucT, BUFFER_LENGTH, "%lu" CSV_SEP_STR "" CSV_SEP_STR "" CSV_SEP_STR, resNodeGetSize(prnArg));
	}
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_TIME(iArgOptions)) { /* name + path */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR "%lu" CSV_SEP_STR, resNodeGetMtimeStr(prnArg), resNodeGetMtimeDiff(prnArg));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, (resNodeGetNameBase(prnArg) != NULL ? resNodeGetNameBase(prnArg) : BAD_CAST ""));
      pucResult = xmlStrcat(pucResult, mucT);

      if (IS_OUT_PATH(iArgOptions)) { /* name + path */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR "=HYPERLINK(\"%s\";\"%s\")" CSV_SEP_STR "\"%s\"" CSV_SEP_STR,
		     (resNodeGetExtension(prnArg) != NULL ? resNodeGetExtension(prnArg) : BAD_CAST ""), resNodeGetNameBaseDir(prnArg),
		     resNodeGetNameBaseDir(prnArg), (resNodeGetNameObject(prnArg) != NULL ? resNodeGetNameObject(prnArg) : BAD_CAST ""));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_OWNER(iArgOptions)) { /* owner */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, (resNodeGetOwner(prnArg) != NULL ? resNodeGetOwner(prnArg) : BAD_CAST "---"));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      if (IS_OUT_TYPE(iArgOptions)) { /* MIME type */
	xmlStrPrintf(mucT, BUFFER_LENGTH, "\"%s\"" CSV_SEP_STR, resNodeGetMimeTypeStr(prnArg));
	pucResult = xmlStrcat(pucResult, mucT);
      }

      pucResult = xmlStrcat(pucResult, BAD_CAST "\n");
      break;

    default:
      /* ignoring */
      break;
    }
  }

  return pucResult;
} /* end of resNodeToCSV() */

#ifdef DEBUG

/*!
\todo change to a single line format

  \param prnArg a pointer to a resource node
  \return TRUE if prnArg is initialized
 */
xmlChar *
resNodeToGraphviz(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  pucResult = BAD_CAST xmlMalloc((BUFFER_LENGTH + 1) * sizeof(xmlChar));
  if (pucResult) {

    switch (resNodeGetType(prnArg)) {
    case rn_type_stdout:
    case rn_type_stderr:
    case rn_type_stdin:
      xmlStrPrintf(pucResult, BUFFER_LENGTH, "// stdin\n");
      break;
    default:
      xmlStrPrintf(pucResult, BUFFER_LENGTH,
		   "node%x [label = \"<f0> %X|name: %s|size: %li|type: %s|<f2> parent:%X|<f3> next:%X|<f4> children:%X|<f5> last:%X\"];\n"
		   "%snode%x:f2 -> node%x:f0;\n"
		   "%snode%x:f3 -> node%x:f0;\n"
		   "%snode%x:f4 -> node%x:f0;\n"
		   "%snode%x:f5 -> node%x:f0;\n",
		   prnArg,
		   //
		   prnArg, resNodeGetNameBase(prnArg), resNodeGetSize(prnArg), resNodeGetTypeStr(prnArg), prnArg->parent, prnArg->next, prnArg->children,
		   prnArg->last,
		   //
		   (prnArg->prev == NULL ? "//" : "//"), prnArg, prnArg->parent,
		   //
		   (prnArg->next == NULL ? "//" : ""), prnArg, prnArg->next,
		   //
		   (prnArg->children == NULL ? "//" : ""), prnArg, prnArg->children,
		   //
		   (prnArg->last == NULL ? "//" : ""), prnArg, prnArg->last);
      break;
    }
  }

  return pucResult;
} /* end of resNodeToGraphviz() */


/*! Resource Node List To Graphviz

\param prnArg -- resNode tree to build as graphviz string
\param iArgOptions bits for options 
\return pointer to buffer with graphviz output
*/
xmlChar *
resNodeListToGraphviz(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg) {
    resNodePtr prnT;

    for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
      xmlChar* pucT = NULL;
      resNodePtr prnChild;

      if ((prnChild = resNodeGetChild(prnT))) {
	if ((pucT = resNodeListToGraphviz(prnChild, iArgOptions))) {
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult, pucT);
	    xmlFree(pucT);
	  }
	  else {
	    pucResult = pucT;
	  }
	}
      }

      if ((pucT = resNodeToGraphviz(prnT, iArgOptions))) {
	if (pucResult) {
	  pucResult = xmlStrcat(pucResult, pucT);
	  xmlFree(pucT);
	}
	else {
	  pucResult = pucT;
	}
      }
    }
  }
  else {
    resNodeSetError(prnArg,rn_error_argv,"Error resNodeListToGraphviz()");
  }
  return pucResult;
} /* end of resNodeListToGraphviz() */

#endif

/*!
\todo change to a single line format

\param prnArg a pointer to a resource node
\return TRUE if prnArg is initialized
*/
xmlChar *
resNodeToJSON(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (resNodeReadStatus(prnArg) && !resNodeIsHidden(prnArg)) {
    pucResult = BAD_CAST xmlMalloc((BUFFER_LENGTH + 1) * sizeof(xmlChar));
    if (pucResult) {

      switch (resNodeGetType(prnArg)) {
      case rn_type_stdout:
	xmlStrPrintf(pucResult, BUFFER_LENGTH, "'type' : 'stdout'");
	break;
      case rn_type_stderr:
	xmlStrPrintf(pucResult, BUFFER_LENGTH, "'type' : 'stderr'");
	break;
      case rn_type_stdin:
	xmlStrPrintf(pucResult, BUFFER_LENGTH, "'type' : 'stdin'");
	break;
      case rn_type_dir:
      case rn_type_dir_in_archive:
      case rn_type_dir_in_zip:
      case rn_type_file:
      case rn_type_file_in_archive:
      case rn_type_file_in_zip:
      case rn_type_symlink:
	xmlStrPrintf(pucResult, BUFFER_LENGTH, "'type' : '%s', 'name' : '%s', 'attr' : '%c%c%c%c', 'size' : %li, 'mtime' : '%s', 'owner' : '%s', 'mime' : '%s'",
		     resNodeIsDir(prnArg) ? "dir" : "",
		     //
		     (resNodeGetNameBase(prnArg) != NULL ? resNodeGetNameBase(prnArg) : BAD_CAST "."),
		     //
		     resNodeIsReadable(prnArg) ? 'r' : '-', resNodeIsWriteable(prnArg) ? 'w' : '-', resNodeIsExecuteable(prnArg) ? 'x' : '-',
		     resNodeIsHidden(prnArg) ? 'h' : '-',
		     //
		     resNodeGetSize(prnArg),
		     //
		     resNodeGetMtimeStr(prnArg),
		     //
		     (resNodeGetOwner(prnArg) != NULL ? resNodeGetOwner(prnArg) : BAD_CAST "undefined"),
		     //
		     resNodeGetMimeTypeStr(prnArg));
	break;
      default:
	xmlStrPrintf(pucResult, BUFFER_LENGTH, "'type' : 'undef'");
	break;
      }
    }
  }

  /*!\todo set kind of error */

  return pucResult;
} /* end of resNodeToJSON() */


/*! Resource Node List To JSON  

\param prnArg -- resNode tree to build as JSON string
\param iArgOptions bits for options 
\return pointer to buffer with JSON
*/
xmlChar *
resNodeListToJSON(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  resNodePtr prnT;

  pucResult = xmlStrdup(BAD_CAST"\n[");
  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    xmlChar *pucT = NULL;
    resNodePtr prnChild;

    pucResult = xmlStrcat(pucResult, BAD_CAST"{");
    pucT = resNodeToJSON(prnT,iArgOptions);
    pucResult = xmlStrcat(pucResult, pucT);
    xmlFree(pucT);

    if ((prnChild = resNodeGetChild(prnT))) {
      pucResult = xmlStrcat(pucResult, BAD_CAST", 'children' : ");
      pucT = resNodeListToJSON(prnChild,iArgOptions);
      pucResult = xmlStrcat(pucResult, pucT);
      xmlFree(pucT);
    }
    pucResult = xmlStrcat(pucResult, BAD_CAST"}");
  }
  pucResult = xmlStrcat(pucResult, BAD_CAST"]\n");
  
  return pucResult;
} /* end of resNodeListToJSON() */


/*! Read and sets the file MIME type of this context.

  \param prnArg the context
 */
xmlChar *
resNodeDatabaseSchemaStr(int iArgOptions)
{
  int i;
  xmlChar *pucResult = NULL;
  xmlChar mpucT[BUFFER_LENGTH];

  pucResult = xmlStrdup(BAD_CAST "PRAGMA journal_mode = OFF;\n"
				 "\n"
				 "CREATE TABLE IF NOT EXISTS 'directory' ("
				 "depth INTEGER, "
				 "type INTEGER, "
				 "name text, "
				 "owner text, "
				 "childs INTEGER, "
				 "size INTEGER, "
				 "ext text, "
				 "object text, "
				 "path text, "
				 "mime INTEGER, "
				 "mtime INTEGER, "
				 "mtime2 text, "
				 "r INTEGER, "
				 "w INTEGER, "
				 "x INTEGER, "
				 "h INTEGER "
				 ");\n\n");

  pucResult = xmlStrcat(pucResult, BAD_CAST "CREATE TABLE IF NOT EXISTS 'meta' (timestamp INTEGER, key text, value text);\n\n");

  pucResult = xmlStrcat(pucResult, BAD_CAST "CREATE TABLE IF NOT EXISTS 'mimetypes' (mime INTEGER, name text);\n\n");

  for (i = MIME_UNKNOWN; i < MIME_END; i++) {
    char *pcMime;

    pcMime = (char *)resMimeGetTypeStr(i);
    if (pcMime) {
      xmlStrPrintf(mpucT, BUFFER_LENGTH, "INSERT INTO 'mimetypes' VALUES (%i,\"%s\");\n", i, BAD_CAST pcMime);
      pucResult = xmlStrcat(pucResult, mpucT);
    }
  }
  pucResult = xmlStrcat(pucResult, BAD_CAST "\n\n");

  pucResult = xmlStrcat(pucResult,
			BAD_CAST "CREATE TABLE IF NOT EXISTS 'queries' (query text);\n"
				 "\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT * FROM meta;\");\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT DISTINCT name FROM directory;\");\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT sum(size)/(1024*1024*1024) AS GB FROM directory;\");\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT sum(size)/1024 AS KBytes, path FROM directory WHERE path in (SELECT DISTINCT path "
				 "FROM directory) GROUP BY path ORDER BY KBytes DESC;\");\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT path || '/' || name AS File,(size / 1048576) AS MB,mtime2 AS MTime FROM directory "
				 "WHERE (size > 1048576) ORDER BY MB DESC;\");\n"
				 "INSERT INTO 'queries' VALUES (\"SELECT count() AS Count, name AS Name FROM directory GROUP BY name ORDER BY Count DESC;\");\n"
				 "\n");

  return pucResult;
} /* end of resNodeDatabaseSchemaStr() */

/*!
 *
 * \todo a more structure oriented db scheme (parent-id?)
\param prnArg a pointer to a resource node
\return TRUE if prnArg is initialized
*/
xmlChar *
resNodeToSQL(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;

  if (prnArg != NULL) {
    pucResult = BAD_CAST xmlMalloc((BUFFER_LENGTH + 1) * sizeof(xmlChar));
    if (pucResult) {
      if (resNodeReadStatus(prnArg) == FALSE || resNodeIsError(prnArg)) {
	time_t system_zeit_1;

	/* add readable time */
	time(&system_zeit_1);

	switch (resNodeGetError(prnArg)) {
	case rn_error_stat:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/stat",
		       resNodeGetNameNormalized(prnArg));
	  break;
	case rn_error_name:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/name",
		       resNodeGetNameNormalized(prnArg));
	  break;
	case rn_error_owner:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/owner",
		       resNodeGetNameNormalized(prnArg));
	  break;
	case rn_error_memory:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/memory",
		       resNodeGetNameNormalized(prnArg));
	  break;
	case rn_error_max_path:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/maxpath",
		       resNodeGetNameNormalized(prnArg));
	  break;
	default:
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "INSERT INTO 'meta' VALUES (%li,\"%s\",\"%s\");\n", (long int)system_zeit_1, BAD_CAST "error/context",
		       resNodeGetNameNormalized(prnArg));
	}
      }
      else {
	xmlChar *pucT;
	xmlChar *pucSqlValues;

	pucSqlValues = xmlStrdup(BAD_CAST "INSERT INTO 'directory' VALUES (");

	xmlStrPrintf(pucResult, BUFFER_LENGTH, "%i,%i,", resPathGetDepth(resNodeGetNameNormalized(prnArg)), resNodeGetType(prnArg));
	pucSqlValues = xmlStrcat(pucSqlValues, pucResult);

	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	if ((pucT = resNodeGetNameBase(prnArg))) {
	  pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	}
	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");

	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	if ((pucT = resNodeGetOwner(prnArg))) {
	  pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	}
	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");

	xmlStrPrintf(pucResult, BUFFER_LENGTH, "%li,", resNodeGetCountChilds(prnArg));
	pucSqlValues = xmlStrcat(pucSqlValues, pucResult);

	if (resNodeIsDir(prnArg)) {
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "%li,\"\",\"\",", resNodeGetRecursiveSize(prnArg));
	  pucSqlValues = xmlStrcat(pucSqlValues, pucResult);
	}
	else {
	  xmlStrPrintf(pucResult, BUFFER_LENGTH, "%li,", resNodeGetSize(prnArg));
	  pucSqlValues = xmlStrcat(pucSqlValues, pucResult);

	  pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	  if ((pucT = resNodeGetExtension(prnArg))) {
	    pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	  }
	  pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");

	  pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	  if ((pucT = resNodeGetNameObject(prnArg))) {
	    pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	  }
	  pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");
	}

	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	if ((pucT = resNodeGetNameBaseDir(prnArg))) {
	  pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	}
	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");

	xmlStrPrintf(pucResult, BUFFER_LENGTH, "%i,%li,", (int)resNodeGetMimeType(prnArg), (long)(resNodeGetMtime(prnArg)));
	pucSqlValues = xmlStrcat(pucSqlValues, pucResult);

	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\"");
	if ((pucT = resNodeGetMtimeStr(prnArg))) {
	  pucSqlValues = xmlStrcat(pucSqlValues, pucT);
	}
	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST "\",");

	xmlStrPrintf(pucResult, BUFFER_LENGTH, "%c,%c,%c,%c", (resNodeIsReadable(prnArg) ? '1' : '0'), (resNodeIsWriteable(prnArg) ? '1' : '0'),
		     (resNodeIsExecuteable(prnArg) ? '1' : '0'), (resNodeIsHidden(prnArg) ? '1' : '0'));
	pucSqlValues = xmlStrcat(pucSqlValues, pucResult);

	pucSqlValues = xmlStrcat(pucSqlValues, BAD_CAST ")");

	xmlFree(pucResult);
	pucResult = pucSqlValues;
	pucResult = xmlStrcat(pucResult, BAD_CAST ";\n");
      }
    }
  }
  return pucResult;
} /* end of resNodeToSQL() */


/*! Resource Node List To SQL

\param prnArg -- resNode tree to build as SQL INSERT statements
\param iArgOptions bits for options 
\return pointer to buffer with SQL INSERTs
*/
xmlChar *
resNodeListToSQL(resNodePtr prnArg, int iArgOptions)
{
  xmlChar *pucResult = NULL;
  resNodePtr prnT;

  for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
    resNodePtr prnChild;
    xmlChar *pucT;

    pucT = resNodeToSQL(prnT,iArgOptions);
    pucResult = xmlStrcat(pucResult, pucT);
    xmlFree(pucT);

    if ((prnChild = resNodeGetChild(prnT))) {
      xmlChar *pucTT;

      pucTT = resNodeListToSQL(prnChild,iArgOptions);
      pucResult = xmlStrcat(pucResult,pucTT);
      xmlFree(pucTT);
    }
  }
  return pucResult;
} /* end of resNodeListToSQL() */


/*! dump a resNode to 'argout' using 'pfArg'

\todo handle links and archives in archive

\param argout -- output stream
\param prnArg -- resNode to dump
\param pfArg -- pointer to format function

\return TRUE if successful, FALSE in case of errors
*/
BOOL_T
resNodeListDumpRecursively(FILE *argout, resNodePtr prnArg, int iArgDetails, xmlChar *(*pfArg)(resNodePtr, int))
{
  BOOL_T fResult = FALSE;
  xmlChar *pucT;
  resNodePtr prnI;

#ifdef HAVE_LIBARCHIVE
  if (resNodeIsDirInArchive(prnArg)) {
    resNodePtr prnRelease;

#ifdef DEBUG
    fputc('\\', stderr);
#endif

    /* archive entries are listed recursively already */
    for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
      resNodeListDumpRecursively(argout, prnI, iArgDetails, pfArg);
      resNodeIncrRecursiveSize(prnArg, resNodeGetRecursiveSize(prnI));
      resNodeIncrChilds(prnArg, 1);
    }

    resNodeIncrRecursiveSize(prnArg, resNodeGetSize(prnArg));

    if ((pucT = (*pfArg)(prnArg, iArgDetails))) {
      fputs((const char*)pucT, argout);
      xmlFree(pucT);
    }

    prnRelease = resNodeGetChild(prnArg);
    resNodeListUnlinkDescendants(prnArg);
    resNodeListFree(prnRelease);

    fflush(argout);
    fResult = TRUE;
  }
  else
#endif
  if (resNodeIsDir(prnArg)) {
    resNodePtr prnRelease;

#ifdef DEBUG
    fputc('/',stderr);
#endif
    
    if (resNodeDirAppendEntries(prnArg, NULL)) {
#if 1
      for (prnI = resNodeGetChild(prnArg); prnI;) { /* output and unlink file contexts from list first */
	if (resNodeIsFile(prnI) || resNodeIsLink(prnI)) {
	  resNodeListDumpRecursively(argout, prnI, iArgDetails, pfArg);
	  resNodeIncrRecursiveSize(prnArg, resNodeGetRecursiveSize(prnI));
	  resNodeIncrChilds(prnArg,1);
	  prnRelease = prnI;
	  prnI = resNodeGetNext(prnI);
	  resNodeListUnlink(prnRelease);
	  resNodeFree(prnRelease);
	}
	else {
	  prnI = resNodeGetNext(prnI);
	}
      }

      for (prnI = resNodeGetChild(prnArg); prnI; ) { /* remaining list contains directories only */
	resNodeListDumpRecursively(argout, prnI, iArgDetails, pfArg);
	resNodeIncrRecursiveSize(prnArg, resNodeGetRecursiveSize(prnI));
	resNodeIncrChilds(prnArg, 1);
	prnRelease = prnI;
	prnI = resNodeGetNext(prnI);
	resNodeListUnlink(prnRelease);
	resNodeListFree(prnRelease);
      }
#else
      for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	resNodeListDumpRecursively(argout,prnI,iArgDetails,pfArg);
	resNodeIncrRecursiveSize(prnArg, resNodeGetRecursiveSize(prnI));
	resNodeIncrChilds(prnArg,1);
      }
#endif
      resNodeIncrRecursiveSize(prnArg, resNodeGetSize(prnArg));
    }

    if ((pucT = (*pfArg)(prnArg, iArgDetails))) {
      fputs((const char*)pucT, argout);
      xmlFree(pucT);
    }

    prnRelease = resNodeGetChild(prnArg);
    resNodeListUnlinkDescendants(prnArg);
    resNodeListFree(prnRelease);

    fflush(argout);
    fResult = TRUE;
  }
#ifdef HAVE_LIBARCHIVE
  else if (resNodeIsArchive(prnArg)) {
    resNodePtr prnRelease;

#ifdef DEBUG
    fputc('a',stderr);
#endif

    if (arcAppendEntries(prnArg, NULL, FALSE)) {
      for (prnI = resNodeGetChild(prnArg); prnI; prnI = resNodeGetNext(prnI)) {
	resNodeListDumpRecursively(argout,prnI,iArgDetails,pfArg);
      }
    }
    
    prnRelease = resNodeGetChild(prnArg);
    resNodeListUnlinkDescendants(prnArg);
    resNodeListFree(prnRelease);      

    if ((pucT = (*pfArg)(prnArg, iArgDetails))) {
      fputs((const char*)pucT, argout);
      xmlFree(pucT);
    }

    fflush(argout);
    fResult = TRUE;
  }
#endif

#if 0
  else if (resNodeIsURL(prnArg)) {
    /*  */
    fResult = TRUE;
  }
  else if (resNodeIsLink(prnArg)) {
    /*  */
    //  fResult = (resNodeResolveLinkChildNew(prnArg) != NULL);
  }
#endif
  
  else if (resNodeIsFile(prnArg) || resNodeIsFileInArchive(prnArg) || resNodeIsZipDocument(prnArg) || resNodeIsLink(prnArg)) {
    /*  */
      
#ifdef DEBUG
    fputc('.',stderr);
#endif
    
    if ((pucT = (pfArg)(prnArg, iArgDetails)) == NULL) {
    }
    else {
      fputs((const char*)pucT, argout);
      xmlFree(pucT);
    }
    fResult = TRUE;
  }
  else {
  }

  return fResult;
} /* end of resNodeListDumpRecursively() */


/*! Resource Node List To a plain tree view

\param prnArg -- resNode tree to build as plain string
\param pucArgPrefix -- pointer to prefix string, representing depth
\param iArgOptions bits for options

\return pointer to buffer with plain output
*/
xmlChar*
resNodeListToPlainTree(resNodePtr prnArg, xmlChar *pucArgPrefix, int iArgOptions)
{
  xmlChar* pucResult = NULL;

  if (prnArg) {
    resNodePtr prnT;
    xmlChar* pucPrefix;

    if (STR_IS_NOT_EMPTY(pucArgPrefix)) {
      pucPrefix = xmlStrncatNew(pucArgPrefix, BAD_CAST"    ", -1);
    }
    else {
      pucPrefix = xmlStrdup(BAD_CAST"    ");
    }

    for (prnT = prnArg; prnT; prnT = resNodeGetNext(prnT)) {
      xmlChar* pucT = NULL;
      xmlChar* pucTT = NULL;
      resNodePtr prnChild;

      pucTT = xmlStrdup(pucPrefix);
      if ((pucT = xmlStrcat(pucTT, resNodeGetNameBase(prnT)))) {
	if (pucResult) {
	  pucResult = xmlStrcat(pucResult, pucT);
	  xmlFree(pucT);
	}
	else {
	  pucResult = pucT;
	}
	pucResult = xmlStrcat(pucResult, BAD_CAST"\n");
      }

      if ((prnChild = resNodeGetChild(prnT))) {
	if ((pucT = resNodeListToPlainTree(prnChild, pucPrefix, iArgOptions))) {
	  if (pucResult) {
	    pucResult = xmlStrcat(pucResult, pucT);
	    xmlFree(pucT);
	  }
	  else {
	    pucResult = pucT;
	  }
	}
      }
    }
    xmlFree(pucPrefix);
  }
  return pucResult;
} /* end of resNodeListToPlainTree() */



#ifdef TESTCODE
#include "test/test_res_node_formats.c"
#endif
