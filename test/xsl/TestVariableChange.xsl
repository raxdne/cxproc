<?xml version="1.0"?>
<xsl:stylesheet xmlns:cxp="http://www.tenbusch.info/cxproc" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:variable name="pattern" select="''"/>
  
  <xsl:variable name="str_path_1" select="'/abc.txt'"/>
  
  <xsl:output method="html" omit-xml-declaration="yes" doctype-public="-//W3C//DTD HTML 4.01//EN" doctype-system="file:///tmp/dummy.dtd"/>

  <xsl:variable name="str_path_2">def.txt</xsl:variable>
  
  <xsl:variable name="str_path_3"> <!-- comment interruptus --> <xsl:text>hij.txt</xsl:text></xsl:variable>

  <!-- no break by comment -->
  <xsl:variable name="int_a" select="123"/>

  <?no-break by PI ?>
  
  <xsl:variable name="flag" select="false()"/>
  
  <xsl:template match="/">
    <xsl:element name="html">
      <xsl:element name="head">
      </xsl:element>
      <xsl:element name="body">
        <xsl:element name="p">
          <xsl:value-of select="concat('str_path_1: ',$str_path_1)"/>
        </xsl:element>
        <xsl:element name="p">
          <xsl:value-of select="concat('str_path_2: ',$str_path_2)"/>
        </xsl:element>
        <xsl:element name="p">
          <xsl:value-of select="concat('str_path_3: ',$str_path_3)"/>
        </xsl:element>
      </xsl:element>
    </xsl:element>
  </xsl:template>
</xsl:stylesheet>
