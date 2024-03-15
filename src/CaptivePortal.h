// Captive Portal
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

// #include "html.h"
// Pre reading on the fundamentals of captive portals https://textslashplain.com/2022/06/24/captive-portals/
#define             FRMW_VERSION         "1.2236"
#define             PRGM_VERSION         "1.0"
#define         NUM_SENSORS 100
const String localIPURL = "http://1.1.1.1";	 // a string version of the local IP with http, used for redirecting clients to your webpage



const char *APssid = "Cân Điện Tử new";  // FYI The SSID can't have a space in it.
// const char * password = "12345678"; //Atleast 8 chars
const char *APpassword = NULL;  // no password

#define MAX_CLIENTS 4	// ESP32 supports up to 10 but I have not tested it yet
#define WIFI_CHANNEL 6	// 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)



const byte DNS_PORT = 53;
IPAddress apIP(1,1,1,1);
IPAddress subnet(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>CAPTIVE PORTAL</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s/app'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
	request->redirect(localIPURL);
  }
};


void PortalSetup(){
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APssid, APpassword);
  WiFi.softAPConfig(apIP, apIP, subnet);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  if(!MDNS.begin("canisoft")) {
     Serial.println("Error starting mDNS");
     return;
  }

  
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.on("/app", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });
  	//======================== Webserver ========================
	// WARNING IOS (and maybe macos) WILL NOT POP UP IF IT CONTAINS THE WORD "Success" https://www.esp8266.com/viewtopic.php?f=34&t=4398
	// SAFARI (IOS) IS STUPID, G-ZIPPED FILES CAN'T END IN .GZ https://github.com/homieiot/homie-esp8266/issues/476 this is fixed by the webserver serve static function.
	// SAFARI (IOS) there is a 128KB limit to the size of the HTML. The HTML can reference external resources/images that bring the total over 128KB
	// SAFARI (IOS) popup browser has some severe limitations (javascript disabled, cookies disabled)

	// Required
	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	// Background responses: Probably not all are Required, but some are. Others might speed things up?
	// A Tier (commonly used by modern systems)
	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home

	// B Tier (uncommon)
	//  server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
	//  server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
	//  server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
	//  server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});

	// return 404 to webpage icon
	server.on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	// webpage icon

	// Serve Basic HTML Page
	// server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
	// 	AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
	// 	response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
	// 	request->send(response);
	// 	Serial.println("Served Basic HTML Page");
	// });
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
    connectClinent = true;
  });
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
  server.begin();
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void Portalloop(){
  dnsServer.processNextRequest();
}

// void setUpWebserver(AsyncWebServer &server, const IPAddress &localIP) {
// 	//======================== Webserver ========================
// 	// WARNING IOS (and maybe macos) WILL NOT POP UP IF IT CONTAINS THE WORD "Success" https://www.esp8266.com/viewtopic.php?f=34&t=4398
// 	// SAFARI (IOS) IS STUPID, G-ZIPPED FILES CAN'T END IN .GZ https://github.com/homieiot/homie-esp8266/issues/476 this is fixed by the webserver serve static function.
// 	// SAFARI (IOS) there is a 128KB limit to the size of the HTML. The HTML can reference external resources/images that bring the total over 128KB
// 	// SAFARI (IOS) popup browser has some severe limitations (javascript disabled, cookies disabled)

// 	// Required
// 	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
// 	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

// 	// Background responses: Probably not all are Required, but some are. Others might speed things up?
// 	// A Tier (commonly used by modern systems)
// 	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
// 	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
// 	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
// 	server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
// 	server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
// 	server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home

// 	// B Tier (uncommon)
// 	//  server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
// 	//  server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
// 	//  server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
// 	//  server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});

// 	// return 404 to webpage icon
// 	server.on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	// webpage icon

// 	// Serve Basic HTML Page
// 	// server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
// 	// 	AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
// 	// 	response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
// 	// 	request->send(response);
// 	// 	Serial.println("Served Basic HTML Page");
// 	// });

// 	// the catch all
	
// }


// void handleConfig() {

//   if (server.method() == HTTP_POST) {
//     strncpy(gateway_settings.ssid,              server.arg("ssid").c_str(),             sizeof(gateway_settings.ssid) );
//     strncpy(gateway_settings.password,          server.arg("password").c_str(),         sizeof(gateway_settings.password) );
//     gateway_settings.ssid[server.arg("ssid").length()]  = gateway_settings.password[server.arg("password").length()]  =  0;  // string terminate
//     strncpy(gateway_settings.pversion, PRGM_VERSION , sizeof(PRGM_VERSION) );
//     EEPROM.put(0, gateway_settings);
//     EEPROM.commit();
//     String s = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>";
//     s += "<meta content='text/html;charset=utf-8' http-equiv='Content-Type'>";
//     s += "<title>Wireless Serial</title>";
//     s += "  <link rel='icon' type='image/png' sizes='16x16' href='data:image/x-icon;base64,AAABAAEAEBAAAAEAIABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAQAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIdUOc6IVTvpiFU654hVOuaIVTvkiVU74IdUOsqFUDaMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACIVDvli1c9/4pWPf+KVj3/ilY9/4pXPf+LVz3/jVk+/4lWPPaBSS5pAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgk064oVRPf+EUDz/hFA8/4RQPP98Rz3/fUk8/4VRPP+KVzz/jVk+/4ROMXwAAAAA/8s1///LNf//yzX//8s1///KM///yjL//8oy///KMv//yjL//8ky//C4Nf+wfjr/f0o8/4pWPP+LVz3/AAAAAP/FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xjP//8wy/76LOf+DTzz/jFg9/4ZTOLX/xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xzP/f0s8/4pWPP+IVTvj/8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8oy/6JwO/+HUzz/ilY88//FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///KMv+fbTv/h1Q8/4pWPPP/xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///GM///xTP/fUg8/4pWPP+JVjri/8Yz///GM///xjP//8Yz///GM///xjP//8Yz///GM///xjP//8Yz///HM///yzL/sX46/4RRPP+MWD3/h1I4sf/GMvT/xjL0/8Yy9P/FMvP4wDP99740//e+NP/3vjT/9740//O6NP/dpzf/m2k7/4JOPP+KVjz/iVY8/wAAAAAAAAAAAAAAAAAAAAAAAAAAd0A74X5IPf98Rzz/fEc8/3xHPP99SDz/gEw8/4dTPP+LVz3/jFg9/4FKL2sAAAAAAAAAAAAAAAAAAAAAAAAAAIhVOuaMWD7/i1c9/4tXPf+LVz3/i1c9/4xYPf+OWT//iFQ75XxDJ0gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACHUji2iFQ6zohTOc2HVDnMh1M5yodUOsaGUjerf0YqVAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//8AAPAPAADwAwAA8AEAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAA8AEAAPADAADwDwAA//8AAA=='/>";
//     s += "<meta content='utf-8' http-equiv='encoding'>";
//     s += "<style>*,::after,::before{box-sizing:border-box}body{margin:0;font-family:system-ui,-apple-system,'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans',sans-serif,'Apple Color Emoji','Segoe UI Emoji','Segoe UI Symbol','Noto Color Emoji';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#fff;-webkit-text-size-adjust:100%;-webkit-tap-highlight-color:transparent}a{color:#0d6efd;text-decoration:underline}a:hover{color:#0a58ca}::-moz-focus-inner{padding:0;border-style:none}.container{width:100%;padding-right:var(--bs-gutter-x,.75rem);padding-left:var(--bs-gutter-x,.75rem);margin-right:auto;margin-left:auto}@media (min-width:576px){.container{max-width:540px}}@media (min-width:768px){.container{max-width:720px}}@media (min-width:992px){.container{max-width:960px}}@media (min-width:1200px){.container{max-width:1140px}}@media (min-width:1400px){.container{max-width:1320px}}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}@media (min-width:768px){.col-md-12{flex:0 0 auto;width:100%}}.d-flex{display:flex!important}.d-inline-flex{display:inline-flex!important}.border-bottom{border-bottom:1px solid #dee2e6!important}.flex-column{flex-direction:column!important}.justify-content-between{justify-content:space-between!important}.align-items-center{align-items:center!important}.mt-2{margin-top:.5rem!important}.mb-4{margin-bottom:1.5rem!important}.py-3{padding-top:1rem!important;padding-bottom:1rem!important}.pb-3{padding-bottom:1rem!important}.fs-4{font-size:calc(1.275rem + .3vw)!important}.text-decoration-none{text-decoration:none!important}.text-dark{color:#212529!important}@media (min-width:768px){.flex-md-row{flex-direction:row!important}.mt-md-0{margin-top:0!important}.ms-md-auto{margin-left:auto!important}}@media (min-width:1200px){.fs-4{font-size:1.5rem!important}}body{display:flex;flex-wrap:nowrap;height:100vh;height:-webkit-fill-available;overflow-x:auto}body>*{flex-shrink:0;min-height:-webkit-fill-available}a{color:#3d568a!important}.logo{padding-left:40px;background-size:40px 18px;background-image:url(data:image/svg+xml,%3Csvg width='469' height='197' viewBox='0 0 469 197' fill='none' xmlns='http://www.w3.org/2000/svg'%3E%3Crect width='469' height='197' fill='%23F2F2F2'/%3E%3Crect width='469' height='197' fill='white'/%3E%3Cpath d='M127.125 196C158.396 196 184.026 186.794 204.016 168.382C224.005 149.97 234 126.212 234 97.1091C234 68.0061 224.104 44.5455 204.312 26.7273C184.521 8.90909 158.792 0 127.125 0H63V196H127.125Z' fill='%233D568A'/%3E%3Cpath d='M0 156V40H127.837C148.103 40 164.197 45.1122 176.118 55.3365C188.039 65.5609 194 79.5962 194 97.4423C194 115.103 187.94 129.324 175.82 140.106C163.501 150.702 147.507 156 127.837 156H0Z' fill='%2332C5FF'/%3E%3Cpath d='M54.8659 136V96.0774L73.1454 119.714H77.9906L96.2701 96.0774V136H111.136V60H106.291L75.568 100.488L44.8452 60H40V136H54.8659Z' fill='white'/%3E%3Cpath d='M135.241 136V108.011C135.241 103.251 136.554 99.6253 139.181 97.1324C141.808 94.6394 145.416 93.393 150.004 93.393H154V79.9084C152.594 79.4551 150.966 79.2285 149.116 79.2285C142.826 79.2285 137.794 81.7215 134.02 86.7073V79.9084H120.256V136H135.241Z' fill='white'/%3E%3Cpath d='M284 98V1H244V98H284Z' fill='%233D568A'/%3E%3Cpath d='M284 197V98H244V197H284Z' fill='%2332C5FF'/%3E%3Cpath d='M401.669 119.703L469 1H424.805L381.5 78.7506L338.492 1H294L361.627 120L401.669 119.703Z' fill='%2332C5FF'/%3E%3Cpath d='M402 197V119.261L381.852 78L362 119.56V197H402Z' fill='%233D568A'/%3E%3C/svg%3E);background-repeat:no-repeat}</style>";
//     s += "</head><body>";
//     s += "<div class='container py-3'>";
//     s += "<header>";
//     s += "  <div class='d-flex flex-column flex-md-row align-items-center pb-3 mb-4 border-bottom' style='border-bottom:1px solid #32C5FF!important;'>";
//     s += "    <a href='/' class='d-flex align-items-center text-dark text-decoration-none'><span class='fs-4 logo' style='height:19px;width:66px;'></span></a><span class='fs-4'>Wi-Fi Setup</span>";
//     s += "    <nav class='d-inline-flex mt-2 mt-md-0 ms-md-auto'>";
//     s += "    </nav>";
//     s += "  </div>";
//     s += "</header>";
//     s += "<body><main>";
//     s += "<div class='row justify-content-between'>";
//     s += "<div class='col-md-12' style='text-align: center;'>";
//     s += "<br /><h4 style='font-size:1.5rem;margin:0;'>Saved!</h4><br /><p>Please restart the device.</p>";
//     s += "</div>";
//     s += "</div></main></body></html>";
//     server.send(200, "text/html", s );

//   } else {

//     uint32_t realSize = 4000;//ESP.getFlashChipRealSize();
//     uint32_t ideSize = 4000;//ESP.getFlashChipSize();
//     String s = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1, user-scalable=no'/>";
//     s += "<meta content='text/html;charset=utf-8' http-equiv='Content-Type'>";
//     s += "<title>Wireless Serial</title>";
//     s += "  <link rel='icon' type='image/png' sizes='16x16' href='data:image/x-icon;base64,AAABAAEAEBAAAAEAIABoBAAAFgAAACgAAAAQAAAAIAAAAAEAIAAAAAAAQAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIdUOc6IVTvpiFU654hVOuaIVTvkiVU74IdUOsqFUDaMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACIVDvli1c9/4pWPf+KVj3/ilY9/4pXPf+LVz3/jVk+/4lWPPaBSS5pAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgk064oVRPf+EUDz/hFA8/4RQPP98Rz3/fUk8/4VRPP+KVzz/jVk+/4ROMXwAAAAA/8s1///LNf//yzX//8s1///KM///yjL//8oy///KMv//yjL//8ky//C4Nf+wfjr/f0o8/4pWPP+LVz3/AAAAAP/FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xjP//8wy/76LOf+DTzz/jFg9/4ZTOLX/xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xzP/f0s8/4pWPP+IVTvj/8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8oy/6JwO/+HUzz/ilY88//FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///KMv+fbTv/h1Q8/4pWPPP/xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///FM///xTP//8Uz///GM///xTP/fUg8/4pWPP+JVjri/8Yz///GM///xjP//8Yz///GM///xjP//8Yz///GM///xjP//8Yz///HM///yzL/sX46/4RRPP+MWD3/h1I4sf/GMvT/xjL0/8Yy9P/FMvP4wDP99740//e+NP/3vjT/9740//O6NP/dpzf/m2k7/4JOPP+KVjz/iVY8/wAAAAAAAAAAAAAAAAAAAAAAAAAAd0A74X5IPf98Rzz/fEc8/3xHPP99SDz/gEw8/4dTPP+LVz3/jFg9/4FKL2sAAAAAAAAAAAAAAAAAAAAAAAAAAIhVOuaMWD7/i1c9/4tXPf+LVz3/i1c9/4xYPf+OWT//iFQ75XxDJ0gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACHUji2iFQ6zohTOc2HVDnMh1M5yodUOsaGUjerf0YqVAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA//8AAPAPAADwAwAA8AEAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAA8AEAAPADAADwDwAA//8AAA=='/>";
//     s += "<meta content='utf-8' http-equiv='encoding'>";
//     s += "<style>*,::after,::before{box-sizing:border-box}body{margin:0;font-family:system-ui,-apple-system,'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans',sans-serif,'Apple Color Emoji','Segoe UI Emoji','Segoe UI Symbol','Noto Color Emoji';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#fff;-webkit-text-size-adjust:100%;-webkit-tap-highlight-color:transparent}a{color:#0d6efd;text-decoration:underline}a:hover{color:#0a58ca}label{display:inline-block}button{border-radius:0}button:focus:not(:focus-visible){outline:0}button,input{margin:0;font-family:inherit;font-size:inherit;line-height:inherit}button{text-transform:none}[type=submit],button{-webkit-appearance:button}::-moz-focus-inner{padding:0;border-style:none}.container{width:100%;padding-right:var(--bs-gutter-x,.75rem);padding-left:var(--bs-gutter-x,.75rem);margin-right:auto;margin-left:auto}@media (min-width:576px){.container{max-width:540px}}@media (min-width:768px){.container{max-width:720px}}@media (min-width:992px){.container{max-width:960px}}@media (min-width:1200px){.container{max-width:1140px}}@media (min-width:1400px){.container{max-width:1320px}}.row{--bs-gutter-x:1.5rem;--bs-gutter-y:0;display:flex;flex-wrap:wrap;margin-top:calc(var(--bs-gutter-y) * -1);margin-right:calc(var(--bs-gutter-x)/ -2);margin-left:calc(var(--bs-gutter-x)/ -2)}.row>*{flex-shrink:0;width:100%;max-width:100%;padding-right:calc(var(--bs-gutter-x)/ 2);padding-left:calc(var(--bs-gutter-x)/ 2);margin-top:var(--bs-gutter-y)}.col-12{flex:0 0 auto;width:100%}@media (min-width:768px){.col-md-12{flex:0 0 auto;width:100%}}.form-label{margin-bottom:.5rem}.form-control{display:block;width:100%;padding:.375rem .75rem;font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#fff;background-clip:padding-box;border:1px solid #ced4da;-webkit-appearance:none;-moz-appearance:none;appearance:none;border-radius:.25rem;transition:border-color .15s ease-in-out,box-shadow .15s ease-in-out}@media (prefers-reduced-motion:reduce){.form-control{transition:none}}.form-control:focus{color:#212529;background-color:#fff;border-color:#86b7fe;outline:0;box-shadow:0 0 0 .25rem rgba(13,110,253,.25)}.form-control::-moz-placeholder{color:#6c757d;opacity:1}.form-control::placeholder{color:#6c757d;opacity:1}.form-control:disabled{background-color:#e9ecef;opacity:1}.form-floating{position:relative}.btn{display:inline-block;font-weight:400;line-height:1.5;color:#212529;text-align:center;text-decoration:none;vertical-align:middle;cursor:pointer;-webkit-user-select:none;-moz-user-select:none;user-select:none;background-color:transparent;border:1px solid transparent;padding:.375rem .75rem;font-size:1rem;border-radius:.25rem;transition:color .15s ease-in-out,background-color .15s ease-in-out,border-color .15s ease-in-out,box-shadow .15s ease-in-out}@media (prefers-reduced-motion:reduce){.btn{transition:none}}.btn:hover{color:#212529}.btn:focus{outline:0;box-shadow:0 0 0 .25rem rgba(13,110,253,.25)}.btn:disabled{pointer-events:none;opacity:.65}.btn-primary{color:#fff;background-color:#0d6efd;border-color:#0d6efd}.btn-primary:hover{color:#fff;background-color:#0b5ed7;border-color:#0a58ca}.btn-primary:focus{color:#fff;background-color:#0b5ed7;border-color:#0a58ca;box-shadow:0 0 0 .25rem rgba(49,132,253,.5)}.btn-primary:active{color:#fff;background-color:#0a58ca;border-color:#0a53be}.btn-primary:active:focus{box-shadow:0 0 0 .25rem rgba(49,132,253,.5)}.btn-primary:disabled{color:#fff;background-color:#0d6efd;border-color:#0d6efd}.btn-lg{padding:.5rem 1rem;font-size:1.25rem;border-radius:.3rem}.d-flex{display:flex!important}.d-inline-flex{display:inline-flex!important}.border-bottom{border-bottom:1px solid #dee2e6!important}.flex-column{flex-direction:column!important}.justify-content-between{justify-content:space-between!important}.align-items-center{align-items:center!important}.mt-2{margin-top:.5rem!important}.mt-3{margin-top:1rem!important}.mb-4{margin-bottom:1.5rem!important}.py-3{padding-top:1rem!important;padding-bottom:1rem!important}.pb-3{padding-bottom:1rem!important}.fs-4{font-size:calc(1.275rem + .3vw)!important}.text-decoration-none{text-decoration:none!important}.text-dark{color:#212529!important}@media (min-width:768px){.flex-md-row{flex-direction:row!important}.mt-md-0{margin-top:0!important}.ms-md-auto{margin-left:auto!important}}@media (min-width:1200px){.fs-4{font-size:1.5rem!important}}body{display:flex;flex-wrap:nowrap;height:100vh;height:-webkit-fill-available;overflow-x:auto}body>*{flex-shrink:0;min-height:-webkit-fill-available}a{color:#3d568a!important}.btn-primary{background-color:#3d568a!important}.logo{padding-left:40px;background-size:40px 18px;background-image:url(\"data:image/svg+xml,%3Csvg width='469' height='197' viewBox='0 0 469 197' fill='none' xmlns='http://www.w3.org/2000/svg'%3E%3Crect width='469' height='197' fill='%23F2F2F2'/%3E%3Crect width='469' height='197' fill='white'/%3E%3Cpath d='M127.125 196C158.396 196 184.026 186.794 204.016 168.382C224.005 149.97 234 126.212 234 97.1091C234 68.0061 224.104 44.5455 204.312 26.7273C184.521 8.90909 158.792 0 127.125 0H63V196H127.125Z' fill='%233D568A'/%3E%3Cpath d='M0 156V40H127.837C148.103 40 164.197 45.1122 176.118 55.3365C188.039 65.5609 194 79.5962 194 97.4423C194 115.103 187.94 129.324 175.82 140.106C163.501 150.702 147.507 156 127.837 156H0Z' fill='%2332C5FF'/%3E%3Cpath d='M54.8659 136V96.0774L73.1454 119.714H77.9906L96.2701 96.0774V136H111.136V60H106.291L75.568 100.488L44.8452 60H40V136H54.8659Z' fill='white'/%3E%3Cpath d='M135.241 136V108.011C135.241 103.251 136.554 99.6253 139.181 97.1324C141.808 94.6394 145.416 93.393 150.004 93.393H154V79.9084C152.594 79.4551 150.966 79.2285 149.116 79.2285C142.826 79.2285 137.794 81.7215 134.02 86.7073V79.9084H120.256V136H135.241Z' fill='white'/%3E%3Cpath d='M284 98V1H244V98H284Z' fill='%233D568A'/%3E%3Cpath d='M284 197V98H244V197H284Z' fill='%2332C5FF'/%3E%3Cpath d='M401.669 119.703L469 1H424.805L381.5 78.7506L338.492 1H294L361.627 120L401.669 119.703Z' fill='%2332C5FF'/%3E%3Cpath d='M402 197V119.261L381.852 78L362 119.56V197H402Z' fill='%233D568A'/%3E%3C/svg%3E\");background-repeat:no-repeat}.alert-danger{position:relative;padding:1rem 1rem;margin-bottom:1rem;border-radius:.25rem;color:#842029;background-color:#f8d7da;border-color:#f5c2c7;}</style>";
//     s += "</head><body>";
//     s += "<div class='container py-3'>";
//     s += "<header>";
//     s += "  <div class='d-flex flex-column flex-md-row align-items-center pb-3 mb-4 border-bottom' style='border-bottom:1px solid #32C5FF!important;'>";
//     s += "    <a href='/' class='d-flex align-items-center text-dark text-decoration-none'><span class='fs-4 logo' style='height:19px;width:66px;'></span></a><span class='fs-4'>Configuration</span>";
//     s += "    <nav class='d-inline-flex mt-2 mt-md-0 ms-md-auto'>";
//     s += "    </nav>";
//     s += "  </div>";
//     s += "</header>";
//     s += "<body><main>";
//     s += "<div class='row justify-content-between'>";
//     if (ideSize != realSize) s += "<div class='alert-danger'>Your flash size (" + String(ideSize) + ") is configured incorrectly. It should be " + String(realSize) + ".</div>";
//     s += "<div class='col-md-12'>";
//     s += "<form action='/' method='post'>";
//     s += "    <div class='col-12 mt-3'><label class='form-label'>Wi-Fi Name</label><input type='text' name='ssid' class='form-control' value='" + String(gateway_settings.ssid) + "'></div>";
//     s += "    <div class='col-12 mt-3'><label class='form-label'>Password</label><input type='password' name='password' class='form-control' value='' autocomplete='off'></div>";
//     s += "    <div class='form-floating'><br/><button class='btn btn-primary btn-lg' type='submit'>Save</button><br /><br /><br /></div>";
//     s += " </form>";
//     s += "</div>";
//     s += "</div></main></body></html>";
//     server.send(200, "text/html", s );
//   }
// }