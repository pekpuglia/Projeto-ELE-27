#include <stdint.h>
#include <Wire.h>
#include "SPI.h"
#include <SD.h>
#include <MS5611.h>
#include "DHT.h"
#include <MQ131.h>

#include <libmaple/iwdg.h>                        //Usada no watchdog
#include <RTClock.h>                              //Usada no RTC
#include "rtadjust.h"                             //Usada no RTC

extern tm_t tm;                                   //Usada no RTC
extern const int chipSelect;
const int SDA = PB7;
const int SCL = PB6;

#define ITACUBE1
//#define ITACUBE2
//#define ITACUBE3

#ifdef ITACUBE1
#define CMD_ASK_TLM "<CMD1:09,00>"        // ItaCube 1
#define TLM_HEADER "<PLD1;%05d;000;%s>\0"     // ItaCube 1
#define DHT22_DEF               // Se definido, será ativado o sensor DHT22
#endif

#ifdef ITACUBE2
#define CMD_ASK_TLM "<CMD2:09,00>"        // ItaCube 2
#define TLM_HEADER "<PLD2;%05d;000;%s>\0"     // ItaCube 2
#endif

#ifdef ITACUBE3
#define CMD_ASK_TLM "<CMD3:09,00>"          // ItaCube 3
#define TLM_HEADER "<PLD3;%05d;000;%s>\0"       // ItaCube 3
#define OZONIO            // Se definido, será ativado o sensor de Ozonio MQ-131
#define DHT22_DEF               // Se definido, será ativado o sensor DHT22
#endif

//------------------------ Defines para controle da compilação - diretivas do compilador
#define WATCHDOG            // Se definido, será compilado o tratamento de watchdog timer
#define MPU6050
#define SDCARD            // Se definido, será ativado o armazenamento de dados no SDCARD
#define BAROMETRO           // Se definido, será ativado o processamento do Barômetro
#define PROC_BUS_MSG      // Se definido, processa mensagens recebidas do BUS
#define PROCESS_SEND_TLM     // Se definido, será compilado o processamento dos parâmetros e salvos no SDCARD localmente
#define PROCESS_RD_TLM       // Se definido, será compilado o processamento dos parâmetros e enviados remotamente
//#define DEBUG_FROM_BUS    // Se definido, será ativado mostrar na serial DEB as mensagens recebidas ao BUS
//#define DEBUG_TO_BUS      // Se definido, será ativado mostrar na serial DEB as mensagens enviadas ao BUS


#define MPU Serial1
#define BUS Serial2
#define DEB Serial3
#define LED PC13          //LED
#define LED_PROC1  PB8    //LED VM de indicação de armazenamento de dados no SD CARD
#define LED_PROC2  PB9    //LED VD de indicação de envio de telemetria
#define OZ_HIGH PA0       //Leitura de OZ High Concentration
#define OZ_LOW PA1        //Leitura de OZ Low Concentration
#define TLM 1
#define CMD 2
#define DAT 3
#define TRUE 1
#define FALSE 0
#define PROC 2            // Indica que está lendo Frame da Serial
#define T_RDS 1000         // Tempo entre leituras dos sensores em ms, escrita em SDCARD
#define T_STL 300        // Tempo entre envio de dados de telemetria armazenados no SDCARD
#define T_RD_DHT 5000     // Tempo entre leituras do sensor DHT22 em ms
#define T_RD_BARO 4000    // Tempo entre leituras do sensor DHT22 em ms
#define T_RD_OZ 6000      // Tempo entre leituras do sensor DHT22 em ms
#define T_TLM 3000        // Tempo entre envio de leituras dos sensores em ms para Estação de Solo
#define T_LED 700         // Tempo de x ms de intervalo entre ligar/deslisgar led de indicação de SW rodando.
#define T_TLM_RETRY 400   // Intervalo de Tempo para reenvio de comando
#define MAX_RETRY_ALLOWED 10 // Maximo de tentativas de retry sem sucesso
#define T_TLM_RETRY_SLOW 3000 // Maximo de tentativas de retry sem sucesso
#define ON 1
#define OFF 0
#define BUFLEN 100
#define MSG_COUNT_POS 6   //PosiÇão do contador de mensagens na mensagem de dados do payload
#define TLM_LGT 12        // Posição para colocação da QTD bytes de Telemetria 
#define IND_START 1       //Ação para piscar os leds de SDCARD e TLM juntos
#define IND_SDCARD 2      //Ação para piscar o led de indicação de escrita no cartão
#define IND_TLM   3       //Ação para piscar o led de indicação de envio de TLM do Payload
#define STA_ATB_2 9   //Posição do início do atributo do comando na Mensagem

//----------------------- Variáveis Globais
File dataFile;
char sDataHora[25];
int  iSetRtc;
int  iCounter1, iCounter2, iCounter3, iRetryTlmCounter, iTimeToRetry;
byte ucSign1 = FALSE, ucSign2 = FALSE, ucSign3 = FALSE;
uint32_t uiQtdDados = 0, uiTlmTimer;
char sRe_buf1[200], sRe_buf2[200], sRe_buf3[200];
unsigned long ulTimeRDSensors, ulTimeToTLM, ulTimeToTLMRetry, ulTimeLed, ulTimeRD_DHT22, ulTimeRD_OZ, ulTimeRD_BARO, ulTimeDat;
unsigned int uiLMsgPld, uiRMsgPld, uiRMsgDat ;  //Contador de mensagens Local (telemetria para SDCARD) e Remoto (telemetria para Ground Station
byte ucLedOnOff;
char sBuffTmp[200];
String dataTLM = "";        //String para dados de Telemetria
String tmpBuf = "";         //String para tratamento de dados - temporário
int iIndex = 0, iAction = 0;
int iTLMStatus, iTLMWait;
bool bBarStatus = FALSE;
bool bSDCARDStatus = FALSE;
bool bDatSDCardSend = FALSE;
typedef struct
{
  unsigned int uiAcelCount;        // Contador de leituras de aceleração
  unsigned int uiAspeedCount;      // Contador de leituras de velecidade angular
  unsigned int uiAngleCount;       // Contador de leituras de ângulo
  unsigned int uiMpuTempCount;     // Contador de leituras de temperatura
  float fAcel[3];           // Aceleração da MPU
  float fAspeed[3];         // Velocidade Angular da MPU
  float fAngle[3];          // ângulo da MPU
  float fMpuTemp;           // Temperatura da MPU
  double dMS5611Press;        // Pressão do Barômetro MS5611
  double dMS5611Temp;       // Temperatura do Barômetro MS5611
  double dMS5611Alt;        // Altitude do Barômetro MS5611
  float fDHT22Temp;         // Temperatura do sensor DHT22
  float fDHT22Hum;          // Umidade do sensor DHT22
  uint16_t uiOZHigh;            //Valor do Ozonio em Alta concentração
  uint16_t uiOZLow;             //Valor do Ozonio em Baixa concentração
} sPD;
sPD stPayloadData;
String tmp;

//----------------
//Protótipos das funções
void BarometroRead();
int BarometroSetup();
void Dht22Setup();
void Dht22Read();
void MpuRead();
int SDcardSetup();
int writeToSDcard();
void rtcStart();
void ComSetup(HardwareSerial, unsigned int);         // Para as seriais 1, 2, 3
void ComSetup0(USBSerial com, unsigned int uiBaud);  // Para serial USB
void PayloadEvent(HardwareSerial com);
void sendCMD(int cmd);
void handler_wdt(void);
void setupTimers(int, long);


void setup()
{
  asm(".global _printf_float"); //Para funcionar os sprintf's no programa
  ComSetup(MPU, 9600);     //Inicia COM de MPU6050
  ComSetup(BUS, 115200);   //Inicia COM que comunica dados com o BUS
  ComSetup(DEB, 115200);   //Inicia COM de Debug
  DEB.println("Programa Iniciado...");

#ifdef SDCARD
  if (!SDcardSetup())
  {
    bSDCARDStatus = TRUE;
    DEB.println("SDCard initialized");
  } else
  {
    bSDCARDStatus = FALSE;// Indica fallha no SDCARD e não o utiliza
    DEB.println("SDCard fail");
  }
#endif
#ifdef WATCHDOG
  iwdg_init(IWDG_PRE_32, 0xfff); //Watchdog activation (tempo de 40k/32 * 4096 = 5,12s)
  handler_wdt();  //Alimenta whatchdog timer
#endif
  pinMode(LED, OUTPUT);
  pinMode(LED_PROC1, OUTPUT);
  pinMode(LED_PROC2, OUTPUT);
  pinMode(chipSelect, OUTPUT); // Declarando CS pin como pino de saída;
#ifdef BAROMETRO
  if (!(BarometroSetup()))
  {
    bBarStatus = TRUE;
    DEB.println("Barometro initialized");
  } else
  {
    bBarStatus = FALSE;
    DEB.println("Barometro fail");
  }
  ulTimeRD_BARO = millis();
#endif
#ifdef DHT22_DEF
  Dht22Setup();
  DEB.println("DHT22 initialized");
  ulTimeRD_DHT22 = millis();
#endif
#ifdef MPU6050
  MpuSetup();
  DEB.println("MPU6050 initialized");
#endif
#ifdef OZONIO
  OZSetup();
  DEB.println("MQ-131 Ozonio Sensor initialized");
  ulTimeRD_OZ = millis();
#endif
  ucLedOnOff = ON;
  //  setupTimers(2, T_LED * 1000);   //  Inicia timers de interrupção para rotina de tratamento do Led Flashing
  uiRMsgPld = uiLMsgPld = uiRMsgDat = 0;
  iSetRtc = FALSE;
  rtcStart();
  iCounter1 = iCounter2 = 0;
  ucSign1 = ucSign2 = 0;
  iTLMStatus = iTLMWait = FALSE;
  indicaLed(IND_START);
  initStructTLM();
  DEB.println("SW START ...");
  ulTimeToTLMRetry = millis();
  ulTimeToTLM = millis();
  uiTlmTimer = T_TLM;
  iTimeToRetry = T_TLM_RETRY;
  bDatSDCardSend = FALSE;
  iRetryTlmCounter = 0; // Zera contador de tentativas de envio de telemtria
} // void setup()

void loop()
{
  PayloadEvent(BUS); // Lê dados recebidos na serial do BUS
#ifdef MPU6050
  SerialEvent1(); // Lê dados recebidos na serial da MPU
  if (!bDatSDCardSend) MpuRead();
#endif //MPU6050

#ifdef BAROMETRO
  if ((millis() - ulTimeRD_BARO) > T_RD_BARO)  // Temporização para leitura de sensores
  {
    if (bBarStatus && (!bDatSDCardSend)) BarometroRead();
    ulTimeRD_BARO = millis();
  }
#endif //BAROMETRO

#ifdef DHT22_DEF
  if ((millis() - ulTimeRD_DHT22) > T_RD_DHT)  // Temporização para leitura de sensores
  {
    if (!bDatSDCardSend) Dht22Read();
    ulTimeRD_DHT22 = millis();
  }
#endif //DHT22_DEF

#ifdef OZONIO
  if ((millis() - ulTimeRD_OZ) > T_RD_OZ)  // Temporização para leitura de sensores
  {
    if (!bDatSDCardSend) OZRead();
    ulTimeRD_OZ = millis();
  }
#endif //OZONIO

#ifdef PROCESS_RD_TLM
  if ((millis() - ulTimeRDSensors) > T_RDS)  // Temporização para leitura de sensores
  {
    ulTimeRDSensors = millis();
    if (!bDatSDCardSend) processDataToSave();
    else processDataToSend();
    iTLMStatus = TRUE;    // Indica que tem os dados no buffer para serem enviados remotamente
  }
#endif //PROCESS_RD_TLM

#ifdef PROCESS_SEND_TLM
  if ((millis() - ulTimeToTLM) > uiTlmTimer)  // Temporização para envio de telemetria para estação de solo
  {
    ulTimeToTLM = millis();
    if (iTLMStatus == TRUE)
    {
      sendCMD(CMD);                   //Envia comando de solicitação de envio de Telemetria
      iTLMWait = TRUE;
      ulTimeToTLMRetry = millis();    //Sincroniza tempo de espera para reenvio
    }
  }

  if ((millis() - ulTimeToTLMRetry) > iTimeToRetry)  // Tenta reebviar comando Temporização para leitura de sensores
  {
    ulTimeToTLMRetry = millis();
    if (iTLMWait == TRUE) sendCMD(CMD);   // Se estiver tentando reenviar comando, reenvia de telemetria
    iRetryTlmCounter++;
  }
#endif //PROCESS_SEND_TLM

#ifdef PROC_BUS_MSG
  if (ucSign2 == TRUE)  //Verifica se tem mensagem do BUS
  {
#ifdef DEBUG_FROM_BUS
    DEB.println(sRe_buf2);
    delay(100);
#endif //DEBUG_FROM_BUS
    if (sRe_buf2[1] == 'G') // Se recebeu uma mensagem de GPS, extrai: Data/Hora para ajustar o RTC do STM32
    {
      tmpBuf = "";
      tmpBuf += sRe_buf2;
      iIndex = tmpBuf.indexOf(";2022");
      sprintf(sDataHora, "s%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c", sRe_buf2[iIndex + 1], sRe_buf2[iIndex + 2], sRe_buf2[iIndex + 3], sRe_buf2[iIndex + 4],
              sRe_buf2[iIndex - 2], sRe_buf2[iIndex - 1], sRe_buf2[iIndex - 5], sRe_buf2[iIndex - 4],   sRe_buf2[iIndex + 6], sRe_buf2[iIndex + 7],
              sRe_buf2[iIndex + 9], sRe_buf2[iIndex + 10], sRe_buf2[iIndex + 12],
              sRe_buf2[iIndex + 13]); // Formato de envio o parser = "sYYYY-MM-DD HH:MM:SS";
      settime(sDataHora, strlen(sDataHora)); // Ajusta o RTC com os dados de DataHora do GPS
      ucSign2 = FALSE;    //Zera indicação de tratamento de mensagens do BUS
    }
#ifdef SDCARD   // Escreve dados no SDCARD
    if (sRe_buf2[1] == 'C') // Se recebeu um comando do BUS, decodifica e executa
    {
      tmpBuf = "";
      tmpBuf += sRe_buf2;
      DEB.print("tmpBuf = ");
      DEB.println(tmpBuf);
      // Testa Comando para enviar telemetria salva em SDCARD
#ifdef ITACUBE1
      if (iIndex = tmpBuf.indexOf("CMD1:10,"))
#endif
#ifdef ITACUBE2
        if (iIndex = tmpBuf.indexOf("CMD2:10,"))
#endif
#ifdef ITACUBE3
          if (iIndex = tmpBuf.indexOf("CMD3:10,"))
#endif
          {
            tmp = "";
            tmp += tmpBuf.substring(STA_ATB_2, STA_ATB_2 + 2);
            DEB.print("tmp = ");
            DEB.println(tmp);
            uiTlmTimer = tmp.toInt() * T_STL;
            if(uiTlmTimer <= 0) uiTlmTimer= T_TLM;
            DEB.print("uiTlmTimer= ");
            DEB.println(uiTlmTimer);
            uiLMsgPld = 0;  //Zera contador de mensagens do PLD
            bDatSDCardSend = FALSE;    //Sinaliza envio de dados do SDCARD
          }
      // Testa Comando para parar de enviar telemetria salva em SDCARD
#ifdef ITACUBE1
      if ((iIndex = tmpBuf.indexOf("CMD1:10,00")) >= 4 )
#endif
#ifdef ITACUBE2
        if ((iIndex = tmpBuf.indexOf("CMD2:10,00")) >= 4 )
#endif
#ifdef ITACUBE3
          if ((iIndex = tmpBuf.indexOf("CMD3:10,00")) >= 4 )
#endif
          {
            bDatSDCardSend = FALSE;   //Sinaliza parada no envio de dados do SDCARD
            uiTlmTimer = T_TLM;
            uiLMsgPld = 0;  //Zera contador de mensagens do PLD
          }
      ucSign2 = FALSE;    //Zera indicação de tratamento de mensagens do BUS
    }
#endif //SDCARD
    if (sRe_buf2[1] == 'R') // Se a msg é de camando reconhecido pelo BUS, envia a telemetria do payload
    {
      sendCMD(TLM);
      iRetryTlmCounter = 0;
      iTLMStatus = FALSE;   // Indica que já enviou a Telemetria remota
      iTLMWait = FALSE;     // Cancela reenvio por ter recebido retorno do BUS
      ucSign2 = FALSE;    //Zera indicação de tratamento de mensagens do BUS
    }
  }

  if ((millis() - ulTimeLed) > T_LED)  // Tenta reebviar comando Temporização para leitura de sensores
  {
    ulTimeLed = millis();
    handler_led();
  }
  if (iRetryTlmCounter >= MAX_RETRY_ALLOWED) iTimeToRetry = T_TLM_RETRY_SLOW; // Se está havendo muitas tentativas de Retry, diminui frequencia
#endif //PROC_BUS_MSG

#ifdef WATCHDOG
  handler_wdt();  //Alimenta whatchdog timer
#endif
}

//----------------------------------------------------------------------------
// Rotina de tratamento do Watchdog  - Alimenta o contador para não zerar    -
//----------------------------------------------------------------------------
void handler_wdt(void)
{
  IWDG_BASE->KR = IWDG_KR_FEED;
}
//--------------------------------------------------------------------------------------
// Rotina de atribuição de rotinas de handler para os timers e configuração dos mesmos -                 -
//--------------------------------------------------------------------------------------
void setupTimers(int tim, long rate)
{
  switch (tim)
  {
    case 2:
      // Setup LED Timer
      Timer2.setMode(TIMER_CH1, TIMER_OUTPUTCOMPARE);
      Timer2.setPeriod(rate); //
      Timer2.setCompare(TIMER_CH1, 1);      // overflow might be small
      Timer2.attachInterrupt(TIMER_CH1, handler_led);
      break;
  }
}

//-------------------------------------------------------------------------------------------------------------------
// Rotina de tratamento do Led de status (running)                                                                  -
//-------------------------------------------------------------------------------------------------------------------
void handler_led(void)
{
  if (ucLedOnOff == ON)
  {
    if (digitalRead(LED)) digitalWrite(LED, OFF);
    else digitalWrite(LED, ON);
  }
}
void indicaLed(int iAction)
{
  for (int i = 0; i < 5; i++)
  {
    if ((iAction == IND_START) || (iAction == IND_SDCARD))
    {
      if (digitalRead(LED_PROC1)) digitalWrite(LED_PROC1, OFF); // Controla Led indicador de operação de escrita em disco
      else digitalWrite(LED_PROC1, ON);
    }
    if ((iAction == IND_START) || (iAction == IND_TLM))
    {
      if (digitalRead(LED_PROC2)) digitalWrite(LED_PROC2, OFF); // Controla Led indicador de operação do Envio de MSG de TLM
      else digitalWrite(LED_PROC2, ON);
    }
    delay(10);
  }
  digitalWrite(LED_PROC1, OFF);
  digitalWrite(LED_PROC2, OFF);
}

void indicaLed1(int iAction)  //Usado para leitura do SDCARD e envio de dados para o BUS
{
  for (int i = 0; i < 3; i++)
  {
    if ((iAction == IND_START) || (iAction == IND_SDCARD))
    {
      if (digitalRead(LED_PROC1)) digitalWrite(LED_PROC1, OFF); // Controla Led indicador de operação de escrita em disco
      else digitalWrite(LED_PROC1, ON);
    }
    if ((iAction == IND_START) || (iAction == IND_TLM))
    {
      if (digitalRead(LED_PROC2)) digitalWrite(LED_PROC2, OFF); // Controla Led indicador de operação do Envio de MSG de TLM
      else digitalWrite(LED_PROC2, ON);
    }
    delay(10);
  }
  digitalWrite(LED_PROC1, OFF);
  digitalWrite(LED_PROC2, OFF);
}

void processDataToSave()
{
  dataTLM = "";
  sprintf(sBuffTmp, "%.0f", stPayloadData.dMS5611Press);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%2.2f", stPayloadData.dMS5611Temp);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%2.2f", stPayloadData.dMS5611Alt);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%2.2f", stPayloadData.fDHT22Temp);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%2.2f", stPayloadData.fDHT22Hum);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%0.2f;%0.2f;%0.2f;%03d", stPayloadData.fAcel[0], stPayloadData.fAcel[1], stPayloadData.fAcel[2], stPayloadData.uiAcelCount);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%0.2f;%0.2f;%0.2f;%03d", stPayloadData.fAspeed[0], stPayloadData.fAspeed[1], stPayloadData.fAspeed[2], stPayloadData.uiAspeedCount);
  dataTLM += String(String(sBuffTmp)) + ";";
  sprintf(sBuffTmp, "%0.2f;%0.2f;%0.2f;%0.2f;%03d;%03X;%03X", stPayloadData.fAngle[0], stPayloadData.fAngle[1], stPayloadData.fAngle[2], stPayloadData.fMpuTemp,
          stPayloadData.uiAngleCount, stPayloadData.uiOZHigh, stPayloadData.uiOZLow);

  dataTLM += String(String(sBuffTmp)) + ";";
  tmpBuf = "";  //Zera buffer temporário
  memset(sBuffTmp, 0, sizeof(sBuffTmp));  // Zera buffer para próxima msg
  tmpBuf = dataTLM;
  rt.breakTime(rt.now(), tm);   // Lê a data/hora do RTC
  sprintf(sBuffTmp, "%05d;%02d%02d%04d;%02d:%02d:%02d;", uiLMsgPld++, tm.day, tm.month, tm.year + 1943, tm.hour, tm.minute, tm.second); // Copia formatados os dados no buf temporário
  tmpBuf.getBytes((unsigned char *) &sBuffTmp[strlen(sBuffTmp)], tmpBuf.length()); // Anexa os dados dos sensores
  sBuffTmp[strlen(sBuffTmp)] = 0;   //Garante que o buffer tem terminação de string
#ifdef SDCARD   // Escreve dados no SDCARD
  if (bSDCARDStatus) writeToSDcard(sBuffTmp, 0);
#endif
  //Zera variáveis referentes ao sensores
}

void processDataToSend()
{
  memset(sBuffTmp, 0, sizeof(sBuffTmp));  // Zera buffer para próxima msg
#ifdef SDCARD   // Escreve dados no SDCARD
  if (bSDCARDStatus && bDatSDCardSend)
  {
    readFromSDcard(sBuffTmp, dataFile);
    //    DEB.print("QtdDados= ");
    //    DEB.println(uiQtdDados);
    //    DEB.println(sBuffTmp);
  }
#endif
  //Zera variáveis referentes ao sensores
}

void initStructTLM()
{
  stPayloadData.fDHT22Hum = 0;
  stPayloadData.fDHT22Temp = 0;
  stPayloadData.dMS5611Temp = 0;
  stPayloadData.dMS5611Press = 0;
  stPayloadData.dMS5611Alt = 0;
}
void initMPU6050()
{
  stPayloadData.fAcel[0] = 0;
  stPayloadData.fAcel[1] = 0;
  stPayloadData.fAcel[2] = 0;
  stPayloadData.fAspeed[0] = 0;
  stPayloadData.fAspeed[1] = 0;
  stPayloadData.fAspeed[2] = 0;
  stPayloadData.fAngle[0] = 0;
  stPayloadData.fAngle[1] = 0;
  stPayloadData.fAngle[2] = 0;
  stPayloadData.fMpuTemp = 0;
}
#ifdef WATCHDOG
//------------------------------------------------------
// Rotina de iniciação dos registradores do watchdog   -
//------------------------------------------------------
void iwdg_init(iwdg_prescaler prescaler, uint16 reload)
{
  IWDG_BASE->KR = IWDG_KR_UNLOCK;
  IWDG_BASE->PR = prescaler;
  IWDG_BASE->RLR = reload;
  IWDG_BASE->KR = IWDG_KR_START;
  IWDG_BASE->KR = IWDG_KR_FEED;
}
#endif
