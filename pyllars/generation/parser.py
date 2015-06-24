'''
Created on Jun 23, 2015

@author: jrusnak
'''
import os.path
import lxml

from StringIO import StringIO

import pyllars
from .processor import ResultsProcessor

class CPPParser(object):
    '''
    Class to process a CPP XML definition into a pre-Python
    results file.  @pyllars.generation.ResultProcessor can 
    be invoked on the resulting file to produce the final Python
    '''


    def __init__(self, to_path):
        '''
        Constructor
        '''
        assert(isinstance(to_path, basestring))
        self._output_dir = to_path
        if not os.path.exists(to_path):
            os.makedirs(to_path)
        elif not os.path.isdir(to_path):
            raise Exception("Not a directory: %s"%to_path)
            
        
    
            
    def process_stream_to_py( self, xmlfile):
        '''
        Process xmlfile as a stream or as a (string) file name
        as a CPP XML definition to produce an interim definition for
        Python generation
        
        @raises Exception if xmlfile is a string that does not represent
            a path to an existing file.
        '''
        from lxml import etree
        if isinstance(xmlfile, basestring):
            if not os.path.exists(xmlfile):
                raise Exception("No such file: "+xmlfile)
            with open(xmlfile,'r') as infile:
                return self.process_stream_to_py(infile)
        try:
            xmlfile.seek(0)
        except:
            pass
        class FileResolver(etree.Resolver):
            def resolve(self, url, pubid, context):
                return self.resolve_filename(url, context)

        parser = etree.XMLParser()
        parser.resolvers.add(FileResolver())
        xml_input = etree.parse(xmlfile, parser)
        import pkgutil
        xsltcode = pkgutil.get_data("pyllars", "./xslt/cpp2python.xslt")        
        s_xslt = etree.parse(StringIO(xsltcode))
        transform = etree.XSLT(s_xslt)
        result = str(transform(xml_input)).replace('\r','')
        import tempfile
        with tempfile.TemporaryFile(mode='r+w') as tmp:
            tmp.write(result)
            tmp.seek(0)
            processor = ResultsProcessor(tmp)
            processor.process( to_path=self._output_dir)
            
            
    
    