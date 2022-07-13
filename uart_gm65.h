#include "esphome.h"

class UartGM65Component : public Component

{
public:
  TextSensor *scan_data = new TextSensor();
  TextSensor *scan_mode = new TextSensor();
  UARTDevice *uart_device ;

  UartGM65Component(UARTComponent *parent) {
    this->uart_device = new UARTDevice(parent);
  }

  void setup() override {
    // get the current config
    scan_mode->publish_state("SETUP");
  }

  int readline(int readch, char *buffer, int len) {
    static int pos = 0;
    int rpos;

    if (readch > 0) {
      switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0; // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  void loop() override {
    const int max_line_length = 80;
    static char buffer[max_line_length];
    while (uart_device->available()) {
      if (readline(uart_device->read(), buffer, max_line_length) > 0) {
        scan_data->publish_state(buffer);
      }
    }
  }
};