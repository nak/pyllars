<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    exclude-result-prefixes="xs"
    version="2.0">
    <xsl:output method="text"/>
    <xsl:template match="Argument" mode="get_quoted_names">&quot;<xsl:choose><xsl:when test="@na,e"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_<xsl:value-of select="position()-1"/></xsl:otherwise></xsl:choose>&quot;<xsl:if test="position()!=last()">, </xsl:if>
    </xsl:template>
  <xsl:template match="//*" mode="get_basic_C_name"><xsl:variable name="name"><xsl:choose>
    <xsl:when test="@name  and @name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_Sruct_<xsl:value-of select="@id"/></xsl:otherwise>
  </xsl:choose></xsl:variable><xsl:value-of select="$name"/></xsl:template>
  <xsl:template match="//*" mode="get_basic_C_wrapper_name"><xsl:variable name="name"><xsl:choose>
    <xsl:when test="@name and @name!=''"><xsl:value-of select="@name"/></xsl:when><xsl:otherwise>Anon_Sruct_<xsl:value-of select="@id"/></xsl:otherwise>
  </xsl:choose></xsl:variable><xsl:value-of select="translate($name,'&lt;&gt;:, ','_____')"/></xsl:template>
  <xsl:template match="//*" mode="get_full_C_name"><xsl:variable name="context" select="@context"/><xsl:choose>
      <xsl:when test="//*[@id=$context]/@name='::' or not(//*[@id=$context]/@name)"></xsl:when>
      <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_full_C_name"/>::</xsl:otherwise></xsl:choose><xsl:apply-templates mode="get_basic_C_name" select="."/>
    </xsl:template>
  <xsl:template match="//*" mode="get_full_wrapper_name"><xsl:variable name="context" select="@context"/><xsl:choose>
    <xsl:when test="//*[@id=$context]/@name='::' or not(//*[@id=$context]/@name)"></xsl:when>
    <xsl:otherwise><xsl:apply-templates select="//*[@id=$context]" mode="get_full_C_name"/>___</xsl:otherwise></xsl:choose><xsl:apply-templates mode="get_basic_C_wrapper_name" select="."/>
  </xsl:template>
  
    <xsl:template match="Struct|Class|PointerType|ReferenceType|CvQualifiedType" mode="generate_python_wrapper_class">
      <xsl:variable name="classid" select="@id"/>
      <xsl:variable name="classname"><xsl:apply-templates select="." mode="get_full_C_name"/></xsl:variable>
      /*********
      * Class to define Python wrapper to C class/type
      **/
      struct <xsl:apply-templates select="." mode="get_full_wrapper_name"/>PythonWrapper:
         public PythonWrapper&lt;<xsl:apply-templates select="." mode="get_full_C_name"/>&gt;{
         
      
          static int init( PythonWrapper* self, PyObject* args, PyObject*kwds){
             if (!self) return -1;
             {<xsl:for-each select="//Constructor[@context=$classid]"><xsl:choose>
          <xsl:when test="count(./Argument)">
            if (status != 0){
                const char* kwlist[] = {<xsl:apply-templates select="./Argument" mode="get_quoted_names"/>}<xsl:if test="position()!=last()">,</xsl:if>};
                char format[<xsl:value-of select="count(./Argument)"/>] = {'O'};
                PyObejct* pyargs[<xsl:value-of select="count(./Argument)"/>] = {NULL};
                status = PyArg_ParseTupleAndKeywords(args, kwds, format, kwlist, <xsl:for-each select="./Argument">&amp;pyargs[<xsl:value-of select="position()-1"/>]<xsl:if test="position()!=last()">, </xsl:if></xsl:for-each>);
                self-&gt;_CObject = new Ctype( <xsl:for-each select="./Argument"><xsl:variable name="type" select="@type"/>toCObject&lt;<xsl:apply-templates select="//*[@id=$type]" mode="get_full_C_name"/>&gt;(pyargs[<xsl:value-of select="position()-1"/>]<xsl:if test="position()=last()">, </xsl:if></xsl:for-each>);
            }            
          </xsl:when>
          <xsl:otherwise>
            self-&gt;_CObject = new Ctype();
            status = 0;
            
          </xsl:otherwise>
        </xsl:choose>
            </xsl:for-each>
            return status;
        
          }
      };
      
    </xsl:template>
    
    <xsl:template match="Namespace" mode="generate_python">
      <xsl:variable name="scopeid" select="@id"/>
      <xsl:apply-templates select="//*[@context=$scopeid]" mode="generate_python_wrapper_class"/>
    </xsl:template>
    
    <xsl:template match="//GCC_XML"><xsl:text> 
        
#include &lt;functional&gt;
#include &lt;Python.h&gt;

namespace __pyllars_internal{
  
  //
  // template fnction to convert python to C object
  //
  template&lt; typename return_type, typename py_type&gt;
  return_type toCObject( PyObject&amp; pyobj){
    return dynamic_cast&lt;py_type&amp;&gt;(pyobj).get_Cobject();
  }
  
  ///////////////////////////////////////////////////////
  /// explicit instantiations for fundamental types

  template&lt;&gt;
  long int toCObject&lt; long int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  int toCObject&lt; int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }


  template&lt;&gt;
  short int toCObject&lt; short int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  char toCObject&lt; char, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  unsigned long int toCObject&lt; unsigned long int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  unsigned int toCObject&lt; unsigned int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  unsigned short int toCObject&lt; unsigned short int, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }


  template&lt;&gt;
  unsigned char toCObject&lt; unsigned char, PyObject&gt;( PyObject&amp; pyobj){
    if (PyInt_Check( &amp;pyobj)){
      return PyInt_AsLong( &amp;pyobj );
    } 
    return PyLong_AsLong( &amp;pyobj );
  }

  template&lt;&gt;
  float toCObject&lt; float, PyObject&gt;( PyObject&amp; pyobj){
    return PyFloat_AsDouble( &amp;pyobj );
  }

  template&lt;&gt;
  double toCObject&lt; double, PyObject&gt;( PyObject&amp; pyobj){
    return PyFloat_AsDouble( &amp;pyobj );
  }
  
  //
  //////////////////////////////////////////


  /**
   * template to convert c object to python
   **/
  template&lt; typename c_type&gt;
  PyObject* toPyObject( const c_type &amp; var){
    return Py_None;//XSLT HERE
  }

  ///////////////////////////////////////////////////
  // explicit instantiations

  template&lt;&gt;
  PyObject* toPyObject&lt;char&gt;( const char &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;short&gt;( const short &amp; var){
    return PyInt_FromLong( var);
  }
  
  template&lt;&gt;
  PyObject* toPyObject&lt;int&gt;( const int &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;long&gt;( const long &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;unsigned char&gt;( const unsigned char &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;unsigned short&gt;( const unsigned short &amp; var){
    return PyInt_FromLong( var);
  }
  
  template&lt;&gt;
  PyObject* toPyObject&lt;unsigned int&gt;( const unsigned int &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;unsigned long&gt;( const unsigned long &amp; var){
    return PyInt_FromLong( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;float&gt;( const float &amp; var){
    return PyFloat_FromDouble( var);
  }

  template&lt;&gt;
  PyObject* toPyObject&lt;double&gt;( const double &amp; var){
    return PyFloat_FromDouble( var);
  }

 
  ///
  //////////////////////////////////

  /**
   * template class for a pool of C-style callbacks
   * mapped to python-function-callbacks.  There is
   * probably some magic way through ffi to do this,
   * but didn't want to go that route just yet
   **/
  template &lt;  const size_t count, 
	      typename return_type,
	      typename pyclass,
	      typename... Args&gt;
  class CallbackPool{
  public:
    
    typedef return_type(*callback_t)( Args...);
    
    static PyObject *pycallbacks[count];
    static callback_t c_callbacks[count];


  template&lt; const size_t index&gt;
  static return_type cb( Args...  args){
    PyObject pyargs[] = { *toPyObject(args)...};
    PyObject* result = PyObject_CallObject( pycallbacks[index],
					    pyargs);
    if (!result){ throw "Invalid argument";}
    return toCObject&lt;return_type, pyclass&gt;(*result);

  }  

  
  };




  template &lt; const size_t count,
	     typename return_type,
	     typename pyclass,
	     typename... Args&gt;
  PyObject* CallbackPool&lt; count, return_type,pyclass, Args...&gt;::pycallbacks[count];
  
  template &lt; const size_t count,
	     typename return_type,
	     typename pyclass,
	     typename... Args&gt;
  typename CallbackPool&lt; count, return_type,pyclass, Args...&gt;::callback_t
  CallbackPool&lt; count, return_type,pyclass, Args...&gt;::c_callbacks[count];
  
  
  template &lt; const size_t count,
	     const size_t index,
	     typename return_type,
	     typename pyclass,
	     typename... Args&gt;
  static return_type __cb( Args...  args){
    typedef CallbackPool&lt;count, return_type, pyclass, Args...&gt; Pool;
    PyObject pyargs[] = { *toPyObject(args)...};
    PyObject* result = PyObject_CallObject( Pool::pycallbacks[index],
					    pyargs);
    if (!result){ throw "Invalid argument";}
    return toCObject&lt;return_type, pyclass&gt;(*result);
    
  }  
  

  /*****************************
   ** This code initializes all the callback function 
   ** pointers recursively before main hits
   **/
  template &lt;  const size_t count, 
	      const size_t index,
	      typename return_type,
	      typename pyclass,
	      typename... Args
	      &gt;
  class CBPoolInitializer{
  public:
    
    CBPoolInitializer(){
      typedef CallbackPool&lt;count, return_type, pyclass, Args...&gt; Pool;
      Pool::c_callbacks[index-1] = &amp;__cb&lt;count, index, return_type, pyclass, Args...&gt;;
      CBPoolInitializer&lt;count, index-1, return_type, pyclass, Args...&gt;();
    }
   
  };
  
  template &lt;  const size_t count, 
	    typename return_type,
	    typename pyclass,
	    typename... Args
	   &gt;
  class CBPoolInitializer&lt;count, 0, return_type, pyclass, Args...&gt;{
  public:
    typedef CallbackPool&lt;count, return_type, pyclass, Args...&gt; Pool;
    
    CBPoolInitializer(){
    }
    
  };
  

  //XSLT HERE
  #define MAX_CB_POOL_DEPTH 898
  CBPoolInitializer&lt; MAX_CB_POOL_DEPTH, MAX_CB_POOL_DEPTH,
		   int, PyObject, int , double&gt; initializer;



  /***************
   * wrapper to python callback, mapping to C-style callback
   **/
  template&lt; typename pyclass, 
	    typename return_type, typename... Args&gt;
  class PyCallbackWrapper{
  public:
    
    typedef CallbackPool&lt; MAX_CB_POOL_DEPTH, 
			  return_type, 
			  pyclass, 
			  Args...&gt; Pool;
    
    PyCallbackWrapper( PyObject* pycb){
      if (cb_index &gt;= MAX_CB_POOL_DEPTH){
	throw "Callbaks exhausted";
      }
      Pool::pycallbacks[cb_index] = pycb;
      Py_INCREF(pycb);
      _my_cb_index = cb_index++;    
    }
    
    
    typename Pool::callback_t get_C_callback(){
      return Pool::c_callbacks[_my_cb_index];
    }
    
    
  private:
    static size_t cb_index ;
    size_t _my_cb_index;
    
  };
  
  
  
  template&lt; typename pyclass, 
	    typename return_type, typename... Args&gt;
  size_t PyCallbackWrapper&lt;pyclass, return_type, Args...&gt;::cb_index = 0;
  
  

  /**
   * helper template function to call C class method with Python args/return
   **/
  template&lt; typename Ctype, typename returnType, typename... Args&gt;
  static PyObject* callMethod( returnType(Ctype::*method)(Args...),
			       PythonWrapper&lt;Ctype&gt;* self,
			       PyObject* const args,
			       PyObject* const kwds,
			       const char* kwlist[],
			       Args&amp;... pyargs){
    if (!self) return -1;
    char format[sizeof...(pyargs)] = {'O'};

    int status = PyArg_ParseTupleAndKeywords(args, kwds, format, kwlist, &amp;pyargs...);
    if (status == 0){
      returnType result = self-&gt;CObject-&gt;*method( toCObject(pyargs)...);
      return toPyObject&lt;returnType&gt;(result);
    } 
    return NULL;/*error*/
  }
  
   /*********
   * Class to define Python wrapper to C class/type
   **/
   template&lt; typename Ctype&gt;
   struct PythonWrapper{
          PyObject_HEAD
      
          Ctype* get_CObject();
      
       private:
          Ctype* _Cobject;
      
          static PyObject* _new( PyTypeObject* type, PyObject* args, PyObject*kwds){
              PythonWrapper  *self;
              self = (PythonWrapper*)type -&gt;tp_alloc(type, 0);
              if ( NULL != self){
                  self-&gt;_CObject = NULL;
              }
              return (PyObject*) self;
          }
      
      
          static void dealloc( PythonWrapper* self){
              if(!self) return;
                  delete self-&gt;_CObject;
                  self-&gt;_CObject = NULL;
                  self-&gt;ob_type-&gt;tp_free((PyObject*)self);
              }
          } 
   };
  
}

    </xsl:text><xsl:apply-templates select="//Namespace[@name!='::']" mode="generate_python"/></xsl:template>
    
</xsl:stylesheet>