#include <drivers/vga.h>
#include <kernel/power_mgmt.h>

//
// Global Vars
//
// Output info
ui16_t output_height = 0;
ui16_t output_width = 0;
// Text info
ui16_t text_col_px = 0;
ui16_t text_line = 0;
ui16_t max_text_line = 0;
int text_color = rgb(255,255,255);
int background_color = rgb(0,0,0);
// Font info
ui8_t font_size = 16; // height

void initialize_video(BOOT_VIDEO_MODE_INFO* VMI) {
    VideoModeInfo = VMI;
    output_height = VideoModeInfo->VerticalResolution;
    output_width  = VideoModeInfo->HorizontalResolution;
    max_text_line = (output_height / font_size) - 1;
    blank_output();
}

void blank_output(void) {
    ui32_t* pos;
    for (ui64_t row = 0; row < VideoModeInfo->VerticalResolution; row++) {
        for (ui64_t col = 0; col < VideoModeInfo->HorizontalResolution; col++) {
            pos = VideoModeInfo->FramebufferPointer;
            // Set row
            pos += row * VideoModeInfo->HorizontalResolution;
            // Set cold
            pos += col;

            // Set color
            *pos = rgb(0,0,0);
        }
    }
    text_line = 0;
    text_col_px = 0;
}

void kputchar(const unsigned char c) {
    if (c == 0) {
        // Ignore if null byte
        return;
    }
    else if (c == '\n') {
        text_col_px = 0;
        if (text_line == max_text_line) {
            kscroll_down();
        }
        else {
            text_line += 1;
        }
        return;
    }
    ui32_t* pos = VideoModeInfo->FramebufferPointer;
    // Advance to current line
    pos += text_line * font_size * VideoModeInfo->HorizontalResolution;
    // Advance to current column
    pos += ((text_col_px)  + 8) - 1; // font width is always 8;
                                      // algorithm starts at the last pixel of the line
    ui32_t* last_pos = pos; // pos at the beginning of the line
    const unsigned char* ch_addr;
    switch (font_size) {
        case 8:
            ch_addr = &vgafont8[c * font_size];
            break;
        case 14:
            ch_addr = &vgafont14[c * font_size];
            break;
        case 16:
            ch_addr = &vgafont16[c * font_size];
            break;
        default:
            return;
    }
    for (unsigned int i = 0; i < font_size; i++) { 
        unsigned char ch = *ch_addr;
        for (ui8_t j = 0; j < 8; j++) {
            if (ch % 2 == 1) { // last bit is set
                *pos = text_color;
            }
            else {
                *pos = background_color;
            }
            ch = ch >> 1;
            pos--;
        }
        ch_addr++;
        last_pos += VideoModeInfo->HorizontalResolution;
        pos = last_pos;
    }
    text_col_px += 9; // 9 instead of 8 to add a space in between chars printed
}

void kscroll_down(void) {
    // start at 2nd line
    ui32_t* pos = VideoModeInfo->FramebufferPointer;
    pos += font_size * VideoModeInfo->HorizontalResolution;
    // Move all pixels up by font_size lines
    for (ui16_t row = 0; row < font_size * max_text_line; row++) {
        memcpy(pos,
                (pos - (font_size * VideoModeInfo->HorizontalResolution)),
                VideoModeInfo->HorizontalResolution * 4);
        pos += VideoModeInfo->HorizontalResolution;
    }
    // Blank last line
    pos = VideoModeInfo->FramebufferPointer;
    pos += max_text_line * font_size * VideoModeInfo->HorizontalResolution;
    for (ui16_t row = 0; row < font_size; row++) {
        for (ui16_t col = 0; col < VideoModeInfo->HorizontalResolution; col++) {
            *pos = rgb(0, 0, 0);
            pos++;
        }
    }
}

void kprint(const char *s) {
    while (*s != '\0') {
        kputchar(*s);
        s++;
    }
}

void kprint_hex(ui64_t h, const bool zero_x) {
    char  string_buffer[UI64_T_MAX_HEX_DIGITS];
    ui8_t stack_top = -1;
    // Append the hex digits to the string buffer
    if (h == 0) {
        stack_top++;
        string_buffer[stack_top] = '0';
    }
    ui16_t tmp0;
    while(h != 0) {
        stack_top++;
        tmp0 = h % 16;
        if (tmp0 >= 10) {
            string_buffer[stack_top] = (char)(tmp0+55);
        }
        else {
            string_buffer[stack_top] = (char)(tmp0+48);
        }
        h /= 16;
    }

    // Reverse digits into the correct order in the StringBuffer
    i8_t tmp_index = stack_top;
    char tmp1;
    for (; tmp_index > (stack_top/2); tmp_index--) {
        tmp1 = string_buffer[stack_top - tmp_index];
        string_buffer[stack_top - tmp_index] = string_buffer[tmp_index];
        string_buffer[tmp_index] = tmp1;
    }
    // Add the null byte
    string_buffer[stack_top+1] = '\0';
    
    // Print the created string
    if (zero_x) {
        kprint("0x");
    }
    kprint(string_buffer);
}

void kprint_num(ui64_t n) {
    char  string_buffer[UI64_T_MAX_DEC_DIGITS];
    ui8_t stack_top = -1;
    // Append the dec digits to the string buffer
    if (n == 0) {
        stack_top++;
        string_buffer[stack_top] = '0';
    }
    ui16_t tmp0;
    while(n != 0) {
        stack_top++;
        tmp0 = n % 10;
        string_buffer[stack_top] = (char)(tmp0+48);
        n /= 10;
    }

    // Reverse digits into the correct order in the StringBuffer
    i8_t tmp_index = stack_top;
    char tmp1;
    for (; tmp_index > (stack_top/2); tmp_index--) {
        tmp1 = string_buffer[stack_top - tmp_index];
        string_buffer[stack_top - tmp_index] = string_buffer[tmp_index];
        string_buffer[tmp_index] = tmp1;
    }
    // Add the null byte
    string_buffer[stack_top+1] = '\0';
    
    // Print the created string
    kprint(string_buffer);
}

void kprint_bin(ui64_t b, ui8_t size_b) {
    char  string_buffer[UI64_T_MAX_BIN_DIGITS];
    ui8_t stack_top = -1;
    // Append the dec digits to the string buffer
    if (b == 0) {
        stack_top++;
        string_buffer[stack_top] = '0';
    }
    ui16_t tmp0;
    for (ui16_t i = 0; i < (size_b*8); i++) {
        stack_top++;
        tmp0 = b % 2;
        string_buffer[stack_top] = (char)(tmp0+48);
        b /= 2;
    }
    // Reverse digits into the correct order in the StringBuffer
    i8_t tmp_index = stack_top;
    char tmp1;
    for (; tmp_index > (stack_top/2); tmp_index--) {
        tmp1 = string_buffer[stack_top - tmp_index];
        string_buffer[stack_top - tmp_index] = string_buffer[tmp_index];
        string_buffer[tmp_index] = tmp1;
    }
    // Add the null byte
    string_buffer[stack_top+1] = '\0';
    
    // Print the created string
    kprint(string_buffer); 
}

void backspace(void) {
    if (text_col_px >= 9) {
        text_col_px -= 9; // One font width back for printing
    }
    else {
        return;
    }
    kputchar(' ');
    text_col_px -= 9; // One font width back for next char 
}
