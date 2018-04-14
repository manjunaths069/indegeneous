<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- Edited with XML Spy v4.2 -->
<!--<!DOCTYPE html
     PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">-->
<!DOCTYPE html [<!ENTITY nbsp "&#160;">] >
<xsl:stylesheet
    xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>
    <xsl:output method="html" encoding="iso-8859-1"/>
    <xsl:template match="/">
        <!--  <LINK REL="StyleSheet" HREF="/scripts/style.css" TYPE="text/css" MEDIA="screen" />-->
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
                            <xsl:value-of select="detaildiagnostics/data/usercommand/definedconstant/@name/../@value"/>
                            <xsl:value-of select="detaildiagnostics/data/usercommand/definedconstant"/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=LogFileSearch" target="_new">
                                <img src="/ART/help.gif" alt="Help" border="0"/>
                            </a>
                        </td>
                    </tr>
                </table>
                <br/>
                <table cellspacing="0" border="0" cellpadding="0">
                    <tr>
                        <td>&nbsp;Server Time&nbsp;&nbsp;&nbsp;</td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/servertime'/>
                        </td>
                        <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Agent Time&nbsp;&nbsp;&nbsp;</td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/timecreated'/>
                        </td>
                    </tr>
                </table>
                <br/>

                <xsl:if test='detaildiagnostics/error'>
                    <table width="100%" cellspacing="0" cellpadding="0">
                        <tr>
                            <td width="50%" colspan="2">
                                &nbsp;Error
                            </td>
                        </tr>
                        <tr>
                            <td>
                                <p>&nbsp;
                                    <pre>
                                        <font color='blue'>
                                            <xsl:value-of select='detaildiagnostics/error'/>
                                        </font>
                                    </pre>
                                </p>
                            </td>
                        </tr>
                    </table>

                </xsl:if>
                <table class="section-header" width="100%" cellspacing="0" cellpadding="0">
                    <tr>
                        <td class="caption">
                            &nbsp;
                            <a name="top">
                                <b>File Names</b>
                            </a>
                        </td>
                    </tr>
                </table>
                <ul>
                    <xsl:for-each select="detaildiagnostics/data/usercommand/filestr">
                        <xsl:for-each select="attribute">
                            <li>
                                <a href="#{@name}">
                                    <xsl:value-of select="@name"/>
                                </a>
                            </li>
                        </xsl:for-each>
                    </xsl:for-each>
                </ul>

                <xsl:for-each select="detaildiagnostics/data/usercommand/filestr">
                    <xsl:for-each select="attribute">
                        <table class="section-header" width="100%" cellspacing="0" cellpadding="0">
                            <tr>
                                <td class="caption">
                                    File :
                                        <a name="{@name}">
                                            <xsl:value-of select="@name"/>
                                        </a>
                                </td>
                            </tr>
                        </table>
                        <pre>
                            <br/>
                            <xsl:value-of select="."/>
                            &nbsp;
                        </pre>
                        <a href="#top">Top</a>
                    </xsl:for-each>
                </xsl:for-each>
                <br/>
                <br/>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
