#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "<your-ssid-name>"
#define wifi_password "<your-ssid-password>"

#define mqtt_server "<your-mqtt-server>"
#define mqtt_user NULL
#define mqtt_password NULL
#define device_id "<device-id>"
#define publish_topic "<device-id>/<topic>/pub"
#define subscribe_topic "<device-id>/<topic>/sub"

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Fungsi ini yang akan dipanggil oleh MQTT jika menerima data
// Pesan yang dikirim melalui MQTT tersimpan pada payload
// Oleh karena itupayload lah yang akan menampung perintah
// untuk device tersebut
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(device_id, mqtt_user, mqtt_password)) {
      client.subscribe(subscribe_topic);
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // setup serial port
  Serial.begin(9600); 
     
  // setup WiFi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

long lastMsg = 0;
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  String pubString = "{\"cpu\":12}";
  char message_buff[pubString.length()+1];
  pubString.toCharArray(message_buff, pubString.length()+1);

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    client.publish(publish_topic, message_buff);
    Serial.println("Publising: "+pubString+" to '"+publish_topic+"'");
  }
}
