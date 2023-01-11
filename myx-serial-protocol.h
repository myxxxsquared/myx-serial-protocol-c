
#ifndef __MYX_SERIAL_PROTOCOL_H__
#define __MYX_SERIAL_PROTOCOL_H__

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
    int receive_max;
    int received;
    int length;
    int is_correct;
    int has_ticks;
    char id;
} myx_serial_receiver;

const char* myx_serial_get_lasterror();
char myx_serial_checksum(int len, char* data);
char myx_serial_checksum_raw(char id, int len, char* data);
int myx_serial_pack(char id, int len, char* data, int buf_len,
                         char* buffer);
int myx_serial_send(char id, int len, char* data,
                         void (*send_callback)(char data, void* extra), void* extra);
int myx_serial_receiver_init(myx_serial_receiver* receiver, char id,
                                  int buffer_len, char* buffer);
int myx_serial_receiver_receive(myx_serial_receiver* receiver, char data);
int myx_serial_receiver_tick(myx_serial_receiver* receiver);
inline char* myx_serial_receiver_last_buffer(myx_serial_receiver* receiver) {
    return receiver->storage;
}
inline int myx_serial_receiver_last_length(myx_serial_receiver* receiver) {
    return receiver->length;
}

#endif  // __MYX_SERIAL_PROTOCOL_H__
