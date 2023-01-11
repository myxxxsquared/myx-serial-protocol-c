#!/bin/sh

set -e
set -x

gcc -DMYX_SERIAL_PROTOCOL_TEST=1 -Wall myx-serial-protocol.c myx-serial-protocol-test.c -o myx-serial-protocol-test
./myx-serial-protocol-test
