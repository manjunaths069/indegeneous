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
                <a name="top"> &nbsp; </a>
                <table class="section-header" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                        <td class="caption">
                            <xsl:value-of select='detaildiagnostics/data/activeinfo/servername'/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=WLogicActiveInfo"
                               target="_new">
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
                                    <font color='blue'>
                                        <xsl:value-of select='detaildiagnostics/error'/>
                                    </font>
                                </p>
                            </td>
                        </tr>
                    </table>

                </xsl:if>

                <ul>
                    <li/>
                    <a href="#queue">Active Queues</a>
                    <li/>
                    <a href="#socket">Open Socket Count / Socket Reader Threads</a>
                </ul>
                <br/>


                <a name="queue">
                    <b>Active Queues</b>
                </a>
                <br/>
                <br/>
                <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <thead>
                    <xsl:for-each select="detaildiagnostics/data/activeinfo/queue[position()=1]">
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
                        <xsl:for-each select="detaildiagnostics/data/activeinfo/queue">
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
                <br/>
                <a href="#top">Top</a>
                <br/>
                <br/>
                <a name="socket">
                    <b>Open Socket Count / Socket Reader Threads</b>
                </a>
                <br/>
                <br/>
                <table class="data" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <thead>
                        <xsl:for-each select="detaildiagnostics/data/activeinfo/socket[position()=1]">
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
                        <xsl:for-each select="detaildiagnostics/data/activeinfo/socket">
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
                <br/>
                <a href="#top">Top</a>
                <br/>
                <br/>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
