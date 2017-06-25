

void wifi_init() {
  // WiFi 모듈 시작
  Serial3.begin(115200);
  WiFi.init(&Serial3);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    //    Serial.print("Attempting to connect to WPA SSID: ");
    //    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  //  Serial.println("You're connected to the network");
  String msg = "Hello This is PMS7003";
  //  httpRequest(msg);
  //  printWifiStatus();
  //  Serial5.begin(9600);
}
