#ifndef __PUBLICMEMBERS__
#define __PUBLICMEMBERS__

namespace trial{

  class PublicMembers{
  public:

    static void static_method(const PublicMembers i);

    static double dbl_static_member;

    PublicMembers():const_short_member(42){
        int_members[0] = const_short_member*2;
        int_members[1] = const_short_member*3;
        int_members[2] = const_short_member*4;
    }

    PublicMembers(short v):const_short_member(v){
        int_members[0] = v*2;
        int_members[1] = v*3;
        int_members[2] = v*4;
    }

    static double static_vararg_method(const char* const pattern, ...);

    const short const_short_member;
    int int_members[3];
    char char_matrix[5][8];

  };
}
#endif
