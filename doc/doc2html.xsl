<?xml version="1.0"?>
<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

	<xsl:output method="html" indent="yes" encoding="UTF-8"/>

	<xsl:template match="*">
		<xsl:copy-of select="."/>
    </xsl:template>

	<xsl:template match="chapter">
		<section class="chapter">
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
				<style>
				img{max-width:100%}
				body{font-family:"Andika", "Corbel", sans-serif}
				</style>
			</head>
			<body>
				<h1><xsl:value-of select="@title" /></h1>
				<xsl:apply-templates select="chapter"/>
			</body>
		</html>
	</xsl:template>

</xsl:stylesheet>