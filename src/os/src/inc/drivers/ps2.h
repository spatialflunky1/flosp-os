#ifndef PS2_H
#define PS2_H

#include <drivers/io.h>
#include <kernel/message.h>
#include <kernel/timers.h>
#include <kernel/power_mgmt.h>

#define TRANSLATION_KEYBOARD 0
#define TRANSLATION_MOUSE    1
#define TRANSLATION_OTHER    2

#define PS2_TIMEOUT_DURATION 100 // ticks (1000 ms)

typedef struct {
    ui8_t byte_count;
    ui8_t data[3];
} ps2_response;

int   ps2_init(void);
void  flush_ps2_output_buffer(void);
ui8_t read_ps2_controller_config(void);
void  write_ps2_controller_config(ui8_t config_byte);
void  ps2_send_cmd(ui8_t cmd);
void  ps2_write_data(ui8_t data);
ui8_t ps2_read_data(void);
bool  is_ps2_init(void);
int   ps2_port_write_data(ui8_t port, ui8_t b);
ui8_t ps2_pol_recieve_byte(void);
int   ps2_wait_for_ack(ui8_t resend_port, ui8_t resend_command);
ps2_response ps2_port_identify(ui8_t port);
// Port device identity translations:
// 0: keyboard
// 1: mouse
// 2: other
ui8_t ps2_port_identity_translation(ps2_response* port_identity);
bool  ps2_check_error(void);
ps2_response* get_ps2_port_identity(ui8_t port);

#endif
