// Struct definitions
typedef struct {
    unsigned short isr_low;    // ISR address low offset
    unsigned short kernel_cs;  // GDT segment selector that is loaded into CS
    unsigned char  ist;        // bits 0-2: Interrupt Stack Table offset
    unsigned char  attributes; // bits 0-3: Gate type, (0xE: interrupt gate
                               //                       0xF: trap gate)
                               // bits 4-5: DPL, defines accessible CPU privilege levels
                               // bit    6: Present (must be 1)
    unsigned short isr_mid;    // ISR address mid offset
    unsigned long  isr_high;   // ISR address high offset
    unsigned long  reserved;   // Set to 0
} __attribute__((packed)) idt_entry_t; // packed : no byte padding

typedef struct {
    unsigned short     limit;
    unsigned long long base;
} __attribute__((packed)) idtr_t;

// Var definitions
__attribute__((aligned(0x10))) static idt_entry_t idt[256]; // idt: array of 256 idt entries
static idtr_t idtr;

// Function definitions
__attribute__((noreturn)) void exception_handler(void);
