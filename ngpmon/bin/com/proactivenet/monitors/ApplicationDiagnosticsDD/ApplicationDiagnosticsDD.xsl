<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE html [<!ENTITY nbsp "&#160;">] >
<xsl:stylesheet xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>
    <xsl:output method="html" encoding="iso-8859-1"/>
    <xsl:template match="/">
        <!--      <LINK REL="StyleSheet" HREF="/scripts/style.css" TYPE="text/css" MEDIA="screen" />-->
        <html>
            <head>
                <meta http-equiv="Expires" content="now"/>
                <meta http-equiv="Cache-Control" content="no-cache,must-revalidate"/>
                <meta http-equiv="Pragma" content="no-cache"/>
                <basefont class="normaltext"/>
            </head>

            <body>
                <table class="section-header" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                        <td class="caption">
                            <xsl:value-of select="detaildiagnostics/data/title"/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=AppSightSummary" target="_new">
                                <img src="/ART/help.gif" alt="Help" border="0"/>
                            </a>
                        </td>
                    </tr>
                </table>
                <xsl:choose>
                    <xsl:when test="detaildiagnostics/data/error">
                        <p><h3>Application Diagnostics encountered an error</h3></p>
                        <p><xsl:value-of select="detaildiagnostics/data/error"/></p>
                    </xsl:when>
                    <xsl:otherwise>
                        <iframe width="100%" scrolling="auto" frameborder="0" height="410">
                            <xsl:attribute name="src">
                                <xsl:value-of select="detaildiagnostics/data/url"/>
                            </xsl:attribute>
                        </iframe>
                    </xsl:otherwise>
                </xsl:choose>

                <xsl:for-each select="detaildiagnostics/data/elem">
					<xsl:comment>
						<xsl:value-of select="@name"/>=<xsl:value-of select="."/>
					</xsl:comment>
				</xsl:for-each>

				<xsl:for-each select="detaildiagnostics/data/debug/elem">
					<xsl:comment>
							<xsl:value-of select="@name"/>=<xsl:value-of select="."/>
					</xsl:comment>
				</xsl:for-each>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>