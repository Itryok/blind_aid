#include <WiFi.h>

#include "src/Websockets/WebSocketsServer.h"
#include "src/Websockets/WebSocketsClient.h"
#include "src/ArduinoJson_5_13_5/ArduinoJson.h"
#include "src/Util_camera/Util_camera.h"
#include "src/ESPAsyncWebServer/ESPAsyncWebServer.h"
#include "src/FileHandle_Util/FileHandle_Util.h"

#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"

#include "src/Local_Server/tensorflow.h"

//#define DBG_PRINT(x)  Serial.println (x)
#define DBG_PRINT(x)

#define ENABLE_FILE_SAVE
//#undef  ENABLE_FILE_SAVE

char *ssid = "AndroidAPE734";
char *password = "rgnk5422";

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 180);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
//IPAddress primaryDNS(8, 8, 8, 8);   //optional
//IPAddress secondaryDNS(8, 8, 4, 4); //optional


#define LED_BUILTIN GPIO_NUM_4
WebSocketsServer *g_pwebSocket = NULL; 
bool g_bClientConnected = false;
uint8_t g_CamNo = 0;
unsigned char *g_pImageData = NULL;
SemaphoreHandle_t  g_xMutex;

#ifdef ENABLE_FILE_SAVE

//javascript
const char webcam_javascript[] PROGMEM = R"rawliteral(
let urlObject;
  

var video = document.getElementById('image');

         
function onOpen(evt) 
         {
                  console.log("Connected wbCAM");
             }
          
               function onClose(evt) 
         {
                  console.log("Closed wbCAM");
             }
          
               function onMessage(evt) 
         {
          const arrayBuffer = evt.data;
          updateIamge(arrayBuffer);
         }  
         
         async function updateIamge(imageData) 
         {
          const image = document.getElementById('image');

          if (urlObject) 
          {
            URL.revokeObjectURL(urlObject) // only required if you do that multiple times
          }
          urlObject = URL.createObjectURL(new Blob([imageData]));

          image.src = urlObject;
         }
 
        function onError(evt) 
        {
                 console.log("Communication error");
                }
 
function openAlert() 
{
    window.alert("Hello World");
}

async function beep() 
         {
          var sound = new Audio("data:audio/wav;base64,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");
          sound.play();
         }
         
  function Send_LED_ON() 
  {
    if(websocket)
    {
      websocket.send(JSON.stringify({FKT_CODE: "10000"}));
      beep();
    }
    else
    {
      alert("Not Connected Alert");
    }
  }
  
  function Send_LED_OFF() 
  {
    if(websocket)
    {
      websocket.send(JSON.stringify({FKT_CODE: "10001"}));
      beep();
    }
    else
    {
      alert("Not Connected Alert");
    }
  }
  
  function Send_WIFI_RESET() 
  {
    if(websocket)
    {
      websocket.send(JSON.stringify({FKT_CODE: "10002"}));
    }
    else
    {
      alert("Demo Alert");
    }
  }
)rawliteral";

const char utility_javascript[] PROGMEM = R"rawliteral(

       var wImageReadCount =0;
       var varOpen = 0;
       var tm =0;
       let web_socket = 0;
       
       var SOCKET_CONNECTING = 0;
       var SOCKET_OPEN = 1;
       var SOCKET_CLOSING = 2;
       var SOCKET_CLOSED = 3;

        async function ImageUpdateGetStatus()
        {
            wImageReadCount++;
        }
        
        async function ImageUpdateReset()
        {
            wImageReadCount = 0;
        }
  
       function ping() 
       {
        web_socket.send('__ping__');
    
        tm = setTimeout(function () 
        {
            web_socket.close();
            varOpen = 0;
            web_socket = 0;
            }, 10000);
       }
       
       function pong() 
       {
          clearTimeout(tm);
          tm =0;
       }
  
         async function beep() 
         {
          var sound = new Audio("data:audio/wav;base64,UklGRnoGAABXQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YQoGAACBhYqFbF1fdJivrJBhNjVgodDbq2EcBj+a2/LDciUFLIHO8tiJNwgZaLvt559NEAxQp+PwtmMcBjiR1/LMeSwFJHfH8N2QQAoUXrTp66hVFApGn+DyvmwhBTGH0fPTgjMGHm7A7+OZSA0PVqzn77BdGAg+ltryxnMpBSl+zPLaizsIGGS57OihUBELTKXh8bllHgU2jdXzzn0vBSF1xe/glEILElyx6OyrWBUIQ5zd8sFuJAUuhM/z1YU2Bhxqvu7mnEoODlOq5O+zYBoGPJPY88p2KwUme8rx3I4+CRZiturqpVITC0mi4PK8aB8GM4nU8tGAMQYfcsLu45ZFDBFYr+ftrVoXCECY3PLEcSYELIHO8diJOQcZaLvt559NEAxPqOPwtmMcBjiP1/PMeS0GI3fH8N2RQAoUXrTp66hVFApGnt/yvmwhBTCG0fPTgjQGHW/A7eSaRw0PVqzl77BeGQc9ltvyxnUoBSh+zPDaizsIGGS56+mjTxELTKXh8bllHgU1jdT0z3wvBSJ0xe/glEILElyx6OyrWRUIRJve8sFuJAUug8/y1oU2Bhxqvu3mnEoPDlOq5O+zYRsGPJLZ88p3KgUme8rx3I4+CRVht+rqpVMSC0mh4fK8aiAFM4nU8tGAMQYfccPu45ZFDBFYr+ftrVwWCECY3PLEcSYGK4DN8tiIOQcZZ7zs56BODwxPpuPxtmQcBjiP1/PMeywGI3fH8N+RQAoUXrTp66hWEwlGnt/yv2wiBDCG0fPTgzQHHG/A7eSaSQ0PVqvm77BeGQc9ltrzxnUoBSh9y/HajDsIF2W56+mjUREKTKPi8blnHgU1jdTy0HwvBSF0xPDglEQKElux6eyrWRUJQ5vd88FwJAQug8/y1oY2Bhxqvu3mnEwODVKp5e+zYRsGOpPX88p3KgUmecnw3Y4/CBVhtuvqpVMSC0mh4PG9aiAFM4nS89GAMQYfccLv45dGCxFYrufur1sYB0CY3PLEcycFKoDN8tiIOQcZZ7rs56BODwxPpuPxtmQdBTiP1/PMey4FI3bH8d+RQQkUXbPq66hWFQlGnt/yv2wiBDCG0PPTgzUGHG3A7uSaSQ0PVKzm7rJeGAc9ltrzyHQpBSh9y/HajDwIF2S46+mjUREKTKPi8blnHwU1jdTy0H4wBiF0xPDglEQKElux5+2sWBUJQ5vd88NvJAUtg87y1oY3Bxtpve3mnUsODlKp5PC1YRsHOpHY88p3LAUlecnw3Y8+CBZhtuvqpVMSC0mh4PG9aiAFMojT89GBMgUfccLv45dGDRBYrufur1sYB0CX2/PEcycFKoDN8tiKOQgZZ7vs56BOEQxPpuPxt2MdBTeP1vTNei4FI3bH79+RQQsUXbTo7KlXFAlFnd7zv2wiBDCF0fLUgzUGHG3A7uSaSQ0PVKzm7rJfGQc9lNrzyHUpBCh9y/HajDwJFmS46+mjUhEKTKLh8btmHwU1i9Xyz34wBiFzxfDglUMMEVux5+2sWhYIQprd88NvJAUsgs/y1oY3Bxpqve3mnUsODlKp5PC1YhsGOpHY88p5KwUlecnw3Y8+ChVgtunqp1QTCkig4PG9ayEEMojT89GBMgUfb8Lv4pdGDRBXr+fur1wXB0CX2/PEcycFKn/M8diKOQgZZrvs56BPEAxOpePxt2UcBzaP1vLOfC0FJHbH79+RQQsUXbTo7KlXFAlFnd7xwG4jBS+F0fLUhDQGHG3A7uSbSg0PVKrl7rJfGQc9lNn0yHUpBCh7yvLajTsJFmS46umkUREMSqPh8btoHgY0i9Tz0H4wBiFzw+/hlUULEVqw6O2sWhYIQprc88NxJQUsgs/y1oY3BxpqvO7mnUwPDVKo5PC1YhsGOpHY8sp5KwUleMjx3Y9ACRVgterqp1QTCkig3/K+aiEGMYjS89GBMgceb8Hu45lHDBBXrebvr1wYBz+Y2/PGcigEKn/M8dqJOwgZZrrs6KFOEAxOpd/js2coGUCLydq6e0MlP3uwybiNWDhEa5yztJRrS0lnjKOkk3leWGeAlZePfHRpbH2JhoJ+fXl9TElTVEQAAABJTkZPSUNSRAsAAAAyMDAxLTAxLTIzAABJRU5HCwAAAFRlZCBCcm9va3MAAElTRlQQAAAAU291bmQgRm9yZ2UgNC41AA==");
          sound.play();
         }

)rawliteral";

#endif

 void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  DynamicJsonBuffer jsonBuffer;
  char ayBuffer[255];
  
  switch (type)
  {
    case WStype_DISCONNECTED:
         g_bClientConnected = false;
         g_CamNo = 0;
         break;

    case WStype_CONNECTED:
         CUtilCamera::GetInstance()->InitReadImage();
         g_bClientConnected = true;
         g_CamNo = num;
         break;

    case WStype_TEXT:
               {
                    DBG_PRINT(String("webSocketEvent:") + String((char*)payload));

                    if(String((char*)payload).equals("__ping__"))
                    {
                      g_pwebSocket->sendTXT(g_CamNo, "__pong__");
                      DBG_PRINT("__ping__");
                      
                      if(CUtilCamera::GetInstance()->ReadImage(NULL) == 0)
                      {
                          CUtilCamera::GetInstance()->InitReadImage();
                      }
                      return;
                                     
                    }
                  
                  
                  if(String((char*)payload).equals("LED_ON"))
                  {
                    DBG_PRINT("Turn LED-ON");
                    digitalWrite(LED_BUILTIN, HIGH);
                    return;
                  }
          
                  if (String((char*)payload).equals("LED_OFF"))
                  {
                    DBG_PRINT("Turn LED-OFF");
                    digitalWrite(LED_BUILTIN, LOW);
                    return;
                  }

                  if (String((char*)payload).equals("RESET"))
                  {
                    DBG_PRINT("Reset WIFI");
                   // CWIFICfgManager::GetInstance()->ResetWifi(0);
                    return;
                  }
                  DBG_PRINT(String("webSocketEvent:") + String((char*)payload));
        
                  JsonObject& json = jsonBuffer.parseObject(String((char*)payload).c_str());
                  
                  if (json.success())
                  {
                      DBG_PRINT(String("webSocketEvent:JSON Parsing OK"));
                      strcpy(ayBuffer, json["FKT_CODE"]);
                      DBG_PRINT(String("webSocketEvent:") + String(ayBuffer));

                      switch(atol(ayBuffer))
                      {
                        case 10000:
                          DBG_PRINT(String("webSocketEvent:") + String("Turn LED-ON"));
                          digitalWrite(LED_BUILTIN, HIGH);
                          break;

                        case 10001:
                          DBG_PRINT(String("webSocketEvent:") + String("Turn LED-OFF"));
                          digitalWrite(LED_BUILTIN, LOW);
                          break; 
                          
                       case 10002:
                          DBG_PRINT(String("webSocketEvent:") + String("WIFI-RESET"));
                          //CWIFICfgManager::GetInstance()->ResetWifi(0);
                          break;
                          
                        default:
                          break;
                      }
                  }
                  else
                  {
                    DBG_PRINT("webSocketEvent:JSON Parsing Failed!");
                  }
               }
      break;

    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      DBG_PRINT(type);
      break;
  }
}

void initWiFi() 
{
  //Setup Static IP
  
  //if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) 
  //{
    //Serial.println("STA Failed to configure");
  //}
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() 
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector\
  \
  
  
  Serial.begin(115200);

  g_xMutex = xSemaphoreCreateMutex();

  initWiFi();

  #ifdef ENABLE_FILE_SAVE

      if(CFileHandle_Util::GetInstance()->FindDataFile("/webcam.js") == false)
      {
        DBG_PRINT(String("Write data to SPIFFS file."));
        CFileHandle_Util::GetInstance()->WriteDataToFile("/webcam.js", (char*)&webcam_javascript[0], strlen(webcam_javascript));
      }
      else
      {
        DBG_PRINT(String("File Exist in SPIFFS file."));
      }

      if(CFileHandle_Util::GetInstance()->FindDataFile("/utility.js") == false)
      {
        DBG_PRINT(String("Write data to SPIFFS file."));
        CFileHandle_Util::GetInstance()->WriteDataToFile("/utility.js", (char*)&utility_javascript[0], strlen(utility_javascript));
      }
      else
      {
        DBG_PRINT(String("File Exist in SPIFFS file."));
      }
 
 #endif  //ENABLE_FILE_SAVE
 

pinMode (LED_BUILTIN, OUTPUT);//Specify that LED pin is
          
          if(CUtilCamera::GetInstance()->UtilCameraInit() == false)
          {
            ESP. restart();
          }
          
  CTensorflow::GetInstance();
          
          g_pwebSocket = new WebSocketsServer(9999);
          if (g_pwebSocket)
          {
            g_pwebSocket->begin();
            g_pwebSocket->onEvent(webSocketEvent);
          }

}

void loop() 
{
long lRet = 0;
unsigned long lStartTime = 0;

   
        if (g_pwebSocket)
        {
          g_pwebSocket->loop();
        }
        else
        {
           return;
        }

        if(g_bClientConnected == true)
        {
          lRet = CUtilCamera::GetInstance()->ReadImage(NULL);
          if(lRet)
          {
            if(g_pImageData)
            {
               free(g_pImageData);
               g_pImageData = NULL;
            }
               
             g_pImageData = (unsigned char*) heap_caps_malloc(lRet, MALLOC_CAP_SPIRAM);

             if(g_pImageData)
             {
               lRet  = CUtilCamera::GetInstance()->ReadImage(g_pImageData);
               
               CUtilCamera::GetInstance()->InitReadImage();
                
               if(lRet)
               {
                  lStartTime = millis();
                  
                  g_pwebSocket->sendBIN(g_CamNo, (const uint8_t*)g_pImageData, lRet);

                  DBG_PRINT(String("loop:Camera Send Time = ") + String(millis() - lStartTime));
               }
              
               
             }
          }
        }
      
}
