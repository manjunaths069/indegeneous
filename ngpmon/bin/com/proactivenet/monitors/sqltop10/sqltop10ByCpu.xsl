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
                            <xsl:value-of select="detaildiagnostics/data/definedconstant/@name/../@value"/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=sqltop10" target="_new">
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
                    <h3>Error displaying data :</h3>
                    <br/>
                    <p>
                        <xsl:value-of select='detaildiagnostics/error'/>
                    </p>
                </xsl:if>

                <table width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                        <td>
                            <table class="data" width="100%" cellspacing="0" cellpadding="0" border="0">
                                <xsl:if test="detaildiagnostics/data/system/attribute">
                                    <thead>
                                    <tr>
                                        <xsl:for-each select="detaildiagnostics/data/system/attribute">
                                            <xsl:choose>
                                                <xsl:when test="position()!=2">
                                                    <td valign="top" colspan="2">
                                                        <table class="data" width="60%" cellspacing="0" cellpadding="0" border="0">
                                                            <tr class="header-column first">
                                                                <th class="header-column first">
                                                                    <xsl:value-of select="@name"/>
                                                                    &nbsp;
                                                                </th>
                                                                <th>&nbsp;
                                                                </th>
                                                            </tr>
                                                            <tr>
                                                                <th>&nbsp;</th>
                                                                <th>&nbsp;</th>
                                                            </tr>
                                                            <xsl:for-each select="state">
                                                                <tr>
                                                                    <td>
                                                                        <xsl:value-of select="@name"/>
                                                                    </td>
                                                                    <td>
                                                                        <xsl:value-of select="."/>
                                                                    </td>
                                                                </tr>
                                                            </xsl:for-each>
                                                            <tr>
                                                                <th>&nbsp;</th>
                                                                <th>&nbsp;</th>
                                                            </tr>
                                                        </table>
                                                    </td>
                                                </xsl:when>
                                            </xsl:choose>
                                        </xsl:for-each>
                                    </tr>
                                    <tr>
                                        <td height="25">&nbsp;</td>
                                    </tr>
                                    </thead>
                                </xsl:if>
                                <!--end <xsl:if test="detaildiagnostics/data/system/attribute" />-->
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td colspan="5">
                            <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
                                <thead>
                                <xsl:for-each select="detaildiagnostics/data/system/sqlquery[position()=1]">
                                    <tr class="header-column first">
                                        <xsl:for-each select="attribute">
                                            <th class="header-column first" style="text-align: center;">
                                                <xsl:value-of select="@name"/>
                                            </th>
                                        </xsl:for-each>
                                    </tr>
                                </xsl:for-each>
                                </thead>
                                <tbody>
                                <xsl:for-each select="detaildiagnostics/data/system/sqlquery">
                                    <tr class="data even">
                                        <xsl:for-each select="attribute">
                                            <xsl:choose>
                                                <xsl:when test="@name='SQL Statement'">
                                                    <td class="data even" style="text-align: left;">
                                                        <xsl:value-of select="."/>
                                                    </td>
                                                </xsl:when>
                                                <xsl:otherwise>
                                                    <td align="center" class="data even">
                                                        <xsl:value-of select="."/>
                                                    </td>
                                                </xsl:otherwise>
                                            </xsl:choose>
                                        </xsl:for-each>
                                    </tr>
                                </xsl:for-each>
                                </tbody>
                            </table>
                        </td>
                    </tr>
                    <!-- Following code is added by Parag to insert table above topN output //-->
                    <tr>
                        <td height="25">&nbsp;</td>
                    </tr>
                    <tr>
                        <td colspan="6">
                            <table width="100%">
                                <tr class="mediumpurple">
                                    <td>
                                        <font class="tableheader">
                                            Currently Running SQL Queries
                                        </font>
                                    </td>
                                </tr>
                                <xsl:for-each select="detaildiagnostics/data/system/attribute/value">
                                    <tr>
                                        <td>
                                            <xsl:if test="../@name='Running-Query'">
                                                <xsl:choose>
                                                    <xsl:when test="position() mod 2 = 1">
                                                        <tr valign="top" class="lightgray" height="18">
                                                            <td align="left" valign="top" class="tablebody">
                                                                <xsl:value-of select="."/>
                                                            </td>
                                                        </tr>
                                                    </xsl:when>
                                                    <xsl:otherwise>
                                                        <tr valign="top" class="lightyellow" height="18">
                                                            <td align="left" valign="top" class="tablebody">
                                                                <xsl:value-of select="."/>
                                                            </td>
                                                        </tr>
                                                    </xsl:otherwise>
                                                </xsl:choose>
                                            </xsl:if>
                                        </td>
                                    </tr>
                                </xsl:for-each>
                            </table>
                        </td>
                    </tr>
                    <tr>
                        <td height="25">&nbsp;</td>
                    </tr>
                    <!-- END of code addition //-->
                </table>
                <p>&nbsp;</p>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
