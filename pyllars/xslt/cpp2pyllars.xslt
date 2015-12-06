<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="1.0">
    <xsl:output method="text"/>
<!-- (1) Look for namespace
(2) (Handle global namespace differently???)
(3)    Process each Struct/Class
         3a) process each public member
         3b) process each public method
(4)      Handle inheritance
(5)    Process each function
-->

<!--############################################################-->
<!--## Template to tokenize strings                           ##-->
<!--############################################################-->

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

<!--##################
  Templates to generate  type names for various classes of types
  ################### -->
   
   <!-- template to generate namespace prefix (scope name) -->
   <xsl:template match="//*[@context!='']" mode="generate_nsprefix"><xsl:param name="as_return" select="''"/><xsl:variable name="contextid" select="@context"/><xsl:choose><xsl:when test="@context"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise></xsl:otherwise></xsl:choose><xsl:apply-templates select="." mode="generate_name"><xsl:with-param name="as_return" select="$as_return"/></xsl:apply-templates>::</xsl:template>
   <xsl:template match="//*[not(@context) and @type!='']" mode="generate_nsprefix"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix"/> </xsl:template>
   <xsl:template match="//*" mode="generate_nsprefix_parent"><xsl:variable name="contextid" select="@context"/><xsl:variable name="typeid" select="@type"/><xsl:choose><xsl:when test="$contextid!=''"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix_parent"/></xsl:otherwise></xsl:choose></xsl:template>


   <xsl:template match="//*[@context!='']" mode="generate_scoped_name"><xsl:param name="as_return" select="''"/><xsl:variable name="contextid" select="@context"/><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/><xsl:apply-templates select="." mode="generate_name"><xsl:with-param name="as_return" select="$as_return"/></xsl:apply-templates></xsl:template>
  <xsl:template match="//*[not(@context)]" mode="generate_scoped_name"><xsl:variable name="typeid" select="@type"/><xsl:variable name="contextid" select="//*[@id=$typeid]/@context"/><xsl:choose><xsl:when test="//*[@id=$contextid]"><xsl:apply-templates select="//*[@id=$contextid]" mode="generate_nsprefix"/></xsl:when><xsl:otherwise><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_nsprefix_parent"/></xsl:otherwise></xsl:choose><xsl:apply-templates select="." mode="generate_name"/></xsl:template>

   <xsl:template match="//Namespace" mode="generate_name"><xsl:if test="@name!='::'"><xsl:value-of select="@name"/></xsl:if></xsl:template>


   <!-- templates to generat the type name (bare, without scope) -->

   <xsl:template match="//Union" mode="generate_name"><xsl:param name="attrname" select="''"/><xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:when test="$attrname=''"><xsl:variable name="typeid" select="@id"/><xsl:variable name="fieldname" select="//Field[@type=$typeid]/@name"/><xsl:variable name="contextid" select="@context"/><xsl:if test="$fieldname!=''">decltype(<xsl:apply-templates select="//*[@id=$contextid]" mode="generate_name"/>::<xsl:value-of select="$fieldname"/>)</xsl:if></xsl:when><xsl:otherwise>decltype(<xsl:value-of select="$attrname"/>)</xsl:otherwise></xsl:choose></xsl:template>
 
  <xsl:template match="//FunctionType" mode="generate_name"><xsl:variable name="returntypeid" select="@returns"/><xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_name"/>(*)(<xsl:apply-templates select="." mode="template_arg_type_list"/>)</xsl:template>

   <xsl:template match="//Function" mode="generate_name"><xsl:value-of select="@name"/></xsl:template>
  
   <xsl:template match="//ReferenceType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>&amp;</xsl:template>

   <xsl:template match="//PointerType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/>*</xsl:template>

   <xsl:template match="//CvQualifiedType" mode="generate_name"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/> const</xsl:template>

   <xsl:template match="//ArrayType" mode="generate_name"><xsl:param name="as_return" select="-1"/><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_name"/><xsl:choose><xsl:when test="$as_return&lt;=0 or string($as_return)='NaN'">[]</xsl:when><xsl:otherwise>[<xsl:value-of select="number($as_return)"/>]</xsl:otherwise></xsl:choose></xsl:template>

   <xsl:template match="//Enumeration|//FundamentalType|//Typedef|//Struct|//Class" mode="generate_name"><xsl:variable name="id" select="@id"/><xsl:variable name="name"><xsl:choose><xsl:when test="@name=''"><xsl:value-of select="//Typedef[@type=$id]/@name"/></xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable><xsl:value-of select="$name"/></xsl:template>

<!--#########################
   Templates for generating argument lists
   ##########################-->
   <xsl:template match="//Constructor|//Method|//FunctionType|//Function" mode="template_arg_type_list"><xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"/><xsl:if test="position()!=last()">,</xsl:if></xsl:for-each></xsl:template>

<!--##################
    Generate code for a class constructors, members, and methods 
    ##################-->

   <xsl:template match="//Constructor">
     <xsl:param name="classname"></xsl:param>
     <xsl:param name="bareclassname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Constructor <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     {
        static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};
        PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::addConstructor(kwlist, PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::create&lt; <xsl:apply-templates select="." mode="template_arg_type_list"/>&gt;);
     }
   </xsl:template> 

   <xsl:template match="//Method">
     <xsl:param name="classname"></xsl:param>
     <xsl:param name="bareclassname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Method <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="returntypeid" select="@returns"/>
     <xsl:variable name="const_modifier"><xsl:if test="number(@const)=1">Const</xsl:if><xsl:if test="number(@static)=1">Class</xsl:if></xsl:variable>
     {
	static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};

        PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::add<xsl:value-of select="$const_modifier"/>Method<xsl:text>
        </xsl:text>&lt;<xsl:text> </xsl:text>name__<xsl:value-of select="@id"/>, <xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_scoped_name"/><xsl:if test="count(//*[@id=$returntypeid])=0">void</xsl:if><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:apply-templates select="." mode="template_arg_type_list"/><xsl:text> </xsl:text> &gt;<xsl:text>
        </xsl:text>( &amp;<xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>, kwlist);
     }
   </xsl:template>

<!--###############################
    Generate code for public attributes of structs/classes/unions
    ############################### -->

   <xsl:template match="//Field">
     <xsl:param name="classname"></xsl:param>

     <xsl:message terminate="no">
###
# Generate Attribute  <xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="typeid" select="@type"/>
     <xsl:variable name="const_modifier"><xsl:if test="number(@const)=1">Const</xsl:if><xsl:if test="number(@static)=1">Class</xsl:if></xsl:variable>
     <xsl:variable name="fieldname"><xsl:value-of select="@name"/></xsl:variable>
     <xsl:if test="$fieldname!=''">
     PythonClassWrapper&lt;<xsl:value-of select="$classname"/>&gt;::add<xsl:value-of select="$const_modifier"/>Attribute<xsl:text>
     </xsl:text>&lt;name__<xsl:value-of select="@id"/>, <xsl:apply-templates select="//*[@id=$typeid]" mode="generate_scoped_name"><xsl:with-param name="attrname"><xsl:value-of select="$classname"/>::<xsl:value-of select="$fieldname"/></xsl:with-param><xsl:with-param name="as_return" select="number(//*[@id=$typeid]/@max)+1"/></xsl:apply-templates> &gt;<xsl:text>
     </xsl:text>(&amp;<xsl:value-of select="$classname"/>::<xsl:value-of select="@name"/>);
     </xsl:if>
   </xsl:template>

<!-- ##################################
     Templates to generate required extern name definitons
     ################################## -->

    <xsl:template match="//Struct|//Class|//Union" mode="generate_externs">
       <xsl:variable name="classid" select="@id"/>
       <xsl:for-each select="//Method[@context=$classid]|//Field[@context=$classid]">
extern const char name__<xsl:value-of select="@id"/>[] = &quot;<xsl:value-of select="@name"/>&quot;;
       </xsl:for-each>

       <xsl:apply-templates select="//Struct[@context=$classid and (not(@access) or @access='public')]|//Class[@context=$classid and @access='public']|//Union[@context=$classid and  (not(@access) or @access='public')]" mode="generate_externs"/>

   </xsl:template>

<!-- ################################
    Templates for generating code to wrap various classes of types/functions
    ################################ -->  
  <xsl:template match="//Function">
     <xsl:param name="nsname"/>
     <xsl:param name="mod_name_in" select="''"/>

     <xsl:message terminate="no">
###
# Generate Function <xsl:value-of select="@name"/>...
###
     </xsl:message>
 
     <xsl:variable name="returntypeid" select="@returns"/>
     <xsl:variable name="mod_name"><xsl:choose><xsl:when test="$mod_name_in!=''"><xsl:value-of select="translate($mod_name_in,':','_')"/></xsl:when><xsl:otherwise> mod_<xsl:value-of select="translate($nsname,':','_')"/></xsl:otherwise></xsl:choose></xsl:variable>
     {
	static const char* kwlist[] = {<xsl:for-each select="./Argument">&quot;<xsl:value-of select="@name"/>&quot;, </xsl:for-each>nullptr};

        PyObject* wrapper = (PyObject*)PythonFunctionWrapper<xsl:text>
        </xsl:text>&lt; <xsl:apply-templates select="//*[@id=$returntypeid]" mode="generate_scoped_name"/><xsl:if test="count(//*[@id=$returntypeid])=0">void</xsl:if><xsl:if test="count(./Argument)>0">, </xsl:if><xsl:apply-templates select="." mode="template_arg_type_list"/><xsl:text> &gt;::create
        </xsl:text>(  &quot;<xsl:value-of select="@name"/>&quot;, <xsl:apply-templates select="." mode="generate_scoped_name"/> , kwlist);
        PyModule_AddObject( wrapper, &quot;<xsl:value-of select="@name"/>&quot;, <xsl:value-of select="$mod_name"/>);
     }
  </xsl:template>

  <xsl:template match="//Typedef">
     <xsl:param name="nsname"/>
     <xsl:param name="mod_name_in" select="''"/>

     <xsl:message terminate="no">
###
# Generate Typedef <xsl:value-of select="@name"/>...
###
     </xsl:message>

     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="fileid"  select="@file"/>
   <!--  <xsl:variable name="filename"><xsl:call-template name="substring-after-last">
           <xsl:with-param name="string"><xsl:value-of select="//File[@id=$fileid]/@name"/></xsl:with-param>
           <xsl:with-param name="char" select="'/'"/>
       </xsl:call-template></xsl:variable> -->
     <xsl:variable name="typeid" select="@id"/>
     <xsl:variable name="mod_name"><xsl:choose><xsl:when test="$mod_name_in!=''"><xsl:value-of select="translate($mod_name_in,':','_')"/></xsl:when><xsl:otherwise> mod_<xsl:value-of select="translate($nsname,':','_')"/></xsl:otherwise></xsl:choose></xsl:variable>
    <xsl:if test="@name">
        <xsl:variable name="typealias" select="//*[@id=$typeid]/@name"/>
        <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable><xsl:text>
    </xsl:text><xsl:if test="@incomplete!='1' or not(@incomplete)">if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>, <xsl:value-of select="$is_complete"/> &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>&quot;, <xsl:value-of select="$mod_name"/> ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }
    
    </xsl:text></xsl:if>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>&amp;, <xsl:value-of select="$is_complete"/> &gt;::initialize<xsl:text>
                  </xsl:text>(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>_ref&quot;,  <xsl:value-of select="$mod_name"/> ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }
    
    </xsl:text>if (PythonCPointerWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>, <xsl:value-of select="$is_complete"/> &gt;::initialize(&quot;<xsl:value-of select="translate(@name,'&lt;&gt;:','___')"/>_ptr&quot;, <xsl:value-of select="$mod_name"/>   ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'");
         status = -1;
    }

</xsl:text>
     </xsl:if>
  </xsl:template>
  
      
  <xsl:template match="//ArrayType[number(@max)&gt;0]">
     <xsl:param name="nsname"/>
     <xsl:param name="mod_name_in" select="''"/>

     <xsl:message terminate="no">
###
# Generate ArrayType with Extend... <xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>
<!--     <xsl:variable name="fileid"  select="@file"/>
     <xsl:variable name="filename"><xsl:call-template name="substring-after-last">
           <xsl:with-param name="string"><xsl:value-of select="//File[@id=$fileid]/@name"/></xsl:with-param>
           <xsl:with-param name="char" select="'/'"/>
       </xsl:call-template></xsl:variable> -->
     <xsl:variable name="mod_name"><xsl:choose><xsl:when test="$mod_name_in!=''"><xsl:value-of select="translate($mod_name_in,':','_')"/></xsl:when><xsl:otherwise> mod_<xsl:value-of select="translate($nsname,':','_')"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:if test="@name">
         <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable>
         <xsl:variable name="typeid" select="@id"/>
         <xsl:variable name="typealias" select="//*[@id=$typeid]/@name"/><xsl:text>
    </xsl:text>if (PythonCPointerWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$typealias"/>, <xsl:value-of select="$is_complete"/>, <xsl:value-of select="@max"/>&gt;::initialize<xsl:text>
      </xsl:text>(&quot;<xsl:value-of select="translate($typealias,'&lt;&gt;:','___')"/>&quot;, <xsl:value-of select="$mod_name"/> ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="@name"/><xsl:text>'&quot;);
         status = -1;
    }
    
</xsl:text>
     </xsl:if>
  </xsl:template>
  
   <xsl:template match="//Struct|//Class|//Union">
     <xsl:param name="nsname"/>
     <xsl:param name="addlname" select="''"/>
     <xsl:param name="mod_name_in" select="''"/>

     <xsl:message terminate="no">
###
# Generate Class/Struct/Union... <xsl:value-of select="@name"/>
###
     </xsl:message>

     <xsl:variable name="classid" select="@id"/>
<!--
     <xsl:variable name="classname"><xsl:choose><xsl:when test="@name='' and //Typedef[@type=$classid]"><xsl:value-of select="//Typedef[@type=$classid]/@name"/></xsl:when><xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise><xsl:apply-templates select="." mode="generate_name"/></xsl:otherwise></xsl:choose></xsl:variable> -->

     <xsl:variable name="classname"><xsl:apply-templates select="." mode="generate_name"/></xsl:variable>
     <xsl:variable name="nsnamebare"><xsl:choose><xsl:when test="$nsname='::'"/><xsl:otherwise><xsl:value-of select="$nsname"/></xsl:otherwise></xsl:choose></xsl:variable>

     <xsl:for-each select="//Field[@context=$classid]">
       <xsl:variable name="typeid" select="@type"/>
     </xsl:for-each>
     
     <xsl:variable name="is_complete"><xsl:choose><xsl:when test="@incomplete='1'">false</xsl:when><xsl:otherwise>true</xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="mod_name"><xsl:choose><xsl:when test="$mod_name_in!=''"><xsl:value-of select="translate($mod_name_in,':','_')"/></xsl:when><xsl:otherwise>mod_<xsl:value-of select="translate($nsname,':','_')"/> </xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:text>
     //Pyllars generation of class or struct or union  </xsl:text><xsl:value-of select="$classname"/> id: <xsl:value-of select="@id"/><xsl:text>
     
     </xsl:text><xsl:if test="$classname!=''"><xsl:if test="not(@incomplete) or @incomplete!='1'">if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/>, <xsl:value-of select="$is_complete"/> &gt;::initialize<xsl:text>
        </xsl:text>(&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>&quot;, <xsl:value-of select="$mod_name"/>) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-class/type for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
     </xsl:text></xsl:if>if (PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/>&amp; , <xsl:value-of select="$is_complete"/> &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>_ref&quot;,  <xsl:value-of select="$mod_name"/> ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-reference for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
     </xsl:text>if (PythonCPointerWrapper&lt; <xsl:value-of select="$nsnamebare"/><xsl:if test="$nsnamebare!=''">::</xsl:if><xsl:value-of select="$classname"/>, <xsl:value-of select="$is_complete"/> &gt;::initialize<xsl:text>
       </xsl:text>(&quot;<xsl:value-of select="translate($classname,'&lt;&gt;:','___')"/>_ptr&quot;, <xsl:value-of select="$mod_name"/> ) &lt; 0)<xsl:text>{
         PyErr_SetString( PyExc_RuntimeError, "Failed to initialize Python-wrapper-to-pointer for '</xsl:text><xsl:value-of select="$classname"/><xsl:text>'&quot;);
         status = -1;
     }
  </xsl:text>
      <xsl:apply-templates select="//Constructor[//*[@id=$classid]/@abstract!='1' and @context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::'"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param><xsl:with-param name="bareclassname" select="$classname"/></xsl:apply-templates>

      <xsl:apply-templates select="//Method[@context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::'"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param><xsl:with-param name="bareclassname" select="$classname"/></xsl:apply-templates>
  
      <xsl:apply-templates select="//Field[@context=$classid and  (not(@access) or @access='public')]"><xsl:with-param name="classname"><xsl:if test="$nsname!='::'"><xsl:value-of select="$nsname"/>::</xsl:if><xsl:value-of select="$classname"/></xsl:with-param></xsl:apply-templates>
</xsl:if>
    //Nested Pyllars definition of typedefs
      <xsl:apply-templates select="//Typedef[@context=$classid and  (not(@access) or @access='public')]|//ArrayType[@context=$classid and  (not(@access) or @access='public')]">
        <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/>::<xsl:value-of select="$classname"/></xsl:with-param>
        <xsl:with-param name="mod_name_in" select="'nullptr'"/>
      </xsl:apply-templates>

      <!-- nested classes/structs -->
      <xsl:apply-templates select="//Class[(@incomplete!='1' or not(@incomplete)) and @context=$classid and  (not(@access) or @access='public')]|//Struct[(@incomplete!='1' or not(@incomplete)) and @context=$classid and  (not(@access) or @access='public')]">
         <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/>::<xsl:value-of select="$classname"/></xsl:with-param>
         <xsl:with-param name="mod_name_in" select="'nullptr'"/>
      </xsl:apply-templates>
      
      <xsl:apply-templates select="//Union[@context=$classid and  (not(@access) or @access='public')]">
         <xsl:with-param name="nsname" select="$nsname"/>
         <xsl:with-param name="addlname">union_<xsl:value-of select="@id"/></xsl:with-param>
      </xsl:apply-templates>

      <!-- Nested defintions -->
      <xsl:for-each select="//Typedef[(@incomplete!='1' or not(@incomplete)) and @context=$classid and  (not(@access) or @access='public')]|//Class[(@incomplete!='1' or not(@incomplete)) and @context=$classid and  (not(@access) or @access='public')]|//Struct[(@incomplete!='1' or not(@incomplete)) and @context=$classid and  (not(@access) or @access='public')]">
         <xsl:variable name="typeid" select="@id"/>
         <xsl:variable name="name"><xsl:choose><xsl:when test="not(@name)"><xsl:value-of select="//*[@id=$typeid]/@name"/></xsl:when><xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
      // Nested class/typedef <xsl:value-of select="$name"/> <xsl:text>
      </xsl:text>PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/> &gt;::addClassMember<xsl:text>
         </xsl:text>(&quot;<xsl:value-of select="$name"/>&quot;, (PyObject*)&amp;PythonClassWrapper&lt; <xsl:value-of select="$nsnamebare"/>::<xsl:value-of select="$classname"/>::<xsl:value-of select="$name"/> &gt;::Type);
      </xsl:for-each>


   </xsl:template>

   <xsl:template match="//Namespace[not(@context)]">
     <xsl:param name="parentname" select="''"/>

     <xsl:message terminate="no">
###
# Generating namespace <xsl:value-of select="@name"/>
###
     </xsl:message>
 
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)"></xsl:when><xsl:otherwise><xsl:if test="$parentname!='' and $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>
<xsl:text>

 
    //Pyllars structs/classes for global namespace </xsl:text>
<!--    <xsl:for-each select="//File[@name!='&lt;builtin&gt;']">
        <xsl:variable name="fileid" select="@id"/>
	<xsl:variable name="filename"><xsl:call-template name="substring-after-last">
           <xsl:with-param name="string"><xsl:value-of select="@name"/></xsl:with-param> 
           <xsl:with-param name="char" select="'/'"/>
       </xsl:call-template></xsl:variable>        <xsl:text>

PyMODINIT_FUNC
init</xsl:text><xsl:value-of select="substring-before($filename,'.')"/>(){<xsl:text>
    int status = 0;
    PyObject* mod_</xsl:text><xsl:value-of select="substring-before($filename,'.')"/> = Py_InitModule3(&quot;<xsl:value-of select="substring-before($filename,'.')"/>&quot;, 
                               nullptr,
			       &quot;pyllars generation for global file-scoped namespace <xsl:value-of select="substring-before($filename,'.')"/>&quot;);
    (void)mod_<xsl:value-of select="substring-before($filename,'.')"/>;
   <xsl:text>
    </xsl:text><xsl:apply-templates select="//Struct[(@incomplete!='1' or not(@incomplete)) and @file=$fileid and @context=$nsid and  (not(@access) or @access='public')]|//Class[(@incomplete!='1' or not(@incomplete)) and @file=$fileid and @context=$nsid and  (not(@access) or @access='public')]">
           <xsl:with-param name="nsname" select="'::'"/>
       </xsl:apply-templates>
   if (status != 0){
     PyErr_Print();
   }
}<xsl:text>

</xsl:text>
    </xsl:for-each>
-->
     <xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates>
   </xsl:template>

   <xsl:template match="//Namespace" mode="generate_externs">
     <xsl:variable name="nsid" select="@id"/>

 
//externs for namespace '<xsl:value-of select="@name"/>'
     <xsl:apply-templates select="//Namespace[@context=$nsid]|//Struct[@context=$nsid and  (not(@access) or @access='public')]|//Class[@context=$nsid and  (not(@access) or @access='public')]|//Union[@context=$nsid and  (not(@access) or @access='public')]|//Function[@context=$nsid and  (not(@access) or @access='public')]" mode="generate_externs"/>
   </xsl:template>

   <xsl:template match="//Namespace[@context]">
     <xsl:param name="parentname"></xsl:param>
     <xsl:variable name="nsname"><xsl:choose><xsl:when test="not(@context)"></xsl:when><xsl:otherwise><xsl:if test="$parentname!='' and $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:value-of select="@name"/></xsl:otherwise></xsl:choose></xsl:variable>
     <xsl:variable name="nsid"><xsl:value-of select="@id"/></xsl:variable>

     <xsl:if test="$parentname='' or $parentname='::'"><xsl:text>

PyMODINIT_FUNC
</xsl:text>init<xsl:value-of select="$nsname"/>(){
    int status = 0;
</xsl:if><xsl:text>
    PyObject* mod_</xsl:text><xsl:value-of select="translate($nsname,':','_')"/> = Py_InitModule3(&quot;<xsl:value-of select="$nsname"/>&quot;, 
                               nullptr,
			       &quot;pyllars for  namespace <xsl:value-of select="$nsname"/>&quot;);<xsl:text>
    </xsl:text><xsl:choose><xsl:when test="$parentname!='' and $parentname!='::'">
    PyModule_AddObject( mod_<xsl:value-of select="translate($parentname,':','_')"/>, &quot;<xsl:value-of select="@name"/>&quot;, mod_<xsl:value-of select="translate($nsname,':','_')"/>);
    </xsl:when>
    <xsl:otherwise>
    (void)mod_<xsl:value-of select="translate($nsname,':','_')"/>;
    </xsl:otherwise></xsl:choose><xsl:text>
    PyObject* mod___</xsl:text><xsl:value-of select="translate($nsname,':','_')"/>     = mod_<xsl:value-of select="translate($nsname,':','_')"/> ;<xsl:text>

    //Pyllars functions
     </xsl:text><xsl:apply-templates select="//Function[@context=$nsid and  (not(@access) or @access='public')]">
       <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates><xsl:text>


    //Pyllars structs/classes for namespace </xsl:text><xsl:value-of select="$nsname"/>
     <xsl:apply-templates select="//Struct[@context=$nsid and  (not(@access) or @access='public')]|//Class[@context=$nsid and  (not(@access) or @access='public')]">
       <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/></xsl:with-param>
     </xsl:apply-templates><xsl:text>

    //Pyllars Typedefs/Array defs for namespace  </xsl:text><xsl:value-of select="$nsname"/>
     <xsl:apply-templates select="//Typedef[@context=$nsid and  (not(@access) or @access='public')]|//ArrayType[@context=$nsid and  (not(@access) or @access='public')]">
       <xsl:with-param name="nsname"><xsl:value-of select="$nsname"/></xsl:with-param>
      </xsl:apply-templates>

     //Pyllars namespace defins for sub-namespace
     <xsl:apply-templates select="//Namespace[@context=$nsid]">
       <xsl:with-param name="parentname"><xsl:if test="$parentname!='' or $parentname!='::'"><xsl:value-of select="$parentname"/>::</xsl:if><xsl:if test="$nsname!='::'"><xsl:value-of select="$nsname"/></xsl:if></xsl:with-param>
     </xsl:apply-templates>

<xsl:if test="$parentname='' or $parentname='::'">
}
</xsl:if>
   </xsl:template>


   <xsl:template match="//GCC_XML"><xsl:text> 
#include &lt;v8.h&gt;
#include &lt;pyllars/pyllars_pointer.h&gt;
#include &lt;pyllars/pyllars_function_wrapper.h&gt;
#include &lt;pyllars/pyllars_classwrapper.h&gt;
#include &lt;pyllars/pyllars_conversions.h&gt;
#include &lt;Python.h&gt;

using namespace __pyllars_internal;


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

namespace pyllars{
}
   //Pyllars structs/classes for global namespace </xsl:text>
     
   <xsl:apply-templates select="//Namespace[@name='v8']" mode="generate_externs"/> 

   <xsl:apply-templates select="//Namespace[@name='v8']">
      <xsl:with-param name="parentname" select="''"/>
   </xsl:apply-templates>

  </xsl:template>

</xsl:stylesheet>
