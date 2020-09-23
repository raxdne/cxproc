<?xml version="1.0"?>
<xsl:stylesheet xmlns:cxp="http://www.tenbusch.info/cxproc" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" omit-xml-declaration="yes" doctype-public="-//W3C//DTD HTML 4.01//EN" doctype-system="file:///tmp/dummy.dtd"/>
  <xsl:variable name="test" select="''"/>
  <xsl:variable name="time" select="'00:00:00'"/>
  <xsl:variable name="host" select="'XYZ'"/>
  <xsl:template match="/">
    <xsl:element name="html">
      <xsl:element name="head">
      </xsl:element>
      <xsl:element name="body">
        <xsl:element name="h1">
          <xsl:value-of select="concat('HELLO ',$test,'!')"/>
        </xsl:element>
        <xsl:element name="p">
          <xsl:value-of select="concat('Its ',$time,' on ',$host)"/>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>
