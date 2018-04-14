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
                            <!-- xsl:value-of select="detaildiagnostics/data/definedconstant/@name/../@value"/-->
                            <xsl:value-of select="detaildiagnostics/data/definedconstant"/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=TMARTDDpage" target="_new">
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

                <xsl:if test="detaildiagnostics/data/system/attribute">
                    <table class="data" cellspacing="0" border="0" cellpadding="0">
                        <tr>
                            <xsl:for-each select="detaildiagnostics/data/system/attribute">
                                <xsl:choose>
                                    <xsl:when test="position()!=1">
                                        <th valign="top">
                                            <table width="100%" cellspacing="0" cellpadding="0" border="0">
                                                <tr class="header-column first">
                                                    <th class="header-column first">
                                                        <xsl:value-of select="@name"/>
                                                    </th>
                                                    <th>
                                                        &nbsp;
                                                    </th>
                                                </tr>
                                                <tr>
                                                    <th>&nbsp;</th>
                                                </tr>
                                                <xsl:for-each select="state">
                                                    <tr>
                                                        <td>&nbsp;&nbsp;&nbsp;
                                                            <xsl:value-of select="@name"/>
                                                        </td>
                                                        <td>&nbsp;&nbsp;&nbsp;
                                                            <xsl:value-of select="."/>
                                                            &nbsp;&nbsp;&nbsp;
                                                        </td>
                                                    </tr>
                                                </xsl:for-each>
                                                <tr>
                                                    <th>&nbsp;</th>
                                                    <th>&nbsp;</th>
                                                </tr>
                                            </table>
                                        </th>
                                    </xsl:when>
                                </xsl:choose>
                            </xsl:for-each>
                            <td valign="top">
                                <table width="100%" cellspacing="0" cellpadding="0" border="0">
                                    <tr class="mediumpurple" height="30">
                                        <td>&nbsp;
                                            <u>
                                            <span class="tableheader">
                                                <xsl:value-of
                                                    select="detaildiagnostics/data/system/attribute/@name"/>
                                                &nbsp;
                                            </span>
                                           </u>
                                        </td>

                                    </tr>
                                    <tr>
                                        <td>&nbsp;</td>
                                        <td>&nbsp;</td>
                                    </tr>
                                    <xsl:for-each select="detaildiagnostics/data/system/attribute/value">
                                        <tr>
                                            <td>&nbsp;
                                                <xsl:if test="../@name='Load-avg'">
                                                    <xsl:value-of select="."/>
                                                </xsl:if>
                                            </td>
                                        </tr>
                                    </xsl:for-each>
                                    <tr>
                                        <td>&nbsp;</td>

                                    </tr>
                                </table>
                                <!--</xsl:if>-->
                            </td>
                        </tr>
                        <tr valign="top" class="lightgray" height="18">
                            <xsl:for-each select="detaildiagnostics/data/system/attribute">
                                <xsl:choose>
                                    <xsl:when test="position()!=1">
                                        <td>
                                            <table width="100%" cellspacing="0" cellpadding="0">
                                                <tr>
                                                    <td width="50%" colspan="2">
                                                        <xsl:if test="@name='Processes'">Total&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</xsl:if>
                                                    </td>
                                                    <td>
                                                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                                        <xsl:value-of select="value"/>
                                                    </td>

                                                </tr>
                                            </table>
                                        </td>
                                    </xsl:when>
                                </xsl:choose>
                            </xsl:for-each>
                            <td>
                                <table width="100%" cellspacing="0" cellpadding="0">
                                    <tr>
                                        <td width="50%" colspan="2">&nbsp;
                                        </td>
                                        <td>&nbsp;
                                        </td>
                                    </tr>
                                </table>
                            </td>
                        </tr>
                        <tr>
                            <td height="25">
                                &nbsp;
                            </td>
                        </tr>
                    </table>
                </xsl:if>

                <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <thead>
                        <xsl:for-each select="detaildiagnostics/data/system/process[position()=1]">
                            <tr class="header-column first">
                                <xsl:for-each select="attribute">
                                    <th class="header-column first" style="text-align: left;">
                                        <xsl:value-of select="@name"/>
                                    </th>
                                </xsl:for-each>
                            </tr>
                        </xsl:for-each>
                    </thead>
                    <tbody>
                        <xsl:for-each select="detaildiagnostics/data/system/process">
                            <tr class="data even">
                                <xsl:for-each select="attribute">
                                    <td class="data even" style="text-align: left;">
                                        <xsl:value-of select="."/>
                                    </td>
                                </xsl:for-each>
                            </tr>
                        </xsl:for-each>
                    </tbody>
                </table>
                <p>&nbsp;</p>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>


