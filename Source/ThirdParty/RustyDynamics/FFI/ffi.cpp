#include <iostream>
#include <cstdint>
#include "../RustyDynamics.h"

int main (void)
{
    TestEntity entities[] = {
        {0.0, 4.0},
        {10.0, 20.5},
    };
    TestWorld world = {
        (uint64_t)entities,
        4,
        2,
    };
    std::cout << "num pow: " << rd_get_pow_2_of_int32(3) << "\n";
    rd_netclient_test_world(&world);

    return 0;
}
