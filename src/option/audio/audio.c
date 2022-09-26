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

#ifdef HAVE_LIBID3TAG
#include <id3tag.h>
#endif

#ifdef HAVE_LIBVORBIS
#include <vorbiscomment/vcedit.h>
/* include of external source file */
#include <vorbiscomment/vcedit.c>
#endif

#include "basics.h"
#include <cxp/cxp.h>
#include <res_node/res_node_io.h>
#include "dom.h"
#include <cxp/cxp_dir.h>
#include "utils.h"

#ifdef HAVE_LIBID3TAG

#include <audio/audio.h>

static xmlChar *
input_id3_get_string(struct id3_tag *tag, char *frame_name);

#endif

#ifdef HAVE_LIBVORBIS

#endif

#ifdef HAVE_LIBID3TAG

/*! \return a UTF8 string for the named tag field

 derived from xmms-mad
*/
xmlChar *
input_id3_get_string(struct id3_tag *tag, char *frame_name)
{
  const id3_ucs4_t *string;
  struct id3_frame *frame;
  union id3_field *field;

  frame = id3_tag_findframe (tag, frame_name, 0);
  if (!frame) return NULL;

  if (frame_name == ID3_FRAME_COMMENT)
      field = id3_frame_field (frame, 3);
  else
      field = id3_frame_field (frame, 1);

  if (!field) return NULL;

  if (frame_name == ID3_FRAME_COMMENT)
      string = id3_field_getfullstring (field);
  else
      string = id3_field_getstrings (field, 0);

  if (!string) return NULL;

  if (frame_name == ID3_FRAME_GENRE)
     string = id3_genre_name (string);

  if (strlen((const char*)string) > 0) {
    return BAD_CAST id3_ucs4_utf8duplicate(string);
  }
  return NULL;
}


/*! parse the 'pucNameFile' named MP3 file and append results at 'pndFile'

  s. http://www.fireflymediaserver.org/ mt-daapd-0.2.3.tar.gz 
  or libid3tag http://www.underbit.com/products/mad/

  tested with http://kid3.sourceforge.net/
*/
BOOL_T
dirParseFileAudioMP3(xmlNodePtr pndFile, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;
  struct id3_file *id3file = NULL;

  PrintFormatLog(2,"Read MP3 file '%s'", resNodeGetNameNormalized(prnArg));

  id3file = id3_file_open(resNodeGetNameNormalizedNative(prnArg), ID3_FILE_MODE_READONLY);
  if (id3file) {
    xmlNodePtr pndId3;
    struct id3_tag *tag = NULL;

    pndId3 = xmlNewChild(pndFile, NULL, BAD_CAST"info", NULL);

    tag = id3_file_tag(id3file);
    if (tag == NULL) {
      /* no id3tag */
    }
    else {
      xmlChar *pucT;
      xmlChar *pucTT;

      /*!\todo set encoding errors to '_' */

      pucT = input_id3_get_string(tag, ID3_FRAME_TRACK);
      if (pucT) {
	xmlChar *pucSep;
	if ((pucSep = (xmlChar *)xmlStrchr((const xmlChar *)pucT,(xmlChar)'/'))) {
	  *pucSep = (xmlChar)'\0';
	}
	xmlNewChild(pndId3, NULL, BAD_CAST"track",pucT);
	xmlFree(pucT);
      }

      pucT = input_id3_get_string(tag,ID3_FRAME_ARTIST);
      if (pucT != NULL && xmlCheckUTF8(pucT)) {
	pucTT = xmlEncodeSpecialChars(pndFile->doc,pucT);
	if (pucTT != NULL) {
	  xmlNewChild(pndId3,NULL,BAD_CAST"artist",pucTT);
	  xmlFree(pucTT);
	}
	xmlFree(pucT);
      }

      pucT = input_id3_get_string(tag,ID3_FRAME_ALBUM);
      if (pucT != NULL && xmlCheckUTF8(pucT)) {
	pucTT = xmlEncodeSpecialChars(pndFile->doc,pucT);
	if (pucTT != NULL) {
	  xmlNewChild(pndId3,NULL,BAD_CAST"album",pucTT);
	  xmlFree(pucTT);
	}
	xmlFree(pucT);
      }

      pucT = input_id3_get_string(tag, ID3_FRAME_TITLE);
      if (pucT != NULL && xmlCheckUTF8(pucT)) {
	pucTT = xmlEncodeSpecialChars(pndFile->doc,pucT);
	if (pucTT != NULL) {
	  xmlNewChild(pndId3,NULL,BAD_CAST"title",pucTT);
	  xmlFree(pucTT);
	}
	xmlFree(pucT);
      }

      pucT = input_id3_get_string(tag,"TLEN");
      if (pucT != NULL && xmlCheckUTF8(pucT)) {
	pucTT = xmlEncodeSpecialChars(pndFile->doc,pucT);
	if (pucTT != NULL) {
	  xmlNewChild(pndId3,NULL,BAD_CAST"length",pucTT);
	  xmlFree(pucTT);
	}
	xmlFree(pucT);
      }

      pucT = input_id3_get_string(tag,ID3_FRAME_COMMENT);
      if (pucT != NULL && xmlCheckUTF8(pucT)) {
	pucTT = xmlEncodeSpecialChars(pndFile->doc,pucT);
	if (pucTT != NULL) {
	  xmlNewChild(pndId3,NULL,BAD_CAST"comment",pucTT);
	  xmlFree(pucTT);
	}
	xmlFree(pucT);
      }

      /*!\bug release tag?? */
      fResult = TRUE;
    }
    id3_file_close(id3file);
  }
  else {
    xmlSetProp(pndFile, BAD_CAST "error", BAD_CAST "MP3");
    PrintFormatLog(1, "Failed to open file as MP3");
  }

  return fResult;
}
/* end of dirParseFileAudioMP3() */

#endif


#ifdef HAVE_LIBVORBIS

/*! Parse the prnArg of an OGG file and append results at 'pndFile'

  \return TRUE if successful
*/
BOOL_T
dirParseFileAudioVorbis(xmlNodePtr pndFile, resNodePtr prnArg)
{
  BOOL_T fResult = FALSE;

  PrintFormatLog(2,"Read ogg file '%s'", resNodeGetNameNormalized(prnArg));

  if (resNodeOpen(prnArg,"r") == FALSE) {
    PrintFormatLog(1, "Failed to open file");
  }
  else {
    vcedit_state *state;

    /* s. main() and print_comments() in vorbis-tools/vorbiscomment/vcomment.c */
    state = vcedit_new_state();
    if (state==NULL) {
      PrintFormatLog(1, "Failed to create state");
    }
    else {
      if (vcedit_open(state, (FILE *)resNodeGetHandleIO(prnArg)) < 0) {
	xmlSetProp(pndFile, BAD_CAST "error", BAD_CAST vcedit_error(state));
	PrintFormatLog(1, "Failed to open file as vorbis: %s", vcedit_error(state));
      }
      else {
	vorbis_comment *vc;
	int i;
	xmlNodePtr pndVorbis;

	pndVorbis = xmlNewChild(pndFile, NULL, BAD_CAST"info", NULL);

	/* extract and display the comments */
	vc = vcedit_comments(state);
	for (i = 0; i < vc->comments; i++) {

	  xmlChar *pucComment = NULL;

	  if (xmlCheckUTF8(BAD_CAST vc->user_comments[i])) {
	    /* is UTF-8 encoding */
	    pucComment = xmlEncodeSpecialChars(pndFile->doc,BAD_CAST vc->user_comments[i]);
	  }
	  else {
	    xmlChar *pucT;

	    pucComment = xmlEncodeSpecialChars(pndFile->doc,BAD_CAST vc->user_comments[i]);
	    for (pucT=pucComment; *pucT; pucT++) {
	      if (isascii(*pucT)) {
		/* OK */
	      }
	      else {
		*pucT = '_';
	      }
	    }
	    *pucT = (xmlChar)'\0';
	    PrintFormatLog(3,"Use dummy value '%s'",pucComment);
	  }

	  if (xmlStrstr(pucComment,BAD_CAST "TRACKNUMBER=")==pucComment) {
	    xmlChar *pucSep;
	    if ((pucSep = (xmlChar *)xmlStrchr(BAD_CAST (pucComment + 12),(xmlChar)'/'))) {
	      *pucSep = (xmlChar)'\0';
	    }
	    xmlNewChild(pndVorbis, NULL, BAD_CAST"track",BAD_CAST (pucComment + 12));
	  }
	  else if (xmlStrstr(pucComment,BAD_CAST "ARTIST=")==pucComment) {
	    xmlNewChild(pndVorbis, NULL, BAD_CAST"artist",BAD_CAST (pucComment + 7));
	  }
	  else if (xmlStrstr(pucComment,BAD_CAST "ALBUM=")==pucComment) {
	    xmlNewChild(pndVorbis, NULL, BAD_CAST"album",BAD_CAST (pucComment + 6));
	  }
	  else if (xmlStrstr(pucComment,BAD_CAST "TITLE=")==pucComment) {
	    xmlNewChild(pndVorbis, NULL, BAD_CAST"title",BAD_CAST (pucComment + 6));
	  }
	  xmlFree(pucComment);
	}
	/*!\todo add length s. vorbis_end() in vorbis-tools-1.0.1/ogginfo/ogginfo2.c */
	/*!\bug release vc?? */
	fResult = TRUE;
      }
      vcedit_clear(state);
    }
    resNodeClose(prnArg);
  }

  return fResult;
}
/* end of dirParseFileAudioVorbis() */

#endif

#ifdef TESTCODE
#include "test/test_audio.c"
#endif

