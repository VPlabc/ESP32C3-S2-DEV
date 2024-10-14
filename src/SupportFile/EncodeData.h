#include <Arduino.h>

typedef struct
{
    boolean Bit0;
    boolean Bit1;
    boolean Bit2;
    boolean Bit3;
    boolean Bit4;
    boolean Bit5;
    boolean Bit6;
    boolean Bit7;
} RespondBitData;

unsigned int EncodeRespond(byte bytel,byte byteh)
{
    int ret = 0;
    byte byte_one = 0;
    byte byte_two = 0;
    byte_two = bytel;
    byte_one = byteh;
    ret = byte_one;
    ret = ret << 8;
    ret = ret | byte_two;
    return ret;
}
byte EncodeRespondByte(boolean a, boolean b, boolean c, boolean d, boolean e, boolean f, boolean g, boolean h)
    {
        byte byte_1 = 0;
        byte_1 = a;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1| b;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | c;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | d;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | e;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | f;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | g;
        byte_1 = byte_1 << 1;
        byte_1 = byte_1 | h;
        return byte_1;
    }

    RespondBitData DecodeBitRespond(unsigned int _data)
    {
        RespondBitData ret;

        ret.Bit0 = _data & B00000001;
        ret.Bit1 = _data & B00000010;
        ret.Bit2 = _data & B00000100;
        ret.Bit3 = _data & B00001000;
        ret.Bit4 = _data & B00010000;
        ret.Bit5 = _data & B00100000;
        ret.Bit6 = _data & B01000000;
        ret.Bit7 = _data & B10000000;

        return ret;
    }