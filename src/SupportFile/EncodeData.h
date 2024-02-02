
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
