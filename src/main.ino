#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>

const char* ssid = "insira o nome da sua rede wifi";
const char* password = "insira a senha da sua rede wifi";

TFT_eSPI tft = TFT_eSPI();


void setup() {
  Serial.begin(115200);
  tft.begin();              
  tft.setRotation(1);       
  tft.fillScreen(TFT_BLACK);
  WiFi.begin(ssid, password);

  for (int i = 0; i < 10; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    delay(1000);
  }
}


// Função para definir um tom de azul claro aproximado da imagem fornecida
unsigned int lightBlueTone() {
  // Valores aproximados para um azul claro com um toque de ciano
  int red = 50;   // Valor de vermelho - pequena quantidade
  int green = 180; // Valor de verde - significativamente mais alto que o vermelho
  int blue = 255;  // Valor de azul - o componente dominante 

  return tft.color565(red, green, blue);
}


void loop() {
  unsigned int color = lightBlueTone(); // Usa a cor azul clara definida pela função

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://api.binance.com/api/v3/ticker/24hr?symbol=BTCUSDT");
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      String bitcoinPrice = String(doc["lastPrice"].as<float>(), 2);
      String priceChangePercent = String(doc["priceChangePercent"].as<float>(), 2) + "%";

      tft.fillScreen(TFT_BLACK);

      // Tamanho e cor para cada linha de texto
      int textSize1 = 3, textSize2 = 3, textSize3 = 2;
      int y1 = 70, y2 = 110, y3 = 150; // Alturas iniciais para cada linha de texto

      // Primeira Linha: Bitcoin Price
      tft.setTextSize(textSize1);
      tft.setTextColor(color, TFT_BLACK);
      int x1 = (tft.width() - tft.textWidth("BITCOIN PRICE")) / 2;
      tft.setCursor(x1, y1);
      tft.println("BITCOIN PRICE");

      // Segunda Linha: Preço do Bitcoin
      tft.setTextSize(textSize2);
      tft.setTextColor(TFT_GOLD, TFT_BLACK);
      int x2 = (tft.width() - tft.textWidth("BTC: $" + bitcoinPrice)) / 2;
      tft.setCursor(x2, y2);
      tft.println("BTC: $" + bitcoinPrice);

      // Terceira Linha: Variação Percentual
      tft.setTextSize(textSize3);
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      int x3 = (tft.width() - tft.textWidth("24h Change: " + priceChangePercent)) / 2;
      tft.setCursor(x3, y3);
      tft.println("24h Change: " + priceChangePercent);

    } else {
      Serial.println("Erro na requisição HTTP");
    }

    http.end();
  } else {
    Serial.println("Desconectado do Wi-Fi");
  }

  delay(10000); // Atualiza a cada minuto
}
