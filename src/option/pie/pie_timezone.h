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

typedef enum {
  PIE_TZ_UTC,
  PIE_TZ_ACDT,
  PIE_TZ_ACST,
  PIE_TZ_ACT,
  PIE_TZ_ADT,
  PIE_TZ_AEDT,
  PIE_TZ_AEST,
  PIE_TZ_AFT,
  PIE_TZ_AKDT,
  PIE_TZ_AKST,
  PIE_TZ_AMST,
  PIE_TZ_AMST_1,
  PIE_TZ_AMT,
  PIE_TZ_AMT_1,
  PIE_TZ_ART,
  PIE_TZ_AST,
  PIE_TZ_AST_1,
  PIE_TZ_AWDT,
  PIE_TZ_AWST,
  PIE_TZ_AZOST,
  PIE_TZ_AZT,
  PIE_TZ_BDT,
  PIE_TZ_BIOT,
  PIE_TZ_BIT,
  PIE_TZ_BOT,
  PIE_TZ_BRT,
  PIE_TZ_BST,
  PIE_TZ_BST_1,
  PIE_TZ_BTT,
  PIE_TZ_CAT,
  PIE_TZ_CCT,
  PIE_TZ_CDT,
  PIE_TZ_CDT_1,
  PIE_TZ_CEDT,
  PIE_TZ_CEST,
  PIE_TZ_CET,
  PIE_TZ_CHADT,
  PIE_TZ_CHAST,
  PIE_TZ_CHOT,
  PIE_TZ_ChST,
  PIE_TZ_CHUT,
  PIE_TZ_CIST,
  PIE_TZ_CIT,
  PIE_TZ_CKT,
  PIE_TZ_CLST,
  PIE_TZ_CLT,
  PIE_TZ_COST,
  PIE_TZ_COT,
  PIE_TZ_CST,
  PIE_TZ_CST_1,
  PIE_TZ_CST_2,
  PIE_TZ_CST_3,
  PIE_TZ_CST_4,
  PIE_TZ_CT,
  PIE_TZ_CVT,
  PIE_TZ_CWST,
  PIE_TZ_CXT,
  PIE_TZ_DAVT,
  PIE_TZ_DDUT,
  PIE_TZ_DFT,
  PIE_TZ_EASST,
  PIE_TZ_EAST,
  PIE_TZ_EAT,
  PIE_TZ_ECT,
  PIE_TZ_ECT_1,
  PIE_TZ_EDT,
  PIE_TZ_EEDT,
  PIE_TZ_EEST,
  PIE_TZ_EET,
  PIE_TZ_EGST,
  PIE_TZ_EGT,
  PIE_TZ_EIT,
  PIE_TZ_EST,
  PIE_TZ_EST_1,
  PIE_TZ_FET,
  PIE_TZ_FJT,
  PIE_TZ_FKST,
  PIE_TZ_FKST_1,
  PIE_TZ_FKT,
  PIE_TZ_FNT,
  PIE_TZ_GALT,
  PIE_TZ_GAMT,
  PIE_TZ_GET,
  PIE_TZ_GFT,
  PIE_TZ_GILT,
  PIE_TZ_GIT,
  PIE_TZ_GMT,
  PIE_TZ_GST,
  PIE_TZ_GST_1,
  PIE_TZ_GYT,
  PIE_TZ_HADT,
  PIE_TZ_HAEC,
  PIE_TZ_HAST,
  PIE_TZ_HKT,
  PIE_TZ_HMT,
  PIE_TZ_HOVT,
  PIE_TZ_HST,
  PIE_TZ_ICT,
  PIE_TZ_IDT,
  PIE_TZ_IOT,
  PIE_TZ_IRDT,
  PIE_TZ_IRKT,
  PIE_TZ_IRST,
  PIE_TZ_IST,
  PIE_TZ_IST_1,
  PIE_TZ_IST_2,
  PIE_TZ_JST,
  PIE_TZ_KGT,
  PIE_TZ_KOST,
  PIE_TZ_KRAT,
  PIE_TZ_KST,
  PIE_TZ_LHST,
  PIE_TZ_LHST_1,
  PIE_TZ_LINT,
  PIE_TZ_MAGT,
  PIE_TZ_MART,
  PIE_TZ_MAWT,
  PIE_TZ_MDT,
  PIE_TZ_MET,
  PIE_TZ_MEST,
  PIE_TZ_MHT,
  PIE_TZ_MIST,
  PIE_TZ_MIT,
  PIE_TZ_MMT,
  PIE_TZ_MSK,
  PIE_TZ_MST,
  PIE_TZ_MST_1,
  PIE_TZ_MST_2,
  PIE_TZ_MUT,
  PIE_TZ_MVT,
  PIE_TZ_MYT,
  PIE_TZ_NCT,
  PIE_TZ_NDT,
  PIE_TZ_NFT,
  PIE_TZ_NPT,
  PIE_TZ_NST,
  PIE_TZ_NT,
  PIE_TZ_NUT,
  PIE_TZ_NZDT,
  PIE_TZ_NZST,
  PIE_TZ_OMST,
  PIE_TZ_ORAT,
  PIE_TZ_PDT,
  PIE_TZ_PET,
  PIE_TZ_PETT,
  PIE_TZ_PGT,
  PIE_TZ_PHOT,
  PIE_TZ_PKT,
  PIE_TZ_PMDT,
  PIE_TZ_PMST,
  PIE_TZ_PONT,
  PIE_TZ_PST,
  PIE_TZ_PST_1,
  PIE_TZ_PYST,
  PIE_TZ_PYT,
  PIE_TZ_RET,
  PIE_TZ_ROTT,
  PIE_TZ_SAKT,
  PIE_TZ_SAMT,
  PIE_TZ_SAST,
  PIE_TZ_SBT,
  PIE_TZ_SCT,
  PIE_TZ_SGT,
  PIE_TZ_SLST,
  PIE_TZ_SRET,
  PIE_TZ_SRT,
  PIE_TZ_SST,
  PIE_TZ_SST_1,
  PIE_TZ_SYOT,
  PIE_TZ_TAHT,
  PIE_TZ_THA,
  PIE_TZ_TFT,
  PIE_TZ_TJT,
  PIE_TZ_TKT,
  PIE_TZ_TLT,
  PIE_TZ_TMT,
  PIE_TZ_TOT,
  PIE_TZ_TVT,
  PIE_TZ_UCT,
  PIE_TZ_ULAT,
  PIE_TZ_USZ1,
  PIE_TZ_UYST,
  PIE_TZ_UYT,
  PIE_TZ_UZT,
  PIE_TZ_VET,
  PIE_TZ_VLAT,
  PIE_TZ_VOLT,
  PIE_TZ_VOST,
  PIE_TZ_VUT,
  PIE_TZ_WAKT,
  PIE_TZ_WAST,
  PIE_TZ_WAT,
  PIE_TZ_WEDT,
  PIE_TZ_WEST,
  PIE_TZ_WET,
  PIE_TZ_WIT,
  PIE_TZ_WST,
  PIE_TZ_YAKT,
  PIE_TZ_YEKT,
  PIE_TZ_Z,
  PIE_TZ_END
} pie_tz_t;

extern int
tzGetRuntimeOffsetToUTC(void);

extern int
tzGetCount(void);

extern int
tzGetNumber(const xmlChar *pchArg);

extern const xmlChar *
tzGetId(int iArg);

extern const xmlChar *
tzGetName(int iArg);

extern const int
tzGetOffset(int iArg);

#ifdef TESTCODE
extern int
pieTimezoneTest(void);
#endif
