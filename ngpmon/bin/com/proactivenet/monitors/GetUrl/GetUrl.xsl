<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE html [<!ENTITY nbsp "&#160;">] >
<xsl:stylesheet
    xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>
    <xsl:output method="html" encoding="iso-8859-1"/>
    <xsl:param name="url" select="'urn:check%20out%20my%20r%E9sum%E9'"/>

    <xsl:variable name="hex" select="'0123456789ABCDEF'"/>
    <xsl:variable name="ascii"> !"#$%&amp;'()*+,-./0123456789:;&lt;=&gt;?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~</xsl:variable>
    <xsl:variable name="latin1">&#160;&#161;&#162;&#163;&#164;&#165;&#166;&#167;&#168;&#169;&#170;&#171;&#172;&#173;&#174;&#175;&#176;&#177;&#178;&#179;&#180;&#181;&#182;&#183;&#184;&#185;&#186;&#187;&#188;&#189;&#190;&#191;&#192;&#193;&#194;&#195;&#196;&#197;&#198;&#199;&#200;&#201;&#202;&#203;&#204;&#205;&#206;&#207;&#208;&#209;&#210;&#211;&#212;&#213;&#214;&#215;&#216;&#217;&#218;&#219;&#220;&#221;&#222;&#223;&#224;&#225;&#226;&#227;&#228;&#229;&#230;&#231;&#232;&#233;&#234;&#235;&#236;&#237;&#238;&#239;&#240;&#241;&#242;&#243;&#244;&#245;&#246;&#247;&#248;&#249;&#250;&#251;&#252;&#253;&#254;&#255;</xsl:variable>


    <xsl:template match="/">
        <LINK REL="StyleSheet" TYPE="text/css" MEDIA="screen"/>
        <html>
            <head>
                <meta http-equiv="Expires" content="now"/>
                <meta http-equiv="Cache-Control" content="no-cache,must-revalidate"/>
                <meta http-equiv="Pragma" content="no-cache"/>
                <basefont class="normaltext"/>
            </head>
            <body>
                <!-- Header -->
                <table class="section-header" width="100%" border="0" cellspacing="0" cellpadding="0">
                    <tr>
                        <td class="caption">
                            <xsl:value-of select="detaildiagnostics/data/GetUrl/definedconstant/@name/../@value"/>
                        </td>
                        <td align="right">
                            <a href="/servlets/com.proactivenet.servlet.HelpServlet?title=GetUrl" target="_new">
                                <img src="/ART/help.gif" alt="Help" border="0"/>
                            </a>
                        </td>
                    </tr>
                </table>
                <!-- End of Header -->

                <br/>

                <!-- showing server time and agent time-->
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
                <!-- end of server time and agent time-->

                <br/>

                <!-- aggregate data -->

                <table cellspacing="0" border="0" cellpadding="0">
                    <tr>
                        <td>&nbsp;
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/ConnectTime/@name'/>
                            &nbsp;&nbsp;&nbsp;
                        </td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/ConnectTime'/>
                        </td>
                        <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/TotalResponseTime/@name'/>
                            &nbsp;&nbsp;&nbsp;
                        </td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/TotalResponseTime'/>
                        </td>
                    </tr>
                    <tr>
                        <td>&nbsp;
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/DNSLookupTime/@name'/>
                            &nbsp;&nbsp;&nbsp;
                        </td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/DNSLookupTime'/>
                        </td>
                        <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/FirstByteDownloadTime/@name'/>
                            &nbsp;&nbsp;&nbsp;
                        </td>
                        <td>
                            <xsl:value-of select='detaildiagnostics/data/GetUrl/aggregate/FirstByteDownloadTime'/>
                        </td>
                    </tr>
                </table>
                <!-- End of aggregate data -->

                <br/>

                <!-- start error message -->
                <xsl:if test='detaildiagnostics/error'>
                    <h3>Error displaying data :</h3>
                    <br/>
                    <p>
                        <xsl:value-of select='detaildiagnostics/error'/>
                    </p>
                </xsl:if>
                <!-- end error message -->

                <!-- starting loop for each Page -->
                <xsl:for-each select="detaildiagnostics/data/GetUrl/Page">
                    <!-- start of table showing PageWhole -->
                    <table class="data" width="100%" cellspacing="0" cellpadding="0" border="0">
                        <caption><xsl:value-of select='PageWhole/@name'/></caption>
                        <thead>
                        <tr>
                            <xsl:for-each select="PageWhole/attribute">
                                <th class="header-column first">
                                    <xsl:value-of select="@name"/>
                                </th>
                            </xsl:for-each>
                        </tr>
                        </thead>
                        <tbody>
                        <tr valign="center" class="data even" height="18">
                            <xsl:for-each select="PageWhole/attribute">
                                <td class="data even">
                                    <xsl:call-template name="decode">
                                        <xsl:with-param name="encoded" select="."/>
                                    </xsl:call-template>
                                </td>
                            </xsl:for-each>
                        </tr>
                        </tbody>
                    </table>
                    <!-- end of table showing PageWhole -->

                    <br></br>

                    <!-- start of table showing PageDetails -->
                    <table class='data' width="100%" border="0" cellspacing="0" cellpadding="0">
                        <caption><xsl:value-of select='PageDetails/@name'/></caption>
                        <xsl:if test="PageDetails/data/attribute">
                        </xsl:if>
                        <thead>
                        <xsl:for-each select="PageDetails/data[position()=1]">
				<xsl:if test="position()=1">
                                    <tr class="header-column first" height="18">
                                        <xsl:for-each select="attribute">
                                            <th class="header-column first" style="text-align: left;">
                                                <xsl:value-of select="@name"/>
                                            </th>
                                        </xsl:for-each>
                                    </tr>
				</xsl:if>
                        </xsl:for-each>
                        </thead>
                        <tbody>
                        <xsl:for-each select="PageDetails/data">
                            <!-- start sort tinkering -->
                            <xsl:sort select="data/attribute/." order="descending"/>
                            <xsl:choose>
                                <xsl:when test="position() mod 2 = 1">
                                    <tr class="data even">
                                        <xsl:for-each select="attribute">
                                            <td class="data even" style="text-align: left;">
                                                <xsl:call-template name="decode">
                                                    <xsl:with-param name="encoded" select="."/>
                                                </xsl:call-template>
                                            </td>
                                        </xsl:for-each>
                                    </tr>
                                </xsl:when>
                                <xsl:otherwise>
                                    <tr class="data even">
                                        <xsl:for-each select="attribute">
                                            <td class="data even" style="text-align: left;">
                                                <xsl:call-template name="decode">
                                                    <xsl:with-param name="encoded" select="."/>
                                                </xsl:call-template>
                                            </td>
                                        </xsl:for-each>
                                    </tr>
                                </xsl:otherwise>
                            </xsl:choose>
                            <!-- end of sort tinkering -->
                        </xsl:for-each>
                        </tbody>
                    </table>
                    <!-- end of table showing PageDetails -->
                    <br></br>
                </xsl:for-each>
                <!-- ending loop for each Page -->
                <p>&nbsp;</p>
            </body>
        </html>
    </xsl:template>

    <xsl:template name="decode">
        <xsl:param name="encoded"/>
        <xsl:choose>
            <xsl:when test="contains($encoded,'%')">
                <xsl:value-of select="substring-before($encoded,'%')"/>
                <xsl:variable name="hexpair"
                              select="translate(substring(substring-after($encoded,'%'),1,2),'abcdef','ABCDEF')"/>
                <xsl:variable name="decimal"
                              select="(string-length(substring-before($hex,substring($hexpair,1,1))))*16 + string-length(substring-before($hex,substring($hexpair,2,1)))"/>
                <xsl:choose>
                    <xsl:when test="$decimal &lt; 127 and $decimal &gt; 31">
                        <xsl:value-of select="substring($ascii,$decimal - 31,1)"/>
                    </xsl:when>
                    <xsl:when test="$decimal &gt; 159">
                        <xsl:value-of select="substring($latin1,$decimal - 159,1)"/>
                    </xsl:when>
                    <xsl:otherwise>?</xsl:otherwise>
                </xsl:choose>
                <xsl:call-template name="decode">
                    <xsl:with-param name="encoded" select="substring(substring-after($encoded,'%'),3)"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$encoded"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>



