bool PM7003_read() {
  bool packetReceived = false;
  unsigned int calcChecksum = 0;
  bool inFrame = false;
  int detectOff = 0;
  char frameBuf[MAX_FRAME_LEN];
  int frameLen = MAX_FRAME_LEN;

//  return true;
  while (!packetReceived) {

    if (Serial.available() > 32) {
      int drain = Serial.available();
      for (int i = drain; i > 0; i--) {
        Serial.read();
      }

    }
    if (Serial.available() > 0) {
      int incomingByte = Serial.read();
      if (!inFrame) {
        if (incomingByte == 0x42 && detectOff == 0) {
          frameBuf[detectOff] = incomingByte;
          PM7003S.frameHeader[0] = incomingByte;
          calcChecksum = incomingByte; // Checksum init!
          detectOff++;
        }
        else if (incomingByte == 0x4D && detectOff == 1) {
          frameBuf[detectOff] = incomingByte;
          PM7003S.frameHeader[1] = incomingByte;
          calcChecksum += incomingByte;
          inFrame = true;
          detectOff++;
        }
        else {
        }
      }
      else {
        frameBuf[detectOff] = incomingByte;
        calcChecksum += incomingByte;
        detectOff++;
        unsigned int val = (frameBuf[detectOff - 1] & 0xff) + (frameBuf[detectOff - 2] << 8);
        switch (detectOff) {
          case 4:
            PM7003S.frameLen = val;
            frameLen = val + detectOff;
            break;
          case 6:
            PM7003S.concPM1_0_CF1 = val;
            break;
          case 8:
            PM7003S.concPM2_5_CF1 = val;
            break;
          case 10:
            PM7003S.concPM10_0_CF1 = val;
            break;
          case 32:
            PM7003S.checksum = val;
            calcChecksum -= ((val >> 8) + (val & 0xFF));
            break;
          default:
            break;
        }

        if (detectOff >= frameLen) {
          packetReceived = true;
          detectOff = 0;
          inFrame = false;
        }
      }
    }
  }
  return (calcChecksum == PM7003S.checksum);
}
