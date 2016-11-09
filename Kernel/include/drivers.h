#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <drivers_constants.h>

// Divers Keyboard
void setKeyboardCallback(void (*f)(uint8_t, int));
void keyboardHandler();

// Divers Video
//void printf(char *format, ...);

// Divers Network