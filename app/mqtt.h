
#ifndef MY_SENSORS_GW_MQTT_H
#define MY_SENSORS_GW_MQTT_H
#include <SmingCore/SmingCore.h>
#include <xstdafx.h>

uint32_t ICACHE_FLASH_ATTR mqttStartClient();
uint32_t ICACHE_FLASH_ATTR mqttCheckClient();
void     ICACHE_FLASH_ATTR mqttSubscribe(tCChar* szTopicFilter);
void     ICACHE_FLASH_ATTR mqttPublish(tCChar* szPfx, tCChar* szTopic, tCChar* szMsg);
void                       mqttOnHttpConfig(HttpRequest &request, HttpResponse &response);

uint32_t                   mqttIsConnected();
String                     mqttServer();

extern uint32_t            g_mqttPktRx;
extern uint32_t            g_mqttPktRxDropped;
extern uint32_t            g_mqttPktTx;

#endif //MY_SENSORS_GW_MQTT_H
