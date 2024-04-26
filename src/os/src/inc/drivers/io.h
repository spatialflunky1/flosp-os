#ifndef IO_H
#define IO_H

#include <datatypes.h>

//
// PS/2
//

// PS/2 controller ports
#define PS2_DATA_PORT    0x60
#define PS2_COMMAND_PORT 0x64
#define PS2_STATUS_PORT  0x64

// PS/2 controller commands
#define PS2_READ_CONFIG_BYTE            0x20
#define PS2_WRITE_CONFIG_BYTE           0x60
#define PS2_CONTROLLER_TEST             0xAA
#define PS2_DISABLE_PORT                0xAD
#define PS2_DISABLE_PORT_2              0xA7
#define PS2_ENABLE_PORT                 0xAE
#define PS2_ENABLE_PORT_2               0xA8
#define PS2_TEST_PORT                   0xAB
#define PS2_TEST_PORT_2                 0xA9
#define PS2_READ_CONTROLLER_OUTPUT_PORT 0xD0
#define PS2_WRITE_BYTE_INPUT_PORT_2     0xD4

// PS/2 test results
#define PS2_CONTROLLER_TEST_PASS 0x55
#define PS2_CONTROLLER_TEST_FAIL 0xFC
#define PS2_PORT_TEST_PASS       0x00
#define PS2_PORT_TEST_FAIL_CL    0x01
#define PS2_PORT_TEST_FAIL_CH    0x02
#define PS2_PORT_TEST_FAIL_DL    0x03
#define PS2_PORT_TEST_FAIL_DH    0x04

// PS/2 device commands
#define PS2_DISABLE_SCANNING 0xF5
#define PS2_ENABLE_SCANNING  0xF4
#define PS2_IDENTIFY         0xF2

// PS/2 keyboard commands
#define PS2_KEYBOARD_SET_SCANCODE_SET 0xF0
#define PS2_KEYBOARD_SET_SCANCODE_1   0x01

// PS/2 responses
#define PS2_ACK    0xFA
#define PS2_RESEND 0xFE

//
// Programmable Interval Timer (PIT)
//

#define PIT_CHANNEL_0_DATA        0x40
#define PIT_MODE_COMMAND_REGISTER 0x43

//
// Programmable Interrupt Controller
//
#define PIC_COMMAND_MASTER 0x20
#define PIC_DATA_MASTER    0x21
#define PIC_COMMAND_SLAVE  0xA0
#define PIC_DATA_SLAVE     0xA1

// End of Interrupt command
#define PIC_EOI 0x20

void outb(ui16_t port, ui8_t value);
ui8_t inb(ui16_t port);

#endif
