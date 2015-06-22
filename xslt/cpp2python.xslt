<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
     version="2.0">
    <xsl:param name="libname" select="NONAME"/>
    <xsl:output media-type="text" method="text"></xsl:output>

    <!-- =====================================
      * Get a basic ctypes from a given fundamental type name
      * @type the typedef name to convert to basic type
      ======================================== -->
    <xsl:template name="get_basic_type">
        <xsl:param name="type"/>
        <xsl:choose>
            <xsl:when test="$type='__int128'">(c_longlong*2)</xsl:when>
            <xsl:when test="$type='__uint28'">(c_ulonglong*2)</xsl:when>
            <xsl:when test="$type='long_int'">c_long</xsl:when>
            <xsl:when test="$type='long_unsigned_int'">c_ulong</xsl:when>
            <xsl:when test="$type='signed_char'">c_char</xsl:when>
            <xsl:when test="$type='char'">c_char</xsl:when>
            <xsl:when test="$type='short_int'">c_short</xsl:when>
            <xsl:when test="$type='unsigned_char'">c_byte</xsl:when>
            <xsl:when test="$type='short_unsigned_int'">c_ushort</xsl:when>
            <xsl:when test="$type='int'">c_int</xsl:when>            
            <xsl:when test="$type='unsigned_int'">c_uint</xsl:when>
            <xsl:when test="$type='bool'">c_char</xsl:when>
            <xsl:when test="$type='void'">void</xsl:when>
            <xsl:otherwise>Unknown_type_<xsl:value-of select="$type"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- =============================
      * Get the namespace name for the gvien node
      ================================ -->
    <xsl:template match="//*" mode="get_namespace">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/><xsl:choose>
            <xsl:when test="//Namespace[@id=$context]"><xsl:if test="//Namespace[@id=$context]/@name!='::'"><xsl:apply-templates select="//Namespace[@id=$context]" mode="get_namespace"><xsl:with-param name="id" select="$context"/></xsl:apply-templates>.</xsl:if><xsl:value-of select="//Namespace[@id=$id]/@name"/></xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_namespace"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- =============================
      * Get the namespace node for the gvien node
      ================================ -->
    <xsl:template match="//*" mode="get_ns_context">
        <xsl:variable name="context" select="@context"/>
        <xsl:choose>
            <xsl:when test="count(//Namespace[@id=$context])>0"><xsl:value-of select="//Namespace[@id=$context]/@id"/></xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_ns_context"></xsl:apply-templates></xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="//*" mode="get_contextid">
        <xsl:variable name="context" select="@context"/>
        <xsl:choose>
            <xsl:when test="$context!=''"><xsl:choose>
                <xsl:when test="count(//Namespace[@id=$context])>0"><xsl:value-of select="$context"/></xsl:when>
                <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]"/></xsl:otherwise>
            </xsl:choose></xsl:when>
            <xsl:otherwise><xsl:variable name="type" select="@type"/><xsl:apply-templates select="//*[@id=$type]" mode="get_contextid"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template  match="//*" mode="gen_type_imports">
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="namespace"><xsl:apply-templates select="." mode="get_namespace"/></xsl:variable>
        <xsl:choose>
            <xsl:when test="$context!=''">
                <xsl:if test="count(//Namespace[@id=$context])>0"><xsl:variable name="typename"><xsl:apply-templates select="." mode="typename"/></xsl:variable>
#@add_import(&quot;<xsl:value-of select="$namespace"/>&quot;,&quot;<xsl:value-of select="$typename"/>&quot;)</xsl:if>
                <xsl:apply-templates select="//*[@id=$context]" mode="gen_type_imports"/>
            </xsl:when>
            <xsl:when test="count(//FunctionType[@id=$type])>0"><xsl:text>                
</xsl:text>#@add_func_defn(&quot;func_<xsl:value-of select="@id"/>&quot;,&quot;<xsl:apply-templates select="." mode="typename"/>)</xsl:when>
            <xsl:otherwise><xsl:apply-templates select="//*[@id=$type]" mode="get_type_imports"/></xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template match="//Field|//Argument" mode="gen_imports">
        <xsl:param name="scopeid"/>
        <xsl:variable name="typee" select="@type"/>
        <xsl:apply-templates select="." mode="gen_type_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
    
    <!-- how to transform elements into typenames !-->
    <xsl:template match="Typedef" mode="typename">
<xsl:value-of select="@name"/>
    </xsl:template>
       
    <xsl:template match="CvQualifiedType" mode="typename">
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename"/></xsl:variable>
        <xsl:if test="@const='1'"><xsl:value-of select="$typename"/></xsl:if>
        <xsl:if test="@const!='1'"><xsl:value-of select="$typename"/></xsl:if>
    </xsl:template>
    
    <xsl:template match="FundamentalType" mode="typename">
        <xsl:call-template name="get_basic_type"><xsl:with-param name="type"><xsl:value-of select="translate(@name,' ','_')"/></xsl:with-param></xsl:call-template>    
    </xsl:template>
    
    <xsl:template match="Struct[@name='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
        <xsl:value-of select="$prefix"/>CStruct_Anon_<xsl:value-of select="@id"/>
    </xsl:template>
    
    <xsl:template match="Struct[@name!='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
        <xsl:value-of select="$prefix"/><xsl:value-of select="translate(@name,'&lt;&gt;,:','____')"/>
    </xsl:template>
    
    <xsl:template match="Class[@name!='']" mode="typename">
        <xsl:param name="scopeid"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="." mode="get_ns_context"/></xsl:variable>
        <xsl:variable name="namespace"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable>
        <xsl:variable name="prefix"><xsl:if test="$contextid!=$scopeid"><xsl:value-of select="$namespace"/>.</xsl:if></xsl:variable>
<xsl:value-of select="$prefix"/><xsl:value-of select="translate(@name,'&lt;&gt;,:','____')"/>
    </xsl:template>
    
    
    <xsl:template match="Union" mode="typename">
CUnion_Anon_<xsl:value-of select="@id"/>
    </xsl:template>
    
    <xsl:template match="PointerType|ReferenceType" mode="typename">
        <xsl:variable name="type" select="@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename">
        </xsl:apply-templates></xsl:variable>
        <xsl:choose>
            <xsl:when test="$typename='void'">c_void_p</xsl:when>
            <xsl:when test="$typename='char'">c_char_p</xsl:when>
            <xsl:when test="$typename='c_char'">c_char_p</xsl:when>
            <xsl:otherwise>POINTER(<xsl:value-of select="$typename"/>)</xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <xsl:template match="ArrayType" mode="typename">\
        <xsl:variable name="type" select="@type" />
        <xsl:variable name="max" select="@max"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$type]" mode="typename">
        </xsl:apply-templates></xsl:variable>
<xsl:if test="$max=''">POINTER(<xsl:value-of select="$typename"/>)</xsl:if>
<xsl:if test="$max!=''">(<xsl:value-of select="$typename"/>*<xsl:value-of select="number($max)+1"/>)</xsl:if>
    </xsl:template>
    
    <xsl:template match="FunctionType" mode="typename">
func_<xsl:value-of select="@id"/>
    </xsl:template>
    
    <xsl:template match="Enumeration" mode="typename"><xsl:param name="pos"/>c_uint</xsl:template>
    
    <!-- ================================================== -->
    <!-- Generate field name/type list for ctypes structures-->
    <!-- ================================================== -->
    
    <!-- 
      * Generate the callaable python argument for a call to native lib
      -->
    <xsl:template match="//Argument" mode="gen_callable">
        <xsl:variable name="context" select="@type"/>
        <xsl:variable name="name"><xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/></xsl:otherwise>
        </xsl:choose></xsl:variable>
        <xsl:choose>
            <xsl:when test="count(//ArrayType[@id=$context]|//PointerType[@id=$context]|//FundamentalType[@id=$context]|//Typedef[@id=$context])>0"><xsl:value-of select="$name"/></xsl:when>
            <xsl:otherwise><xsl:value-of select="$name"/>._cobject</xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
 
    <!-- 
      * Generate meta data: import dependencnies and typedefinitions needed for a Field declaration
      * @param scopeid  The id of the Namespace to which the meta data is to be generated
      -->
    <xsl:template match="//Field|//Argument" mode="gen_meta_data">
        <xsl:param name="scopeid"/>
        <xsl:param name="typeid" select="@type"/>
        <xsl:variable name="contextid"><xsl:apply-templates select="//*[@id=$typeid]" mode="get_contextid"/></xsl:variable>
        <xsl:choose><xsl:when test="$contextid!=''">        
        <xsl:if test="$contextid!=$scopeid">
             <xsl:variable name="prefix"><xsl:apply-templates select="//*[@id=$contextid]" mode="get_namespace"/></xsl:variable><xsl:text>
</xsl:text>
<xsl:apply-templates select="//*[@id=$typeid]" mode="gen_type_imports"/>
</xsl:if>    
        </xsl:when><xsl:otherwise>
            <xsl:apply-templates select="."><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param><xsl:with-param name="typeid" select="@type"/></xsl:apply-templates>
        </xsl:otherwise></xsl:choose>
    </xsl:template>

    <!-- 
       * Generate a field definition for a Struct as it would appear in a ctypes field declaration
       -->
    <xsl:template match="//Field" mode="gen_field_entry">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="name"><xsl:choose>
            <xsl:when test="@name!=''"><xsl:value-of select="@name"/></xsl:when>
            <xsl:otherwise>member<xsl:value-of select="@id"/></xsl:otherwise>
        </xsl:choose></xsl:variable>
        <xsl:variable name="type" select="@type"/>(&quot;<xsl:value-of select="$name"/>&quot;,<xsl:apply-templates select="//*[@id=$type]" mode="typename"/>),
    </xsl:template>
   
    <!-- how to transform elements into typedefinitions !-->
    <xsl:template match="FundamentalType" mode="gen_meta_data">
        <xsl:param name="name"></xsl:param><xsl:text>
</xsl:text>#@typedefs[&quot;<xsl:value-of select="$name"/>&quot;] = &quot;<xsl:call-template name="get_basic_type" ><xsl:with-param name="type"><xsl:value-of select="translate(@name,' ','_')"/></xsl:with-param></xsl:call-template>&quot;
    </xsl:template>
    
    <xsl:template match="Struct" mode="typedefinition">
        <xsl:param name="scopeid"/><xsl:variable name="id" select="@id"/>
        <xsl:variable name="prefix"><xsl:if test="@name=''">CStruct_Anon<xsl:value-of select="@id"/></xsl:if></xsl:variable><xsl:text>
</xsl:text><xsl:apply-templates select="//Field[@context=$id]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>
class </xsl:text><xsl:value-of select="$prefix"/><xsl:value-of select="@name"/><xsl:text>( Structure):
    _fields_ = [ </xsl:text><xsl:apply-templates mode="gen_field_entry" select="//Field[@context=$id]"/>]
<xsl:apply-templates select="//Field[@context=$id]" mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>
</xsl:text><xsl:apply-templates select="//Field[@context=$id]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>

</xsl:text><xsl:apply-templates select="//Method[@context=$id]" mode="typedefinition"><xsl:with-param name="typename"><xsl:value-of select="$prefix"/><xsl:value-of select="@name"/>/></xsl:with-param><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
    
    
    <xsl:template match="Union" mode="typedefinition">
        <xsl:param name="pos"/>
        <xsl:param name="scopeid"/>
        <xsl:param name="name"/><xsl:variable name="id" select="@id"/>
        <xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>        

class CUnion_Anon_</xsl:text><xsl:value-of select="@id"/><xsl:text>( Union ):
    _fields_ = [ </xsl:text><xsl:apply-templates mode="gen_field_entry" select="//Field[@context=$id]"/>]
        <xsl:text>
    
</xsl:text>
    </xsl:template>
    
    <xsl:template match="Constructor" mode="generate_def">
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context" select="@context"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$context]" mode="typename"/></xsl:variable><xsl:text>

    @staticmethod
    def new</xsl:text><xsl:value-of select="position()"/>(self, <xsl:for-each select="./Argument">
        <xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/>,</xsl:when><xsl:otherwise>arg<xsl:value-of select="position()"/>,</xsl:otherwise></xsl:choose></xsl:for-each>):<xsl:text>
        </xsl:text><xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:text>
        </xsl:text><xsl:variable name="name"> <xsl:choose><xsl:when test="@name!=''"><xsl:value-of select="@name"/>,</xsl:when>
            <xsl:otherwise>arg<xsl:value-of select="position()"/>,</xsl:otherwise></xsl:choose></xsl:variable><xsl:text>
        assert(isinstance(</xsl:text><xsl:apply-templates select="." mode="gen_callable"/>,<xsl:value-of select="$typename"/>))
        </xsl:for-each><xsl:text>
        obj =  </xsl:text><xsl:value-of select="$typename"/><xsl:text>
        obj._cobject = nativelib.</xsl:text><xsl:apply-templates select="//*[@id=$context]" mode="typename"/>_new(<xsl:for-each select="./Argument"><xsl:apply-templates select="." mode="gen_callable"/>, </xsl:for-each>)<xsl:text>
        return obj
        </xsl:text>
   </xsl:template>
    
    <xsl:template match="Method" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:param name="typename"/>
        <xsl:variable name="mname" select="@name"/>
        <xsl:variable name="count" select="count(//Method[@name=$mname])"/>
        <xsl:variable name="ext"><xsl:if test="$count>1">_<xsl:value-of select="position()"/></xsl:if></xsl:variable>
    def <xsl:value-of select="@name"/><xsl:value-of select="$ext"/>(self, <xsl:for-each select="./Argument"><xsl:value-of select="@name"/>,</xsl:for-each>):<xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:text>
        assert(isinstance(</xsl:text><xsl:apply-templates select="." mode="gen_callable"/>,<xsl:apply-templates select="//*[@id=$typeid]" mode="typename"/>)</xsl:for-each><xsl:text>
        return nativelib.</xsl:text><xsl:value-of select="$typename"/>__<xsl:value-of select="@name"/>(self._cobject, <xsl:for-each select="./Argument"><xsl:apply-templates select="." mode="gen_callable"/>,</xsl:for-each>)
        <xsl:for-each select="./Argument"><xsl:variable name="typeid" select="@type"/><xsl:apply-templates select="//*[@id=$typeid]" mode="gen_imports"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>
        </xsl:for-each> 
        <xsl:apply-templates select=".//Argument" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
  
    <xsl:template match="Class" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="typename"><xsl:apply-templates select="." mode="typename"/></xsl:variable>
        <xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates><xsl:text>        

class </xsl:text><xsl:value-of select="@name"/><xsl:text>( object ):
    </xsl:text><xsl:apply-templates select="//Constructor[@context=$id and @access='public']" mode="generate_def"></xsl:apply-templates>    <xsl:text>
    def __init__(self,*args):
        self._cobject = None</xsl:text><xsl:for-each select="//Constructor[@context=$id and @access='public']"><xsl:text>
        try:
            self._cobject = nativelib.</xsl:text><xsl:value-of select="@name"/>_new<xsl:value-of select="position()"/>(*args)<xsl:text>
         except:
             pass
         </xsl:text></xsl:for-each><xsl:text>
        if self._cobject is None:
             raise Exception("No signature found to create object")

    def copy(self):
        return </xsl:text><xsl:apply-templates select="." mode="typename"/>( self._cobject )<xsl:text>

    def __del__(self)
        nativelib.</xsl:text><xsl:apply-templates select="." mode="typename"/>_delete(self._cobject)
        self._cobjet = None<xsl:text>
            
    </xsl:text><xsl:apply-templates select="//Method[@context=$id]" mode="typedefinition"><xsl:with-param name="typename" select="@name"/><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>

    <xsl:template match="FunctionType" mode="gen_meta_data">
        <xsl:param name="pos"/>
        <xsl:param name="incontext"/>
        <xsl:param name="scopeid"/>
        <xsl:variable name="id" select="@id"/>
        <xsl:variable name="context"><xsl:value-of select="//Typedef[@type=$id]/@context"/></xsl:variable>
          <xsl:variable name="return" select="@returns"/>
        <xsl:variable name="returntype"><xsl:apply-templates select="//*[@id=$return]" mode="typename">
            <xsl:with-param name="pos" select="$pos"/>
        </xsl:apply-templates></xsl:variable><xsl:text>
#@-functiontypes.append(&quot;&quot;&quot;func</xsl:text><xsl:value-of select="@id"/> = CFUNCTYPE( <xsl:value-of select="$returntype"/>, <xsl:for-each select="./Argument">
    <xsl:variable name="argtype" select="@type"/>
#-    <xsl:apply-templates select="//*[@id=$argtype]" mode="typename"><xsl:with-param name="pos" select="$pos"/></xsl:apply-templates>, 
</xsl:for-each>#--)&quot;&quot;&quot;)    
    </xsl:template>   
    
    <xsl:template match="Enumeration" mode="typedefinition"><xsl:text>>

class</xsl:text><xsl:value-of select="translate(@name,'&lt;&gt;,:','____')"/>(object):
<xsl:for-each select="./EnumValue"><xsl:text>
    </xsl:text><xsl:value-of select="@name"/> = <xsl:value-of select="@init"/>
</xsl:for-each>
    </xsl:template>
    
    <xsl:template match="//Typedef" mode="typedefinition">
        <xsl:param name="scopeid"/>
        <xsl:variable name="typeid" select="@type"/>
        <xsl:variable name="typename"><xsl:apply-templates select="//*[@id=$typeid]" mode="typename">
        </xsl:apply-templates></xsl:variable><xsl:text>
            
#typedef
</xsl:text><xsl:value-of select="@name"/> = <xsl:value-of select="$typename"/>
<xsl:apply-templates select="//*[@id=$typeid]" mode="gen_meta_data"><xsl:with-param name="scopeid" select="$scopeid"/></xsl:apply-templates>
    </xsl:template>
  
    <!-- The workhorse to generate the definitions -->
    <xsl:template match="//*" mode="generate_definitions">  
        <xsl:param name="scopeid"/>
        <xsl:variable name="myid" select="@id"/>
        <xsl:for-each select="//Typedef[@context=$myid]|//Class[@context=$myid]|//Struct[@context=$myid]|//Union[@context=$myid]"><xsl:variable name='type' select='@type'/>
            <xsl:variable name="fileid" select="@file"/>
            <xsl:variable name="location" select="//File[@id=$fileid]/@name"/><xsl:if test="not(starts-with($location,'//'))"><xsl:apply-templates mode="typedefinition" select=".">
            <xsl:with-param name="scopeid" select="$scopeid"/>
</xsl:apply-templates></xsl:if></xsl:for-each>
    </xsl:template>
    
    <!-- root processing node -->
    <xsl:template match="/GCC_XML">
        <xsl:for-each select="Namespace"><xsl:variable name="scopeid" select="@id"/>
            <xsl:text>
=====</xsl:text><xsl:value-of select="$libname"/><xsl:text>=======
try:
    libnative = CDLL(&quot;</xsl:text><xsl:value-of select="$libname"/><xsl:text>&quot;)
except:
    libnative = None
    
def init( libname ):
    libnative = CDLL(libname)


====</xsl:text><xsl:apply-templates select="." mode="get_namespace"></xsl:apply-templates>
from ctypes import *
from lib<xsl:value-of select="$libname"/>.function_types import *

libnative = CDLL(lib<xsl:value-of select="$libname"/>.libname())

    
<xsl:apply-templates select="." mode="generate_definitions"><xsl:with-param name="scopeid" select="$scopeid"></xsl:with-param></xsl:apply-templates>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>