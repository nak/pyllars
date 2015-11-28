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
   <xsl:template match="//*" mode="generate_scope_prnt"><xsl:variable name="contextid" select="@context"/><xsl:apply-templates match="//*[@id=$contextid]" mode="generate_scope_prnt"/><xsl:if test="@name!='::' and @name!=''"><xsl:value-of select="@name"/>::</xsl:if></xsl:template>
   <xsl:template match="//*" mode="generate_scope"><xsl:variable name="contextid" select="@context"/><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_scope_prnt"/></xsl:template>
   <xsl:template match="//FunctionType" mode="generate_name"><xsl:variable name="returntypeid" select="@returns"/><xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_name"/>(*)(<xsl:apply-templates select="." mode="template_arg_parameters"/>)</xsl:template>
   <xsl:template match="//ReferenceType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>&amp;</xsl:template>
   <xsl:template match="//PointerType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>*</xsl:template>
   <xsl:template match="//CvQualifiedType" mode="generate_name">
<xsl:variable name="typeid" select="@type"/>const <xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/></xsl:template>
   <xsl:template match="//FundamentalType|//Typedef|//Struct|//Class" mode="generate_name"><xsl:apply-templates select="." mode="generate_scope"/><xsl:value-of select="@name"/></xsl:template>

   <xsl:template match="//Constructor|//Method|//FunctionType" mode="template_arg_parameters"><xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each></xsl:template>

   <xsl:template match="//Constructor">
     <xsl:param name="classname"></xsl:param>
     PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::addConstructor( PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::create&lt; <xsl:apply-templates select="." mode="template_arg_parameters"/>&gt;);
   </xsl:template>

   <xsl:template match="//Method">
     <xsl:param name="classname"></xsl:param>
     <xsl:variable name="returntypeid" select="@returns"/>
     PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::addMethod(&quot;<xsl:value-of select="@name"/>&quot;, PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::create&lt; <xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_name"/><xsl:if test="count(//*[@id=$returntypeid])=0">void</xsl:if><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:apply-templates select="." mode="template_arg_parameters"/>&gt;);
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
     //Pyllars generation of class or struct </xsl:text><xsl:value-of select="$classname"/><xsl:text>
    </xsl:text>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/> &gt;::initialize(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>&quot;,  mod_<xsl:value-of select="translate($truensname,':','_')"/> ) &lt; 0)<xsl:text>{
         PyErr_SetFromString( PyExc_RunTimeException, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
    }
    </xsl:text>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/>&amp; &gt;::initialize(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>&quot;,  mod_<xsl:value-of select="translate($truensname,':','_')"/> ) &lt; 0)<xsl:text>{
         PyErr_SetFromString( PyExc_RunTimeException, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
    }
    </xsl:text>if (PythonCPointerWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/> &gt;::initialize(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>_ptr&quot;,  mod_<xsl:value-of select="translate($truensname,':','_')"/> ) &lt; 0)<xsl:text>{
         PyErr_SetFromString( PyExc_RunTimeException, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
    }
  </xsl:text>
  <xsl:apply-templates select="//Constructor[@context=$classid and @access='public']"><xsl:with-param name="classname"><xsl:value-of select="$nsname"/>::<xsl:value-of select="$classname"/></xsl:with-param></xsl:apply-templates>

  <xsl:apply-templates select="//Method[@context=$classid and @access='public']"><xsl:with-param name="classname"><xsl:value-of select="$nsname"/>::<xsl:value-of select="$classname"/></xsl:with-param></xsl:apply-templates>
  
  <xsl:apply-templates sleect="//Field[@access='public' amd @context=$classid]"/>
   </xsl:template>

   <xsl:template match="//Namespace[not(@context)]">
	
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)">pyllars</xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
<xsl:text>
 
    //Pyllars structs/classes for global namespace </xsl:text>
    <xsl:for-each select="//File">
        <xsl:variable name="fileid" select="@id"/>
	<xsl:variable name="filename"><xsl:call-template name="substring-after-last">
           <xsl:with-param name="string"><xsl:value-of select="@name"/></xsl:with-param>
           <xsl:with-param name="char" select="'/'"/>
       </xsl:call-template></xsl:variable>
<xsl:text>PyMODINIT_FUNC
int init</xsl:text><xsl:value-of select="substring-before($filename,'.')"/>(){<xsl:text>
    int status = 0;
    PyObject* mod_</xsl:text><xsl:value-of select="substring-before($filename,'.')"/> = Py_InitModule3(&quot;<xsl:value-of select="substring-before($filename,'.')"/>&quot;, 
                               nullptr,
			       &quot;pyllars generation for global file-scoped namespace <xsl:value-of select="substring-before($filename,'.')"/>&quot;);
   <xsl:text>
    </xsl:text><xsl:apply-templates select="//Struct[@file=$fileid and @context=$nsid and @name!='']|//Class[@file=$fileid and @context=$nsid and @name!='']">
           <xsl:with-param name="nsname" select="'::'"/>
       </xsl:apply-templates>
    return status;
}<xsl:text>

</xsl:text>
    </xsl:for-each>

     <xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates>
   </xsl:template>



   <xsl:template match="//Namespace[@context]">
     <xsl:param name="parentname">""</xsl:param>
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)">pyllars</xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
<xsl:text>
</xsl:text>int init<xsl:value-of select="$nsname"/>(){
    int status = 0;<xsl:text>

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

PyMODINIT_FUNC
initpyllars(){
  
    PyObject* m = Py_InitModule3(&quot;pyllars&quot;, nullptr,
	    		         &quot;global pyllars definitions&quot;);
    PythonClassWrapper&lt; char&amp;&gt;::initialize(&quot;char_ref&quot;, m );
    PythonCPointerWrapper&lt;char&gt;::initialize(&quot;char_ptr&quot;, m );
    PythonClassWrapper&lt;short&amp;&gt;::initialize(&quot;short_ref&quot;, m );
    PythonCPointerWrapper&lt;short&gt;::initialize(&quot;short_ptr&quot;, m );
    PythonClassWrapper&lt; int&amp;&gt;::initialize(&quot;int_ref&quot;, m );
    PythonCPointerWrapper&lt; int&gt;::initialize(&quot;int_ptr&quot;, m );
    PythonClassWrapper&lt;long int&amp;&gt;::initialize(&quot;long_int_ref&quot;, m );
    PythonCPointerWrapper&lt;long int&gt;::initialize(&quot;long_int_ptr&quot;, m );
    PythonClassWrapper&lt;long long int&amp;&gt;::initialize(&quot;long_long_int_ref&quot;, m );
    PythonCPointerWrapper&lt;long long int&gt;::initialize(&quot;long_long_int_ptr&quot;, m );


    PythonClassWrapper&lt;unsigned char&amp;&gt;::initialize(&quot;unsigned_char_ref&quot;, m );
    PythonCPointerWrapper&lt;unsigned char&gt;::initialize(&quot;unsigned_char_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned short&amp;&gt;::initialize(&quot;unsigned_short_ref&quot;, m );
    PythonCPointerWrapper&lt;unsigned short&gt;::initialize(&quot;unsigned_short_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned  int&amp;&gt;::initialize(&quot;unsigned_int_ref&quot;, m );
    PythonCPointerWrapper&lt;unsigned  int&gt;::initialize(&quot;unsigned_int_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned long int&amp;&gt;::initialize(&quot;unsigned_long_int_ref&quot;, m );
    PythonCPointerWrapper&lt;unsigned long int&gt;::initialize(&quot;unsigned_long_int_ptr&quot;, m );
    PythonClassWrapper&lt;unsigned long long int&amp;&gt;::initialize(&quot;unsigned_long_long_int_ref&quot;, m );
    PythonCPointerWrapper&lt;unsigned long long int&gt;::initialize(&quot;unsigned_long_long_int_ptr&quot;, m );

}
</xsl:text>
   <xsl:apply-templates select="//Namespace[not(@context)]">
    <xsl:with-param name="parentname" value=""/>
   </xsl:apply-templates>

  </xsl:template>

</xsl:stylesheet>