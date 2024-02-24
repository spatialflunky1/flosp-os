#ifndef IDT_H
#define IDT_H

#include <drivers/vga.h>
#include <kernel/datatypes.h>
#include <kernel/message.h>
#include <kernel/power_mgmt.h>

#define MAX_IDT_ENTRIES 256

#define PIC_DATA_MASTER 0x21
#define PIC_DATA_SLAVE  0xA1

extern void* isr_stub_table[];

// Struct definitions
typedef struct {
    ui16_t isr_low;    // ISR address low offset
    ui16_t kernel_cs;  // Segment selector that is loaded into CS
    ui8_t  ist;        // Interrupt Stack Table offset, will be set 0 to disable the IST mechanism
    ui8_t  attributes; // bits 0-3: Gate type, (0xE: interrupt gate
                               //               0xF: trap gate)
                               // bit    4: Reserved (set 0)
                               // bits 5-6: DPL, defines accessible CPU privilege levels
                               // bit    7: Present (must be 1)
    ui16_t isr_mid;    // ISR address mid offset
    ui32_t isr_high;   // ISR address high offset
    ui32_t reserved;   // Set to 0
} __attribute__((packed)) idt_entry_t; // packed : no byte padding

typedef struct {
    ui16_t        limit;
    idt_entry_t*    base;
} __attribute__((packed)) idtr_t;

typedef struct {
    ui64_t rdi;
    ui64_t int_vector;
    ui64_t error;
    ui64_t error_code;

    ui64_t iretq_rip;
    ui64_t iretq_cs;
    ui64_t iretq_flags;
    ui64_t iretq_rsp;
    ui64_t iretq_ss;
} cpu_status_t;

typedef struct {
    ui64_t rdi;
    ui64_t irq_vector;

    ui64_t iretq_rip;
    ui64_t iretq_cs;
    ui64_t iretq_flags;
    ui64_t iretq_rsp;
    ui64_t iretq_ss;
} cpu_status_irq_t; // same as non-irq just no error

// Var definitions
__attribute__((aligned(0x10))) static idt_entry_t idt[MAX_IDT_ENTRIES]; // idt: array of idt entries
static idtr_t idtr;

// Function definitions
cpu_status_t* exception_handler(cpu_status_t* cpu_status);
cpu_status_irq_t* irq_handler(cpu_status_irq_t* cpu_status);
void idt_set_descriptor(ui8_t vect, void* isr, ui8_t flags);
void mask_pic_interrupts();
void idt_init(void);

#endif
