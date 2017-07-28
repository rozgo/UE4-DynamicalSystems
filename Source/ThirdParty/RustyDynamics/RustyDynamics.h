#pragma once

extern "C" {

    int32_t rd_get_pow_2_of_int32(int32_t num);
    void* rd_netclient_open(const char* addr);
    void rd_netclient_drop(void* client);
    void rd_netclient_uuid(void* client, char* uuid);
    void rd_netclient_msg_push(void* client, const char* bytes, uint32_t count);
    uint32_t rd_netclient_msg_pop(void* client, char* uuid);

}
