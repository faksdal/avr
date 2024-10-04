// steps for updating the display, writing memory content to the device

// - set pageadddress SSD1306_PAGEADDR		0x22
// - set page start						0x00
// - set page end address					0xff

// - set column address SSD1306_COLUMNADDR	0x21
// - column start address					0
// - column end address					(WIDTH - 1)

// - start i2c transmission, sending slave address as parameter, beginTransmission
// - write the value 0x40
// - write the scren buffer, one byte at a time until done. There is a check here, if we exceed WIRE_MAX, the bus shuts down
//   and restarts. This is probably some hardware thing. WIRE_MAX equals 32 in this piece of code
//   uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
//    uint8_t *ptr = buffer;
//    if (wire) { // I2C
//      wire->beginTransmission(i2caddr);
//      WIRE_WRITE((uint8_t)0x40);
//      uint8_t bytesOut = 1;
//      while (count--) {
//        if (bytesOut >= WIRE_MAX) {
//          wire->endTransmission();
//          wire->beginTransmission(i2caddr);
//          WIRE_WRITE((uint8_t)0x40);
//          bytesOut = 1;
//        }
//        WIRE_WRITE(*ptr++);
//        bytesOut++;
//    }
//    wire->endTransmission();
//
//    } else { // SPI
//      SSD1306_MODE_DATA
//      while (count--)
//        SPIwrite(*ptr++);
//    }

// it remain to figure out how we write to the buffer...

// this is Adafruits' implementation of drawPixel
/*
void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    // Pixel is in-bounds. Rotate coordinates if needed.
    switch (getRotation()) {
    case 1:
      ssd1306_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      break;
    case 3:
      ssd1306_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }

    switch (color) {
    case SSD1306_WHITE:
      buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
      break;
    case SSD1306_BLACK:
      buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
      break;
    case SSD1306_INVERSE:
      buffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
      break;
    }
  }
}
*/
