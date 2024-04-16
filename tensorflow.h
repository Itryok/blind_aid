//*********************************************************************
//** Module       : TcpDebug.h
//** Directory    : \HandheldTester_ESP8266
//** Author/Date  : Anupam Kumar / 09.2017
//** Description  : Declaration of  CTcpDebug class.
//**
//** ------------------------------------------------------------------
//** Changes
//** Name/Date    : <Name>/RBIN / <mm/yyyy>
//** Description  : <Description>
//** ------------------------------------------------------------------
//** $Author: $
//** $Revision: 1.1 $
//** $Date:  $
//*********************************************************************

#ifndef _TENSORFLOW_SVR_
#define _TENSORFLOW_SVR_

//#include "..\WifiCfgManager\WifiCfgManager.h"

#include "FS.h"
//#include "WiFi.h"
#include "SPIFFS.h"

#include "..\ESPAsyncWebServer\ESPAsyncWebServer.h"


//#define TSFLOW_PRINT(x)  Serial.println (x)
#define TSFLOW_PRINT(x)


class CTensorflow
{
  AsyncWebServer *m_pTensorflow;

  static CTensorflow* m_pCInstance;

public:

  static CTensorflow* GetInstance();

  CTensorflow(long lPort);

public:

   
};

#endif //_TCPDBGSVR_
