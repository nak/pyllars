struct Opaque;

Opaque *opaque_factory();

int opaque_get_value(Opaque * const self);

void opaque_set_value(Opaque* self, const int value);

float opaque_get_float_value(const Opaque& self);

void opaque_set_float_value(Opaque& self, const float value);



