<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="xml"/>
  <!-- transform a pie/dir into an accelaration index file of all xsl files -->
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="@*|node()">
    <xsl:choose>
      <!-- in valids -->
      <xsl:when test="self::meta"/>
      <xsl:when test="self::dir[@name='.svn']"/>
      <xsl:when test="self::dir[starts-with(@name,'tmp')]"/>
      <xsl:when test="self::dir[not(descendant::file)]"/>
      <xsl:when test="self::dir[parent::pie]">
        <!-- root directory, set @name to '.' -->
        <xsl:element name="{name()}">
          <xsl:attribute name="name">
            <xsl:value-of select="'.'"/>
          </xsl:attribute>
          <xsl:apply-templates select="@mtime|node()"/>
        </xsl:element>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:apply-templates select="@name|@mtime|node()"/>
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
