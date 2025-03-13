#include <ESP8266WiFi.h>      // Biblioteca para Wi-Fi
#include <ESP8266WebServer.h> // Biblioteca para o servidor web
#include <ESP8266mDNS.h>      // Biblioteca para mDNS
#include <WiFiManager.h>      // Biblioteca para gerenciar o Wi-Fi

#define relePin 0             // Pino do relé (GPIO0)

ESP8266WebServer server(80);  // Servidor na porta 80

void setup() {
  pinMode(relePin, OUTPUT);   // Configura o pino do relé
  digitalWrite(relePin, LOW); // Estado inicial: desligado
  Serial.begin(115200);       // Debug

  // --- Configuração do WiFiManager ---
  WiFiManager wm;
  bool res = wm.autoConnect("ESP01 Connection", "password");
  if (!res) {
    Serial.println("Falha ao conectar ao Wi-Fi");
    delay(5000);
    ESP.restart();
  } else {
    Serial.println("Conectado! IP: ");
    Serial.println(WiFi.localIP()); // Mostra o IP para debug
  }

  // --- Configuração do mDNS ---
  if (!MDNS.begin("esp01")) {
    Serial.println("Erro ao configurar o mDNS!");
    while (1) { delay(1000); }
  }
  Serial.println("mDNS responder iniciado");

  // --- Rotas do servidor ---
  server.on("/ligar", []() {
    digitalWrite(relePin, HIGH);
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Adiciona o cabeçalho CORS
    server.send(200, "text/plain", "Rele ligado");
  });

  server.on("/desligar", []() {
    digitalWrite(relePin, LOW);
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Adiciona o cabeçalho CORS
    server.send(200, "text/plain", "Rele desligado");
  });

  server.on("/state", []() {
    String state = digitalRead(relePin) ? "Ligado" : "Desligado";
    server.sendHeader("Access-Control-Allow-Origin", "*"); // Adiciona o cabeçalho CORS
    server.send(200, "text/plain", state);
  });

  server.begin();
  MDNS.addService("http", "tcp", 80); // Registra o serviço HTTP no mDNS
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient(); // Processa requisições
  MDNS.update();        // Mantém o mDNS ativo
}