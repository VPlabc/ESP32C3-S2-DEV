/*******************************************
 * Name.......:  Esc/Pos Printer Library
 * Version....:  1.0.3
 * Description:  A Library to control an Esc/Pos thermal printer by an arduino via serial connection
 * Project....:  https://github.com/cranties/escposprinter
 * License....:  You may use this work under the terms of either the MIT License or the GNU General Public License (GPL) Version 3
 * Keywords...:  thermal, micro, receipt, printer, serial, tm-t88, tm88, tmt88, EscPos, tm20, tm-20
 * ********************************************/

#ifndef escposprinter_h
#define escposprinter_h

 #include "Arduino.h"
//  #include "SoftwareSerial.h"


class EscPos : public Print {
public:
  EscPos(Stream *s, int baud = 9600);
size_t write(uint8_t c);
size_t cwrite(char c);
int read();

void start();

int getStatus();
void characterSet(uint8_t n, uint8_t page);
void lineSpacing(uint8_t n);
void defaultLineSpacing();
void set80mm();
void set58mm();
void effectBold();
void effectDoubleHeight();
void effectDoubleWidth();
void effectUnderline();
void effectOff();
void reverseOff();
void reverseOn();
void justifyLeft();
void justifyCenter();
void justifyRight();
void barcodeHeight();
void barcodeWidth();
void barcodeTextPosition(uint8_t n);
void barcodeStartPrint();
void barcodeEndPrint();
void qrcodeStartPrint(uint8_t len);
void qrcodeEndPrint();

void feed(uint8_t n);
void feed();
void partialCut();
void fullCut();
void margin(int nL, int nH);
private:
  int _rxPin;  
  int _txPin;
  int baudrate{9600};  // 19200
  Stream *_printer;
  // HardwareSerial * _printer;
};

#endif
