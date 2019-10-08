#ifndef _ANONYMOUS__
#define _ANONYMOUS__

namespace unused{
}

typedef char * str;
typedef const char* const const_str;


namespace outside{
    class ExternalDependency{
    };
}

namespace trial{

    namespace AnonymousInnerTypes{

        class AnonInner{
        public:
            enum {FIRST=1, SECOND=2, FOURTH=4};

            struct {
               unsigned int word1: 4;
               unsigned word2: 4;
               struct {
                  bool bool1:1;
               };
            } anon_struct_instance;

            struct{
                int anon1;
                struct{
                    int recursiveAnon2;
                };
            };


            union {
               int intval;
               struct{
                  short sval1;
                  short sval2;
               };
            };

        };

    }

}

#endif
