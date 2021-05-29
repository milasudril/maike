<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="html" indent="yes" encoding="UTF-8"/>

<xsl:template match="@*|node()">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()" />
	</xsl:copy>
</xsl:template>

<xsl:template match="figure/caption">
	<figcaption>
		<xsl:apply-templates select="@*|node()" />
	</figcaption>
</xsl:template>

<xsl:template match="chapter">
	<section class="chapter">
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
		<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="section">
	<section class="section">
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
		<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="subsection">
	<section class="subsection">
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
		<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="subsubsection">
	<section class="subsubsection">
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
		<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="paragraph">
	<section class="paragraph">
		<xsl:attribute name="id"><xsl:value-of select="@id" /></xsl:attribute>
		<h1><xsl:value-of select="@title" /></h1>
		<xsl:apply-templates select="./*"/>
	</section>
</xsl:template>

<xsl:template match="/document">
<xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html&gt;
</xsl:text>
<html>
<head>
<title>
<xsl:value-of select="@title" />
</title>
<link rel="stylesheet" href="format.css" />
</head>
<body>
<h1><xsl:value-of select="@title" /></h1>
<xsl:apply-templates select="chapter"/>
</body>
</html>
</xsl:template>

</xsl:stylesheet>