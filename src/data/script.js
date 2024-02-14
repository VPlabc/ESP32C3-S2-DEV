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