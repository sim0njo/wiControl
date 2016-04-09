
#ifndef MY_SENSORS_GW_MQTT_H
#define MY_SENSORS_GW_MQTT_H
#include <SmingCore/SmingCore.h>

void ICACHE_FLASH_ATTR mqttStartClient();
void ICACHE_FLASH_ATTR mqttCheckClient();
void ICACHE_FLASH_ATTR mqttPublishMessage(String topic, String message);
void                   mqttOnHttpConfig(HttpRequest &request, HttpResponse &response);

bool                   mqttIsConfigured();
bool                   mqttIsConnected();
String                 mqttServer();

extern unsigned long   g_dwMqttPktRx;
extern unsigned long   g_dwMqttPktTx;

#endif //MY_SENSORS_GW_MQTT_H
