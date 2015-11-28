<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="2.0">
    <xsl:output method="text"/>
<!-- (1) Look for namespace
(2) (Handle global namespace differently???)
(3)    Process each Struct/Class
         3a) process each public member
         3b) process each public method
(4)      Handle inheritance
(5)    Process each function
-->
 <xsl:template name="substring-after-last">
    <xsl:param name="string"/>
    <xsl:param name="char"/>

    <xsl:choose>
      <xsl:when test="contains($string, $char)">
        <xsl:call-template name="substring-after-last">
          <xsl:with-param name="string" select="substring-after($string, $char)"/>
          <xsl:with-param name="char" select="$char"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$string"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

   <xsl:template match="//Struct|//Class">
     <xsl:param name="nsname"/>
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="classid" select="@id"/>
     <xsl:variable name="fileid"  select="@file"/>
     <xsl:variable name="classname" select="@name"/>
     <xsl:variable name="filename"><xsl:call-template name="substring-after-last">
           <xsl:with-param name="string"><xsl:value-of select="//File[@id=$fileid]/@name"/></xsl:with-param>
           <xsl:with-param name="char" select="'/'"/>
       </xsl:call-template></xsl:variable>
     <xsl:variable name="truensname"><xsl:choose ><xsl:when test="$nsname='::'"><xsl:value-of select="substring-before($filename,'.')"/></xsl:when><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:text>
     //Pyllars generation of class or struct </xsl:text><xsl:value-of select="$classname"/>
     <xsl:if test="$nsname='::'">
  PyObject* mod_<xsl:value-of select="translate($truensname,':','_')"/> = Py_InitModule3(&quot;<xsl:value-of select="$truensname"/>&quot;, 
                               nullptr,
			       &quot;pyllars generation for global file-scoped namespace <xsl:value-of select="$truensname"/>&quot;);
     </xsl:if><xsl:text>
    </xsl:text>if (PythonCPointerWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/> &gt;::initialize(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>&quot;,  mod_<xsl:value-of select="translate($truensname,':','_')"/> ) &lt; 0)<xsl:text>{
         PyErr_SetFromString( PyExc_RunTimeException, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
    }
  </xsl:text>
  <xsl:apply-templates select="//Constructor[@context=$classid and @access='public']"/>
  <xsl:apply-templates select="//Method[@context=$classid and @access='public']"/>
  <xsl:apply-templates sleect="//Field[@access='public' amd @context=$classid]"/>
   </xsl:template>

   <xsl:template match="//Namespace">
     <xsl:param name="parentname">""</xsl:param>
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)">pyllars</xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
<xsl:text>
</xsl:text>int init_module<xsl:if test="not(@context)">_<xsl:value-of select="@name"/></xsl:if>(){
  int status = 0;
  PyObject* mod_<xsl:value-of select="translate($nsname,':','_')"/> = Py_InitModule3(&quot;<xsl:value-of select="$nsname"/>&quot;, 
                               nullptr,
			       &quot;pyllars generation for namespace '<xsl:value-of select="$nsname"/>'&quot;);<xsl:text>

     //Pyllars structs/classes for namespace </xsl:text><xsl:value-of select="$nsname"/>
     <xsl:apply-templates select="//Struct[@context=$nsid and @name!='']|//Class[@context=$nsid and @name!='']">
       <xsl:with-param name="nsname"><xsl:value-of select="@name"/></xsl:with-param>
     </xsl:apply-templates>
}
     <xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates>
   </xsl:template>
   <xsl:template match="//GCC_XML"><xsl:text> 
      
#include &lt;pyllars/pyllars_pointer.h&gt;
#include &lt;pyllars/pyllars_function_wrapper.h&gt;
#include &lt;pyllars/pyllars_classwrapper.h&gt;
#include &lt;pyllars/pyllars_conversions.h&gt;
#include &lt;Python.h&gt;
</xsl:text>
   <xsl:apply-templates select="//Namespace[not(@context)]">
    <xsl:with-param name="parentname" value=""/>
   </xsl:apply-templates>

  </xsl:template>

</xsl:stylesheet>