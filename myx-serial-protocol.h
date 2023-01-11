
#ifndef __MYX_SERIAL_PROTOCOL_H__
#define __MYX_SERIAL_PROTOCOL_H__

#include <stddef.h>

enum {
    MYX_SERIAL_RECV_INCOMPLETE,
    MYX_SERIAL_RECV_COMPLETE,
    MYX_SERIAL_RECV_ERROR_ID,
    MYX_SERIAL_RECV_ERROR_LENGTH,
    MYX_SERIAL_RECV_ERROR_CHECKSUM,
    MYX_SERIAL_RECV_ERROR_TIMEOUT
};

typedef struct _myx_serial_receiver {
    char* storage;
    size_t receive_max;
    size_t received;
    size_t length;
    int is_correct;
    int has_ticks;
    char id;
} myx_serial_receiver;

const char* myx_serial_get_lasterror();
char myx_serial_checksum(size_t len, char* data);
char myx_serial_checksum_raw(char id, size_t len, char* data);
intptr_t myx_serial_pack(char id, size_t len, char* data, size_t buf_len,
                         char* buffer);
intptr_t myx_serial_send(char id, size_t len, char* data,
                         void (*send_callback)(char data));
intptr_t myx_serial_receiver_init(myx_serial_receiver* receiver, char id,
                                  size_t buffer_len, char* buffer);
intptr_t myx_serial_receiver_receive(myx_serial_receiver* receiver, char data);
intptr_t myx_serial_receiver_tick(myx_serial_receiver* receiver);
inline char* myx_serial_receiver_last_buffer(myx_serial_receiver* receiver) {
    return receiver->storage;
}
inline size_t myx_serial_receiver_last_length(myx_serial_receiver* receiver) {
    return receiver->length;
}

#endif  // __MYX_SERIAL_PROTOCOL_H__
