
#include "publicmembers.hpp"

namespace trial{

    double PublicMembers::static_vararg_method(const char* const pattern, ...){
        return 1.23456;
    }

    void PublicMembers::static_method(const PublicMembers i){
    }

    double PublicMembers::dbl_static_member = 4.2;


}
