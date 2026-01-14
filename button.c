#include "button.h"
#include "string.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include <stdio.h>

#define MAX_BUTTONS 8
static button_t* button_instances[MAX_BUTTONS] = {NULL};
static int button_count = 0;

static const uint32_t DEBOUNCE_DELAY_MS = 50;

static button_t* find_button_by_pin(uint pin) {
    for (int i = 0; i < button_count; i++) {
        if (button_instances[i] && button_instances[i]->pin == pin) {
            return button_instances[i];
        }
    }
    return NULL;
}

void button_gpio_interrupt_handler(uint gpio, uint32_t events) {
    button_t* button = find_button_by_pin(gpio);
    if (!button || !button->interrupt_enabled) {
        return;
    }

    absolute_time_t current_time = get_absolute_time();

    if (absolute_time_diff_us(button->last_interrupt_time, current_time) > (DEBOUNCE_DELAY_MS * 1000)) {
        button->last_interrupt_time = current_time;

        if (events & GPIO_IRQ_EDGE_FALL) {
            printf("*** BUTTON PRESSED! *** (pin %d, interrupt)\n", gpio);

            if (button->callback != NULL) {
                button->callback();
            }
        }
    }
}

void button_init(button_t* button, uint pin) {
    if (!button || button_count >= MAX_BUTTONS) {
        printf("Button init failed: invalid button or too many buttons\n");
        return;
    }

    button->pin = pin;
    button->callback = NULL;
    button->last_interrupt_time = get_absolute_time();
    button->interrupt_enabled = false;
    button->button_pressed = false;
    button->last_button_state = true;
    button->last_poll_time = get_absolute_time();
    button->poll_initialized = false;

    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);

    button_instances[button_count] = button;
    button_count++;

    printf("Button initialized on pin %d\n", pin);
}

void button_set_callback(button_t* button, button_press_callback_t callback) {
    if (button) {
        button->callback = callback;
    }
}

void button_enable_interrupts(button_t* button) {
    if (!button) return;

    gpio_set_irq_enabled_with_callback(button->pin, GPIO_IRQ_EDGE_FALL, true, &button_gpio_interrupt_handler);
    button->interrupt_enabled = true;
    printf("Button interrupts enabled on pin %d\n", button->pin);
}

void button_disable_interrupts(button_t* button) {
    if (!button) return;

    gpio_set_irq_enabled(button->pin, GPIO_IRQ_EDGE_FALL, false);
    button->interrupt_enabled = false;
    printf("Button interrupts disabled on pin %d\n", button->pin);
}

void button_check_state(button_t* button, int t, const char* context) {
    if (!button) return;

    if (!button->poll_initialized) {
        button->last_poll_time = get_absolute_time();
        button->poll_initialized = true;
    }

    bool current_button_state = gpio_get(button->pin);
    absolute_time_t current_time = get_absolute_time();

    if (absolute_time_diff_us(button->last_poll_time, current_time) > (DEBOUNCE_DELAY_MS * 1000)) {
        if (button->last_button_state && !current_button_state && !button->button_pressed) {
            button->button_pressed = true;
            button->last_poll_time = current_time;
            if (context && strlen(context) > 0) {
                printf("*** BUTTON PRESSED! *** Time: %d (%s, pin %d, poll)\n", t, context, button->pin);
            } else {
                printf("*** BUTTON PRESSED! *** Time: %d (pin %d, poll)\n", t, button->pin);
            }

            if (button->callback != NULL) {
                button->callback();
            }
        }
        else if (!button->last_button_state && current_button_state && button->button_pressed) {
            button->button_pressed = false;
            button->last_poll_time = current_time;
            printf("*** BUTTON RELEASED! *** Time: %d (pin %d, poll)\n", t, button->pin);
        }

        button->last_button_state = current_button_state;
    }
}