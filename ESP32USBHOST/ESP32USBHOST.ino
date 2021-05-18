
#define DEBUG
#define DEBUGEXTRA

extern "C" {
#ifdef DEBUGEXTRA  
  uint8_t DEBUGEXTRA_VAR=1;
#else
  uint8_t DEBUGEXTRA_VAR=0;
#endif  
}

#include "src/USBSoftHost/ESP32-USBSoftHost.hpp"
#include "src/USBSoftHost/usbkbd.h"                 // KeyboardReportParser

//================================================================================

#define DP_P0  22  // Data+
#define DM_P0  21  // Data-
#define DP_P1  -1  // not used
#define DM_P1  -1  // not used
#define DP_P2  -1  // not used
#define DM_P2  -1  // not used
#define DP_P3  -1  // not used
#define DM_P3  -1  // not used

usb_pins_config_t USB_Pins_Config =
{
  DP_P0, DM_P0,
  DP_P1, DM_P1,
  DP_P2, DM_P2,
  DP_P3, DM_P3
};


//================================================================================
static void my_USB_DetectCB( uint8_t usbNum, void * dev )
{
  sDevDesc *device = (sDevDesc*)dev;
#ifdef DEBUG
  printf("New device detected on USB#%d\n", usbNum);
  printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
  printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
  printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
  printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
  printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
  printf("desc.idVendor           = 0x%04x\n", device->idVendor);
  printf("desc.idProduct          = 0x%04x\n", device->idProduct);
  printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
  printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
  printf("desc.iProduct           = 0x%02x\n", device->iProduct);
  printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
  printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
  // if( device->iProduct == mySupportedIdProduct && device->iManufacturer == mySupportedManufacturer ) {
  //   myListenUSBPort = usbNum;
  // }
#endif
}

//********************************************************************************
uint8_t essential_key = 0;
uint8_t lastkey[6] = {0}; ///max 6 keys can be pressed
byte* keymap;
//********************************************************************************
//EMULATE JOYSTICK BY KEYBOARDS:
uint8_t KEY_UP = 0;      /// ↑
uint8_t KEY_DOWN = 0;    /// ↓
uint8_t KEY_LEFT = 0;    /// ←
uint8_t KEY_RIGHT = 0;   /// →
uint8_t KEY_CROSS = 0;   /// X
uint8_t KEY_SQUARE = 0;  /// □
uint8_t KEY_SHARE = 0;   /// START
uint8_t KEY_OPTIONS = 0; /// SELECT
uint8_t KEY_CIRCLE = 0;  /// O
uint8_t KEY_TRIANGLE = 0; /// Δ
//********************************************************************************
const char keycharmap[256][6] = {
  " ", "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "PRINT", "SCRLL", "PAUSE",
  "~", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKS", "INSER", "HOME", "PUP", "NUMLC", "NUM /", "NUM *", "NUM -",
  "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "\\", "DEL", "END", "PDOWN", "NUM 7", "NUM 8", "NUM 9", "NUM +",
  "CAPS", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "ENTER", "NUM 4", "NUM 5", "NUM 6",
  "SHIFT", "Z", "X", "C", "V", "B", "N", "M", ",", ".", "/", "SHIFT", "UP", "NUM 1", "NUM 2", "NUM 3", "ENTER",
  "CTRL", "ALT", "SPACE", "ALT", "CTRL", "LEFT", "DOWN", "RIGHT", "NUM 0", "NUM .", "GUI", "GUI", "MENU"
};
//********************************************************************************
uint8_t mouse_button=0;
uint8_t mouse_left_button=0;
uint8_t mouse_right_button=0;
uint8_t mouse_middle_button=0;
int16_t mouse_X_POSITION=0;
int16_t mouse_Y_POSITION=0;
int16_t mouse_Z_POSITION=0;
//********************************************************************************
//********************************************************************************
//********************************************************************************
uint8_t keypress(uint8_t scancode) {
  switch (scancode) {
    case 0x29: // ESC
      keymap[1] = 1;
      return (1);
      break;
    case 0x3a: // F1
      keymap[2] = 1;
      return (2);
      break;
    case 0x3b: // F2
      keymap[3] = 1;
      return (3);
      break;
    case 0x3c: // F3
      keymap[4] = 1;
      return (4);
      break;
    case 0x3d: // F4
      keymap[5] = 1;
      return (5);
      break;
    case 0x3e: // F5
      keymap[6] = 1;
      return (6);
      break;
    case 0x3f: // F6
      keymap[7] = 1;
      return (7);
      break;
    case 0x40: // F7
      keymap[8] = 1;
      return (8);
      break;
    case 0x41: // F8
      keymap[9] = 1;
      return (9);
      break;
    case 0x42: // F9
      keymap[0x0a] = 1;
      return (0x0a);
      break;
    case 0x43: // F10
      keymap[0x0b] = 1;
      return (0x0b);
      break;
    case 0x44: // F11
      keymap[0x0c] = 1;
      return (0x0c);
      break;
    case 0x45: // F12
      keymap[0x0d] = 1;
      return (0x0d);
      break;
    case 0x46: // PrintScreen
      keymap[0x0e] = 1;
      return (0x0e);
      break;
    case 0x47: // ScrollLock
      keymap[0x0f] = 1;
      return (0x0f);
      break;
    case 0x48: // Pause/Break
      keymap[0x10] = 1;
      return (0x10);
      break;
    case 0x35: // ~
      keymap[0x11] = 1;
      return (0x11);
      break;
    case 0x1e: // 1
      keymap[0x12] = 1;
      return (0x12);
      break;
    case 0x1f: // 2
      keymap[0x13] = 1;
      return (0x13);
      break;
    case 0x20: // 3
      keymap[0x14] = 1;
      return (0x14);
      break;
    case 0x21: // 4
      keymap[0x15] = 1;
      return (0x15);
      break;
    case 0x22: // 5
      keymap[0x16] = 1;
      return (0x16);
      break;
    case 0x23: // 6
      keymap[0x17] = 1;
      return (0x17);
      break;
    case 0x24: // 7
      keymap[0x18] = 1;
      return (0x18);
      break;
    case 0x25: // 8
      keymap[0x19] = 1;
      return (0x19);
      break;
    case 0x26: // 9
      keymap[0x1a] = 1;
      return (0x1a);
      break;
    case 0x27: // 0
      keymap[0x1b] = 1;
      return (0x1b);
      break;
    case 0x2d: // -
      keymap[0x1c] = 1;
      return (0x1c);
      break;
    case 0x2e: // =
      keymap[0x1d] = 1;
      return (0x1d);
      break;
    case 0x2a: // BACKSPACE
      keymap[0x1e] = 1;
      return (0x1e);
      break;
    case 0x2b: // TAB
      keymap[0x26] = 1;
      return (0x26);
      break;
    case 0x14: // Q
      keymap[0x27] = 1;
      return (0x27);
      break;
    case 0x1a: // W
      keymap[0x28] = 1;
      return (0x28);
      break;
    case 0x08: // E
      keymap[0x29] = 1;
      return (0x29);
      break;
    case 0x15: // R
      keymap[0x2a] = 1;
      return (0x2a);
      break;
    case 0x17: // T
      keymap[0x2b] = 1;
      return (0x2b);
      break;
    case 0x1c: // Y
      keymap[0x2c] = 1;
      return (0x2c);
      break;
    case 0x18: // U
      keymap[0x2d] = 1;
      return (0x2d);
      break;
    case 0x0c: // I
      keymap[0x2e] = 1;
      return (0x2e);
      break;
    case 0x12: // O
      keymap[0x2f] = 1;
      return (0x2f);
      break;
    case 0x13: // P
      keymap[0x30] = 1;
      return (0x30);
      break;
    case 0x2f: // {
      keymap[0x31] = 1;
      return (0x31);
      break;
    case 0x30: // }
      keymap[0x32] = 1;
      return (0x32);
      break;
    case 0x32: // \\
      keymap[0x33] = 1;
      return (0x33);
      break;
    case 0x39: // CAPSLOCK
      keymap[0x3b] = 1;
      return (0x3b);
      break;
    case 0x04: // A
      keymap[0x3c] = 1;
      return (0x3c);
      break;
    case 0x16: // S
      keymap[0x3d] = 1;
      return (0x3d);
      break;
    case 0x07: // D
      keymap[0x3e] = 1;
      return (0x3e);
      break;
    case 0x09: // F
      keymap[0x3f] = 1;
      return (0x3f);
      break;
    case 0x0a: // G
      keymap[0x40] = 1;
      return (0x40);
      break;
    case 0x0b: // H
      keymap[0x41] = 1;
      return (0x41);
      break;
    case 0x0d: // J
      keymap[0x42] = 1;
      return (0x42);
      break;
    case 0x0e: // K
      keymap[0x43] = 1;
      return (0x43);
      break;
    case 0x0f: // L
      keymap[0x44] = 1;
      return (0x44);
      break;
    case 0x33: // ;
      keymap[0x45] = 1;
      return (0x45);
      break;
    case 0x34: // '
      keymap[0x46] = 1;
      return (0x46);
      break;
    case 0x28: // ENTER
      keymap[0x47] = 1;
      return (0x47);
      break;
    case 0x1d: // Z
      keymap[0x4c] = 1;
      return (0x4c);
      break;
    case 0x1b: // X
      keymap[0x4d] = 1;
      return (0x4d);
      break;
    case 0x06: // C
      keymap[0x4e] = 1;
      return (0x4e);
      break;
    case 0x19: // V
      keymap[0x4f] = 1;
      return (0x4f);
      break;
    case 0x05: // B
      keymap[0x50] = 1;
      return (0x50);
      break;
    case 0x11: // N
      keymap[0x51] = 1;
      return (0x51);
      break;
    case 0x10: // M
      keymap[0x52] = 1;
      return (0x52);
      break;
    case 0x36: // ,
      keymap[0x53] = 1;
      return (0x53);
      break;
    case 0x37: // .
      keymap[0x54] = 1;
      return (0x54);
      break;
    case 0x38: // /
      keymap[0x55] = 1;
      return (0x55);
      break;
    case 0x2c: // SPACEBAR
      keymap[0x5e] = 1;
      return (0x5e);
      break;
    case 0x65: // CONTEXT BUTTON
      keymap[0x68] = 1;
      return (0x68);
      break;
    case 0x49: // INSERT
      keymap[0x1f] = 1;
      return (0x1f);
      break;
    case 0x4a: // HOME
      keymap[0x20] = 1;
      return (0x20);
      break;
    case 0x4b: // PAGEUP
      keymap[0x21] = 1;
      return (0x21);
      break;
    case 0x4c: // DELETE
      keymap[0x34] = 1;
      return (0x34);
      break;
    case 0x4d: // END
      keymap[0x35] = 1;
      return (0x35);
      break;
    case 0x4e: // PAGEDOWN
      keymap[0x36] = 1;
      return (0x36);
      break;
    case 0x52: // UP
      keymap[0x57] = 1;
      return (0x57);
      break;
    case 0x50: // LEFT
      keymap[0x61] = 1;
      return (0x61);
      break;
    case 0x51: // DOWN
      keymap[0x62] = 1;
      return (0x62);
      break;
    case 0x4f: // RIGHT
      keymap[0x63] = 1;
      return (0x63);
      break;
    case 0x53: // NUMLOCK
      keymap[0x22] = 1;
      return (0x22);
      break;
    case 0x54: // NUM /
      keymap[0x23] = 1;
      return (0x23);
      break;
    case 0x55: // NUM *
      keymap[0x24] = 1;
      return (0x24);
      break;
    case 0x56: // NUM -
      keymap[0x25] = 1;
      return (0x25);
      break;
    case 0x57: // NUM +
      keymap[0x3a] = 1;
      return (0x3a);
      break;
    case 0x58: // NUM ENTER
      keymap[0x5b] = 1;
      return (0x5b);
      break;
    case 0x63: // NUM .
      keymap[0x65] = 1;
      return (0x65);
      break;
    case 0x62: // NUM 0
      keymap[0x64] = 1;
      return (0x64);
      break;
    case 0x59: // NUM 1
      keymap[0x58] = 1;
      return (0x58);
      break;
    case 0x5a: // NUM 2
      keymap[0x59] = 1;
      return (0x59);
      break;
    case 0x5b: // NUM 3
      keymap[0x5a] = 1;
      return (0x5a);
      break;
    case 0x5c: // NUM 4
      keymap[0x48] = 1;
      return (0x48);
      break;
    case 0x5d: // NUM 5
      keymap[0x49] = 1;
      return (0x49);
      break;
    case 0x5e: // NUM 6
      keymap[0x4a] = 1;
      return (0x4a);
      break;
    case 0x5f: // NUM 7
      keymap[0x37] = 1;
      return (0x37);
      break;
    case 0x60: // NUM 8
      keymap[0x38] = 1;
      return (0x38);
      break;
    case 0x61: // NUM 9
      keymap[0x39] = 1;
      return (0x39);
      break;
    default:
      return (0);
      break;
  }
}
//********************************************************************************
uint8_t keyrelease(uint8_t scancode) {
  switch (scancode) {
    case 0x29: // ESC
      keymap[1] = 0;
      return (1);
      break;
    case 0x3a: // F1
      keymap[2] = 0;
      return (2);
      break;
    case 0x3b: // F2
      keymap[3] = 0;
      return (3);
      break;
    case 0x3c: // F3
      keymap[4] = 0;
      return (4);
      break;
    case 0x3d: // F4
      keymap[5] = 0;
      return (5);
      break;
    case 0x3e: // F5
      keymap[6] = 0;
      return (6);
      break;
    case 0x3f: // F6
      keymap[7] = 0;
      return (7);
      break;
    case 0x40: // F7
      keymap[8] = 0;
      return (8);
      break;
    case 0x41: // F8
      keymap[9] = 0;
      return (9);
      break;
    case 0x42: // F9
      keymap[0x0a] = 0;
      return (0x0a);
      break;
    case 0x43: // F10
      keymap[0x0b] = 0;
      return (0x0b);
      break;
    case 0x44: // F11
      keymap[0x0c] = 0;
      return (0x0c);
      break;
    case 0x45: // F12
      keymap[0x0d] = 0;
      return (0x0d);
      break;
    case 0x46: // PrintScreen
      keymap[0x0e] = 0;
      return (0x0e);
      break;
    case 0x47: // ScrollLock
      keymap[0x0f] = 0;
      return (0x0f);
      break;
    case 0x48: // Pause/Break
      keymap[0x10] = 0;
      return (0x10);
      break;
    case 0x35: // ~
      keymap[0x11] = 0;
      return (0x11);
      break;
    case 0x1e: // 1
      keymap[0x12] = 0;
      return (0x12);
      break;
    case 0x1f: // 2
      keymap[0x13] = 0;
      return (0x13);
      break;
    case 0x20: // 3
      keymap[0x14] = 0;
      return (0x14);
      break;
    case 0x21: // 4
      keymap[0x15] = 0;
      return (0x15);
      break;
    case 0x22: // 5
      keymap[0x16] = 0;
      return (0x16);
      break;
    case 0x23: // 6
      keymap[0x17] = 0;
      return (0x17);
      break;
    case 0x24: // 7
      keymap[0x18] = 0;
      return (0x18);
      break;
    case 0x25: // 8
      keymap[0x19] = 0;
      return (0x19);
      break;
    case 0x26: // 9
      keymap[0x1a] = 0;
      return (0x1a);
      break;
    case 0x27: // 0
      keymap[0x1b] = 0;
      return (0x1b);
      break;
    case 0x2d: // -
      keymap[0x1c] = 0;
      return (0x1c);
      break;
    case 0x2e: // =
      keymap[0x1d] = 0;
      return (0x1d);
      break;
    case 0x2a: // BACKSPACE
      keymap[0x1e] = 0;
      return (0x1e);
      break;
    case 0x2b: // TAB
      keymap[0x26] = 0;
      return (0x26);
      break;
    case 0x14: // Q
      keymap[0x27] = 0;
      return (0x27);
      break;
    case 0x1a: // W
      keymap[0x28] = 0;
      return (0x28);
      break;
    case 0x08: // E
      keymap[0x29] = 0;
      return (0x29);
      break;
    case 0x15: // R
      keymap[0x2a] = 0;
      return (0x2a);
      break;
    case 0x17: // T
      keymap[0x2b] = 0;
      return (0x2b);
      break;
    case 0x1c: // Y
      keymap[0x2c] = 0;
      return (0x2c);
      break;
    case 0x18: // U
      keymap[0x2d] = 0;
      return (0x2d);
      break;
    case 0x0c: // I
      keymap[0x2e] = 0;
      return (0x2e);
      break;
    case 0x12: // O
      keymap[0x2f] = 0;
      return (0x2f);
      break;
    case 0x13: // P
      keymap[0x30] = 0;
      return (0x30);
      break;
    case 0x2f: // {
      keymap[0x31] = 0;
      return (0x31);
      break;
    case 0x30: // }
      keymap[0x32] = 0;
      return (0x32);
      break;
    case 0x32: // \\
      keymap[0x33] = 0;
      return (0x33);
      break;
    case 0x39: // CAPSLOCK
      keymap[0x3b] = 0;
      return (0x3b);
      break;
    case 0x04: // A
      keymap[0x3c] = 0;
      return (0x3c);
      break;
    case 0x16: // S
      keymap[0x3d] = 0;
      return (0x3d);
      break;
    case 0x07: // D
      keymap[0x3e] = 0;
      return (0x3e);
      break;
    case 0x09: // F
      keymap[0x3f] = 0;
      return (0x3f);
      break;
    case 0x0a: // G
      keymap[0x40] = 0;
      return (0x40);
      break;
    case 0x0b: // H
      keymap[0x41] = 0;
      return (0x41);
      break;
    case 0x0d: // J
      keymap[0x42] = 0;
      return (0x42);
      break;
    case 0x0e: // K
      keymap[0x43] = 0;
      return (0x43);
      break;
    case 0x0f: // L
      keymap[0x44] = 0;
      return (0x44);
      break;
    case 0x33: // ;
      keymap[0x45] = 0;
      return (0x45);
      break;
    case 0x34: // '
      keymap[0x46] = 0;
      return (0x46);
      break;
    case 0x28: // ENTER
      keymap[0x47] = 0;
      return (0x47);
      break;
    case 0x1d: // Z
      keymap[0x4c] = 0;
      return (0x4c);
      break;
    case 0x1b: // X
      keymap[0x4d] = 0;
      return (0x4d);
      break;
    case 0x06: // C
      keymap[0x4e] = 0;
      return (0x4e);
      break;
    case 0x19: // V
      keymap[0x4f] = 0;
      return (0x4f);
      break;
    case 0x05: // B
      keymap[0x50] = 0;
      return (0x50);
      break;
    case 0x11: // N
      keymap[0x51] = 0;
      return (0x51);
      break;
    case 0x10: // M
      keymap[0x52] = 0;
      return (0x52);
      break;
    case 0x36: // ,
      keymap[0x53] = 0;
      return (0x53);
      break;
    case 0x37: // .
      keymap[0x54] = 0;
      return (0x54);
      break;
    case 0x38: // /
      keymap[0x55] = 0;
      return (0x55);
      break;
    case 0x2c: // SPACEBAR
      keymap[0x5e] = 0;
      return (0x5e);
      break;
    case 0x65: // CONTEXT BUTTON
      keymap[0x68] = 0;
      return (0x68);
      break;
    case 0x49: // INSERT
      keymap[0x1f] = 0;
      return (0x1f);
      break;
    case 0x4a: // HOME
      keymap[0x20] = 0;
      return (0x20);
      break;
    case 0x4b: // PAGEUP
      keymap[0x21] = 0;
      return (0x21);
      break;
    case 0x4c: // DELETE
      keymap[0x34] = 0;
      return (0x34);
      break;
    case 0x4d: // END
      keymap[0x35] = 0;
      return (0x35);
      break;
    case 0x4e: // PAGEDOWN
      keymap[0x36] = 0;
      return (0x36);
      break;
    case 0x52: // UP
      keymap[0x57] = 0;
      return (0x57);
      break;
    case 0x50: // LEFT
      keymap[0x61] = 0;
      return (0x61);
      break;
    case 0x51: // DOWN
      keymap[0x62] = 0;
      return (0x62);
      break;
    case 0x4f: // RIGHT
      keymap[0x63] = 0;
      return (0x63);
      break;
    case 0x53: // NUMLOCK
      keymap[0x22] = 0;
      return (0x22);
      break;
    case 0x54: // NUM /
      keymap[0x23] = 0;
      return (0x23);
      break;
    case 0x55: // NUM *
      keymap[0x24] = 0;
      return (0x24);
      break;
    case 0x56: // NUM -
      keymap[0x25] = 0;
      return (0x25);
      break;
    case 0x57: // NUM +
      keymap[0x3a] = 0;
      return (0x3a);
      break;
    case 0x58: // NUM ENTER
      keymap[0x5b] = 0;
      return (0x5b);
      break;
    case 0x63: // NUM .
      keymap[0x65] = 0;
      return (0x65);
      break;
    case 0x62: // NUM 0
      keymap[0x64] = 0;
      return (0x64);
      break;
    case 0x59: // NUM 1
      keymap[0x58] = 0;
      return (0x58);
      break;
    case 0x5a: // NUM 2
      keymap[0x59] = 0;
      return (0x59);
      break;
    case 0x5b: // NUM 3
      keymap[0x5a] = 0;
      return (0x5a);
      break;
    case 0x5c: // NUM 4
      keymap[0x48] = 0;
      return (0x48);
      break;
    case 0x5d: // NUM 5
      keymap[0x49] = 0;
      return (0x49);
      break;
    case 0x5e: // NUM 6
      keymap[0x4a] = 0;
      return (0x4a);
      break;
    case 0x5f: // NUM 7
      keymap[0x37] = 0;
      return (0x37);
      break;
    case 0x60: // NUM 8
      keymap[0x38] = 0;
      return (0x38);
      break;
    case 0x61: // NUM 9
      keymap[0x39] = 0;
      return (0x39);
      break;
    default:
      return (0);
      break;
  }
}


static void my_USB_PrintCB(uint8_t usbNum, uint8_t byte_depth, uint8_t* data, uint8_t data_len)
{
  // if( myListenUSBPort != usbNum ) return;
#ifdef DEBUG
  printf("in: ");
  for (int k = 0; k < data_len; k++) {
    printf("0x%02x ", data[k] );
  }
#endif

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
  if (data_len == 8) { //it is keyboard
//--------------------------------------------------------------------------------
    if ((data[0] & 1) == 1 && (essential_key & 1) != 1) {
#ifdef DEBUG
      printf("LEFT CTRL PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x5c] = 1; // LEFT CTRL
    } else if ((essential_key & 1) == 1 && (data[0] & 1) != 1) {
#ifdef DEBUG
      printf("LEFT CTRL RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x5c] = 0; // LEFT CTRL
    }
    if ((data[0] & 2) == 2 && (essential_key & 2) != 2) {
#ifdef DEBUG
      printf("LEFT SHIFT PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x4b] = 1; // LEFT SHIFT
    } else if ((essential_key & 2) == 2 && (data[0] & 2) != 2) {
#ifdef DEBUG
      printf("LEFT SHIFT RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x4b] = 0; // LEFT SHIFT
    }
    if ((data[0] & 4) == 4 && (essential_key & 4) != 4) {
#ifdef DEBUG
      printf("LEFT ALT PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x5d] = 1; // LEFT ALT
    } else if ((essential_key & 4) == 4 && (data[0] & 4) != 4) {
#ifdef DEBUG
      printf("LEFT ALT RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x5d] = 0; // LEFT ALT
    }
    if ((data[0] & 8) == 8 && (essential_key & 8) != 8) {
#ifdef DEBUG
      printf("LEFT GUIIN PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x66] = 1; // LEFT GUI
    } else if ((essential_key & 8) == 8 && (data[0] & 8) != 8) {
#ifdef DEBUG
      printf("LEFT GUI RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x66] = 0; // LEFT GUI
    }
    if ((data[0] & 0x10) == 0x10 && (essential_key & 0x10) != 0x10) {
#ifdef DEBUG
      printf("RIGHT CTRL PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x60] = 1; // RIGHT CTRL
    } else if ((essential_key & 0x10) == 0x10 && (data[0] & 0x10) != 0x10) {
#ifdef DEBUG
      printf("RIGHT CTRL RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x60] = 0; // RIGHT CTRL
    }
    if ((data[0] & 0x20) == 0x20 && (essential_key & 0x20) != 0x20) {
#ifdef DEBUG
      printf("RIGHT SHIFT PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x56] = 1; // RIGHT SHIFT
    } else if ((essential_key & 0x20) == 0x20 && (data[0] & 0x20) != 0x20) {
#ifdef DEBUG
      printf("RIGHT SHIFT RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x56] = 0; // RIGHT SHIFT
    }
    if ((data[0] & 0x40) == 0x40 && (essential_key & 0x40) != 0x40) {
#ifdef DEBUG
      printf("RIGHT ALT PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x5f] = 1; // RIGHT ALT
    } else if ((essential_key & 0x40) == 0x40 && (data[0] & 0x40) != 0x40) {
#ifdef DEBUG
      printf("RIGHT ALT RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x5f] = 0; // RIGHT ALT
    }
    if ((data[0] & 0x80) == 0x80 && (essential_key & 0x80) != 0x80) {
#ifdef DEBUG
      printf("RIGHT GUI PRESSED\n");
#endif
      essential_key = data[0];
      keymap[0x67] = 1; // RIGHT GUI
    } else if ((essential_key & 0x80) == 0x80 && (data[0] & 0x80) != 0x80) {
#ifdef DEBUG
      printf("RIGHT GUI RELEASED\n");
#endif
      essential_key = data[0];
      keymap[0x67] = 0; // RIGHT GUI

    }
//--------------------------------------------------------------------------------
    for (int8_t tmp = 0; tmp < sizeof(lastkey); tmp++) {

      if (data[tmp + 2] != 1) {
        if (lastkey[tmp] != data[tmp + 2] && lastkey[tmp] == 0) {
          //pressed key
#ifdef DEBUG
          printf("[KEYPRESS]%d=0x%02x\n", tmp, data[tmp + 2]);
#endif
          lastkey[tmp] = data[tmp + 2];
          keypress(lastkey[tmp]); // PRESSED KEY
#ifdef DEBUGEXTRA
          printf("%s\n", keycharmap[keypress(lastkey[tmp])]);
#endif
        } else if (lastkey[tmp] != data[tmp + 2] && data[tmp + 2] != 0) {
          if (lastkey[tmp + 1] == data[tmp + 2]) {
#ifdef DEBUG
            printf("[KEYRELEASE+REPLACE]%d=0x%02x\n", tmp, lastkey[tmp]);
#endif
            keyrelease(lastkey[tmp]); // RELEASED KEY
            for (int8_t i = tmp; i < sizeof(lastkey) - 1; i++) lastkey[i] = lastkey[i + 1];
          }
        } else if (data[tmp + 2] == 0 && lastkey[tmp] != 0) {
#ifdef DEBUG
          printf("[KEYRELEASE]%d=0x%02x\n", tmp, lastkey[tmp]);
#endif
          //released key
          keyrelease(lastkey[tmp]); // RELEASED KEY
          lastkey[tmp] = 0;
        }
      } else {
#ifdef DEBUG
        printf("KEYOVERLOAD\n");
#endif
        for (uint16_t tmp = 0; tmp < 256; tmp++) keymap[tmp] = 0; // RELEASE ALL KEYS!
      }
    }
//--------------------------------------------------------------------------------
  } else if (data_len == 4) { //it is mouse
    if ((data[0] & 1) == 1 && (mouse_button & 1) != 1) {
       mouse_button = data[0];
       mouse_left_button=1;
#ifdef DEBUG
       printf("LEFT MOUSE BUTTON PRESSED\n");
#endif
    } else if ((mouse_button & 1) == 0 && (data[0] & 1) != 0) {
       mouse_button = data[0];
       mouse_left_button=0;      
#ifdef DEBUG
       printf("LEFT MOUSE BUTTON RELEASED\n");
#endif
    }
    if ((data[0] & 2) == 2 && (mouse_button & 2) != 2) {
       mouse_button = data[0];
       mouse_right_button=1;
#ifdef DEBUG
       printf("RIGHT MOUSE BUTTON PRESSED\n");
#endif
    } else if ((mouse_button & 2) == 0 && (data[0] & 2) != 0) {
       mouse_button = data[0];
       mouse_right_button=0;      
#ifdef DEBUG
       printf("RIGHT MOUSE BUTTON RELEASED\n");
#endif
    }
    if ((data[0] & 4) == 4 && (mouse_button & 4) != 4) {
       mouse_button = data[0];
       mouse_middle_button=1;
#ifdef DEBUG
       printf("MIDDLE MOUSE BUTTON PRESSED\n");
#endif
    } else if ((mouse_button & 4) == 0 && (data[0] & 4) != 0) {
       mouse_button = data[0];
       mouse_middle_button=0;      
#ifdef DEBUG
       printf("MIDDLE MOUSE BUTTON RELEASED\n");
#endif
    }
//--------------------------------------------------------------------------------   
    if (data[1]!=0) { //X POSITION
      if ((uint8_t)data[1]>0 && (uint8_t)data[1]<128 ) { // +X
        mouse_X_POSITION+=(uint8_t)data[1];
#ifdef DEBUG
         printf("MOUSE X POSITION: %d [+%d]\n",mouse_X_POSITION,data[1]);       
#endif
      } else 
      if ((uint8_t)data[1]>127 && (uint8_t)data[1]<256 ) { // -X
        mouse_X_POSITION-=(256 - (uint8_t)data[1]);
#ifdef DEBUG
         printf("MOUSE X POSITION: %d [-%d]\n",mouse_X_POSITION,(256-data[1]));       
#endif
      }
    }  
//--------------------------------------------------------------------------------   
    if (data[2]!=0) { //Y POSITION
      if ((uint8_t)data[2]>0 && (uint8_t)data[2]<128 ) { // +Y
        mouse_Y_POSITION+=(uint8_t)data[2];
#ifdef DEBUG
         printf("MOUSE Y POSITION: %d [+%d]\n",mouse_Y_POSITION,data[2]);       
#endif
      } else 
      if ((uint8_t)data[2]>127 && (uint8_t)data[2]<256 ) { // -Y
        mouse_Y_POSITION-=(256 - (uint8_t)data[2]);
#ifdef DEBUG
         printf("MOUSE Y POSITION: %d [-%d]\n",mouse_Y_POSITION,(256-data[2]));       
#endif
      }
    }  
//--------------------------------------------------------------------------------   
    if (data[3]!=0) { //Z POSITION
      if ((uint8_t)data[3]>0 && (uint8_t)data[3]<128 ) { // +Y
        mouse_Z_POSITION+=(uint8_t)data[3];
#ifdef DEBUG
         printf("MOUSE Z POSITION: %d [+%d]\n",mouse_Z_POSITION,data[3]);       
#endif
      } else 
      if ((uint8_t)data[3]>127 && (uint8_t)data[3]<256 ) { // -Y
        mouse_Z_POSITION-=(256 - (uint8_t)data[3]);
#ifdef DEBUG
         printf("MOUSE Z POSITION: %d [-%d]\n",mouse_Z_POSITION,(256-data[3]));       
#endif
      }
//--------------------------------------------------------------------------------       
    }
//--------------------------------------------------------------------------------
  } else { //I do not know what is it, this device?
    
  }
//--------------------------------------------------------------------------------
#ifdef DEBUG
  printf("\n");
#endif
}

//================================================================================

void setup() {
  Serial.begin(115200);

  keymap = (byte*)malloc(256); ///malloc array !

  delay(200);
  Serial.println("USB init: ");
  delay(1000);

  USH.init( USB_Pins_Config, my_USB_DetectCB, my_USB_PrintCB );


}

void loop() {

}
