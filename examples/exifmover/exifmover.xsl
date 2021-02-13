<?xml version="1.0"?>
<xsl:stylesheet xmlns:cxp="http://www.tenbusch.info/cxproc" xmlns:exif="http://www.w3.org/2003/12/exif/ns" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <!-- -->
  <xsl:variable name="str_dir" select="'.'"/>
  <xsl:variable name="str_dir_target" select="'.'"/>
  <xsl:output method="xml"/>
  <xsl:template match="/">
    <xsl:element name="cxp:make">
      <xsl:apply-templates select="//file[starts-with(@type,'image/')]"/>
    </xsl:element>
  </xsl:template>
  <xsl:template match="file[@size &gt; 0]">
    <xsl:variable name="str_name" select="@name"/>
    <xsl:variable name="str_destination">
      <xsl:choose>
        <xsl:when test="exif/date">
          <!-- there are EXIF information -->
          <xsl:value-of select="concat($str_dir_target,'/',exif/model,'/',translate(substring-before(exif/date,' '),':','/'),'/')"/>
        </xsl:when>
        <xsl:when test="image/*[contains(name(),'DateTime')][1]/@value">
          <!-- there are EXIF information -->
          <xsl:value-of select="concat($str_dir_target,'/',translate(substring-before(image/*[contains(name(),'DateTime')][1]/@value,' '),':','/'),'/')"/>
        </xsl:when>
        <xsl:otherwise>
          <!-- use file modification time by default -->
          <xsl:value-of select="concat($str_dir_target,'/',translate(substring-before(@mtime2,'T'),'-','/'),'/')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:element name="cxp:system">
      <xsl:attribute name="message">
        <xsl:value-of select="concat('Move ','&quot;',$str_name,'&quot;')"/>
      </xsl:attribute>
    </xsl:element>
    <xsl:element name="cxp:copy">
      <xsl:attribute name="from">
        <xsl:value-of select="concat($str_dir,'/',$str_name)"/>
      </xsl:attribute>
      <xsl:attribute name="to">
        <xsl:value-of select="$str_destination"/>
      </xsl:attribute>
      <xsl:attribute name="delete">yes</xsl:attribute>
    </xsl:element>
    <!-- move exiting comment file too -->
    <xsl:if test="parent::dir/child::file[@name = concat(translate($str_name,'.','_'),'.txt')]">
      <xsl:element name="cxp:copy">
	<xsl:attribute name="from">
          <xsl:value-of select="concat($str_dir,'/',translate($str_name,'.','_'),'.txt')"/>
	</xsl:attribute>
	<xsl:attribute name="to">
          <xsl:value-of select="$str_destination"/>
	</xsl:attribute>
	<xsl:attribute name="delete">yes</xsl:attribute>
      </xsl:element>
    </xsl:if>
  </xsl:template>
  <xsl:template match="*"/>
</xsl:stylesheet>
