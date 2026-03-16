#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

// Keycodes from the HID spec
#pragma region HID SPEC
// #define HID_KEY_A 0x04
// #define HID_KEY_B 0x05
// #define HID_KEY_C 0x06
// #define HID_KEY_D 0x07
// #define HID_KEY_F 0x09
// #define HID_KEY_J 0x0D
// #define HID_KEY_K 0x0E
// #define HID_KEY_S 0x16
// #define HID_KEY_W 0x0A

// #define HID_KEY_ARR_UP  0x52
// #define HID_KEY_ARR_DOWN  0x51
// #define HID_KEY_ARR_LEFT  0x50
// #define HID_KEY_ARR_RIGHT  0x4F
#pragma endregion

// Wiring: GPIO PINS
#pragma region PINS
const uint8_t BTN_0 = 14;
const uint8_t BTN_1 = 15;
const uint8_t BTN_2 = 16;
const uint8_t BTN_3 = 17;
const uint8_t BTN_4 = 18;
const uint8_t BTN_5 = 19;
const uint8_t BTN_6 = 20;
const uint8_t BTN_7 = 21;

const uint8_t SYS_WIRE = 22;

const uint8_t SYS_LED_0 = 10;
const uint8_t SYS_LED_1 = 11;
const uint8_t SYS_LED_2 = 12;
const uint8_t SYS_LED_3 = 13;
#pragma endregion

#pragma region TINY_USB
// -------------------------------------------------------
// Sends up to 6 simultaneous keys + a modifier (Shift etc.)
// Pass NULL for keys to send a release (all keys up)
// -------------------------------------------------------
void send_report(uint8_t modifier, uint8_t* keys) {
    if (!tud_hid_ready()) return;

    if (keys) {
        tud_hid_keyboard_report(0, modifier, keys);
    } else {
        tud_hid_keyboard_report(0, 0, NULL);
    }
}

// -------------------------------------------------------
// Start of Required TinyUSB stubs
// -------------------------------------------------------
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen) {
    (void) instance; (void) report_id;
    (void) report_type; (void) buffer; (void) reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const* buffer, uint16_t bufsize) {
    (void) instance; (void) report_id;
    (void) report_type; (void) buffer; (void) bufsize;
}

void tud_hid_report_complete_cb(uint8_t instance,
                                 uint8_t const* report, uint16_t len) {
    (void) instance; (void) report; (void) len;
}
// -------------------------------------------------------
// END of TinyUSB stubs
// -------------------------------------------------------
#pragma endregion

void gpio_init_button(const uint8_t& GPIN){
    gpio_init(GPIN);
    gpio_set_dir(GPIN, GPIO_IN);
    gpio_pull_up(GPIN);
}

void gpio_init_led(const uint8_t& GPIN){
    gpio_init(GPIN);
    gpio_set_dir(GPIN, GPIO_OUT);
}

bool SYS_FLAG = 0; //I don't have a button sad, kinda goofy to do this without having it as a button wire already keeps a state
bool BUTTON_STATE_ARR[8] = {0};
uint8_t CURRENT_LAYOUT_ID = 0;

struct key_layout
{
    const uint8_t key_0;
    const uint8_t key_1;
    const uint8_t key_2;
    const uint8_t key_3;
    const uint8_t key_4;
    const uint8_t key_5;
    const uint8_t key_6;
    const uint8_t key_7;
};

// Hard coded keyboard layouts
key_layout LAYOUTS[8]{ 
    {HID_KEY_A, HID_KEY_W, HID_KEY_S, HID_KEY_D, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT}, // Layout 0
    {HID_KEY_D, HID_KEY_F, HID_KEY_S, HID_KEY_A, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_J, HID_KEY_K}, // Layout 1
    {HID_KEY_A, HID_KEY_C, HID_KEY_D, HID_KEY_E, HID_KEY_F, HID_KEY_G, HID_KEY_H, HID_KEY_I}, // Layout 2
    {HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_M, HID_KEY_N, HID_KEY_O, HID_KEY_P, HID_KEY_Q}, // Layout 3
    {HID_KEY_R, HID_KEY_S, HID_KEY_T, HID_KEY_U, HID_KEY_V, HID_KEY_W, HID_KEY_X, HID_KEY_Y}, // Layout 4
    {HID_KEY_Z, HID_KEY_0, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, HID_KEY_6}, // Layout 5
    {HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_D, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_UP, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT}, // Layout 6
    {HID_KEY_A, HID_KEY_S, HID_KEY_W, HID_KEY_D, HID_KEY_R, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_UP, HID_KEY_ARROW_RIGHT}  // Layout 7
};

#pragma region main
int main() {
    board_init();
    tusb_init();

    // PIN initialization
    gpio_init_button(BTN_0);
    gpio_init_button(BTN_1);
    gpio_init_button(BTN_2);
    gpio_init_button(BTN_3);
    gpio_init_button(BTN_4);
    gpio_init_button(BTN_5);
    gpio_init_button(BTN_6);
    gpio_init_button(BTN_7);

    gpio_init_button(SYS_WIRE);

    gpio_init_led(SYS_LED_0);
    gpio_init_led(SYS_LED_1);
    gpio_init_led(SYS_LED_2);
    gpio_init_led(SYS_LED_3);

    // gpio_put(SYS_LED_1, 1);
    // gpio_put(SYS_LED_2, 1);
    // gpio_put(SYS_LED_3, 1);

    while (true){
        tud_task(); // keep USB alive — must be called every loop

        // pull-up: 0 = pressed
        BUTTON_STATE_ARR[0] = gpio_get(BTN_0) == 0;  
        BUTTON_STATE_ARR[1] = gpio_get(BTN_1) == 0;
        BUTTON_STATE_ARR[2] = gpio_get(BTN_2) == 0;
        BUTTON_STATE_ARR[3] = gpio_get(BTN_3) == 0;

        BUTTON_STATE_ARR[4] = gpio_get(BTN_4) == 0;  
        BUTTON_STATE_ARR[5] = gpio_get(BTN_5) == 0;
        BUTTON_STATE_ARR[6] = gpio_get(BTN_6) == 0;
        BUTTON_STATE_ARR[7] = gpio_get(BTN_7) == 0;

        if (gpio_get(SYS_WIRE) == 0){ // Is pressed
            if (SYS_FLAG == 0){ // Device not in SYSTEM mode already
                SYS_FLAG = 1;
                gpio_put(SYS_LED_0, 1);
            }
        }
        else{ // Not pressed
            if (SYS_FLAG == 1){ // Still in SYSTEM mode
                SYS_FLAG = 0;
                gpio_put(SYS_LED_0, 0);
            }
        }

        // Build a report with whatever keys are currently held
        // The keyboard report holds up to 6 simultaneous keys
        uint8_t keys[6] = {0};
        uint8_t count = 0;

        if (SYS_FLAG){
            for (uint8_t i = 0; i < 8; i++){
                if (BUTTON_STATE_ARR[i] == 1){
                    gpio_put(SYS_LED_1, 0);
                    gpio_put(SYS_LED_2, 0);
                    gpio_put(SYS_LED_3, 0);
                    if (i == 1 || i == 3 || i == 5 || i == 7){
                        gpio_put(SYS_LED_1, 1);
                    }
                    if (i == 2 || i == 3 || i == 6 || i == 7){
                        gpio_put(SYS_LED_2, 1);
                    }
                    if (i == 4 || i == 5 || i == 6 || i == 7){
                        gpio_put(SYS_LED_3, 1);
                    }
                    CURRENT_LAYOUT_ID = i;
                    break;
                }
            }
            send_report(0, NULL); // Nothing to send cuz in system
        }
        else{
            if (BUTTON_STATE_ARR[0]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_0; }
            if (BUTTON_STATE_ARR[1]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_1; }
            if (BUTTON_STATE_ARR[2]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_2; }
            if (BUTTON_STATE_ARR[3]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_3; }
            if (BUTTON_STATE_ARR[4]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_4; }
            if (BUTTON_STATE_ARR[5]) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_5; }
            // 6 KEY MAX NEED CHECK AFTER
            if (BUTTON_STATE_ARR[6] && count < 6) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_6; }
            if (BUTTON_STATE_ARR[7] && count < 6) { keys[count++] = LAYOUTS[CURRENT_LAYOUT_ID].key_7; }

            if (count > 0) {
                send_report(0, keys); // 0 = no modifier (no Shift/Ctrl/etc.)
            } else {
                send_report(0, NULL); // nothing held — send release
            }
        }

        sleep_ms(10); // poll rate — 10ms = 100 times per second
    }
}
#pragma endregion