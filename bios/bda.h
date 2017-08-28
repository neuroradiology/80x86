#pragma once

struct __attribute__((packed)) bios_data_area {
    unsigned short com_addresses[4];
    unsigned short lpt_addresses[4];
    unsigned short equipment_list;
    unsigned char ir_kbd_error_count;
    unsigned short mem_kbytes;
    unsigned char reserved1;
    unsigned char ps2_bios_control;
    unsigned char keyboard_flags[2];
    unsigned char keypad_entry;
    unsigned short kbd_buffer_head;
    unsigned short kbd_buffer_tail;
    unsigned short kbd_buffer[16];
    unsigned char drive_recalibration_status;
    unsigned char diskette_motor_status;
    unsigned char motor_shutoff_count;
    unsigned char diskette_status;
    unsigned char nec_diskette_motor_status[7];
    unsigned char video_mode;
    unsigned short num_screen_cols;
    unsigned short video_regen_buffer_bytes;
    unsigned short video_regen_offset;
    unsigned short cursor_offsets[8];
    unsigned char cursor_end;
    unsigned char cursor_start;
    unsigned char active_page;
    unsigned short crt_controller_base;
    unsigned char crt_mode_control;
    unsigned char cga_pallette_mask;
    unsigned char cassette_take_ctrl[5];
    unsigned short timer_counter_low;
    unsigned short timer_counter_high;
};