#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SLT-4G_B7E92";
const char* password = "prolink12345";

const char* mqtt_server = "34.201.77.145";

WiFiClient espClient;
PubSubClient client(espClient);

int Sensor_input = 35;    //Digital pin 5 for sensor input/
int BUZZER = 2;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(Sensor_input, INPUT);
  pinMode(BUZZER, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message

  if (String(topic) == "dustbin/gasdetector") {
    Serial.print("Changing output to ");
    if(messageTemp == "gas_detected"){
      buzzerOn();
    }
    else if(messageTemp == "gas_not_detected"){
      buzzerOof();
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("dustbin/gasdetector");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  int sensor_Aout = analogRead(Sensor_input);  //Analog value read function/

  Serial.print("Gas state: ");
  Serial.println(sensor_Aout);
  char light[8];
  dtostrf(sensor_Aout, 1, 2, gas);
  client.publish("dustbin/gassensor", gas);

  delay(1000);                 //DELAY of 1 sec/
}

void buzzerOn(){
  Serial.println("Gas detected");
  for (int i =0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
    delay(500);
  }
}

void buzzerOof(){
  digitalWrite(BUZZER, LOW);
  Serial.println("No Gas");
}
