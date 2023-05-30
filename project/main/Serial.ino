
void ComSetup(HardwareSerial com, unsigned int uiBaud)
{
  com.begin(uiBaud);
  if (com == DEB)
  {
    ucSign3 = FALSE;
  }
  if (com == BUS)
  {
    ucSign2 = FALSE;
  }
  if (com == MPU)
  {
    ucSign1 = FALSE;
  }
}
void ComSetup0(USBSerial com, unsigned int uiBaud)
{
  com.begin(uiBaud);
}

void PayloadEvent(HardwareSerial com)
{
  while (com.available())
  {
    if (com == BUS)
    {
      if (ucSign2 == PROC)
      {
        sRe_buf2[iCounter2++] = com.read();
        if (sRe_buf2[iCounter2 - 1] == '>')
        {
          ucSign2 = TRUE;
          sRe_buf2[iCounter2] = 0;
          iCounter2 = iCounter2 - 1;
 //         DEB.println(sRe_buf2);
          return;
        }
      }
      if ((com.read()) == '<')
      {
        iCounter2 = 1;
        sRe_buf2[0] = '<';
        ucSign2 = PROC;
      }
    }
  }
}

void SerialEvent1()
{
  while (Serial1.available())
  {
    sRe_buf1[iCounter1] = (unsigned char)Serial1.read();
    if ((iCounter1 == 0) && (sRe_buf1[0] != 0x55)) return;  // Procura o Header
    iCounter1++;
    if (iCounter1 == 11)
    {
      iCounter1 = 0;
      ucSign1 = 1;
    }
  }
}

void sendCMD(int cmd)
{
  char tmpbuf[200];
  char tmp[4];
  if (cmd == CMD)   // Envia a o comando de envio de telemetria
  {
    BUS.println(CMD_ASK_TLM);
    delay(60);  // Tempo para enviar dados
  }

  if (cmd == TLM)   //Envia a telemetria
  {
    sprintf(tmpbuf, TLM_HEADER, uiRMsgPld++, (char *)&sBuffTmp[MSG_COUNT_POS]); // Copia formatados os dados no buf temporário
    sprintf(tmp, "%03d", strlen(tmpbuf));
    memcpy((char *) &tmpbuf[TLM_LGT], tmp, 3);          // Copia qtd bytes dentro do buffer de telemetria
    BUS.print(tmpbuf);                                  // Envia o cabeçalho dos dados para o BUS
    delay(150);   //Tempo para enviar dados
#ifdef DEBUG_TO_BUS
    DEB.println();
    DEB.print("#Dados to Bus=");
    DEB.println(strlen(tmpbuf));
    DEB.print(tmpbuf); //Mostra o que está enviando, na serial de debug
#endif
    indicaLed(IND_TLM);
  }
  if (cmd == DAT)   //Envia os dados coletados
  {
    sprintf(tmpbuf, TLM_HEADER, uiRMsgDat++, (char *)&sBuffTmp[MSG_COUNT_POS]); // Copia formatados os dados no buf temporário
    sprintf(tmp, "%03d", strlen(tmpbuf));
    memcpy((char *) &tmpbuf[TLM_LGT], tmp, 3);          // Copia qtd bytes dentro do buffer de telemetria
    BUS.print(tmpbuf);                                  // Envia o cabeçalho dos dados para o BUS
    delay(100);   //Tempo para enviar dados
#ifdef DEBUG_TO_BUS
    DEB.println();
    DEB.print("#Dados to Bus=");
    DEB.println(strlen(tmpbuf));
    DEB.print(tmpbuf); //Mostra o que está enviando, na serial de debug
#endif
    indicaLed1(IND_TLM);
  }
}
