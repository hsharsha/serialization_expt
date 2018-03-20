#include <iostream>

int main(void)
{
    extern int test_flatbuffers(size_t);
    extern int test_capnp(size_t);
    extern int test_avro_serialization(size_t);
    extern int test_json_serialization(size_t);
    test_flatbuffers(1000000);
    test_capnp(1000000);
    test_avro_serialization(1000000);
    test_json_serialization(1000000);
    return 0;
}
