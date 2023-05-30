#ifdef MPU6050
/*
  This code is used for connecting arduino to serial mpu6050 module, and test in arduino uno R3 board.
  connect map:
  arduino   mpu6050 module
  VCC    5v/3.3v
  TX     RX<-0
  TX     TX->1
  GND    GND
  note:
  because arduino download and mpu6050 are using the same serial port, you need to un-connect 6050 module when you want to download program to arduino.
  Created 14 Nov 2013
  by Zhaowen

  serial mpu6050 module can be found in the link below:
  http://item.taobao.com/item.htm?id=19785706431
*/

float a[3], w[3], angle[3], T;
void MpuSetup()
{
  stPayloadData.uiAcelCount = 0;
  stPayloadData.uiAspeedCount = 0;
  stPayloadData.uiAngleCount = 0;
  stPayloadData.uiMpuTempCount = 0;
}

void MpuRead()
{
  if (ucSign1)
  {
    ucSign1 = 0;
    if (sRe_buf1[0] == 0x55)   // Verifica o Header
    {
      switch (sRe_buf1 [1])
      {
        case 0x51:
          //       DEB.println("leitura de Aceleração");
          stPayloadData.fAcel[0] = (short(sRe_buf1 [3] << 8 | sRe_buf1 [2])) / 32768.0 * 16;
          stPayloadData.fAcel[1] = (short(sRe_buf1 [5] << 8 | sRe_buf1 [4])) / 32768.0 * 16;
          stPayloadData.fAcel[2] = (short(sRe_buf1 [7] << 8 | sRe_buf1 [6])) / 32768.0 * 16;
          stPayloadData.fMpuTemp = (short(sRe_buf1 [9] << 8 | sRe_buf1 [8])) / 340.0 + 36.25;
          if (stPayloadData.uiAcelCount++ >= 999) stPayloadData.uiAcelCount = 0;
          stPayloadData.uiMpuTempCount++;
          break;
        case 0x52:
          //       DEB.println("leitura de Ang Speed");
          stPayloadData.fAspeed[0] = (short(sRe_buf1 [3] << 8 | sRe_buf1 [2])) / 32768.0 * 2000;
          stPayloadData.fAspeed[1] = (short(sRe_buf1 [5] << 8 | sRe_buf1 [4])) / 32768.0 * 2000;
          stPayloadData.fAspeed[2] = (short(sRe_buf1 [7] << 8 | sRe_buf1 [6])) / 32768.0 * 2000;
          stPayloadData.fMpuTemp = (short(sRe_buf1 [9] << 8 | sRe_buf1 [8])) / 340.0 + 36.25;
          if (stPayloadData.uiAspeedCount++ >= 999) stPayloadData.uiAspeedCount = 0;
          stPayloadData.uiMpuTempCount++;
          break;
        case 0x53:
          //       DEB.println("leitura de Ang");
          stPayloadData.fAngle[0] = (short(sRe_buf1 [3] << 8 | sRe_buf1 [2])) / 32768.0 * 180;
          stPayloadData.fAngle[1] = (short(sRe_buf1 [5] << 8 | sRe_buf1 [4])) / 32768.0 * 180;
          stPayloadData.fAngle[2] = (short(sRe_buf1 [7] << 8 | sRe_buf1 [6])) / 32768.0 * 180;
          stPayloadData.fMpuTemp = (short(sRe_buf1 [9] << 8 | sRe_buf1 [8])) / 340.0 + 36.25;
          if (stPayloadData.uiAngleCount++ >= 999) stPayloadData.uiAngleCount = 0;
          if (stPayloadData.uiMpuTempCount++ >= 999) stPayloadData.uiMpuTempCount = 0;
          break;
      }
    }
  }
}
#endif
