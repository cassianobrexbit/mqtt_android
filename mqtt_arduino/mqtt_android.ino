#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h> 
#include "DHT.h"

// Define pinos do ESP8266 - NodeMCU
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3
#define D10 1

#define DHTPIN 5          //pin where the dht11 is connected

DHT dht(DHTPIN, DHT11); 

const char* ssid = "ssid_da_rede_wifi";
const char* passwd  = "senha_da_rede_wifi";

const char* mqtt_server = "url_do_broker_mqtt";
const char *mqtt_user = "user_do_broker_mqtt";
const char *mqtt_passwd = "senha_do_broker_mqtt";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi(){

  delay(100);
  Serial.println("Conectando WiFi");
  WiFi.begin(ssid,passwd);

  while(WiFi.status() != WL_CONNECTED){

    delay(500);
    Serial.print(".");
    
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("Endereco de IP");
  Serial.println(WiFi.localIP());
  
}

void reconnect(){

  while(!client.connected()){

    Serial.print("Buscando conexao com MQTT..");
    String clientId = "ESP8266ClientId-";
    clientId += String(random(0xfff),HEX);

    if(client.connect(clientId.c_str(), mqtt_user, mqtt_passwd)){

      Serial.println("Conectado");
      client.publish("outTopic", "Ola Mundo");
      client.subscribe("inTopic");
      
    }else{

      Serial.print("Falhou, rc = ");
      Serial.print(client.state());
      Serial.print("Tente novamente em 5 segundos");

      delay(5000);
      
    }
    
  }
  
}

void callback(char* topic, byte* payload, unsigned int length){

  Serial.print("Mensagem recebida[");
  Serial.print(topic);
  Serial.print("]");

  for(int i = 0; i < length; i++){

    Serial.print((char)payload[i]);
    
  }

  Serial.println();
  
}


void setup() {

  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  
  client.setServer(mqtt_server, 18103);
  client.setCallback(callback);
  reconnect();
  
  dht.begin();

}

void loop() {


  if(!client.connected()){

    reconnect();
  
  }

  client.loop();

  
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  delay(1000);

  if (!isnan(h) && !isnan(t)) {

      String hh = String(h);
      String msg = String(t);
    
      Serial.print("Publicar mensagem: ");
      Serial.println(msg);
    
      int numt = t;
      char cstr[16];
      itoa(numt, cstr, 10);
    
      int numh = h;
      char cshr[16];
      itoa(numh, cshr, 10);
    
      delay(1500);
    
      client.publish("dht", cstr);
      client.publish("bmp", cshr);
    
      Serial.println("Umidade: ");
      Serial.print(h);
      Serial.println(" %\t");
      Serial.println("Temperatura: ");
      Serial.print(t);
      Serial.print(" *C ");
      Serial.println("\t");
  }
  
  delay(10000);

}
