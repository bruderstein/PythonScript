<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://wixtoolset.org/schemas/v4/wxs" xmlns:wix="http://wixtoolset.org/schemas/v4/wxs" version="1.0">

	<xsl:output method="xml" indent="yes" />
	<xsl:param name="realDirectoryId" select="'D_SampleScripts'" />
	<xsl:include href="fixDirectoryAndFileRefs.xsl" />

</xsl:stylesheet>