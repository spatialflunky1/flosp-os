#ifndef IDT_H
#define IDT_H

#include "../drivers/vga.h"
#include "datatypes.h"

#define MAX_IDT_ENTRIES 256

extern void* isr_stub_table[];

// Struct definitions
typedef struct {
    uint16_t isr_low;    // ISR address low offset
    uint16_t kernel_cs;  // Segment selector that is loaded into CS
    uint8_t  ist;        // Interrupt Stack Table offset, will be set 0 to disable the IST mechanism
    uint8_t  attributes; // bits 0-3: Gate type, (0xE: interrupt gate
                               //                 0xF: trap gate)
                               // bit    4: Reserved (set 0)
                               // bits 5-6: DPL, defines accessible CPU privilege levels
                               // bit    7: Present (must be 1)
    uint16_t isr_mid;    // ISR address mid offset
    uint32_t isr_high;   // ISR address high offset
    uint32_t reserved;   // Set to 0
} __attribute__((packed)) idt_entry_t; // packed : no byte padding

typedef struct {
    uint16_t        limit;
    idt_entry_t*    base;
} __attribute__((packed)) idtr_t;

typedef struct {
    uint64_t rdi;
    uint64_t int_vector;
    uint64_t error;

    uint64_t iretq_rip;
    uint64_t iretq_cs;
    uint64_t iretq_flags;
    uint64_t iretq_rsp;
    uint64_t iretq_ss;
} cpu_status_t;

// Var definitions
__attribute__((aligned(0x10))) static idt_entry_t idt[MAX_IDT_ENTRIES]; // idt: array of idt entries
static idtr_t idtr;

// Function definitions
void exception_handler(cpu_status_t* cpu_status);
void idt_set_descriptor(uint8_t vect, void* isr, uint8_t flags);
void idt_init(void);

#endif
