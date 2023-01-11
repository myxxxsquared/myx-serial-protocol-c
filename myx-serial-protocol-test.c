
#ifdef MYX_SERIAL_PROTOCOL_TEST

#include "myx-serial-protocol.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct _receiving_status {
    myx_serial_receiver* receiver;
    char* buffer;
    int len;
    int received_last;
} receiving_status;

void receiver_send_callback(char data, void* extra) {
    receiving_status* status = (receiving_status*)extra;
    myx_serial_receiver* receiver = status->receiver;
    int result = myx_serial_receiver_receive(receiver, data);
    switch (result) {
        case MYX_SERIAL_RECV_INCOMPLETE: {
            if (status->received_last) {
                printf("Error: Incomplete after complete.\n");
            }
            break;
        }
        case MYX_SERIAL_RECV_COMPLETE: {
            if (status->received_last) {
                printf("Error: Duplicate complete.\n");
                break;
            }
            status->received_last = 1;
            if (status->len != myx_serial_receiver_last_length(receiver)) {
                printf("Error: Length mismatch.\n");
                break;
            }
            char* received = myx_serial_receiver_last_buffer(receiver);
            for (int i = 0; i < status->len; i++) {
                if (status->buffer[i] != received[i]) {
                    printf("Error: Data mismatch.\n");
                    break;
                }
            }
            break;
        }
    }
}

int main() {
    const char ID = 0x03;
    const int MAX_LEN = 32;
    const int TEST_RUN = 128;

    char buffer[MAX_LEN + 3];
    myx_serial_receiver receiver;
    myx_serial_receiver_init(&receiver, ID, sizeof(buffer), buffer);

    char sending_buf[MAX_LEN];

    for (int i = 0; i < TEST_RUN; i++) {
        int current_len = rand() % MAX_LEN;
        for (int j = 0; j < current_len; j++) {
            sending_buf[j] = rand() & 0xff;
        }
        receiving_status status;
        status.receiver = &receiver;
        status.buffer = sending_buf;
        status.len = current_len;
        status.received_last = 0;
        myx_serial_send(ID, current_len, sending_buf, receiver_send_callback,
                        &status);
    }

    return 0;
}

#endif  // MYX_SERIAL_PROTOCOL_TEST
