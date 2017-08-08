#pragma once

extern "C" {

    typedef struct TestEntity {
        float x;
        float y;
    } TestEntity;

    typedef struct TestWorld {
        uint64_t entities_ptr;
        uint64_t entities_cap;
        uint64_t entities_len;
    } TestWorld;

    typedef struct RustVec {
        uint64_t vec_ptr;
        uint64_t vec_cap;
        uint64_t vec_len;
    } RustVec;

    typedef struct AvatarPack {
        uint8_t id;
        float px, py, pz, pw;
        float rx, ry, rz, rw;
    } AvatarPack;

    typedef struct RigidBodyPack {
        uint8_t id;
        float px, py, pz, pw;
        float lx, ly, lz, lw;
        // float rx, ry, rz;
        // float ax, ay, az;
    } RigidBodyPack;

    typedef struct WorldPack {
        RustVec avatarparts;
        RustVec rigidbodies;
    } WorldPack;

    int32_t rd_get_pow_2_of_int32(int32_t num);
    void* rd_netclient_open(const char* addr);
    void rd_netclient_drop(void* client);
    void rd_netclient_uuid(void* client, char* uuid);
    void rd_netclient_msg_push(void* client, const char* bytes, uint32_t count);
    RustVec* rd_netclient_msg_pop(void* client);
    void rd_netclient_msg_drop(RustVec* msg);

    void rd_netclient_push_world(void* client, const WorldPack* world);
    WorldPack* rd_netclient_dec_world(const char* bytes, uint32_t count);
    void rd_netclient_drop_world(WorldPack* world);

    void rd_netclient_test_world(const TestWorld* world);
}
