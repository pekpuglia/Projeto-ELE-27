#ifdef SDCARD
const int chipSelect = PA4;

int SDcardSetup()
{
  DEB.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, 1);
  delay(100);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    DEB.println("Card failed, or not present");
    delay(100);
    return (1); //Indica processo de iniciação FALHO
  }
  DEB.println("card initialized.");
  // Escreve cabeçalho no SDCard
  sprintf(sBuffTmp, "<;ID#;DATA;HORA;PRE;T1;ALT;T2;UM;AX;AY;AZ;CTA;WX;WY;WZ;CTW;ANX;ANY;ANZ;TMPU;CMPU;OZH;OZL;>");
  sBuffTmp[strlen(sBuffTmp)] = 0;   //Garante que o buffer tem terminação de string
  writeToSDcard(sBuffTmp, 1);
  return (0); //Indica processo de iniciação OK
}// void SDcardInit()

int writeToSDcard(char *currentDataString, int iCab)
{
  int iTmp;
  char sTmp[200];
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile)
  {
    if (iCab) sprintf(sTmp, "%s", currentDataString);
    else
    {
      sprintf(sTmp, "<;%s;>", currentDataString);
      iTmp = strlen(sTmp);
      if (iTmp >= 200)
      {
        DEB.println("Error:Data to long to be writen into SDCARD ...");
        return (1);
      }
    }
    dataFile.println(sTmp);
    indicaLed(IND_SDCARD);
    digitalWrite(LED_PROC1, OFF);
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    DEB.println("error opening datalog.csv");
    return (1);
    bSDCARDStatus = FALSE;  //Indica falha na abertura ou escrita no arquivo
  }
    dataFile.close();
  return (0);
}
int readFromSDcard(char *currentDataString, File dataFile)
{
  uint32_t iTmp = 0;
  char sData[200];

  // if the file is available, read to it:
  if (dataFile)
  {
    //    while (dataFile.available())
    while (iTmp <= 200)
    {
      dataFile.available();
      currentDataString[iTmp++] = dataFile.read();
      if (currentDataString[iTmp - 1] == '>')
      {
        currentDataString[iTmp] = 0;
        uiQtdDados = uiQtdDados + iTmp;
        DEB.print("QtdDados= ");
        DEB.println(uiQtdDados);
        DEB.println(sBuffTmp);
        indicaLed1(IND_SDCARD);
        digitalWrite(LED_PROC1, OFF);
        return 1;
      }
      if (currentDataString[iTmp - 1] == '<')
      {
        iTmp = 1;
        currentDataString[0] = '<';
      }
    }
    // print to the serial port too:
  }
  else // if the file isn't open, pop up an error:
  {
    DEB.println("error opening datalog.csv");
    bSDCARDStatus = FALSE;  //Indica falha na abertura ou escrita no arquivo
    return (1);
  }
  return iTmp;
}
#endif
