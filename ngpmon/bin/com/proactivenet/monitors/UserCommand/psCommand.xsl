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
                            'ps' Output on
                            <xsl:value-of select="detaildiagnostics/data/header2"/>
                            <!--
                            <xsl:value-of select="detaildiagnostics/data/definedconstant/@name/../@value" />
                                  <xsl:value-of select="detaildiagnostics/data/definedconstant" />
                            //-->
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=psCommand" target="_new">
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
                <table class="section-header" width="100%" cellspacing="0" cellpadding="0">
                    <tr>
                        <td class="caption">
                            Output
                        </td>
                    </tr>
                </table>
                <table width="100%" cellspacing="0" cellpadding="0">
                    <tr>
                        <td>
                            <pre>
                                <font style="font-family:monospace;">
                                    <xsl:value-of select="detaildiagnostics/data/usercommand"/>
                                </font>
                            </pre>
                        </td>
                    </tr>
                </table>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>
