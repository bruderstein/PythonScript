<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://wixtoolset.org/schemas/v4/wxs" xmlns:wix="http://wixtoolset.org/schemas/v4/wxs" version="1.0">
	<xsl:output method="xml" indent="yes" />
	<xsl:template match="wix:Wix">
		<Wix>
			<xsl:apply-templates select="wix:Fragment[position() != 1]" />
		</Wix>
	</xsl:template>
	
	<xsl:template match="wix:Component[@Directory=/wix:Wix/wix:Fragment/wix:DirectoryRef[@Id='D_PythonScript']/wix:Directory/@Id]">
		<Component>
			<xsl:attribute name="Directory"><xsl:value-of select="$realDirectoryId" /></xsl:attribute>
			<xsl:copy-of select="@Id" />
			<xsl:copy-of select="@Guid" />
			<xsl:apply-templates />
		</Component>
	</xsl:template>
	
	<xsl:template match="wix:DirectoryRef[@Id=/wix:Wix/wix:Fragment/wix:DirectoryRef[@Id='D_PythonScript']/wix:Directory/@Id]">
		<DirectoryRef>
			<xsl:attribute name="Id"><xsl:value-of select="$realDirectoryId" /></xsl:attribute>
			<xsl:apply-templates />
		</DirectoryRef>
	</xsl:template>

	<xsl:template match="wix:File">
		<xsl:variable name="rawFileName">
			<xsl:call-template name="extractFileName">
					<xsl:with-param name="path" select="@Source" />
			</xsl:call-template> 
		</xsl:variable>
		<RemoveFile Id="Remove_{@Id}" On="install">
			<xsl:attribute name="Name">
				<xsl:value-of select="$rawFileName" /> 
			</xsl:attribute>
		</RemoveFile>

		<xsl:if test="substring-after($rawFileName, '.') = 'py'">
			<RemoveFile Id="RemovePyc_{@Id}" On="install">
				<xsl:attribute name="Name">
					<xsl:value-of select="$rawFileName" /> 
					<xsl:text>c</xsl:text>
				</xsl:attribute>
				
			</RemoveFile>
		</xsl:if>
		<File>
			<xsl:copy-of select="./@*" />
			<xsl:copy-of select="./*" />
		</File>
	</xsl:template>
	

	<!-- Recursively strips the paths away until only the filename remains, and gives that back -->
	<xsl:template name="extractFileName">
		<xsl:param name="path" />
		<xsl:variable name="remainingPath">
			<xsl:value-of select="substring-after($path, '\')" />
		</xsl:variable>

		<xsl:choose>
			<xsl:when test="$remainingPath = ''">
				<xsl:value-of select="$path" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="extractFileName">
					<xsl:with-param name="path" select="$remainingPath" />
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>
	
	<xsl:template match="*" >
		<xsl:copy>
			<xsl:copy-of select="./@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>