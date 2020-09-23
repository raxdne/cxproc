//
//
// s. https://developer.mozilla.org/en/E4X_Tutorial
// http://forums.devx.com/showthread.php?t=154826

var strVcf = "BEGIN:VCARD\nVERSION:2.1\nN:;Fritz;Eierschale\nFN:Fritz Eierschale\nEMAIL;PREF;INTERNET:fritz.eierschale@example.org\nEMAIL;INTERNET:eierschale@example.org\nORG;ENCODING=QUOTED-PRINTABLE:H=FChnerkacke GmbH\nTITLE;ENCODING=QUOTED-PRINTABLE:Gesch=E4ftsleiter\nADR;HOME;ENCODING=QUOTED-PRINTABLE:;;Eierschalensollbruchsteg 12;Irgen=\n dwodadrau=DFen;;1010101\nLABEL;HOME;ENCODING=QUOTED-PRINTABLE:Eierschalensollbruchsteg 12=0D=0A=\n Irgendwodadrau=DFen=0D=0A1010101\nX-GENDER:Male\nREV:20010816T131855Z\nEND:VCARD";

var arrVcf = strVcf.split("\n");

var domPie = <pie/>;
domPie.appendChild(<vcard/>);
var elementVcard = domPie.vcard;
elementVcard.appendChild(<name/>);
elementVcard.appendChild(<mail/>);
elementVcard.appendChild(<phone/>);

for (i=0; i<arrVcf.length; i++) {
    var element;
    var arrLine = arrVcf[i].split(":");

    switch (arrLine[0]) {
    case 'BEGIN':
    case 'END':
	break;
    case 'EMAIL;INTERNET':
	elementVcard.mail.work = arrLine[1];
	break;
    case 'EMAIL;PREF;INTERNET':
	elementVcard.mail.private = arrLine[1];
	break;
    case 'N':
	elementVcard.@id = arrLine[1].replace(/[^A-Z0-9]/gi,'');

	elementVcard.name = arrLine[1];
	break;
    case 'X-GENDER':
	elementVcard.gender = arrLine[1];
	break;
    case 'FN':
	//elementVcard.setAttribute("id","fff");
	break;
    default: 
	//print(arrLine[1]);
    }
}

//print(elementVcard.toXMLString());
elementVcard.toXMLString();