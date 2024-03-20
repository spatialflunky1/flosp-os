/* Physical loading address of the kernel */
KERNEL_PHYS_START = 1M;
/* Size of the kernel stack */
KERNEL_STACK_SIZE = 0x4000;
/* Kernel entry point function */
ENTRY (main)
/* Memory segments */
SECTIONS
{
    . = KERNEL_PHYS_START;
    kernel_start = .;

    .text : ALIGN (4K)
    {
        *(.text*)
    }

    .rodata : ALIGN (4K)
    {
        *(.rodata*)
    }

    .data : ALIGN (4K)
    {
        *(.data*)
    }

    .bss : ALIGN (4K)
    {
        *(COMMON)
        *(.bss*)

        . = ALIGN (16);
        stack_bottom = .;
        . += KERNEL_STACK_SIZE;
        stack_top = .;
    }

    kernel_end = .;
}
