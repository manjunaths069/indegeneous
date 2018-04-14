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
         <meta http-equiv="Expires" content="now" />
         <meta http-equiv="Cache-Control" content="no-cache,must-revalidate" />
         <meta http-equiv="Pragma" content="no-cache" />
         <basefont class="normaltext" />
        </head>

       <body link="#333399" alink="#333399" vlink="#333399" >
       <basefont ID="FO2" />
       <table class="section-header" border="0" width="100%" CELLSPACING="0" CELLPADDING="0">
         <tr class="mediumpurple" height="20">
           <td class="caption" height="18">
            <!--<span class="heading"> -->
            <b>&nbsp;<xsl:value-of select="detaildiagnostics/data/definedconstant/@name/../@value" /></b>
           </td>
	   <td align="right"><a href="/servlets/com.proactivenet.servlet.HelpServlet?title=sqltop10" target="_new"><img src="/ART/help.gif" alt="Help" border="0" /></a> </td>
         </tr>
       </table>
       <br/>
       <table CELLSPACING="0" border="0" CELLPADDING="0">
         <tr>
            <td>&nbsp;Server Time&nbsp;&nbsp;&nbsp;</td>
            <td><xsl:value-of select='detaildiagnostics/servertime'/></td>
            <td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Agent Time&nbsp;&nbsp;&nbsp;</td>
            <td><xsl:value-of select='detaildiagnostics/timecreated'/></td>
         </tr>
       </table>
       <br/>

       <xsl:if test='detaildiagnostics/error'>
         <h3> Error displaying data :</h3>
         <br/>
         <p><xsl:value-of select='detaildiagnostics/error'/></p>
       </xsl:if>

       <table width="100%" cellspacing="0" cellpadding="0" border="0">
         <tr>
	   <td>
             <table  width="100%" cellspacing="0" cellpadding="0" border="0">
               <xsl:if test="detaildiagnostics/data/system/attribute" >
	         <tr>
                   <xsl:for-each select="detaildiagnostics/data/system/attribute">
                     <xsl:choose>
                       <xsl:when test="position()!=2">
                         <td valign="top" colspan="2">
                           <table  width="65%" cellspacing="0" cellpadding="0"  border="0">
                             <tr class="lightgray" HEIGHT="15">
                               <td class="tableheader">&nbsp;
                                <span style="color:black"><b>&nbsp;
			            <xsl:value-of select="@name"/>&nbsp;
                                </b></span>
                               </td>
			       <td>&nbsp;
			       </td>
                             </tr>
                             <tr>
                               <td>&nbsp;</td>
                               <td>&nbsp;</td>
                             </tr>
                             <xsl:for-each select="state">
                               <tr>
                                 <td>&nbsp;
                                 <xsl:value-of select="@name"/>&nbsp;
                                 </td>
                                 <td >&nbsp;
                                   <xsl:value-of select="."/>&nbsp;
                                 </td>
                               </tr>
                             </xsl:for-each>
                             <tr>
                               <td>&nbsp;</td> <td>&nbsp;</td>
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
               </xsl:if ><!--end <xsl:if test="detaildiagnostics/data/system/attribute" />-->
	     </table>
	   </td>
	 </tr>
  	<!-- Query Separator -Start -->
		<tr>
		 <td colspan="6">
                   <table width="100%">
                     <tr class="lightgray" height="15">
                      <td align="center" valign="center">
                        <span class="tableheader" style="color:black">
			    <b>Top SQL Queries</b>
			</span>
		      </td>
		    </tr>
		   </table>
		  </td>
		 </tr>
	<!-- Query Separator -End -->
	 <tr>
	   <td colspan="5">
	     <table width="100%" cellspacing="0" cellpadding="0" border="0">
               <xsl:for-each select="detaildiagnostics/data/system/sqlquery">
                 <xsl:if test="position()=1">
                   <tr class="mediumpurple" height="18">
                     <xsl:for-each select="attribute">
                       <td align="center" valign="center" class="tableheader" >
                           <xsl:value-of select="@name"/> 
                       </td>
                       <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                       </TD>
                     </xsl:for-each>
                   </tr>
            	 </xsl:if>
                 <xsl:choose>
                   <xsl:when test="position() mod 2 = 1">
                     <tr valign="top" class="lightgray" height="18">
                       <xsl:for-each select="attribute">
		         <xsl:choose>
		           <xsl:when test="@name='SQL Statement'">
                             <td  align="left" valign="top" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                             </td>
		 	   </xsl:when>
		    	   <xsl:otherwise>
                    	     <td  align="center" valign="center" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                     	     </td>
		    	   </xsl:otherwise>
		    	 </xsl:choose>
                     	 <xsl:if test="position()!=last()">
                           <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                           </TD>
                         </xsl:if>
                       </xsl:for-each>
                     </tr>
                   </xsl:when>
                   <xsl:otherwise>
                     <tr valign="top" class="lightyellow" height="18">
                       <xsl:for-each select="attribute">
		         <xsl:choose>
		           <xsl:when test="@name='SQL Statement'">
                    	     <td  align="left" valign="top" class="tablebody"  >
                               <xsl:value-of select="."/>&nbsp;
                     	     </td>
		           </xsl:when>
		           <xsl:otherwise>
                    	     <td  align="center" valign="center" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                     	     </td>
		           </xsl:otherwise>
		         </xsl:choose>
                         <xsl:if test="position()!=last()">
                           <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                           </TD>
                         </xsl:if>
                       </xsl:for-each>
                     </tr>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:for-each>
             </table>
           </td>
         </tr>
	     
         <tr>
	   <td height="25">&nbsp;</td>
         </tr>
	 <!-- Query Separator -Start -->
		<tr>
		 <td colspan="6">
                   <table width="100%">
                     <tr class="lightgray" height="15">
                      <td align="center" valign="center">
                        <span class="tableheader" style="color:black">
			    <b>Currently Running Queries</b>
			</span>
		      </td>
		    </tr>
		   </table>
		  </td>
		 </tr>
	<!-- Query Separator -End -->
         <tr>
	 
             <td colspan="5">
	     <table width="100%" cellspacing="0" cellpadding="0" border="0">
               <xsl:for-each select="detaildiagnostics/data/system/exesqlquery">
                 <xsl:if test="position()=1">
                   <tr class="mediumpurple" height="18">
                     <xsl:for-each select="attribute">
                       <td align="center" valign="center" class="tableheader" >
                           <xsl:value-of select="@name"/> 
                       </td>
                       <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                       </TD>
                     </xsl:for-each>
                   </tr>
            	 </xsl:if>
                 <xsl:choose>
                   <xsl:when test="position() mod 2 = 1">
                     <tr valign="top" class="lightgray" height="18">
                       <xsl:for-each select="attribute">
		         <xsl:choose>
		           <xsl:when test="@name='SQL Statement'">
                             <td  align="left" valign="top" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                             </td>
		 	   </xsl:when>
		    	   <xsl:otherwise>
                    	     <td  align="center" valign="center" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                     	     </td>
		    	   </xsl:otherwise>
		    	 </xsl:choose>
                     	 <xsl:if test="position()!=last()">
                           <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                           </TD>
                         </xsl:if>
                       </xsl:for-each>
                     </tr>
                   </xsl:when>
                   <xsl:otherwise>
                     <tr valign="top" class="lightyellow" height="18">
                       <xsl:for-each select="attribute">
		         <xsl:choose>
		           <xsl:when test="@name='SQL Statement'">
                    	     <td  align="left" valign="top" class="tablebody"  >
                               <xsl:value-of select="."/>&nbsp;
                     	     </td>
		           </xsl:when>
		           <xsl:otherwise>
                    	     <td  align="center" valign="center" class="tablebody"  >
                                 <xsl:value-of select="."/>&nbsp;
                     	     </td>
		           </xsl:otherwise>
		         </xsl:choose>
                         <xsl:if test="position()!=last()">
                           <TD HEIGHT="1" WIDTH="1" class="mediumpurple"><img src="/ART/1x1clear.gif" />
                           </TD>
                         </xsl:if>
                       </xsl:for-each>
                     </tr>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:for-each>
             </table>
           </td>
	  </tr>
         <tr>
           <td height="25">&nbsp;</td>
         </tr>
       </table>
       <p>&nbsp;</p>
     </body>
   </html>
 </xsl:template>
</xsl:stylesheet>




