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



#include <libxml/xmlstring.h>
#include <libxml/tree.h>

#include "basics.h"
#include <cxp/cxp_timezone.h>
#include "utils.h"

int tzOffsetToUTC = 9999; /*!> default Offset to UTC in Minutes while runtime */

/*
  http://en.wikipedia.org/wiki/List_of_time_zone_abbreviations

  http://www.timeanddate.com/time/zones/
  
  http://www.unicode.org/cldr/charts/29/supplemental/zone_tzid.html
  
  https://en.wikipedia.org/wiki/Tz_database
*/

const xmlChar* ppucTZId[] = {
  BAD_CAST "UTC",
  BAD_CAST "ACDT",
  BAD_CAST "ACST",
  BAD_CAST "ACT",
  BAD_CAST "ADT",
  BAD_CAST "AEDT",
  BAD_CAST "AEST",
  BAD_CAST "AFT",
  BAD_CAST "AKDT",
  BAD_CAST "AKST",
  BAD_CAST "AMST",
  BAD_CAST "AMST",
  BAD_CAST "AMT",
  BAD_CAST "AMT",
  BAD_CAST "ART",
  BAD_CAST "AST",
  BAD_CAST "AST",
  BAD_CAST "AWDT",
  BAD_CAST "AWST",
  BAD_CAST "AZOST",
  BAD_CAST "AZT",
  BAD_CAST "BDT",
  BAD_CAST "BIOT",
  BAD_CAST "BIT",
  BAD_CAST "BOT",
  BAD_CAST "BRT",
  BAD_CAST "BST",
  BAD_CAST "BST",
  BAD_CAST "BTT",
  BAD_CAST "CAT",
  BAD_CAST "CCT",
  BAD_CAST "CDT",
  BAD_CAST "CDT",
  BAD_CAST "CEDT",
  BAD_CAST "CEST",
  BAD_CAST "CET",
  BAD_CAST "CHADT",
  BAD_CAST "CHAST",
  BAD_CAST "CHOT",
  BAD_CAST "ChST",
  BAD_CAST "CHUT",
  BAD_CAST "CIST",
  BAD_CAST "CIT",
  BAD_CAST "CKT",
  BAD_CAST "CLST",
  BAD_CAST "CLT",
  BAD_CAST "COST",
  BAD_CAST "COT",
  BAD_CAST "CST",
  BAD_CAST "CST",
  BAD_CAST "CST",
  BAD_CAST "CST",
  BAD_CAST "CST",
  BAD_CAST "CT",
  BAD_CAST "CVT",
  BAD_CAST "CWST",
  BAD_CAST "CXT",
  BAD_CAST "DAVT",
  BAD_CAST "DDUT",
  BAD_CAST "DFT",
  BAD_CAST "EASST",
  BAD_CAST "EAST",
  BAD_CAST "EAT",
  BAD_CAST "ECT",
  BAD_CAST "ECT",
  BAD_CAST "EDT",
  BAD_CAST "EEDT",
  BAD_CAST "EEST",
  BAD_CAST "EET",
  BAD_CAST "EGST",
  BAD_CAST "EGT",
  BAD_CAST "EIT",
  BAD_CAST "EST",
  BAD_CAST "EST",
  BAD_CAST "FET",
  BAD_CAST "FJT",
  BAD_CAST "FKST",
  BAD_CAST "FKST",
  BAD_CAST "FKT",
  BAD_CAST "FNT",
  BAD_CAST "GALT",
  BAD_CAST "GAMT",
  BAD_CAST "GET",
  BAD_CAST "GFT",
  BAD_CAST "GILT",
  BAD_CAST "GIT",
  BAD_CAST "GMT",
  BAD_CAST "GST",
  BAD_CAST "GST",
  BAD_CAST "GYT",
  BAD_CAST "HADT",
  BAD_CAST "HAEC",
  BAD_CAST "HAST",
  BAD_CAST "HKT",
  BAD_CAST "HMT",
  BAD_CAST "HOVT",
  BAD_CAST "HST",
  BAD_CAST "ICT",
  BAD_CAST "IDT",
  BAD_CAST "IOT",
  BAD_CAST "IRDT",
  BAD_CAST "IRKT",
  BAD_CAST "IRST",
  BAD_CAST "IST",
  BAD_CAST "IST",
  BAD_CAST "IST",
  BAD_CAST "JST",
  BAD_CAST "KGT",
  BAD_CAST "KOST",
  BAD_CAST "KRAT",
  BAD_CAST "KST",
  BAD_CAST "LHST",
  BAD_CAST "LHST",
  BAD_CAST "LINT",
  BAD_CAST "MAGT",
  BAD_CAST "MART",
  BAD_CAST "MAWT",
  BAD_CAST "MDT",
  BAD_CAST "MET",
  BAD_CAST "MEST",
  BAD_CAST "MHT",
  BAD_CAST "MIST",
  BAD_CAST "MIT",
  BAD_CAST "MMT",
  BAD_CAST "MSK",
  BAD_CAST "MST",
  BAD_CAST "MST",
  BAD_CAST "MST",
  BAD_CAST "MUT",
  BAD_CAST "MVT",
  BAD_CAST "MYT",
  BAD_CAST "NCT",
  BAD_CAST "NDT",
  BAD_CAST "NFT",
  BAD_CAST "NPT",
  BAD_CAST "NST",
  BAD_CAST "NT",
  BAD_CAST "NUT",
  BAD_CAST "NZDT",
  BAD_CAST "NZST",
  BAD_CAST "OMST",
  BAD_CAST "ORAT",
  BAD_CAST "PDT",
  BAD_CAST "PET",
  BAD_CAST "PETT",
  BAD_CAST "PGT",
  BAD_CAST "PHOT",
  BAD_CAST "PKT",
  BAD_CAST "PMDT",
  BAD_CAST "PMST",
  BAD_CAST "PONT",
  BAD_CAST "PST",
  BAD_CAST "PST",
  BAD_CAST "PYST",
  BAD_CAST "PYT",
  BAD_CAST "RET",
  BAD_CAST "ROTT",
  BAD_CAST "SAKT",
  BAD_CAST "SAMT",
  BAD_CAST "SAST",
  BAD_CAST "SBT",
  BAD_CAST "SCT",
  BAD_CAST "SGT",
  BAD_CAST "SLST",
  BAD_CAST "SRET",
  BAD_CAST "SRT",
  BAD_CAST "SST",
  BAD_CAST "SST",
  BAD_CAST "SYOT",
  BAD_CAST "TAHT",
  BAD_CAST "THA",
  BAD_CAST "TFT",
  BAD_CAST "TJT",
  BAD_CAST "TKT",
  BAD_CAST "TLT",
  BAD_CAST "TMT",
  BAD_CAST "TOT",
  BAD_CAST "TVT",
  BAD_CAST "UCT",
  BAD_CAST "ULAT",
  BAD_CAST "USZ1",
  BAD_CAST "UYST",
  BAD_CAST "UYT",
  BAD_CAST "UZT",
  BAD_CAST "VET",
  BAD_CAST "VLAT",
  BAD_CAST "VOLT",
  BAD_CAST "VOST",
  BAD_CAST "VUT",
  BAD_CAST "WAKT",
  BAD_CAST "WAST",
  BAD_CAST "WAT",
  BAD_CAST "WEDT",
  BAD_CAST "WEST",
  BAD_CAST "WET",
  BAD_CAST "WIT",
  BAD_CAST "WST",
  BAD_CAST "YAKT",
  BAD_CAST "YEKT",
  BAD_CAST "Z",
  NULL
};

const xmlChar* ppucTZName[] = {
  BAD_CAST "Coordinated Universal Time",
  BAD_CAST "Australian Central Daylight Savings Time",
  BAD_CAST "Australian Central Standard Time",
  BAD_CAST "ASEAN Common Time",
  BAD_CAST "Atlantic Daylight Time",
  BAD_CAST "Australian Eastern Daylight Savings Time",
  BAD_CAST "Australian Eastern Standard Time",
  BAD_CAST "Afghanistan Time",
  BAD_CAST "Alaska Daylight Time",
  BAD_CAST "Alaska Standard Time",
  BAD_CAST "Amazon Summer Time (Brazil)[1]",
  BAD_CAST "Armenia Summer Time",
  BAD_CAST "Amazon Time (Brazil)[2]",
  BAD_CAST "Armenia Time",
  BAD_CAST "Argentina Time",
  BAD_CAST "Arabia Standard Time",
  BAD_CAST "Atlantic Standard Time",
  BAD_CAST "Australian Western Daylight Time",
  BAD_CAST "Australian Western Standard Time",
  BAD_CAST "Azores Standard Time",
  BAD_CAST "Azerbaijan Time",
  BAD_CAST "Brunei Time",
  BAD_CAST "British Indian Ocean Time",
  BAD_CAST "Baker Island Time",
  BAD_CAST "Bolivia Time",
  BAD_CAST "Brasilia Time",
  BAD_CAST "Bangladesh Standard Time",
  BAD_CAST "British Summer Time (British Standard Time from Feb 1968 to Oct 1971)",
  BAD_CAST "Bhutan Time",
  BAD_CAST "Central Africa Time",
  BAD_CAST "Cocos Islands Time",
  BAD_CAST "Central Daylight Time (North America)",
  BAD_CAST "Cuba Daylight Time[3]",
  BAD_CAST "Central European Daylight Time",
  BAD_CAST "Central European Summer Time (Cf. HAEC)",
  BAD_CAST "Central European Time",
  BAD_CAST "Chatham Daylight Time",
  BAD_CAST "Chatham Standard Time",
  BAD_CAST "Choibalsan",
  BAD_CAST "Chamorro Standard Time",
  BAD_CAST "Chuuk Time",
  BAD_CAST "Clipperton Island Standard Time",
  BAD_CAST "Central Indonesia Time",
  BAD_CAST "Cook Island Time",
  BAD_CAST "Chile Summer Time",
  BAD_CAST "Chile Standard Time",
  BAD_CAST "Colombia Summer Time",
  BAD_CAST "Colombia Time",
  BAD_CAST "Central Standard Time (North America)",
  BAD_CAST "China Standard Time",
  BAD_CAST "Central Standard Time (Australia)",
  BAD_CAST "Central Summer Time (Australia)",
  BAD_CAST "Cuba Standard Time",
  BAD_CAST "China time",
  BAD_CAST "Cape Verde Time",
  BAD_CAST "Central Western Standard Time (Australia) unofficial",
  BAD_CAST "Christmas Island Time",
  BAD_CAST "Davis Time",
  BAD_CAST "Dumont d'Urville Time",
  BAD_CAST "AIX specific equivalent of Central European Time[4]",
  BAD_CAST "Easter Island Standard Summer Time",
  BAD_CAST "Easter Island Standard Time",
  BAD_CAST "East Africa Time",
  BAD_CAST "Eastern Caribbean Time (does not recognise DST)",
  BAD_CAST "Ecuador Time",
  BAD_CAST "Eastern Daylight Time (North America)",
  BAD_CAST "Eastern European Daylight Time",
  BAD_CAST "Eastern European Summer Time",
  BAD_CAST "Eastern European Time",
  BAD_CAST "Eastern Greenland Summer Time",
  BAD_CAST "Eastern Greenland Time",
  BAD_CAST "Eastern Indonesian Time",
  BAD_CAST "Eastern Standard Time (North America)",
  BAD_CAST "Eastern Standard Time (Australia)",
  BAD_CAST "Further-eastern European Time",
  BAD_CAST "Fiji Time",
  BAD_CAST "Falkland Islands Standard Time",
  BAD_CAST "Falkland Islands Summer Time",
  BAD_CAST "Falkland Islands Time",
  BAD_CAST "Fernando de Noronha Time",
  BAD_CAST "Galapagos Time",
  BAD_CAST "Gambier Islands",
  BAD_CAST "Georgia Standard Time",
  BAD_CAST "French Guiana Time",
  BAD_CAST "Gilbert Island Time",
  BAD_CAST "Gambier Island Time",
  BAD_CAST "Greenwich Mean Time",
  BAD_CAST "South Georgia and the South Sandwich Islands",
  BAD_CAST "Gulf Standard Time",
  BAD_CAST "Guyana Time",
  BAD_CAST "Hawaii-Aleutian Daylight Time",
  BAD_CAST "Heure Avancée d'Europe Centrale francised name for CEST",
  BAD_CAST "Hawaii-Aleutian Standard Time",
  BAD_CAST "Hong Kong Time",
  BAD_CAST "Heard and McDonald Islands Time",
  BAD_CAST "Khovd Time",
  BAD_CAST "Hawaii Standard Time",
  BAD_CAST "Indochina Time",
  BAD_CAST "Israel Daylight Time",
  BAD_CAST "Indian Ocean Time",
  BAD_CAST "Iran Daylight Time",
  BAD_CAST "Irkutsk Time",
  BAD_CAST "Iran Standard Time",
  BAD_CAST "Indian Standard Time",
  BAD_CAST "Irish Standard Time[5]",
  BAD_CAST "Israel Standard Time",
  BAD_CAST "Japan Standard Time",
  BAD_CAST "Kyrgyzstan time",
  BAD_CAST "Kosrae Time",
  BAD_CAST "Krasnoyarsk Time",
  BAD_CAST "Korea Standard Time",
  BAD_CAST "Lord Howe Standard Time",
  BAD_CAST "Lord Howe Summer Time",
  BAD_CAST "Line Islands Time",
  BAD_CAST "Magadan Time",
  BAD_CAST "Marquesas Islands Time",
  BAD_CAST "Mawson Station Time",
  BAD_CAST "Mountain Daylight Time (North America)",
  BAD_CAST "Middle European Time Same zone as CET",
  BAD_CAST "Middle European Saving Time Same zone as CEST",
  BAD_CAST "Marshall Islands",
  BAD_CAST "Macquarie Island Station Time",
  BAD_CAST "Marquesas Islands Time",
  BAD_CAST "Myanmar Time",
  BAD_CAST "Moscow Time",
  BAD_CAST "Malaysia Standard Time",
  BAD_CAST "Mountain Standard Time (North America)",
  BAD_CAST "Myanmar Standard Time",
  BAD_CAST "Mauritius Time",
  BAD_CAST "Maldives Time",
  BAD_CAST "Malaysia Time",
  BAD_CAST "New Caledonia Time",
  BAD_CAST "Newfoundland Daylight Time",
  BAD_CAST "Norfolk Time",
  BAD_CAST "Nepal Time",
  BAD_CAST "Newfoundland Standard Time",
  BAD_CAST "Newfoundland Time",
  BAD_CAST "Niue Time",
  BAD_CAST "New Zealand Daylight Time",
  BAD_CAST "New Zealand Standard Time",
  BAD_CAST "Omsk Time",
  BAD_CAST "Oral Time",
  BAD_CAST "Pacific Daylight Time (North America)",
  BAD_CAST "Peru Time",
  BAD_CAST "Kamchatka Time",
  BAD_CAST "Papua New Guinea Time",
  BAD_CAST "Phoenix Island Time",
  BAD_CAST "Pakistan Standard Time",
  BAD_CAST "Saint Pierre and Miquelon Daylight time",
  BAD_CAST "Saint Pierre and Miquelon Standard Time",
  BAD_CAST "Pohnpei Standard Time",
  BAD_CAST "Pacific Standard Time (North America)",
  BAD_CAST "Philippine Standard Time",
  BAD_CAST "Paraguay Summer Time (South America)[6]",
  BAD_CAST "Paraguay Time (South America)[7]",
  BAD_CAST "Réunion Time",
  BAD_CAST "Rothera Research Station Time",
  BAD_CAST "Sakhalin Island time",
  BAD_CAST "Samara Time",
  BAD_CAST "South African Standard Time",
  BAD_CAST "Solomon Islands Time",
  BAD_CAST "Seychelles Time",
  BAD_CAST "Singapore Time",
  BAD_CAST "Sri Lanka Time",
  BAD_CAST "Srednekolymsk Time",
  BAD_CAST "Suriname Time",
  BAD_CAST "Samoa Standard Time",
  BAD_CAST "Singapore Standard Time",
  BAD_CAST "Showa Station Time",
  BAD_CAST "Tahiti Time",
  BAD_CAST "Thailand Standard Time",
  BAD_CAST "Indian/Kerguelen",
  BAD_CAST "Tajikistan Time",
  BAD_CAST "Tokelau Time",
  BAD_CAST "Timor Leste Time",
  BAD_CAST "Turkmenistan Time",
  BAD_CAST "Tonga Time",
  BAD_CAST "Tuvalu Time",
  BAD_CAST "Coordinated Universal Time",
  BAD_CAST "Ulaanbaatar Time",
  BAD_CAST "Kaliningrad Time",
  BAD_CAST "Uruguay Summer Time",
  BAD_CAST "Uruguay Standard Time",
  BAD_CAST "Uzbekistan Time",
  BAD_CAST "Venezuelan Standard Time",
  BAD_CAST "Vladivostok Time",
  BAD_CAST "Volgograd Time",
  BAD_CAST "Vostok Station Time",
  BAD_CAST "Vanuatu Time",
  BAD_CAST "Wake Island Time",
  BAD_CAST "West Africa Summer Time",
  BAD_CAST "West Africa Time",
  BAD_CAST "Western European Daylight Time",
  BAD_CAST "Western European Summer Time",
  BAD_CAST "Western European Time",
  BAD_CAST "Western Indonesian Time",
  BAD_CAST "Western Standard Time",
  BAD_CAST "Yakutsk Time",
  BAD_CAST "Yekaterinburg Time",
  BAD_CAST "Zulu Time (Coordinated Universal Time)",
  NULL
};

const float fTZOffset[] = {
  0.0f,	// UTC
  10.5f,	// ACDT
  9.5f,	// ACST
  8.0f,	// ACT
  -3.0f,	// ADT
  11.0f,	// AEDT
  10.0f,	// AEST
  4.5f,	// AFT
  -8.0f,	// AKDT
  -9.0f,	// AKST
  -3.0f,	// AMST
  5.0f,	// AMST
  -4.0f,	// AMT
  4.0f,	// AMT
  -3.0f,	// ART
  3.0f,	// AST
  -4.0f,	// AST
  9.0f,	// AWDT
  8.0f,	// AWST
  -1.0f,	// AZOST
  4.0f,	// AZT
  8.0f,	// BDT
  6.0f,	// BIOT
  -12.0f,	// BIT
  -4.0f,	// BOT
  -3.0f,	// BRT
  6.0f,	// BST
  1.0f,	// BST
  6.0f,	// BTT
  2.0f,	// CAT
  6.5f,	// CCT
  -5.0f,	// CDT
  -4.0f,	// CDT
  2.0f,	// CEDT
  2.0f,	// CEST
  1.0f,	// CET
  13.75f,	// CHADT
  12.75f,	// CHAST
  8.0f,	// CHOT
  10.0f,	// ChST
  10.0f,	// CHUT
  -8.0f,	// CIST
  8.0f,	// CIT
  -10.0f,	// CKT
  -3.0f,	// CLST
  -4.0f,	// CLT
  -4.0f,	// COST
  -5.0f,	// COT
  -6.0f,	// CST
  8.0f,	// CST
  9.5f,	// CST
  10.5f,	// CST
  -5.0f,	// CST
  8.0f,	// CT
  -1.0f,	// CVT
  8.75f,	// CWST
  7.0f,	// CXT
  7.0f,	// DAVT
  10.0f,	// DDUT
  1.0f,	// DFT
  -5.0f,	// EASST
  -6.0f,	// EAST
  3.0f,	// EAT
  -4.0f,	// ECT
  -5.0f,	// ECT
  -4.0f,	// EDT
  3.0f,	// EEDT
  3.0f,	// EEST
  2.0f,	// EET
  0.0f,	// EGST
  -1.0f,	// EGT
  9.0f,	// EIT
  -5.0f,	// EST
  10.0f,	// EST
  3.0f,	// FET
  12.0f,	// FJT
  -3.0f,	// FKST
  -3.0f,	// FKST
  -4.0f,	// FKT
  -2.0f,	// FNT
  -6.0f,	// GALT
  -9.0f,	// GAMT
  4.0f,	// GET
  -3.0f,	// GFT
  12.0f,	// GILT
  -9.0f,	// GIT
  0.0f,	// GMT
  -2.0f,	// GST
  4.0f,	// GST
  -4.0f,	// GYT
  -9.0f,	// HADT
  2.0f,	// HAEC
  -10.0f,	// HAST
  8.0f,	// HKT
  5.0f,	// HMT
  7.0f,	// HOVT
  -10.0f,	// HST
  7.0f,	// ICT
  3.0f,	// IDT
  3.0f,	// IOT
  4.5f,	// IRDT
  8.0f,	// IRKT
  3.5f,	// IRST
  5.5f,	// IST
  1.0f,	// IST
  2.0f,	// IST
  9.0f,	// JST
  6.0f,	// KGT
  11.0f,	// KOST
  7.0f,	// KRAT
  9.0f,	// KST
  10.5f,	// LHST
  11.0f,	// LHST
  14.0f,	// LINT
  12.0f,	// MAGT
  -9.5f,	// MART
  5.0f,	// MAWT
  -6.0f,	// MDT
  1.0f,	// MET
  2.0f,	// MEST
  12.0f,	// MHT
  11.0f,	// MIST
  -9.5f,	// MIT
  6.5f,	// MMT
  3.0f,	// MSK
  8.0f,	// MST
  -7.0f,	// MST
  6.5f,	// MST
  4.0f,	// MUT
  5.0f,	// MVT
  8.0f,	// MYT
  11.0f,	// NCT
  -2.5f,	// NDT
  11.5f,	// NFT
  5.75f,	// NPT
  -3.5f,	// NST
  -3.5f,	// NT
  -11.0f,	// NUT
  13.0f,	// NZDT
  12.0f,	// NZST
  6.0f,	// OMST
  5.0f,	// ORAT
  -7.0f,	// PDT
  -5.0f,	// PET
  12.0f,	// PETT
  10.0f,	// PGT
  13.0f,	// PHOT
  5.0f,	// PKT
  -2.0f,	// PMDT
  -3.0f,	// PMST
  11.0f,	// PONT
  -8.0f,	// PST
  8.0f,	// PST
  -3.0f,	// PYST
  -4.0f,	// PYT
  4.0f,	// RET
  -3.0f,	// ROTT
  11.0f,	// SAKT
  4.0f,	// SAMT
  2.0f,	// SAST
  11.0f,	// SBT
  4.0f,	// SCT
  8.0f,	// SGT
  5.5f,	// SLST
  11.0f,	// SRET
  -3.0f,	// SRT
  -11.0f,	// SST
  8.0f,	// SST
  3.0f,	// SYOT
  -10.0f,	// TAHT
  7.0f,	// THA
  5.0f,	// TFT
  5.0f,	// TJT
  13.0f,	// TKT
  9.0f,	// TLT
  5.0f,	// TMT
  13.0f,	// TOT
  12.0f,	// TVT
  0.0f,	// UCT
  8.0f,	// ULAT
  2.0f,	// USZ1
  -2.0f,	// UYST
  -3.0f,	// UYT
  5.0f,	// UZT
  -4.5f,	// VET
  10.0f,	// VLAT
  4.0f,	// VOLT
  6.0f,	// VOST
  11.0f,	// VUT
  12.0f,	// WAKT
  2.0f,	// WAST
  1.0f,	// WAT
  1.0f,	// WEDT
  1.0f,	// WEST
  0.0f,	// WET
  7.0f,	// WIT
  8.0f,	// WST
  9.0f,	// YAKT
  5.0f,	// YEKT
  0.0f,	// Z
  -25.0f	// END
};


/*!
\param
\return
*/
int
tzGetRuntimeOffsetToUTC(void)
{
  return 0;
}
/* end of tzGetRuntimeOffsetToUTC() */


/*!
\param
\return
*/
int
tzGetCount(void) 
{
  return PIE_TZ_END;
}
/* end of tzGetCount() */


/*!
\param
\return 
*/
int
tzGetNumber(const xmlChar *pucArg)
{
  int i;

  for (i = 0; i < PIE_TZ_END; i++) {
    if (xmlStrstr(pucArg, ppucTZId[i]) == pucArg || xmlStrcasecmp(ppucTZName[i], pucArg) == 0) {
      break;
    }
  }
  return i;
}
/* end of tzGetNumber() */


/*!
\param
\return
*/
const xmlChar*
tzGetId(int iArg) 
{
  assert(iArg > -1);
  assert(iArg < PIE_TZ_END);

  return ppucTZId[iArg];
}
/* end of tzGetId() */


/*!
\param
\return
*/
const xmlChar*
tzGetName(int iArg) 
{
  assert(iArg > -1);
  assert(iArg < PIE_TZ_END);

  return ppucTZName[iArg];
}
/* end of tzGetName() */


/*!
\param
\return
*/
const int
tzGetOffset(int iArg) 
{
  assert(iArg > -1);
  assert(iArg < PIE_TZ_END);

  return (int) (fTZOffset[iArg] * 60.0f);
}
/* end of tzGetOffset() */


#ifdef TESTCODE
#include "test/test_cxp_timezone.c"
#endif
