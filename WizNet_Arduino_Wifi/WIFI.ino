boolean httpRequest(String message)
{
  boolean reqOk = true;

  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 9999)) {
    //    Serial.println("Connecting...");

    // send the HTTP PUT request
    String argMsg = message;
    //    String getMsg = "GET "+ /wifi/" + argMsg + " HTTP/1.0";
    //    Serial.println(getMsg);
    String getMsg = "GET " + argMsg + " HTTP/1.0";
    client.println(getMsg);
    client.println("Host: 218.149.135.113");
    client.println("Connection: close");
    client.println();

    Serial.println("Request OK");
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
    reqOk = false;
  }

  return reqOk;
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String getBinary(String str) {
  String message = str;
  message.toLowerCase();
  int length = message.length();
  String binaryNumber = "";

  for (int i = 0; i <= length; i++) {
    char c = message.charAt(i);
    int letter  ;
    Serial.println(letter);
    binaryNumber = binaryNumber + String(c, BIN) + " ";
  }
  return binaryNumber;
}

