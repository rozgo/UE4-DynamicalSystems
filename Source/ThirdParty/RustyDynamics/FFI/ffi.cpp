#include <iostream>
#include <cstdint>
#include "../RustyDynamics.h"

int main (void)
{
    TestEntity entities[] = {
        {0.0, 4.0},
        {10.0, 20.5},
    };
    TestWorld test_world = {
        (uint64_t)entities,
        4,
        2,
    };
    std::cout << "num pow: " << rd_get_pow_2_of_int32(3) << "\n";
    rd_netclient_test_world(&test_world);

    RigidBodyPack bodies[] = {
        {10, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
    };
    AvatarPack parts[] = {
        {10, 1.0, 1.1, 1.2, 1.3, 2.0, 2.1, 2.2, 2.3},
    };

    printf("sizeof<AvatarPack> %lu\n", sizeof(AvatarPack));
    printf("sizeof<RigidBodyPack> %lu\n", sizeof(RigidBodyPack));
    printf("sizeof<RustVec> %lu\n", sizeof(RustVec));
    printf("sizeof<WorldPack> %lu\n", sizeof(WorldPack));

    RustVec bodies_vec;
    bodies_vec.vec_ptr = (size_t)&bodies[0];
    bodies_vec.vec_len = 1;
    bodies_vec.vec_cap = 1;
    
    RustVec parts_vec;
    parts_vec.vec_ptr = (size_t)&parts[0];
    parts_vec.vec_len = 1;
    parts_vec.vec_cap = 1;

    WorldPack real_world;
    real_world.avatarparts = parts_vec;
    real_world.rigidbodies = bodies_vec;

    rd_netclient_real_world(&real_world);

    return 0;
}
