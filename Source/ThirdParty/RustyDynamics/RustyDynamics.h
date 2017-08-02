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

    // typedef struct RustyPing {
    //     uint64_t uuid_ptr;
    //     uint64_t uuid_cap;
    //     uint64_t uuid_len;
    // } Ping;

    // typedef struct RigidBodyPack {
    //     uint8_t id;
    //     float px, py, pz;
    //     float lx, ly, lz;
    //     float rx, ry, rz;
    //     float ax, ay, az;
    // } RigidBodyPack;

    int32_t rd_get_pow_2_of_int32(int32_t num);
    void* rd_netclient_open(const char* addr);
    void rd_netclient_drop(void* client);
    void rd_netclient_uuid(void* client, char* uuid);
    void rd_netclient_msg_push(void* client, const char* bytes, uint32_t count);
    uint32_t rd_netclient_msg_pop(void* client, char* uuid);

    void rd_netclient_test_world(const TestWorld* world);

    // void rd_netclient_msg_push_ping(void* client, const RustyPing* ping);
    // RustyPing* rd_netclient_msg_dec_ping(const char* bytes, uint32_t count);
    // void rd_netclient_msg_drop_ping(RustyPing* ping);

}
