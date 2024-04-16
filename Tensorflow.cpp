//*********************************************************************
//** Module       : TcpDebug.cpp
//** Directory    : \
//** Author/Date  : Anupam Kumar / 10.2017
//** Description  : Implementation of  CTcpDebug class.
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

#include "tensorflow.h"


const char tensorflow_html_head[] PROGMEM = R"rawliteral(<html>
<head>

<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
<script src="utility.js"></script>
<script src="https:/cdn.jsdelivr.net/npm/@tensorflow/tfjs@latest"></script> 
<script src="https:/cdn.jsdelivr.net/npm/@tensorflow-models/mobilenet"></script> 
<script src="https:/cdn.jsdelivr.net/npm/@tensorflow-models/blazeface"></script>

</head>

<body> 

<article>

<header>
      <nav>
        <a href="TensorFlow/local/main.html"> TensorFlow Js Face Detection</a>
      </nav>
    </header>

<div class="imagectrl"> 
  <p id="status">Not connected</p>
 <canvas id="canvas" width="640" height="480"></canvas>
 <p id="info">Info</p>
</div> 

</article>
  
<script type="text/javascript">
  var watchdog = 0;
  var img = 0;
  let urlObject;
  let net; 
  let model_blazeface;
  let model_others;
  
  state = document.getElementById("status");
  
  info = document.getElementById("info");
  
  var canvas = document.getElementById('canvas'),
        context = canvas.getContext('2d'),
        video = document.getElementById('image');
 
  if (window.WebSocket === undefined) 
  {
        state.innerHTML = "sockets not supported";
    }
  else 
  {
                  if (typeof String.prototype.startsWith != "function") 
          {
                     String.prototype.startsWith = function (str) 
           {
                        return this.indexOf(str) == 0;
                     };
                  }
				  
				  img = new Image;
				  
          console.log('Successfully loaded model');
          app_mobilenet_load();
          setInterval(showTime, 250);
                  window.addEventListener("load", onLoad, false);
    }
  
  
  function showTime() 
  {
    var d = new Date();
	
    if(varOpen === 0)
	{
		clearInterval(watchdog);
		varOpen = 1;
		state.innerHTML = d.toLocaleTimeString().concat(",Communication Error");
		onLoad();
	}
	else
	{
		ImageUpdateGetStatus();
		
		if (wImageReadCount === 25) 
		{
			ping();
			return;				
		}
			
		if (web_socket.readyState === SOCKET_OPEN) 
		{
			state.innerHTML = d.toLocaleTimeString();
			app_mobilenet_find();
		}
	}
		
  }

  
         
  async function app_mobilenet_find() 
  {
   draw(img, context, 640, 480);
  }
  
  async function app_mobilenet_load() 
  {
      console.log('Loading mobilenet..');
      state.innerHTML = "Loading mobilenet..";
      // Load the model.
      model_blazeface = await blazeface.load();
	  net = await mobilenet.load();
      state.innerHTML = "Successfully loaded blazeface";
      state.className = "success";
    }
         function onOpen(evt) 
         {
            console.log("Connected wbCAM");
			state.innerHTML = "Connected wbCAM";
			state.className = "success";
			varOpen = 1;
	     }
          
         function onClose(evt) 
		{
            console.log("Closed wbCAM");
			state.innerHTML = "Closed wbCAM";
			state.className = "offline";
			varOpen = 0;
        }
          
		  function onError(evt) 
        {
            console.log("Communication error");
            state.innerHTML = "Communication error";
            state.className = "offline";
			varOpen = 0;
        }
		  
         function onMessage(evt) 
         {
          const arrayBuffer = evt.data;
		  
          ImageUpdateReset();
		  
		  if (arrayBuffer == '__pong__') 
		  {
				pong();
				return;
		  }

          if (urlObject) 
          {
            URL.revokeObjectURL(urlObject)
          }
          urlObject = URL.createObjectURL(new Blob([arrayBuffer]));
		  if(img)
			img.src = urlObject;
         }  
         
          async function draw(video,context, width, height)
        {
          context.drawImage(video,0,0,width,height);
          const returnTensors = false;
          const predictions = await model_blazeface.estimateFaces(video, returnTensors);
            if (predictions.length > 0)
            {
               console.log(predictions);
               for (let i = 0; i < predictions.length; i++) 
               {
                 const start = predictions[i].topLeft;
                 const end = predictions[i].bottomRight;
                 var probability = predictions[i].probability;
                 const size = [end[0] - start[0], end[1] - start[1]];
                 context.beginPath();
                 context.strokeStyle="green";
                 context.lineWidth = "4";
                 context.rect(start[0], start[1],size[0], size[1]);
                 context.stroke();
                 var prob = (probability[0]*100).toPrecision(5).toString();
                 var text = prob+"%";
                 context.fillStyle = "red";
                 context.font = "13pt sans-serif";
                 context.fillText(text,start[0]+5,start[1]+20);
                 state.innerHTML = "Face detected.";
                 beep();
              }
             }
             else
             {
             
			 const result = await net.classify(video);
				   
				   if (result.length > 0)
				   {
						state.innerHTML = JSON.stringify(result);
						
				var arr = result;
				
				for (var i = 0; i < arr.length; i++)
				{
					var obj = arr[i];					
					
					for (var key in obj)
					{
						value = obj[key];
					
						if(typeof value === "string")
						{
							var nameArr = value.split(',');
							
							if(nameArr.length)
							{
								for (var j = 0; j < nameArr.length; j++)
								{
									var obj_name = nameArr[j];	
									
									if(obj_name === "helicopter")
									{
										info.innerHTML = obj_name;
										
										
									}
									
									if(obj_name === "computer mouse")
									{
										info.innerHTML = obj_name;
										
									}
									
									if(obj_name === "screwdriver")
									{
										info.innerHTML = obj_name;
										
									}
									
									if(obj_name === "computer keyboard")
									{
										info.innerHTML = obj_name;
										var audio = new Audio("C:\ESP32_Cam\Esp32_Cam\src\Local_Server\Recording.mp3");
										audio.play();
										
										
									}
									
									
								}
							}
						}
					}
				}
				   }
               
             }
        }
  
        

        function onLoad() 
        {
                  web_socket = new WebSocket("ws:/)rawliteral";
				  
	
const char tensorflow_html_tail[] PROGMEM = R"rawliteral(");
                  web_socket.onopen = function(evt) { onOpen(evt) };
                  web_socket.onclose = function(evt) { onClose(evt) };
                  web_socket.onmessage = function(evt) { onMessage(evt) };
                  web_socket.onerror = function(evt) { onError(evt) };
        }
</script>

</body>

</html>)rawliteral";

CTensorflow* CTensorflow::m_pCInstance = NULL;

//********************************************************************************
// Function Name        : GetInstance
// Input(s)             : None
// Output               : CTcpDebug*
// Class                : CTcpDebug
// Description          : Static method to create singleton class object.
// Author               : Anupam Kumar
// Creation Date        : 2017-10-15
// Modifications        :
//********************************************************************************

CTensorflow* CTensorflow::GetInstance() 
{
    if(m_pCInstance == NULL) 
    {
        m_pCInstance = new CTensorflow(8082);
    }
    return(m_pCInstance);
}


//********************************************************************************
// Function Name        : CTcpDebug
// Input(s)             : long lPort
// Output               : None
// Class                : CTcpDebug
// Description          : Constructor with port number as input parameter. Start Server.
// Author               : Anupam Kumar
// Creation Date        : 2017-10-15
// Modifications        :
//********************************************************************************

CTensorflow::CTensorflow(long lPort):m_pTensorflow(NULL)
{
    if(!m_pTensorflow)
	{
		 m_pTensorflow = new AsyncWebServer(lPort);
		 
		 if(m_pTensorflow)
		 {
			 
			  m_pTensorflow->on("/utility.js", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				TSFLOW_PRINT(String("Send utility.js file to client!"));
				request->send(SPIFFS, "/utility.js", "text/javascript");
			  });
			  
			  m_pTensorflow->on("/", HTTP_GET, [](AsyncWebServerRequest * request)
			  {
				 String strHTML = String(tensorflow_html_head);
				 
				 strHTML += String(WiFi.localIP().toString().c_str()) + String(":");
				 
				 //strHTML += String(CWIFICfgManager::GetInstance()->GetWebCamPort()) + String(tensorflow_html_tail);

				 strHTML += String(9999) + String(tensorflow_html_tail);
				 
				 request->send_P(200, "text/html", strHTML.c_str());
			  });
	
			m_pTensorflow->begin();
		 }
	}
}


