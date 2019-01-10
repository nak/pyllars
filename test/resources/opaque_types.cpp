#include "opaque_types.hpp"

struct Opaque{
    int int_value;
    float float_value;
};

Opaque *opaque_factory(){
    Opaque* new_op = new Opaque();
    new_op->int_value = 0;
    new_op->float_value = 0.0f;
    return new_op;
}

int opaque_get_value(Opaque * const self){
    return self->int_value;
}

void opaque_set_value(Opaque* self, const int value){
    self->int_value = value;
}

float opaque_get_float_value(const Opaque& self){
    return self.float_value;
}

void opaque_set_float_value(Opaque& self, const float value){
    self.float_value = value;
}



