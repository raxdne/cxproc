
var domResult = <html/>;

domResult.appendChild(<body/>);
domResult.body.appendChild(<h1/>);
domResult.body.appendChild(<ol/>);
for (i=0; i<100; i++) {
    domResult.body.ol.appendChild(<li/>);
    domResult.body.ol.li[i] = "Q" + (i * i);
}
domResult.body.h1 = "Header";

domResult.toXMLString();
