#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_RELE1 4  // Define o pino do Relé 1
#define PIN_RELE2 5  // Define o pino do Relé 2
#define TOPICO_SUBSCRIBE_RELE "topico_AULA_IOT"
#define ID_MQTT  "esp32_mqtt- Aula1"  

const char* SSID = "ETEC PC"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "0123456789";

const char* BROKER_MQTT = "test.mosquitto.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT

WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient);

void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);

void initWiFi(void) 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
      
    reconnectWiFi();
}

void initMQTT(void) 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);
    //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{   
    String msg;
    /* obtem a string do payload recebido */
    for(int i = 0; i < length; i++) {
        char c = (char)payload[i];
        msg += c;
    }
    Serial.print("Chegou a seguinte string via MQTT: "); 
    Serial.println(msg);
    
    /* Aciona ou desliga os reles dependendo da string recebida */
    if (msg.equals("0")) {
        digitalWrite(PIN_RELE1, LOW); // Desliga o Relé 1
        Serial.println("Relé 1 desligado mediante comando MQTT");
    }
    if (msg.equals("1")) {
        digitalWrite(PIN_RELE1, HIGH); // Liga o Relé 1
        Serial.println("Relé 1 ligado mediante comando MQTT");
    }
    if (msg.equals("2")) {
        digitalWrite(PIN_RELE2, HIGH); // Liga o Relé 2
        Serial.println("Relé 2 ligado mediante comando MQTT");
    }
    if (msg.equals("3")) {
        digitalWrite(PIN_RELE2, LOW); // Desliga o Relé 2
        Serial.println("Relé 2 desligado mediante comando MQTT");
    }
    if (msg.equals("4")) {
        Serial.println("Pisca Relés alternadamente 10 vezes.");
        for(int x=0; x<10; x++) {
            digitalWrite(PIN_RELE1, HIGH);
            digitalWrite(PIN_RELE2, LOW); 
            delay(100);
            digitalWrite(PIN_RELE1, LOW);
            digitalWrite(PIN_RELE2, HIGH);  
            delay(100);
        }
    }
    if (msg.equals("5")) {
        digitalWrite(PIN_RELE1, LOW);
        digitalWrite(PIN_RELE2, LOW);
        Serial.println("Ambos os Relés desligados mediante comando MQTT");
    }
}

void reconnectMQTT(void){
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE_RELE);
            
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
      
     reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void reconnectWiFi(void) 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
          
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
      
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.println(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);  
    pinMode(PIN_RELE1, OUTPUT);
    digitalWrite(PIN_RELE1, LOW); // Relé desligado no início
    pinMode(PIN_RELE2, OUTPUT);
    digitalWrite(PIN_RELE2, LOW); // Relé desligado no início
    /* Inicializa a conexao wi-fi */
    initWiFi();
 
    /* Inicializa a conexao ao broker MQTT */
    initMQTT();
}

void loop() {
  VerificaConexoesWiFIEMQTT();
  MQTT.loop();
  delay(2000);
}
