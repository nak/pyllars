from .base import *
from .namespaces import *
from .fundamentals import *
from .structs import *


def _get_generator(clazz: type, src_path: str, indent: str) -> "Generator":
    generator_class = globals().get(clazz.__name__)
    if not generator_class or not generator_class.is_generatable():
        return None
    if generator_class:
        return generator_class(src_path, indent)
    else:
        logging.error("Did not find generator for class %s" % clazz.__name__)
        return Generator(src_path, indent)
