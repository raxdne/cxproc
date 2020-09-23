/*
  cxproc - Configurable Xml PROCessor

  Copyright (C) 1999..2020 by Alexander Tenbusch

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
#include <libxml/tree.h>

#include "basics.h"
#include "utils.h"
#include <res_node/res_node.h>
#include <cxp/cxp.h>
#include <petrinet/petrinet.h>


#define DEBUG_SEARCH_

/****************************************************************************

  private name macros
*/

#define NAME_STATE      BAD_CAST  "stelle"

#define NAME_TRANSITION BAD_CAST  "transition"

#define NAME_EDGE       BAD_CAST  "relation"

#define KNOTS_MAX 128

#define EDGES_MAX (128 * 2)

#define LOOPS_MAX (128 * 128) 		/*!< number of maximal loops while path searching */

/****************************************************************************

  type definition: 
*/
typedef enum {
  state,
  transition
} knot_type_t;


/* ***************************************************************************

  type definition: knot_t
 */

/*! knot structure
 */
typedef struct {
  knot_type_t type;             /*!< type of element */
  xmlChar *pucId;		/*!< id of knot */
  BOOL_T fMarker;		/*!< output marker flag */
  index_t i;                    /*!< index in array, redundant but performant */
  index_t d;			/*!< minimum edge distance between p->start and this knot */
  xmlNodePtr pndSource;		/*!< pointer to according XML element node */
} knot_t;


/* ***************************************************************************

   preprocessor macros for access to knot_t
 */

#define statep(N) (N != NULL && (N)->type == state)

#define transitionp(N) (N != NULL && (N)->type == transition)

/* warning: not nice -> better pnetIsEdge() */

#define edge_st(P,s,t) (P->e[s][t] & 1)

#define edge_ts(P,t,s) (P->e[s][t] & 2)

#define KNOTID(N) (N != NULL ? (N)->pucId : NULL)

#define state_index(N) (statep(N) ? (N)->i : ERROR_INDEX)

#define transition_index(N) (transitionp(N) ? (N)->i : ERROR_INDEX)

/****************************************************************************

  type definition: edge_t
*/
typedef struct {
  knot_t *from;			/*!< ptr to origin knot of edge */
  knot_t *to;			/*!< ptr to target knot of edge */
  BOOL_T fMarker;		/*!< output marker flag */
  xmlNodePtr pndSource;		/*!< pointer to according XML element node */
} edge_t;


/****************************************************************************

  type definition: petrinet_t
*/

typedef struct {
  xmlNode *pndRoot;		/*!< ptr to the root element of DOM */
  xmlChar *pucId;		/*!< id of petrinet */
  index_t n_s;			/*!< number of states */
  knot_t *s;			/*!< pointer to array of states */
  index_t n_t;			/*!< number of transitions */
  knot_t *t;			/*!< pointer to array of transitions */
  index_t n_e;			/*!< number of edges */
  edge_t *edge;			/*!< array of edges */
  index_t **e;			/*!< 2D matrix of edges */
  knot_t **path;		/*!< pointer for path buffer */
  index_t n_paths;		/*!< number of found path between 'start' and 'target' */
  index_t l_path;		/*!< last knot index in path */
  index_t l_min;		/*!< minimum length of valid path */
  index_t l_max;		/*!< maximum length of valid path */
  index_t n_max;		/*!< maximum number of result paths */
  knot_t *start;		/*!< pointer to start states */
  knot_t *target;		/*!< pointer to target states */
} petrinet_t;


/****************************************************************************

   Declaration of private functions (prototypes)
*/

static petrinet_t *
pnetNew(xmlDocPtr pdocXml, cxpContextPtr pccArg);

static BOOL_T
pnetSetStart(petrinet_t *ppnArg, xmlChar *pucId, cxpContextPtr pccArg);

static BOOL_T
pnetSetTarget(petrinet_t *ppnArg, xmlChar *pucId, cxpContextPtr pccArg);

static BOOL_T 
pnetSearchPathForward(petrinet_t *ppnArg);

static BOOL_T 
pnetSearchBranchBackward(petrinet_t *ppnArg);

static index_t 
pnetGetStateIndexForId(petrinet_t *ppnArg, xmlChar *pucId);

static index_t 
pnetGetTransitionIndexForId(petrinet_t *ppnArg, xmlChar *pucId);

static edge_t *
pnetGetEdgePtrFromPtr(petrinet_t *ppnArg, knot_t *ptr_from, knot_t *ptr_to);

static void
pnetFree(petrinet_t *ppnArg);

static void
pnetSetPathMarker(petrinet_t *ppnArg, BOOL_T f);

static void
pnetSetMarker(petrinet_t *ppnArg, BOOL_T f);

static BOOL_T
pnetSetPathLength(petrinet_t *ppnArg, char *str_min, char *str_max, cxpContextPtr pccArg);

static xmlNodePtr
pnetParsePathXml(petrinet_t *ppnArg, xmlNodePtr pndParent, xmlNsPtr pnsXsl, cxpContextPtr pccArg);

static xmlDocPtr
pnetProcessPathnet(petrinet_t *ppnArg, BOOL_T flagXml, cxpContextPtr pccArg);

static xmlDocPtr
pnetProcessPathtable(petrinet_t *ppnArg, cxpContextPtr pccArg);

static xmlDocPtr
pnetProcessPathtableSource(petrinet_t *ppnArg, cxpContextPtr pccArg);

static xmlNodePtr
pnetParsePathXmlSource(petrinet_t *ppnArg, xmlNodePtr pndParent, xmlNsPtr pnsXsl, cxpContextPtr pccArg);

static BOOL_T
pnetNewEdges(petrinet_t *ppnArg, cxpContextPtr pccArg);

static edge_t *
pnetInitEdges(petrinet_t *ppnArg, edge_t *pEdgeArg, xmlNode * pndArg, cxpContextPtr pccArg);

static BOOL_T 
pnetIsMemberTransition(petrinet_t *ppnArg, knot_t *t) ;

static BOOL_T 
pnetIsMemberState(petrinet_t *ppnArg, knot_t *s) ;

static BOOL_T
pnetNewKnots(petrinet_t *ppnArg, cxpContextPtr pccArg);

static BOOL_T
pnetInitKnots(petrinet_t *ppnArg, xmlNode *pndArg, cxpContextPtr pccArg);

static void
pnetPrintKnots(petrinet_t *ppnArg, cxpContextPtr pccArg);

static void
pnetPrintEdges(petrinet_t *ppnArg, cxpContextPtr pccArg);

static void
pnetPrint(petrinet_t *ppnArg, cxpContextPtr pccArg);

static BOOL_T
pnetNewPath(petrinet_t *ppnArg, cxpContextPtr pccArg);

static void
pnetPrintPath(petrinet_t *ppnArg, cxpContextPtr pccArg);

/****************************************************************************

   Definition of functions
*/


/*! process the required  files
 */
xmlDocPtr
pnetProcessNode(xmlNodePtr pndArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;

  if (pndArg) {
    xmlNodePtr pndXml;

    cxpCtxtLogPrint(pccArg, 3,"Processing PATH");

    if (pndArg != NULL && (pndXml = domGetFirstChild(pndArg, NAME_XML))) {
      /*  */
      xmlDocPtr pdocT;

      pdocT = cxpProcessXmlNode(pndXml, pccArg);
      if (pdocT) {
	xmlDocPtr pdocPnet;
	petrinet_t *ppnNew;

	/*! apply XSL */
	if (cxpProcessTransformations(pdocT, pndArg, &pdocPnet, NULL, pccArg) && pdocPnet != NULL) {
	  xmlFreeDoc(pdocT);

	  ppnNew = pnetNew(pdocPnet, pccArg);
	  if (ppnNew) {
	    xmlChar *pucAttrStart;
	    xmlChar *pucAttrNMax;

	    pnetSetPathLength(ppnNew,
	      (char *)domGetAttributePtr(pndArg, BAD_CAST "lmin"),
	      (char *)domGetAttributePtr(pndArg, BAD_CAST "lmax"), pccArg);

	    pucAttrStart = domGetAttributePtr(pndArg, BAD_CAST "start");
	    if (pucAttrStart != NULL && xmlStrlen(pucAttrStart) > 0 && pnetSetStart(ppnNew, pucAttrStart, pccArg)) {

	      /*!\todo insert meta element */

	      if ((pucAttrNMax = domGetAttributePtr(pndArg, BAD_CAST "nmax"))) {
		ppnNew->n_max = atoi((const char *)pucAttrNMax);
	      }
	      else {
		ppnNew->n_max = 30;
	      }

	      if (IS_NODE(pndArg, NAME_PATHTABLE)) {
		xmlChar *pucAttrTarget;

		/* target knot required */
		pucAttrTarget = domGetAttributePtr(pndArg, BAD_CAST "target");
		if (pucAttrTarget != NULL && xmlStrlen(pucAttrTarget) > 0 && pnetSetTarget(ppnNew, pucAttrTarget, pccArg)) {
		  if (xmlStrEqual(domGetAttributePtr(pndArg, BAD_CAST"type"), BAD_CAST"xml")) {
		    pdocResult = pnetProcessPathtableSource(ppnNew, pccArg);
		  }
		  else {
		    pdocResult = pnetProcessPathtable(ppnNew, pccArg);
		  }
		}
		else {
		  cxpCtxtLogPrint(pccArg, 1, "Cant set target STELLE");
		}
	      }
	      else if (IS_NODE(pndArg, NAME_PATHNET)) {
		if (xmlStrEqual(domGetAttributePtr(pndArg, BAD_CAST"type"), BAD_CAST"xml")) {
		  pdocResult = pnetProcessPathnet(ppnNew, TRUE, pccArg);
		}
		else {
		  pdocResult = pnetProcessPathnet(ppnNew, FALSE, pccArg);
		}
	      }
	      else {
		cxpCtxtLogPrint(pccArg, 1, "Unknown element '%s'", (pndArg ? pndArg->name : BAD_CAST""));
	      }
	    }
	    else {
	      cxpCtxtLogPrint(pccArg, 1, "Cant set start STELLE");
	    }
	    pnetFree(ppnNew);
	  }
	  xmlFreeDoc(pdocPnet);
	}
      }
    }
    else {
      cxpCtxtLogPrint(pccArg, 1,"No valid attributes in '%s'",pndArg->name);
    }
  }
  return pdocResult;
}
/* end of pnetProcessNode() */


/*!

\param xml DOM of PETRINET

\return the pointer of the created petrinet
*/
petrinet_t *
pnetNew(xmlDocPtr pdocXml, cxpContextPtr pccArg)
{
  petrinet_t *ppnResult = NULL;

  ppnResult = (petrinet_t *) xmlMemMalloc(sizeof(petrinet_t));
  if (ppnResult) {
    memset(ppnResult, 0, sizeof(petrinet_t));

    ppnResult->pndRoot = xmlDocGetRootElement(pdocXml); /* Get the root element node */
    
    /* set id of petrinet */
    ppnResult->pucId = domGetAttributePtr(ppnResult->pndRoot, BAD_CAST  "id");
    if (KNOTID(ppnResult)==NULL) {
      /* no valid id set */
      ppnResult->pucId = xmlStrdup(BAD_CAST  "anonymous");
    }

    /* 
     */
    if (pnetNewKnots(ppnResult,pccArg) && pnetNewEdges(ppnResult,pccArg) && pnetNewPath(ppnResult,pccArg)) {
      /* success */
    }
    else {
      pnetFree(ppnResult);
      ppnResult = NULL;
    }
  }
  else {
    cxpCtxtLogPrint(pccArg, 1, "No more memory");
  }
  return ppnResult;
}
/* End of pnetNew() */


/*!

\param ppnArg pointer to the petrinet

\return success flag
*/
BOOL_T
pnetNewPath(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && ppnArg->n_s > 0 && ppnArg->n_t > 0) {
    int i;
    int ciSize;

    ciSize = (ppnArg->n_s + ppnArg->n_t + 1) * sizeof(knot_t); /* fixed array size */

    xmlFree(ppnArg->path);
    ppnArg->path = (knot_t **) xmlMemMalloc(ciSize);
    if (ppnArg->path) {
      memset(ppnArg->path, 0, ciSize);
      ppnArg->l_path = ERROR_INDEX;
      ppnArg->l_min = ERROR_INDEX;
      ppnArg->l_max = ERROR_INDEX;
    }
    else {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
    }
    fResult = TRUE;
  }
  return fResult;
}
/* End of pnetNewPath() */


/*! print current path

\param ppnArg pointer to the petrinet

\return 
*/
void
pnetPrintPath(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  if (ppnArg != NULL && ppnArg->path != NULL) {
    int i;

    for (i=0; i <= ppnArg->l_path; i++) {
      cxpCtxtLogPrint(pccArg, 1," (%s '%s')", statep(ppnArg->path[i]) ? NAME_STATE : NAME_TRANSITION, ppnArg->path[i]->pucId);
    }
  }
}
/* End of pnetPrintPath() */


/*!

\param ppnArg pointer to the petrinet

\return success flag
*/
BOOL_T
pnetNewKnots(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && ppnArg->pndRoot != NULL) {
    int i;
    int ciSize;

    ciSize = KNOTS_MAX * sizeof(knot_t); /* fixed array size */
    
    ppnArg->n_s = 0;
    ppnArg->s = (knot_t *) xmlMemMalloc(ciSize);
    if (ppnArg->s == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
      return fResult;
    }    
    memset(ppnArg->s, 0, ciSize);

    ppnArg->n_t = 0;
    ppnArg->t = (knot_t *) xmlMemMalloc(ciSize);
    if (ppnArg->t == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
      return fResult;
    }    
    memset(ppnArg->t, 0, ciSize);

    pnetInitKnots(ppnArg, ppnArg->pndRoot, pccArg);

    fResult = ppnArg->n_s > 0 && ppnArg->s[0].pucId != NULL && ppnArg->n_t > 0 && ppnArg->t[0].pucId != NULL;
  }
  return fResult;
}
/* End of pnetNewKnots() */


/*! initialize all found knots

\param ppnArg pointer to the petrinet
\param current_knot
\param pndArg current node for recursive call

\return 
*/
BOOL_T
pnetInitKnots(petrinet_t *ppnArg, xmlNode *pndArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && pndArg != NULL) {
    xmlNode *pndT;

    for (pndT = pndArg; pndT; pndT = pndT->next) {
      if (pndT->type != XML_ELEMENT_NODE) {
	/* skip */
      }
      else if (IS_NODE(pndT, NAME_STATE)) {
	int i = ppnArg->n_s;

	assert(i > -1);
	ppnArg->s[i].i = i;
	ppnArg->s[i].type = state;
	ppnArg->s[i].pucId = domGetAttributePtr(pndT, BAD_CAST  "id");
	ppnArg->s[i].pndSource = pndT;
	cxpCtxtLogPrint(pccArg, 4,"'%s' '%s'", NAME_STATE, ppnArg->s[i].pucId);
	ppnArg->n_s++;
      }
      else if (IS_NODE(pndT, NAME_TRANSITION)) {
	int i = ppnArg->n_t;
	
	assert(i > -1);
	ppnArg->t[i].i = i;
	ppnArg->t[i].type = transition;
	ppnArg->t[i].pucId = domGetAttributePtr(pndT, BAD_CAST  "id");
	ppnArg->t[i].pndSource = pndT;
	cxpCtxtLogPrint(pccArg, 4,"'%s' '%s'", NAME_TRANSITION, ppnArg->t[i].pucId);
	ppnArg->n_t++;
      }
      else {
	pnetInitKnots(ppnArg, pndT->children, pccArg);
      }
    }
    fResult = TRUE;
  }
  return fResult;
}
/* End of pnetInitKnots() */


/*! print all found knots

\param ppnArg pointer to the petrinet

\return 
*/
void
pnetPrintKnots(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  if (ppnArg) {
    int i;

    for (i=0; i < ppnArg->n_s; i++) {
      cxpCtxtLogPrint(pccArg, 1,"s%i '%s' '%s'", i, NAME_STATE, ppnArg->s[i].pucId);
    }

    for (i=0; i < ppnArg->n_t; i++) {
      cxpCtxtLogPrint(pccArg, 1,"t%i '%s' '%s'", i, NAME_TRANSITION, ppnArg->t[i].pucId);
    }
  }
}
/* End of pnetPrintKnots() */


/*!

\param ppnArg pointer to the petrinet

\return success flag
*/
BOOL_T
pnetNewEdges(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && ppnArg->pndRoot != NULL && ppnArg->n_s > 0 && ppnArg->n_t > 0) {
    index_t i;
    index_t j;
    /* index_t **ptr;   */	/* pointer to new allocated memory */
    index_t *ptr_b;
    edge_t *tail;
    int ciSize;
    int ciMatrixSize;

    ciSize = EDGES_MAX * sizeof(edge_t); /* fixed array size */
    ciMatrixSize = ppnArg->n_s * ppnArg->n_t * sizeof(index_t); /* array size */

    if ((ptr_b = (index_t *) xmlMemMalloc(ciMatrixSize)) == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
    }
    else if ((ppnArg->e = (index_t **) xmlMemMalloc(EDGES_MAX * sizeof(index_t*))) == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
    }
    else if ((ppnArg->edge = (edge_t *) xmlMemMalloc(ciSize)) == NULL) {
      cxpCtxtLogPrint(pccArg, 1, "No more memory");
    }
    else {
      /* array of line pointers */
#if 1
      memset(ptr_b, 0, ciMatrixSize);
      for (j=0; j<ppnArg->n_s; j++) {
	ppnArg->e[j] = ptr_b + j * ppnArg->n_t;
      }
#else 
      for (j=0; j<ppnArg->n_s; j++) {
	ppnArg->e[j] = ptr_b + j * ppnArg->n_t;
	/* set all possible edges to 'none' */
	for (i=0; i < ppnArg->n_t; i++) {
	  ppnArg->e[j][i] = 0;
	  /*!\todo optimize with memset(ptr_b) ?? */
	}
      }
#endif
      
      /* edges */
      memset(ppnArg->edge, 0, ciSize);
      tail = pnetInitEdges(ppnArg, ppnArg->edge, ppnArg->pndRoot,pccArg);
      tail->from = NULL;
      ppnArg->n_e = (tail - ppnArg->edge);
      cxpCtxtLogPrint(pccArg, 2,"Valid '%s' nodes: %i", NAME_EDGE, ppnArg->n_e);
      
      fResult = TRUE;
    }
  }
  return fResult;
}
/* End of pnetNewEdges() */


/*! initialize all found edges

\param ppnArg pointer to the petrinet
\param pEdgeArg
\param pndArg current node for recursive call

\return 
*/
edge_t *
pnetInitEdges(petrinet_t *ppnArg, edge_t *pEdgeArg, xmlNode * pndArg, cxpContextPtr pccArg)
{
  if (ppnArg != NULL) {
    xmlNode *pndT;

    for (pndT = pndArg; pndT; pndT = pndT->next) {

      if (IS_NODE(pndT,NAME_EDGE)) {
	/* pndT is a relation node */
	index_t a;
	index_t b;
	xmlChar *pucAttrFrom;
	xmlChar *pucAttrTo;

	pEdgeArg->pndSource = pndT;
	pEdgeArg->fMarker = TRUE;
	pucAttrFrom = domGetAttributePtr(pndT, BAD_CAST  "from");
	pucAttrTo = domGetAttributePtr(pndT, BAD_CAST  "to");

	if (pucAttrFrom && pucAttrTo) {
	  a = pnetGetStateIndexForId(ppnArg, pucAttrFrom);
	  if (a == ERROR_INDEX) {
	    /* it was a wrong assumtion,
	     OR there is no attribute 'from'
	     OR there is no valid stelle with id 'pucAttrFrom'
	     */
	    a = pnetGetTransitionIndexForId(ppnArg, pucAttrFrom);
	    if (a == ERROR_INDEX) {
	      /* ignore invalid relations */
	      cxpCtxtLogPrint(pccArg, 1,"Edge invalid '%s' -> '%s'", pucAttrFrom, pucAttrTo);
	    }
	    else {
	      b = pnetGetStateIndexForId(ppnArg, pucAttrTo);
	      if (b != ERROR_INDEX) {
		/* b is a valid stelle, a is a valid transition */
		ppnArg->e[b][a] |= 2;

		pEdgeArg->from  = &(ppnArg->t[a]);
		pEdgeArg->to    = &(ppnArg->s[b]);
		pEdgeArg++;
	      }
	    }
	  }
	  else {
	    /* right assumtion */
	    b = pnetGetTransitionIndexForId(ppnArg, pucAttrTo);

	    if (b != ERROR_INDEX) {
	      /* a is a valid stelle, b is a valid transition */
	      ppnArg->e[a][b] |= 1;

	      pEdgeArg->from  = &(ppnArg->s[a]);
	      pEdgeArg->to    = &(ppnArg->t[b]);
	      pEdgeArg++;
	    }
	    else {
	      /* ignore invalid relations */
	      cxpCtxtLogPrint(pccArg, 1,"Edge invalid '%s' -> '%s'", pucAttrFrom, pucAttrTo);
	    }
	  }
	}
      }
      else {
	pEdgeArg = pnetInitEdges(ppnArg,pEdgeArg,pndT->children,pccArg);
      }
    }
  }
  return pEdgeArg;
} /* End of pnetInitEdges() */


/*! print all found edges

\param ppnArg pointer to the petrinet

\return 
*/
void
pnetPrintEdges(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  if (ppnArg) {
    int i;

    for (i=0; i < ppnArg->n_e; i++) {
      cxpCtxtLogPrint(pccArg, 1,"e%i '%s' '%s' -> '%s'",
		      i, NAME_EDGE, KNOTID(ppnArg->edge[i].from), KNOTID(ppnArg->edge[i].to));
    }
  }
}
/* End of pnetPrintEdges() */


/*! print petrinet

\param ppnArg pointer to the petrinet

\return 
*/
void
pnetPrint(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  if (ppnArg) {
    pnetPrintKnots(ppnArg, pccArg);
    pnetPrintEdges(ppnArg, pccArg);
    //pnetPrintPath(ppnArg, pccArg);
    //pnetPrintMatrix(ppnArg, pccArg);
  }
}
/* End of pnetPrint() */


/*! release the allocated mem of a petrinet_t
*/
void
pnetFree(petrinet_t *ppnArg)
{
  if (ppnArg != NULL) {
    xmlMemFree(ppnArg->s);

    xmlMemFree(ppnArg->t);

    xmlMemFree(ppnArg->edge);

    if (ppnArg->e) {
      xmlMemFree(*ppnArg->e);		/* release the block */
      xmlMemFree(ppnArg->e);		/* release the pointer array */
    }

    xmlMemFree(ppnArg->path);

    xmlFree(ppnArg->pucId);

    memset(ppnArg, 0, sizeof(petrinet_t));

    xmlMemFree(ppnArg);
  }
}
/* End of pnetFree() */


#if 0
/*! searches the index number of 
 */
index_t
pnetGetEdgeIndexFromId(petrinet_t *ppnArg, xmlChar *id_from, xmlChar *id_to)
{
  index_t i;

  if (ppnArg && id_from && id_to) { 
    for (i=0; i < ppnArg->n_e; i++) {
      if ((!xmlStrcasecmp(KNOTID(ppnArg->edge[i].from), id_from) && !xmlStrcasecmp(KNOTID(ppnArg->edge[i].to), id_to))) {
	return i;
      }
    }
  } 

  return ERROR_INDEX;
}
/* End of pnetGetEdgeIndexFromId() */
#endif


/*! searches the index number of 
 */
edge_t *
pnetGetEdgePtrFromPtr(petrinet_t *ppnArg, knot_t *ptr_from, knot_t *ptr_to)
{
  index_t i;

  if (ppnArg && ptr_from && ptr_to) { 
    for (i=0; i < ppnArg->n_e; i++) {
      if (ppnArg->edge[i].from == ptr_from
	  && ppnArg->edge[i].to   == ptr_to) {
	return &(ppnArg->edge[i]);
      }
    }
  }

  return NULL;
}
/* End of pnetGetEdgeIndexFromId() */


index_t
pnetGetStateIndexForId(petrinet_t *ppnArg, xmlChar *pucId)
{
  /* pnetGetStateIndexForId() - searches the index number of 'pucId' in 'ppnArg->s[]'
  */

  index_t i;

  if (ppnArg && pucId) { 
    for (i=0; i < ppnArg->n_s; i++) {
      if (xmlStrEqual(ppnArg->s[i].pucId, pucId)) {
	return i;
      }
    }
  } 

  return ERROR_INDEX;
}
/* End of pnetGetStateIndexForId() */


index_t
pnetGetTransitionIndexForId(petrinet_t *ppnArg, xmlChar *pucId)
{
  /* pnetGetTransitionIndexForId() - searches the index number of 'pucId' in 'ppnArg->t[]'
  */

  index_t i;

  if (ppnArg && pucId) {
    for (i=0; i < ppnArg->n_t; i++) {
      if (xmlStrEqual(ppnArg->t[i].pucId, pucId)) {
	return i;
      }
    }
  }

  return ERROR_INDEX;
}
/* End of pnetGetTransitionIndexForId() */

#if 0
knot_t *
pnetGetStatePtrForId(petrinet_t *ppnArg, xmlChar *pucId) 
{
  if (ppnArg && pucId) {
    index_t i = pnetGetStateIndexForId(ppnArg,pucId);
    if (i > -1 && i < ppnArg->n_s) {
      return &(ppnArg->s[i]);
    }
  }

  return NULL;
}


knot_t *
pnetGetTransitionPtrForId(petrinet_t *ppnArg, xmlChar *pucId) 
{
  if (ppnArg && pucId) {
    index_t i = pnetGetTransitionIndexForId(ppnArg,pucId);
    if (i > -1 && i < ppnArg->n_t) {
      return &(ppnArg->t[i]);
    }
  }

  return NULL;
}


edge_t *
pnetGetEdgePtrFromId(petrinet_t *ppnArg, xmlChar *id_from, xmlChar *id_to)
{
  if (ppnArg && id_from && id_to) {
    index_t i = pnetGetEdgeIndexFromId(ppnArg,id_from,id_to);
    if (i > -1 && i < ppnArg->n_e) {
      return &(ppnArg->edge[i]);
    }
  }

  return NULL;
}
#endif


BOOL_T
pnetIsEdge(petrinet_t *ppnArg, knot_t *a, knot_t *b)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && a != NULL && b != NULL) {
    if (statep(a)) {
      if (transitionp(b)) {
	fResult = (ppnArg->e[a->i][b->i] & 1) != 0;
      }
    }
    else if (transitionp(a)) {
      if (statep(b)) {
	fResult = (ppnArg->e[b->i][a->i] & 2) != 0;
      }
    }
  }
  return fResult;
}
/* End of pnetIsEdge() */


BOOL_T
pnetIsMemberTransition(petrinet_t *ppnArg, knot_t *t) 
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && t != NULL) {
    index_t i;

    if (transitionp(t)) {
      for (i=0; i < ppnArg->l_path; i++) {
	if ( transitionp(ppnArg->path[i]) && ppnArg->path[i] == t ) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
}


BOOL_T
pnetIsMemberState(petrinet_t *ppnArg, knot_t *s) 
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL && s != NULL) {
    index_t i;

    if (statep(s)) {
      for (i=0; i < ppnArg->l_path; i++) {
	if ( statep(ppnArg->path[i]) && ppnArg->path[i] == s ) {
	  fResult = TRUE;
	  break;
	}
      }
    }
  }
  return fResult;
}


/*! sets the 'ppnArg->start' or 'ppnArg->target'

a transition as start makes no sense !
*/
BOOL_T
pnetSetStart(petrinet_t *ppnArg, xmlChar *pucId, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL) {
    index_t s;

    if (STR_IS_NOT_EMPTY(pucId)) {
      s = pnetGetStateIndexForId(ppnArg,pucId);
      if (s != ERROR_INDEX) {
	cxpCtxtLogPrint(pccArg, 2,"Set start to '%s' -> %i", pucId, s);
	ppnArg->start = &(ppnArg->s[s]);
	ppnArg->path[0] = ppnArg->start;
	fResult = TRUE;
      }
      else {
	cxpCtxtLogPrint(pccArg, 1,"State '%s' not valid", pucId);
      }
    }
    else {
      /* value of pucId not defined */
      cxpCtxtLogPrint(pccArg, 1,"Set start to 'open'");
      ppnArg->start = NULL;
      fResult = TRUE;
    }

    ppnArg->l_path  = 0;
    ppnArg->n_paths = 0;
  }
  return fResult;
}
/* End of pnetSetStart() */


/*! sets the 'ppnArg->start' or 'ppnArg->target'

a transition as start makes no sense !
*/
BOOL_T
pnetSetTarget(petrinet_t *ppnArg, xmlChar *pucId, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL) {
    index_t s;

    if (STR_IS_NOT_EMPTY(pucId)) {
      s = pnetGetStateIndexForId(ppnArg,pucId);
      if (s != ERROR_INDEX) {
	cxpCtxtLogPrint(pccArg, 2,"Set end to '%s' -> %i", pucId, s);
	ppnArg->target = &(ppnArg->s[s]);
	ppnArg->path[0] = ppnArg->start;
	fResult = TRUE;
      }
      else {
	cxpCtxtLogPrint(pccArg, 1,"State '%s' not valid", pucId);
      }
    }
    else {
      /* value of pucId not defined */
      cxpCtxtLogPrint(pccArg, 1,"Set end to 'open'");
      ppnArg->target = NULL;
      fResult = TRUE;
    }

    ppnArg->l_path  = 0;
    ppnArg->n_paths = 0;
  }
  return fResult;
}
/* End of pnetSetTarget() */


/*! sets the 

*/
BOOL_T
pnetSetPathLength(petrinet_t *ppnArg, char *pchMin, char *pchMax, cxpContextPtr pccArg)
{
  BOOL_T fResult = FALSE;

  if (ppnArg != NULL) {
    char *e;

    if (pchMin) {
      ppnArg->l_min = (index_t) strtol(pchMin, &e, 10);
      if (!e || ppnArg->l_min < 1) {
	/* conversion error -> back to init value */
	ppnArg->l_min = ERROR_INDEX;
      }
      fResult = TRUE;
      cxpCtxtLogPrint(pccArg, 1,"l_min = '%i'", ppnArg->l_min);
    }

    if (pchMax) {
      ppnArg->l_max = (index_t) strtol(pchMax, &e, 10);
      if (!e || ppnArg->l_max < 1) {
	/* conversion error -> back to init value */
	ppnArg->l_max = ERROR_INDEX;
      }
      fResult = TRUE;
      cxpCtxtLogPrint(pccArg, 1,"l_max = '%i'", ppnArg->l_max);
    }
  }
  return fResult;
}
/* End of pnetSetPathLength() */


/*! search for an new path 

ppnArg->start and ppnArg->target must be states!! see pnetSetStartTarget()
*/
BOOL_T
pnetSearchPathForward(petrinet_t *ppnArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG_SEARCH
  fprintf(stderr,"pnetSearchPathForward()\n");
#endif
    
  if (ppnArg != NULL) {
    index_t i;
    knot_t *l_knot;

    /*! \bug bidrectional relation is also a path */

    /*   assert(statep(ppnArg->start) || transitionp(ppnArg->start)); */
    /*   assert(statep(ppnArg->target) || transitionp(ppnArg->target)); */
    assert(statep(ppnArg->start));
    assert(statep(ppnArg->target));
    assert(ppnArg->path[0] == ppnArg->start);

    l_knot = ppnArg->path[ppnArg->l_path]; /* ptr to last knot */

#ifdef DEBUG_SEARCH
    fprintf(stderr,"%i %s '%s': last\n", ppnArg->l_path, statep(l_knot) ? NAME_STATE : NAME_TRANSITION, l_knot->pucId);
#endif
    
    if (ppnArg->l_max != ERROR_INDEX && ppnArg->l_path >= ppnArg->l_max) {
#ifdef DEBUG_SEARCH
      fprintf(stderr,"path is too long\n");
#endif
    }
    else if (l_knot == ppnArg->target && ppnArg->l_path > 0) {
      /* target is reached, path is yet complete,
       probably pnetSearchBranchBackward() found a short way */

      if (ppnArg->l_min != ERROR_INDEX && ppnArg->l_path < ppnArg->l_min) {
#ifdef DEBUG_SEARCH
	fprintf(stderr,"path is too short\n");
#endif
      }
      else {
#ifdef DEBUG_SEARCH
	fprintf(stderr,"path is OK\n");
#endif
	ppnArg->n_paths++;   /* count this path and */
	fResult = TRUE;    /* leave the recursion */
      }
    }
    else if (statep(l_knot)) {
      /* last element is a state -> search for a transition knot */

      index_t l_state = state_index(l_knot);

      for (i=0; i < ppnArg->n_t; i++) { /* all transitions */
	if (edge_st(ppnArg,l_state,i)
	    && (! pnetIsMemberTransition(ppnArg, &(ppnArg->t[i])))) {
	  /* found a valid transition,
	   with an edge to l_state and
	   not yet included in path */
	  ppnArg->l_path++;
	  ppnArg->path[ppnArg->l_path] = &(ppnArg->t[i]); /* enclose the pointer into path */
#ifdef DEBUG_SEARCH
	  fprintf(stderr,"%i %s '%s': added + recursion\n", ppnArg->l_path, statep(ppnArg->path[ppnArg->l_path]) ? NAME_STATE : NAME_TRANSITION, ppnArg->path[ppnArg->l_path]->pucId);
#endif
	  fResult = pnetSearchPathForward(ppnArg);	/* search recursive next knot for path[] */
	  break;
	}
      }
    }
    else if (transitionp(l_knot)) {
      /* last element is a transition -> search for a state knot */

      index_t l_trans = transition_index(l_knot);

      for (i=0; i < ppnArg->n_s; i++) { /* all states */
	knot_t *s_ptr = &(ppnArg->s[i]);
	if (edge_ts(ppnArg,l_trans,i)) {
	  /* found a valid state with an edge to l_trans */
	  if (pnetIsMemberState(ppnArg, s_ptr)) {
	    if (s_ptr == ppnArg->start && s_ptr == ppnArg->target) {
	      /* special case: its a circle! */
	      ppnArg->l_path++;
	      ppnArg->path[ppnArg->l_path] = s_ptr;
	      ppnArg->n_paths++;   /* count this path and */
	      fResult = TRUE;    /* leave the recursion */
#ifdef DEBUG_SEARCH
	      fprintf(stderr,"%i %s '%s': its a circle\n", ppnArg->l_path, statep(s_ptr) ? NAME_STATE : NAME_TRANSITION, s_ptr->pucId);
#endif
	      break;
	    }
#ifdef DEBUG_SEARCH
	    fprintf(stderr,"%i %s '%s': included already\n", ppnArg->l_path, statep(s_ptr) ? NAME_STATE : NAME_TRANSITION, s_ptr->pucId);
#endif
	  }
	  else {
	    /* not yet included in path */
	    ppnArg->l_path++;
	    ppnArg->path[ppnArg->l_path] = s_ptr;
	    if (ppnArg->path[ppnArg->l_path] == ppnArg->target) {
	      /* target is reached, path is completed */
	      ppnArg->n_paths++;   /* count this path and */
	      fResult = TRUE;    /* leave the recusion */
#ifdef DEBUG_SEARCH
	      fprintf(stderr,"%i %s '%s': ready\n", ppnArg->l_path, statep(s_ptr) ? NAME_STATE : NAME_TRANSITION, s_ptr->pucId);
#endif
	      break;
	    }
#ifdef DEBUG_SEARCH
	    fprintf(stderr,"%i %s '%s': added + recursion\n", ppnArg->l_path, statep(s_ptr) ? NAME_STATE : NAME_TRANSITION, s_ptr->pucId);
#endif
	    fResult = pnetSearchPathForward(ppnArg);	/* search recursive next knot for path[] */
	    break;
	  }
	}
      }
    }
    else {
      assert(FALSE);
    }
  }
  return fResult;			/* nothing more found */
}
/* End of pnetSearchPathForward() */


/*! search for the last possible branch 
 */
BOOL_T
pnetSearchBranchBackward(petrinet_t *ppnArg)
{
  BOOL_T fResult = FALSE;

#ifdef DEBUG_SEARCH
  fprintf(stderr,"pnetSearchBranchBackward()\n");
#endif
    
  if (ppnArg != NULL && statep(ppnArg->start) && statep(ppnArg->target) && ppnArg->path != NULL && ppnArg->path[0] == ppnArg->start) {
    index_t i;
    knot_t *ol_knot;
    knot_t *l_knot;

    if (ppnArg->l_path < 1) {
      /* can't change the start, nothing more found */
#ifdef DEBUG_SEARCH
      fprintf(stderr,"Cannot step backward\n");
#endif
    }
    else {
      ol_knot = ppnArg->path[ppnArg->l_path]; /* ptr to old last knot */
#ifdef DEBUG_SEARCH
      fprintf(stderr,"%i %s '%s': old last\n", ppnArg->l_path, statep(ol_knot) ? NAME_STATE : NAME_TRANSITION, ol_knot->pucId);
#endif

      ppnArg->l_path--; /* one step backward */

      l_knot = ppnArg->path[ppnArg->l_path]; /* ptr to last knot */
#ifdef DEBUG_SEARCH
      fprintf(stderr,"%i %s '%s': one step backward\n", ppnArg->l_path, statep(l_knot) ? NAME_STATE : NAME_TRANSITION, l_knot->pucId);
#endif

      if (statep(l_knot)) {
	/* last element is a state -> search for a transition knot */

	index_t l_state = state_index(l_knot);
	index_t l_trans = transition_index(ol_knot); /* index of removed step */

	for (i=l_trans+1; i < ppnArg->n_t; i++) { /* next transitions after removed one */
	  if (edge_st(ppnArg,l_state,i)
	      && ! pnetIsMemberTransition(ppnArg, &(ppnArg->t[i]))) {
	    /* found a valid transition,
	   with an edge to l_state and
	   not yet included in path */
	    ppnArg->l_path++;
	    ppnArg->path[ppnArg->l_path] = &(ppnArg->t[i]);
	    fResult = TRUE;		/* leave backward recursion */
#ifdef DEBUG_SEARCH
	    fprintf(stderr,"%i %s '%s': removed + added + recursion\n", ppnArg->l_path, statep(ppnArg->path[ppnArg->l_path]) ? NAME_STATE : NAME_TRANSITION, ppnArg->path[ppnArg->l_path]->pucId);
#endif
	    return fResult;
	  }
	}
	fResult = pnetSearchBranchBackward(ppnArg);
      }
      else {
	/* last element is a transition */

	index_t l_trans = transition_index(l_knot);
	index_t l_state = state_index(ol_knot);

	for (i=l_state+1; i < ppnArg->n_s; i++) {
	  if (edge_ts(ppnArg,l_trans,i)
	      && ! pnetIsMemberState(ppnArg, &(ppnArg->s[i]))) {
	    /* found a state */
	    ppnArg->l_path++;
	    ppnArg->path[ppnArg->l_path] = &(ppnArg->s[i]);
	    fResult = TRUE;
#ifdef DEBUG_SEARCH
	    fprintf(stderr,"%i %s '%s': next\n", ppnArg->l_path, statep(ppnArg->path[ppnArg->l_path]) ? NAME_STATE : NAME_TRANSITION, ppnArg->path[ppnArg->l_path]->pucId);
#endif
	    return fResult;
	  }
	}
	fResult = pnetSearchBranchBackward(ppnArg);
      }
    }
  }
  return fResult;
}
/* End of pnetSearchBranchBackward() */


#if 0
/*! computes the distance between two knots

\param ppnArg petrinet record
\param a pointer to start knot
\param b pointer to target knot

\return the lowest number of edges between a and b

 0 means no path between knots, or start_id is same as target_id
*/
index_t
pnetGetKnotDistance(petrinet_t *ppnArg, knot_t *a, knot_t *b)
{
  index_t result = 0;

  if (a != b) {
    index_t k = 0;
    
    ppnArg->start = a;
    ppnArg->target = b;

    /* find the minimum distance value */
    do {
      if (pnetSearchPathForward(ppnArg)) {
	if (result == 0 || ppnArg->l_path < result) {
	  result = ppnArg->l_path;
	}
      }
      k++;
    } while (k < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));
  }

  return result;
}
/* End of pnetGetKnotDistance() */
#endif

void
pnetSetPathMarker(petrinet_t *ppnArg, BOOL_T f)
{
  if (ppnArg != NULL) {
    /*
     set all knots in ppnArg->path on f
     */
    index_t i;
    edge_t *e;

    for (i=0; i <= ppnArg->l_path; i++) {
      ppnArg->path[i]->fMarker = f;	/* set knot */
      if (i && (e = pnetGetEdgePtrFromPtr(ppnArg,ppnArg->path[i-1],ppnArg->path[i]))) {
	e->fMarker = f;		/* set edge */
	e->from->fMarker = f;
	e->to->fMarker = f;
      }
    }
  }
}
/* End of pnetSetPathMarker() */


#if 0
/*! set all knots in paths between state  on f
*/
BOOL_T
petrinetPathnet_out_flag(petrinet_t *ppnArg, BOOL_T f, cxpContextPtr pccArg)
{

  if (ppnArg->start && ppnArg->target) {

    pnetSetMarker(ppnArg,!f);

    do {
      if (pnetSearchPathForward(ppnArg)) {
	pnetSetPathMarker(ppnArg,f);
      }
    } while (pnetSearchBranchBackward(ppnArg));

    cxpCtxtLogPrint(pccArg, 1,"%i path between '%s' and '%s' found",
	    ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));
  }

  return (ppnArg->nPaths > 0);
}
/* End of petrinetPathnet_out_flag() */
#endif


void
pnetSetMarker(petrinet_t *ppnArg, BOOL_T f)
{
  if (ppnArg != NULL) {
    index_t i;

    assert(ppnArg->s);
    assert(ppnArg->n_s > ERROR_INDEX && ppnArg->n_s < KNOTS_MAX);
    for (i=0; i < ppnArg->n_s; i++) {
      ppnArg->s[i].fMarker = f;
    }

    assert(ppnArg->t);
    assert(ppnArg->n_t > ERROR_INDEX && ppnArg->n_t < KNOTS_MAX);
    for (i=0; i < ppnArg->n_t; i++) {
      ppnArg->t[i].fMarker = f;
    }

    assert(ppnArg->edge);
    assert(ppnArg->n_e > ERROR_INDEX && ppnArg->n_e < EDGES_MAX);
    for (i=0; i < ppnArg->n_e; i++) {
      ppnArg->edge[i].fMarker = f;
    }
  }
}
/* End of pnetSetMarker() */


#if 0
/*! sets all VALID in DOM according to fMarker flags in petrinet
*/
BOOL_T
pnetSetValidOut(petrinet_t *ppnArg)
{
  knot_t *k;
  index_t i;

  assert(ppnArg);

  for (k=ppnArg->s; KNOTID(k); k++) {
    if (!k->fMarker) {
      xmlSetProp(k->node, BAD_CAST "VALID", BAD_CAST "no");
    }
  }

  for (k=ppnArg->t; KNOTID(k); k++) {
    if (!k->fMarker) {
      xmlSetProp(k->node, BAD_CAST "VALID", BAD_CAST "no");
    }
  }

  for (i=0; i < ppnArg->n_e; i++) {
    if (!ppnArg->edge[i].fMarker) {
      xmlSetProp(ppnArg->edge[i].node, BAD_CAST "VALID", BAD_CAST "no");
    }
  }
  return TRUE;
}
/* end of pnetSetValidOut() */
#endif


/*! compiles a filter XSL of pathnet

  \param ppnArg pointer to used petrinet
  \param flagXml result is XML code if TRUE, XSL code if FALSE

  \result XSL DOM for filtering origin petrinet XML or copy of XML DOM with valid attributes

  \todo multithreading path searching (reentrant versions of pnetSearchPathForward() and pnetSearchBranchBackward())

  \todo weight nodes according to the count of path members
*/
xmlDocPtr
pnetProcessPathnet(petrinet_t *ppnArg, BOOL_T flagXml, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlNsPtr pnsXsl = domGetNsXsl();
  index_t j = 0;

  if (ppnArg != NULL) {
    xmlNodePtr nodeT;
    xmlNodePtr pndXslStylesheet;
    xmlNodePtr pndXslTemplate;
    xmlNodePtr pndXslChoose;
    xmlNodePtr pndXslWhen;
    xmlNodePtr pndXslOtherwise;
    xmlNodePtr pndKnot;
    xmlChar mpucT[BUFFER_LENGTH];

    knot_t *k;
    index_t i;

    pnetSetMarker(ppnArg,FALSE);
    if (ppnArg->start) {
      if (ppnArg->target) {
	/* start and target are defined */
	do {
	  if (pnetSearchPathForward(ppnArg)) {
	    pnetSetPathMarker(ppnArg,TRUE);
	  }
	  j++;
	} while (j < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));
	cxpCtxtLogPrint(pccArg, 1,"%i path between '%s' and '%s' found",
	    ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));
      }
      else {
	/* open target */
	for (k=ppnArg->s; KNOTID(k); k++) {
	  if (k==ppnArg->start) {
	    /* no circles are allowed here */
	    cxpCtxtLogPrint(pccArg, 2,"Skip '%s'", KNOTID(k));
	  }
	  else {
	    /* target knot */
	    if (pnetSetTarget(ppnArg, KNOTID(k), pccArg)) {
	      do {
		if (pnetSearchPathForward(ppnArg)) {
		  pnetSetPathMarker(ppnArg,TRUE);
		}
		j++;
	      } while (j < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));

	      cxpCtxtLogPrint(pccArg, 2,"%i path between '%s' and '%s' found",
		  ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));
	    }
	    else {
	      cxpCtxtLogPrint(pccArg, 1, "Cant set target STELLE");
	      //pnetFree(ppnArg);
	      break;
	    }
	  }
	}
      }
    }
    else {
      if (ppnArg->target) {
	/* open start */
	for (k=ppnArg->s; KNOTID(k); k++) {
	  if (k==ppnArg->target) {
	    /* no circles are allowed here */
	    cxpCtxtLogPrint(pccArg, 2,"Skip '%s'", KNOTID(k));
	  }
	  else {
	    /* target knot */
	    if (pnetSetStart(ppnArg, KNOTID(k), pccArg)) {
	      do {
		if (pnetSearchPathForward(ppnArg)) {
		  pnetSetPathMarker(ppnArg,TRUE);
		}
		j++;
	      } while (j < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));

	      cxpCtxtLogPrint(pccArg, 1,"%i path between '%s' and '%s' found",
		  ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));
	    }
	    else {
	      cxpCtxtLogPrint(pccArg, 1, "Cant set start STELLE");
	      //pnetFree(ppnArg);
	      break;
	    }
	  }
	}
      }
      else {
	/* neither start nor target */
	assert(FALSE);
      }
    }

    if (flagXml) {
      /* no preparation needed */
    }
    else {
      pdocResult = xmlNewDoc(BAD_CAST "1.0");

      pndXslStylesheet = xmlNewDocNode(pdocResult, pnsXsl, BAD_CAST "stylesheet", NULL);
      xmlSetProp(pndXslStylesheet, BAD_CAST "version", BAD_CAST "1.0");
      xmlDocSetRootElement(pdocResult, pndXslStylesheet);

      nodeT = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "output", NULL);
      xmlSetProp(nodeT, BAD_CAST "method", BAD_CAST "xml");
      xmlSetProp(nodeT, BAD_CAST "encoding", BAD_CAST "ISO-8859-1");

      pndXslTemplate = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "template", NULL);
      xmlSetProp(pndXslTemplate, BAD_CAST "match", BAD_CAST "@*|node()");

      pndXslChoose = xmlNewChild(pndXslTemplate, pnsXsl, BAD_CAST "choose", NULL);
      /* add a dummy when, because empty */
      pndXslWhen = xmlNewChild(pndXslChoose, NULL, BAD_CAST "when", NULL);
      xmlSetProp(pndXslWhen, BAD_CAST "test", BAD_CAST "false()");
    }

    for (k=ppnArg->s; KNOTID(k); k++) {
      if (k->fMarker == FALSE) {
	cxpCtxtLogPrint(pccArg, 3,"Filter STELLE '%s'",KNOTID(k));
	if (flagXml) {
	  xmlSetProp(k->pndSource,BAD_CAST"valid",BAD_CAST"no");
	}
	else {
	  xmlStrPrintf(mpucT,BUFFER_LENGTH, "@id='%s'",KNOTID(k));
	  pndXslWhen = xmlNewChild(pndXslChoose, NULL, BAD_CAST "when", NULL);
	  xmlSetProp(pndXslWhen, BAD_CAST "test", mpucT);
	}
      }
    }

    for (k=ppnArg->t; KNOTID(k); k++) {
      if (k->fMarker == FALSE) {
	cxpCtxtLogPrint(pccArg, 3,"Filter TRANSITION '%s'",KNOTID(k));
	if (flagXml) {
	  xmlSetProp(k->pndSource,BAD_CAST"valid",BAD_CAST"no");
	}
	else {
	  xmlStrPrintf(mpucT,BUFFER_LENGTH, "@id='%s'",KNOTID(k));
	  pndXslWhen = xmlNewChild(pndXslChoose, NULL, BAD_CAST "when", NULL);
	  xmlSetProp(pndXslWhen, BAD_CAST "test", mpucT);
	}
      }
    }

    for (i=0; i < ppnArg->n_e; i++) {
      assert(ppnArg->edge[i].from);
      assert(ppnArg->edge[i].to);
      if  (ppnArg->edge[i].fMarker == FALSE || ppnArg->edge[i].from->fMarker == FALSE || ppnArg->edge[i].to->fMarker == FALSE) {
	cxpCtxtLogPrint(pccArg, 3,"Filter relation '%s' -> '%s'", KNOTID(ppnArg->edge[i].from), KNOTID(ppnArg->edge[i].to));
	if (flagXml) {
	  xmlSetProp(ppnArg->edge[i].pndSource,BAD_CAST"valid",BAD_CAST"no");
	}
	else {
	  xmlStrPrintf(mpucT,BUFFER_LENGTH, "@from='%s' and @to='%s'",KNOTID(ppnArg->edge[i].from), KNOTID(ppnArg->edge[i].to));
	  pndXslWhen = xmlNewChild(pndXslChoose, NULL, BAD_CAST "when", NULL);
	  xmlSetProp(pndXslWhen, BAD_CAST "test", mpucT);
	}
      }
    }

    if (flagXml) {
      pdocResult = xmlCopyDoc(ppnArg->pndRoot->doc,1);
    }
    else {
      pndXslOtherwise = xmlNewChild(pndXslChoose, NULL, BAD_CAST "otherwise", NULL);

      pndKnot = xmlNewChild(pndXslOtherwise, pnsXsl, BAD_CAST "copy", NULL);

      pndKnot = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "apply-templates", NULL);
      xmlSetProp(pndKnot,BAD_CAST "select", BAD_CAST "@*|node()");
    }
  }
  return pdocResult;
}
/* End of pnetProcessPathnet() */


/*! compiles a filter XSL of pathtable

  \param ppnArg pointer to used petrinet

  \result XSL DOM for filtering origin petrinet XML
*/
xmlDocPtr
pnetProcessPathtable(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlNsPtr pnsXsl = domGetNsXsl();

  if (ppnArg != NULL && ppnArg->start != NULL && ppnArg->target != NULL) {
    pdocResult = xmlNewDoc(BAD_CAST "1.0");
    if (pdocResult) {
      int i = 0;
      int k = 0;
      xmlNodePtr pndT;
      xmlNodePtr pndPathtable;
      xmlNodePtr pndXslStylesheet;
      xmlNodePtr pndXslTemplate;
      xmlNodePtr pndAttributeCount;
      xmlNodePtr pndKnot;
      xmlChar mpucT[BUFFER_LENGTH];

      pndXslStylesheet = xmlNewDocNode(pdocResult, pnsXsl, BAD_CAST "stylesheet", NULL);
      xmlSetProp(pndXslStylesheet, BAD_CAST "version", BAD_CAST "1.0");
      xmlDocSetRootElement(pdocResult, pndXslStylesheet);

      pndT = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "output", NULL);
      xmlSetProp(pndT, BAD_CAST "method", BAD_CAST "xml");
      xmlSetProp(pndT, BAD_CAST "encoding", BAD_CAST "ISO-8859-1");

      pndXslTemplate = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "template", NULL);
      xmlSetProp(pndXslTemplate, BAD_CAST "match", BAD_CAST "/");

      pndPathtable = xmlNewChild(pndXslTemplate, NULL, BAD_CAST "element", NULL);
      xmlSetProp(pndPathtable, BAD_CAST "name", BAD_CAST "pathtable");

      pndAttributeCount = xmlNewChild(pndPathtable, pnsXsl, BAD_CAST "attribute", NULL);
      xmlSetProp(pndAttributeCount, BAD_CAST "name", BAD_CAST "count");

      pnetSetMarker(ppnArg,FALSE);
      do {
	if (pnetSearchPathForward(ppnArg)) {
	  i++;
	  pnetSetPathMarker(ppnArg,TRUE);
	  if (i < ppnArg->n_max) {
	    if (!pnetParsePathXml(ppnArg,pndPathtable,pnsXsl,pccArg)) {
	      cxpCtxtLogPrint(pccArg, 1,"kk() failed");
	      break;
	    }
	  }
	}
	k++;
      } while (k < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));

      xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i",i);
      /*     xmlSetProp(pndPathtable, BAD_CAST "COUNT", mpucT); */
      xmlNewTextChild(pndAttributeCount, pnsXsl, BAD_CAST "text", mpucT);

      cxpCtxtLogPrint(pccArg, 2,"%i path between '%s' and '%s' found",
	  ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));

      /* create rest of templates
       */
      pndXslTemplate = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "template", NULL);
      xmlSetProp(pndXslTemplate, BAD_CAST "match", NAME_STATE);

      pndKnot = xmlNewChild(pndXslTemplate, NULL, BAD_CAST "element", NULL);
      xmlSetProp(pndKnot, BAD_CAST "name", NAME_STATE);
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "@id");
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "h");
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "abstract");

      pndXslTemplate = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "template", NULL);
      xmlSetProp(pndXslTemplate, BAD_CAST "match", NAME_TRANSITION);

      pndKnot = xmlNewChild(pndXslTemplate, NULL, BAD_CAST "element", NULL);
      xmlSetProp(pndKnot, BAD_CAST "name", NAME_TRANSITION);
#if 1
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "@id");
#else
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "attribute", NULL);
      xmlSetProp(pndT, BAD_CAST "name", BAD_CAST "id");
      pndT = xmlNewChild(pndT, pnsXsl, BAD_CAST "value-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "@id");
#endif
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "h");
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "abstract");
      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "make");

      pndT = xmlNewChild(pndKnot, NULL, BAD_CAST "element", NULL);
      xmlSetProp(pndT, BAD_CAST "name", BAD_CAST "parent");
      pndT = xmlNewChild(pndT, pnsXsl, BAD_CAST "value-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "../h");

      pndXslTemplate = xmlNewChild(pndXslStylesheet, pnsXsl, BAD_CAST "template", NULL);
      xmlSetProp(pndXslTemplate, BAD_CAST "match", NAME_EDGE);

      pndKnot = xmlNewChild(pndXslTemplate, NULL, BAD_CAST "element", NULL);
      xmlSetProp(pndKnot, BAD_CAST "name", NAME_EDGE);

      pndT = xmlNewChild(pndKnot, pnsXsl, BAD_CAST "copy-of", NULL);
      xmlSetProp(pndT, BAD_CAST "select", BAD_CAST "abstract");
    }
  }
  return pdocResult;
}
/* End of pnetProcessPathtable() */


/*! compiles a DOM of a path

\param ppnArg pointer to used petrinet
\param pndParent parent in DOM
\param pnsXsl namespace in DOM

\result complete DOM of a path
 */
xmlNodePtr
pnetParsePathXml(petrinet_t *ppnArg, xmlNodePtr pndParent, xmlNsPtr pnsXsl, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

  if (ppnArg != NULL) {
    int i;
    knot_t *k;
    edge_t *e;
    xmlNodePtr pndStep;
    xmlChar mpucT[BUFFER_LENGTH];

    pndResult = xmlNewChild(pndParent, NULL, BAD_CAST "element", NULL);
    if (pndResult) {
      xmlSetProp(pndResult, BAD_CAST "name", BAD_CAST "path");

      for (i=0; i <= ppnArg->l_path; i++) {
	k = ppnArg->path[i];

	/* of this RELATION */
	if (i>0) {
	  e = pnetGetEdgePtrFromPtr(ppnArg,ppnArg->path[i-1],k);
	  if (e) {
	    pndStep = xmlNewChild(pndResult, pnsXsl, BAD_CAST "apply-templates",NULL);
	    xmlStrPrintf(mpucT,BUFFER_LENGTH, "//*[@from = '%s' and @to = '%s']", KNOTID(e->from), KNOTID(e->to));
	    xmlSetProp(pndStep,BAD_CAST "select", mpucT);
	  }
	}
	pndStep = xmlNewChild(pndResult, pnsXsl, BAD_CAST "apply-templates",NULL);
	xmlStrPrintf(mpucT,BUFFER_LENGTH, "//*[@id = '%s']", KNOTID(k));
	xmlSetProp(pndStep,BAD_CAST "select", mpucT);
      }
    }
  }
  return pndResult;
}
/* End of pnetParsePathXml() */


/*! compiles a filter XSL of pathtable

  \param ppnArg pointer to used petrinet

  \result XSL DOM for filtering origin petrinet XML
*/
xmlDocPtr
pnetProcessPathtableSource(petrinet_t *ppnArg, cxpContextPtr pccArg)
{
  xmlDocPtr pdocResult = NULL;
  xmlNsPtr pnsXsl = domGetNsXsl();

  if (ppnArg != NULL && ppnArg->start != NULL && ppnArg->target != NULL) {
    int i = 0;
    int k = 0;
    xmlNodePtr pndPathtable;
    xmlNodePtr pndXslStylesheet;
    xmlChar mpucT[BUFFER_LENGTH];


    pdocResult = xmlNewDoc(BAD_CAST "1.0");

    pndXslStylesheet = xmlNewDocNode(pdocResult, NULL, BAD_CAST "petrinet", NULL);
    xmlSetProp(pndXslStylesheet, BAD_CAST "class", BAD_CAST "pathtable");
    xmlDocSetRootElement(pdocResult, pndXslStylesheet);
    pndPathtable = xmlNewChild(pndXslStylesheet, NULL, BAD_CAST "pathtable", NULL);

    pnetSetMarker(ppnArg,FALSE);
    do {
      if (pnetSearchPathForward(ppnArg)) {
	i++;
	pnetSetPathMarker(ppnArg,TRUE);
	if (i < ppnArg->n_max) {
	  if (!pnetParsePathXmlSource(ppnArg,pndPathtable,pnsXsl,pccArg)) {
	    cxpCtxtLogPrint(pccArg, 1,"kk() failed");
	    break;
	  }
	}
      }
      k++;
    } while (k < LOOPS_MAX && pnetSearchBranchBackward(ppnArg));

    xmlStrPrintf(mpucT,BUFFER_LENGTH, "%i",i);
    xmlSetProp(pndPathtable, BAD_CAST "COUNT", mpucT); 

    cxpCtxtLogPrint(pccArg, 2,"%i path between '%s' and '%s' found",
	ppnArg->n_paths, KNOTID(ppnArg->start), KNOTID(ppnArg->target));
  }

  return pdocResult;
}
/* End of pnetProcessPathtableSource() */


/*! compiles a DOM of a path

\param ppnArg pointer to used petrinet
\param pndParent parent in DOM
\param pnsXsl namespace in DOM

\result complete DOM of a path
 */
xmlNodePtr
pnetParsePathXmlSource(petrinet_t *ppnArg, xmlNodePtr pndParent, xmlNsPtr pnsXsl, cxpContextPtr pccArg)
{
  xmlNodePtr pndResult = NULL;

  if (ppnArg != NULL) {
    int i;
    knot_t *k;
    edge_t *e;
    xmlNodePtr pndStep;

    pndResult = xmlNewChild(pndParent, NULL, BAD_CAST "path", NULL);

    for (i=0; i <= ppnArg->l_path; i++) {
      k = ppnArg->path[i];

      /* of this RELATION */
      if (i>0) {
	e = pnetGetEdgePtrFromPtr(ppnArg,ppnArg->path[i-1],k);
	if (e) {
	  assert(e->pndSource);
	  if ((pndStep = xmlCopyNode(e->pndSource,1))) {
	    xmlAddChild(pndResult,pndStep);
	  }
	  else {
	    cxpCtxtLogPrint(pccArg, 1,"kk() failed");	  
	  }
	}
      }

      assert(k);
      assert(k->pndSource);
      if ((pndStep = xmlCopyNode(k->pndSource,1))) {
	xmlAddChild(pndResult,pndStep);
      }
      else {
	cxpCtxtLogPrint(pccArg, 1,"kk() failed");
      }
    }
  }
  return pndResult;
}
/* End of pnetParsePathXmlSource() */


#ifdef TESTCODE
#include "test/test_petrinet.c"
#endif

