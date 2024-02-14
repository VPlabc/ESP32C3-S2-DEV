#include "Arduino.h"

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>ESP IOT DASHBOARD</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" type="image/png" href="favicon.png">
        <style>html {font-family: Arial, Helvetica, sans-serif;display: inline-block;text-align: center;}h1 {font-size: 1.8rem;color: white;}.topnav {overflow: hidden;background-color: #0A1128;}body {margin: 0;}.content {padding: 50px;}.card-grid {max-width: 800px;margin: 0 auto;display: grid;grid-gap: 2rem;grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}.card {background-color: white;box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);}.card-title {font-size: 1.2rem;font-weight: bold;color: #034078}.reading {font-size: 1.2rem;color: #1282A2;}.button {padding: 15px 50px;font-size: 24px;text-align: center;outline: none;color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);   }   /*.button:hover {background-color: #0f8b8d}*/   .button:active { background-color: #0f8b8d; box-shadow: 2 2px #CDCDCD; transform: translateY(2px);   }   .state { font-size: 1.5rem; color:#8c8c8c; font-weight: bold;   }   .content {padding: 30px;max-width: 600px;margin: 0 auto;  }  .card {background-color: #F8F7F9;;box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);padding-top:10px;padding-bottom:20px;  }    
        </style>
        </head>    <body>
<div class="topnav">
<h1>Cân Điện tử</h1>
        </div>
        <div class="content">
            <div class="card-grid">
                <div class="card">
                    <p class="card-title"> Tên NV</p>
                    <input type="text" class="reading" id="name" />
                    <p class="card-title"> Ca làm việc</p>
                    <input type="text" class="reading" id="ca" />
                    <p class="card-title"> Loại hàng</p>
                    <input type="text" class="reading" id="loai" />
                    <p><button id="buttoncheck" class="button">Check</button></p>
                </div>
                <div class="card">
                    <p class="card-title"> khoi luong</p>
                    <p class="reading"><span id="khoiluong"></span></p>
                </div>
                <div class="card">
                    <p class="card-title"> Printer State</p>
                    <p class="reading"><span id="Printer State"></span></p>
                </div>
            </div>
        </div>
          <div class="content">
            <div class="card">
              <p class="state">state: <span id="State">%STATE%</span></p>
              <p><button id="button" class="button">Toggle</button></p>
            </div>
          </div>
        <script>
            var gateway = `ws://4.3.2.1/ws`;
            var websocket;
            // Init web socket when the page loads
            window.addEventListener('load', onload);

            function onload(event) {
                initWebSocket();
                initButton();
                initButtonCheck();
            }

            function getReadings(){
                websocket.send("getReadings");
            }

            function initWebSocket() {
                console.log('Trying to open a WebSocket connection…');
                websocket = new WebSocket(gateway);
                websocket.onopen = onOpen;
                websocket.onclose = onClose;
                websocket.onmessage = onMessage;
            }

            // When websocket is established, call the getReadings() function
            function onOpen(event) {
                console.log('Connection opened');
                getReadings();
            }

            function onClose(event) {
                console.log('Connection closed');
                setTimeout(initWebSocket, 2000);
            }

            // Function that receives the message from the ESP32 with the readings
            function onMessage(event) {
                // console.log(event.data);
                var myObj = JSON.parse(event.data);
                var keys = Object.keys(myObj);

                for (var i = 0; i < keys.length; i++){
                    var key = keys[i];
                    document.getElementById(key).innerHTML = myObj[key];
                }   
                var state;
                // console.log(keys[2]);
                // console.log(myObj[keys[2]]);
                if(keys[2] == "State") {
                    state = "none";
                    if (myObj[keys[2]] == "1"){
                    state = "ON";
                    }
                    else{
                    state = "OFF";
                    }
                    // document.getElementById('state').innerHTML = state;
                }
            }

            
            function initButton() {
                document.getElementById('button').addEventListener('click', toggle);
            }
            function initButtonCheck() {
                document.getElementById('buttoncheck').addEventListener('click', checkdata);
            }
            function checkdata(){
                var a = document.getElementById("name").value;
                var b = document.getElementById("ca").value;
                var c = document.getElementById("loai").value;
                var dataout = "{'IN':'ok','NV':'"+a+"','CA':'"+b+"','LOAI':'"+c+"'}";
                console.log(dataout);
                websocket.send(dataout);
            }
            function toggle(){
                websocket.send('toggle');
            }
        </script>
    </body>
</html>)rawliteral";