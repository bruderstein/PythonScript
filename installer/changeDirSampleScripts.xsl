<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://schemas.microsoft.com/wix/2006/wi" xmlns:wix="http://schemas.microsoft.com/wix/2006/wi" version="1.0">

	<xsl:output method="xml" indent="yes" />
	<xsl:param name="realDirectoryId" select="'D_SampleScripts'" />
	<xsl:include href="fixDirectoryAndFileRefs.xsl" />

</xsl:stylesheet>