
#include "myx-serial-protocol.h"

#include <stddef.h>

static const char* myx_serial_lasterror = NULL;

const char* myx_serial_get_lasterror() { return myx_serial_lasterror; }

char myx_serial_checksum(int len, char* data) {
    short result = 0;
    for (int i = 0; i < len; i++) {
        result += (short)(data[i]);
    }
    result = (result >> 8) + (result & 0xff);
    result = (result >> 8) + (result & 0xff);
    return result;
}

char myx_serial_checksum_raw(char id, int len, char* data) {
    short result = 0;
    result += (short)(id);
    result += (short)(len & 0xff);
    for (int i = 0; i < len; i++) {
        result += (short)(data[i]);
    }
    result = (result >> 8) + (result & 0xff);
    result = (result >> 8) + (result & 0xff);
    return result;
}

int myx_serial_pack(char id, int len, char* data, int buf_len, char* buffer) {
    if (len > 0xff) {
        myx_serial_lasterror = "Length too long";
        return -1;
    }
    if (buf_len < len + 3) {
        myx_serial_lasterror = "Buffer too short";
        return -1;
    }

    buffer[0] = id;
    buffer[1] = (char)len;
    for (int i = 0; i < len; i++) {
        buffer[i + 2] = data[i];
    }
    buffer[len + 2] = myx_serial_checksum_raw(id, len, data);
    return (int)(len + 3);
}

int myx_serial_send(char id, int len, char* data,
                    void (*send_callback)(char data, void* extra),
                    void* extra) {
    if (len > 0xff) {
        myx_serial_lasterror = "Length too long";
        return -1;
    }
    send_callback(id, extra);
    send_callback((char)len, extra);
    for (int i = 0; i < len; i++) {
        send_callback(data[i], extra);
    }
    send_callback(myx_serial_checksum_raw(id, len, data), extra);
    return (int)(len + 3);
}

int myx_serial_receiver_init(myx_serial_receiver* receiver, char id,
                             int buffer_len, char* buffer) {
    if (buffer_len < 3) {
        myx_serial_lasterror = "Buffer must be longer than 3";
        return -1;
    }
    receiver->storage = buffer;
    receiver->receive_max = buffer_len - 3;
    receiver->received = 0;
    receiver->id = id;
    return 0;
}

int myx_serial_receiver_receive(myx_serial_receiver* receiver, char data) {
    receiver->has_ticks = 0;
    switch (receiver->received) {
        case 0: {
            receiver->storage[receiver->received] = data;
            receiver->received = 1;
            if (data == receiver->id) {
                receiver->is_correct = 1;
                return MYX_SERIAL_RECV_INCOMPLETE;
            } else {
                receiver->is_correct = 0;
                return MYX_SERIAL_RECV_ERROR_CHECKSUM;
            }
            break;
        }
        case 1: {
            receiver->storage[receiver->received] = data;
            receiver->received = 2;
            if (receiver->is_correct) {
                receiver->length = (int)data;
                if (receiver->length > receiver->receive_max) {
                    receiver->is_correct = 0;
                    return MYX_SERIAL_RECV_ERROR_LENGTH;
                } else {
                    return MYX_SERIAL_RECV_INCOMPLETE;
                }
            } else {
                return MYX_SERIAL_RECV_INCOMPLETE;
            }
            break;
        }
        default: {
            if (receiver->is_correct) {
                receiver->storage[receiver->received] = data;
                receiver->received += 1;
                if (receiver->received == receiver->length + 3) {
                    receiver->received = 0;
                    if (myx_serial_checksum(receiver->length + 2,
                                            receiver->storage) == data) {
                        return MYX_SERIAL_RECV_COMPLETE;
                    } else {
                        return MYX_SERIAL_RECV_ERROR_CHECKSUM;
                    }
                } else {
                    return MYX_SERIAL_RECV_INCOMPLETE;
                }
            } else {
                receiver->received += 1;
                if (receiver->received == receiver->length + 3) {
                    receiver->received = 0;
                }
                return MYX_SERIAL_RECV_INCOMPLETE;
            }
            break;
        }
    }
    return MYX_SERIAL_RECV_INCOMPLETE;
}

int myx_serial_receiver_tick(myx_serial_receiver* receiver) {
    if (receiver->received != 0) {
        if (!receiver->has_ticks) {
            receiver->has_ticks = 1;
            return MYX_SERIAL_RECV_INCOMPLETE;
        } else {
            receiver->received = 0;
            return MYX_SERIAL_RECV_ERROR_TIMEOUT;
        }
    }
    return MYX_SERIAL_RECV_INCOMPLETE;
}

char* myx_serial_receiver_last_buffer(myx_serial_receiver* receiver) {
    return receiver->storage + 2;
}

int myx_serial_receiver_last_length(myx_serial_receiver* receiver) {
    return receiver->length;
}