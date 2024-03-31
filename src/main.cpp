
#pragma region Definitions

#pragma endregion

#pragma region Headers

#include "ApplicationConfiguration.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "time.h"

#if defined(ENABLE_WEB)
#include "ArduinoJson.h"
#endif

#if defined(ENABLE_WIFI)
#include <WiFi.h>
#endif

#if defined(ENABLE_ETH)
#include <WiFi.h>
#include <ETH.h>
#endif

#if defined(ENABLE_OTA)

#include <ArduinoOTA.h>

#endif

#if defined(ENABLE_WG)

#include <WireGuard-ESP32.h>

#endif

#if defined(ENABLE_BMP085)

#include <Adafruit_BMP085.h>

#endif

#if defined(ENABLE_WEB)
#include <WebServer.h>
#endif

#include <made4home.h>
#include <FxTimer.h>

#pragma endregion

#pragma region Variables

/**
 * @brief Time structure now information.
 * 
 */
struct tm TimenowInfo_g;

/**
 * @brief Time structure uptime information.
 * 
 */
struct tm UptimeInfo_g;

/** 
 * @brief Update timer instance.
 */
FxTimer *UpdateTimer_g;

/**
 * @brief Silo 1 model.
 * 
 */
SiloModel *Silo1_g;

/**
 * @brief Silo 2 model.
 * 
 */
SiloModel *Silo2_g;

/**
 * @brief Silo 3 model.
 * 
 */
SiloModel *Silo3_g;

/**
 * @brief Silo 4 model.
 * 
 */
SiloModel *Silo4_g;

#if defined(ENABLE_ETH)

/**
 * @brief Ethernet connection state.
 * 
 */
static bool EthernetConnected_g = false;

#endif

#if defined(ENABLE_WG)

/**
 * @brief [Interface] Address
 * 
 */
IPAddress LocalIP_g(172,33,6,5);

/**
 * @brief Wire Guard client.
 * 
 */
static WireGuard WireGuardClient_g;

#endif

#if defined(ENABLE_WEB)

/**
 * @brief index.html file pointer.
 * 
 */
extern const char html_index[] asm("_binary_html_index_html_start");

/**
 * @brief msg.js file pointer.
 * 
 */
extern const char js_msg[] asm("_binary_html_msg_js_start");

/**
 * @brief style.css file pointer.
 * 
 */
extern const char css_style[] asm("_binary_html_style_css_start");

/**
 * @brief WEB server.
 * 
 */
WebServer WebServer_g(WEB_SERVER_PORT);

#endif

#if defined(ENABLE_MB)

/**
 * @brief The RS485 module has no half-duplex, so the parameter with the DE/RE pin is required!
 * 
 */
ModbusClientRTU *ModbusClient_g;

/**
 * @brief Coil data.
 * 
 */
CoilData CoilData_g(12);

#endif

#if defined(ENABLE_BMP085)

/**
 * @brief BMP180 sensor.
 * 
 */
Adafruit_BMP085 BMP085_g;

float Temperate_g;
int32_t Pressure_g;
float Altitude_g;

#endif

#if defined(ENABLE_WEB)

/**
 * @brief 
 * 
 */
StaticJsonDocument<400> JsonResponse_g;

/**
 * @brief 
 * 
 */
StaticJsonDocument<QUEUE_SIZE> JsonSensorData1_g;

/**
 * @brief 
 * 
 */
StaticJsonDocument<10> JsonMessagesData_g;

/**
 * @brief Global message counter.
 * 
 */
int GlobalMessageCounter_g = 0;

#endif

float UpdateProgress_g = 0;

#pragma endregion

#pragma region Prototypes

/**
 * @brief Update uptime.
 * 
 */
void update_uptime();

/**
 * @brief Convert uptime to string.
 * 
 * @return String uptime text.
 */
String uptime_string();

/**
 * @brief Update now.
 *  
 */
void update_now_time();

/**
 * @brief Convert current time to string.
 * 
 * @return String 
 */
String now_string();

#if defined(ENABLE_ETH)

/**
 * @brief Network event state handler.
 * 
 * @param event Event input.
 */
void wifi_event(WiFiEvent_t event);

#endif

#if defined(ENABLE_WEB)

void cb_web_root();

void cb_js_msg();

void cb_css_style();

void cb_web_data();

void cb_web_not_found();

#endif

#if defined(ENABLE_WEB)

/**
 * @brief Set up the sensor data.
 * 
 */
void setup_sensor_data();

/**
 * @brief Clear a specific message.
 * 
 * @param index Message index.
 */
void clear_web_message(int index);

/**
 * @brief Set WEB message to screen.
 *
 * @param index Index of the message.
 * @param text Text of the message.
 * @param color Color of the message.
 * @param dt Display time of the message.
 * @param nox No X button flag of the message.
 */
void set_web_message(String text, String color, int dt, bool nox=false);

#endif

#pragma endregion

void setup()
{
#if defined(ENABLE_DEBUG_PORT)
    // Init Serial monitor
    Serial.begin(DEFAULT_BAUDRATE, SERIAL_8N1);
    while (!Serial) {}
#endif

    Made4Home.setup();

#if defined(ENABLE_WIFI)
    Serial.print("Connecting to the AP: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while( !WiFi.isConnected() )
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Successfully connected to AP: ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif

#if defined(ENABLE_ETH)
    Serial.println("Connecting to the ETH.");

    // Attach the network events.
    WiFi.onEvent(wifi_event);

    // Run the Ethernet.
    ETH.begin(
        PIN_ETH_PHY_ADDR,
        PIN_ETH_PHY_POWER,
        PIN_ETH_PHY_MDC,
        PIN_ETH_PHY_MDIO,
        PIN_ETH_PHY_TYPE,
        PIN_ETH_CLK_MODE);

    while( EthernetConnected_g == false )
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Successfully connected to ETH.");
    Serial.print("IP address: ");
    Serial.println(ETH.localIP());
#endif

#if defined(ENABLE_WG)
    Serial.println("Adjusting system time...");
    configTime(2 * 60 * 60, 0, "ntp.jst.mfeed.ad.jp", "ntp.nict.jp", "time.google.com");
    update_now_time();
    update_uptime();

    Serial.println("Initializing WireGuard...");
    WireGuardClient_g.begin(
        LocalIP_g,
        WG_PRIVATE_KEY,
        WG_ENDPOINT,
        WG_PUBLIC_KEY,
        WG_PORT);
    Serial.println("WireGuard client started.");
#endif

#if defined(ENABLE_OTA)

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    ArduinoOTA.setPassword(OTA_PASS);

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
            Serial.println();
        })
        .onEnd([]() {
            Serial.println("End");
            Serial.println();
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            UpdateProgress_g = (progress / (total / 100));
            Serial.printf("Progress: %.0f%%", UpdateProgress_g);
            Serial.println();
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
            Serial.println();
        });

    ArduinoOTA.begin();

#endif

#if defined(ENABLE_WEB)
    WebServer_g.on("/", cb_web_root);
    WebServer_g.on("/msg.js", cb_js_msg);
    WebServer_g.on("/style.css", cb_css_style);
    WebServer_g.on("/data", cb_web_data);
    WebServer_g.onNotFound(cb_web_not_found);
    WebServer_g.begin();
    Serial.println("WEB Server started");
#endif

#if defined(ENABLE_WEB)
    setup_sensor_data();
#endif

#if defined(ENABLE_TEST_MAINTENANCE_MSG)
    set_web_message("Обадете се на поддръжката!", "#ff0000", 0, true);
#endif

    set_web_message("Добре дошли!", "#00FF00", 5, false);
}

void loop()
{
    update_now_time();

    update_uptime();

#if defined(ENABLE_OTA)
    ArduinoOTA.handle();
#endif

#if defined(ENABLE_WEB)
    WebServer_g.handleClient();
#endif

}

#pragma region Functions

/**
 * @brief Update uptime.
 * 
 */
void update_uptime()
{
    long millisecs = millis();
    UptimeInfo_g.tm_sec = int((millisecs / 1000) % 60);
    UptimeInfo_g.tm_min = int((millisecs / (1000 * 60)) % 60);
    UptimeInfo_g.tm_hour = int((millisecs / (1000 * 60 * 60)) % 24);
    UptimeInfo_g.tm_mday = int((millisecs / (1000 * 60 * 60 * 24)) % 365);
}

/**
 * @brief Convert uptime to string.
 * 
 * @return String uptime text.
 */
String uptime_string()
{
    static String UptimeStringL;

    UptimeStringL = String(UptimeInfo_g.tm_mday);
    UptimeStringL += ".";
    if(UptimeInfo_g.tm_hour < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += String(UptimeInfo_g.tm_hour);
    UptimeStringL += ":";
    if(UptimeInfo_g.tm_min < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += String(UptimeInfo_g.tm_min);
    UptimeStringL += ":";
    if(UptimeInfo_g.tm_sec < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += String(UptimeInfo_g.tm_sec);

    return UptimeStringL;
}

/**
 * @brief Update now.
 *  
 */
void update_now_time()
{
    while (!getLocalTime(&TimenowInfo_g))
    {
        Serial.println("Failed to obtain time");
    }
}

/**
 * @brief Convert current time to string.
 * 
 * @return String 
 */
String now_string()
{
    static String UptimeStringL;

    UptimeStringL = String(TimenowInfo_g.tm_hour);
    if(TimenowInfo_g.tm_hour < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += ":";
    if(TimenowInfo_g.tm_min < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += String(TimenowInfo_g.tm_min);
    UptimeStringL += ":";
    if(TimenowInfo_g.tm_sec < 10)
    {
        UptimeStringL += "0";
    }
    UptimeStringL += String(TimenowInfo_g.tm_sec);
    return UptimeStringL;
}

#if defined(ENABLE_ETH)
/**
 * @brief Network event state handler.
 * 
 * @param event Event input.
 */
void wifi_event(WiFiEvent_t event)
{
    switch (event)
    {
    case ARDUINO_EVENT_ETH_START:
        Serial.println("ETH Started");
        //set eth hostname here
        ETH.setHostname("made4home");
        break;
    case ARDUINO_EVENT_ETH_CONNECTED:
        Serial.println("ETH Connected");
        break;
    case ARDUINO_EVENT_ETH_GOT_IP:
        Serial.print("ETH MAC: ");
        Serial.print(ETH.macAddress());
        Serial.print(", IPv4: ");
        Serial.print(ETH.localIP());
        if (ETH.fullDuplex())
        {
            Serial.print(", FULL_DUPLEX");
        }
        Serial.print(", ");
        Serial.print(ETH.linkSpeed());
        Serial.println("Mbps");
        EthernetConnected_g = true;
        break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        Serial.println("ETH Disconnected");
        EthernetConnected_g = false;
        break;
    case ARDUINO_EVENT_ETH_STOP:
        Serial.println("ETH Stopped");
        EthernetConnected_g = false;
        break;
    default:
        break;
    }
}
#endif

#if defined(ENABLE_WEB)

void cb_web_root()
{
    // https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer/examples/HttpBasicAuthSHA1
    // https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/HttpBasicAuthSHA1/HttpBasicAuthSHA1.ino
    // const char* www_username_hex = "hexadmin";
    // const char* www_password_hex = "8cb124f8c277c16ec0b2ee00569fd151a08e342b";
    // if (WebServer_g.authenticateBasicSHA1(www_username_hex, www_password_hex)) {
    //     WebServer_g.send(200, "text/html", html_index);
    // }
    // WebServer_g.on("/login", HTTPMethod::HTTP_POST, [](){

    // });

    if(!WebServer_g.authenticate(HTTP_USER, HTTP_PASS))
    {
        return WebServer_g.requestAuthentication();
    }
    else
    {
        WebServer_g.send(200, "text/html", html_index);
    }
}

void cb_js_msg()
{
    WebServer_g.send(200, "text/javascript", js_msg);
}

void cb_css_style()
{
    WebServer_g.send(200, "text/css", css_style);
}

void cb_web_data()
{
    String ResponseL;

    // Status fields.
#if defined(ENABLE_WIFI)
    JsonResponse_g["status"]["if"] = "WiFi";
    JsonResponse_g["status"]["local_ip"] = WiFi.localIP().toString();
#endif
#if defined(ENABLE_ETH)
    JsonResponse_g["status"]["if"] = "Ethernet";
    JsonResponse_g["status"]["local_ip"] = ETH.localIP().toString();
#endif
    JsonResponse_g["status"]["wg_ip"] = LocalIP_g.toString();
    JsonResponse_g["status"]["web_port"] = String(WEB_SERVER_PORT);
    JsonResponse_g["status"]["uptime"] = uptime_string();

    // Set messages for displaying.
    JsonResponse_g["msgs"] = JsonMessagesData_g;

    // Serialize data.
    serializeJson(JsonResponse_g, ResponseL);

    // Send page headers.
    WebServer_g.sendHeader("Content-Type", "application/json");

    // Send page.
    WebServer_g.send(200, "text/json", ResponseL);
}

void cb_web_not_found()
{
    WebServer_g.send(404, "text/plain", "Not found");
}

#endif

#if defined(ENABLE_MB)

float midBigEndianToFloat(const uint8_t bytes[4])
{
    uint32_t value = 0;
    
    // Construct the 32-bit integer from the 4 bytes in Mid-Big Endian order
    value |= ((uint32_t)bytes[0] << 24);
    value |= ((uint32_t)bytes[1] << 16);
    value |= ((uint32_t)bytes[2] << 8);
    value |= bytes[3];
    
    // Interpret the bits as a floating-point number
    float result;
    memcpy(&result, &value, sizeof(float));
    
    return result;
}

ModbusMessage get_distance(byte slave_id)
{
    // Define message
    static ModbusMessage MessageL;

    // Set message for reading the distance.
    MessageL.setMessage(slave_id, READ_HOLD_REGISTER, 4, 2);

    return MessageL;
}

ModbusMessage set_ao(byte slave_id, int value)
{
    // Define message
    static ModbusMessage MessageL;

    // Set message for reading the distance.
    MessageL.setMessage(slave_id, WRITE_MULT_REGISTERS, 0, 1, {(uint16_t)map(value, 0, 100, 0, 20400)});

    return MessageL;
}

void cb_mb_response(ModbusMessage response, uint32_t token) 
{
    static uint8_t FunctionCodeL = 0;
    static uint8_t SlaveIDL = 0;
    static uint8_t InputsL = 0;
    static uint8_t MessageContentL[4];

    // For debug purpose only.
    // for (uint16_t index = 0; index < response.size(); index++)
    // {
    //     LOG_I("Index: %02X - %02X\n", index, response[index]);
    // }

    // Get slave ID
    SlaveIDL = response.getServerID();

    // Get FC.
    FunctionCodeL = response.getFunctionCode();

    // If it is READ_DISCR_INPUT
    if (FunctionCodeL == READ_DISCR_INPUT && SlaveIDL == MB_BI_SLAVE_ID)
    {
        InputsL = response[3];      
        Made4Home.digitalWrite(0, (InputsL & 0x01) ? 1 : 0);
        Made4Home.digitalWrite(1, (InputsL & 0x02) ? 1 : 0);
        Made4Home.digitalWrite(2, (InputsL & 0x04) ? 1 : 0);
        Made4Home.digitalWrite(3, (InputsL & 0x08) ? 1 : 0);
    }
    // If it is holding registers and slave ID is 1.
    // if (FunctionCodeL == READ_HOLD_REGISTER && SlaveIDL == MB_US_SENSOR_1_ID && token == MB_US_SENSOR_1_TOKEN)

    if (SlaveIDL == MB_US_SENSOR_1_ID && token == MB_US_SENSOR_1_TOKEN)
    {
        // Copy the data.
        for (uint16_t index = 0; index < 4; index++)
        {
            MessageContentL[index] = response[index+3];
        }

        // Convert the data to double.
        float DistanceL = midBigEndianToFloat(MessageContentL);

        if (DistanceL != 0.0)
        {
            // From DM to M.
            DistanceL = DM2M(DistanceL);

            LOG_I("Distance: %f\r\n", DistanceL);

            // Update model.
            Silo1_g->setDistance(DistanceL);

            // Turn ON/OFF outputs when levels are achieved.
            float pcv = Silo1_g->getPercentage();
            if (pcv >= SILO_1_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(0, 1);
            }
            if (pcv <= SILO_1_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(0, 0);
            }

            // Update chart data.
            for (int index = 0; index < QUEUE_SIZE - 1; index++)
            {
                JsonSensorData1_g[index] = JsonSensorData1_g[index + 1];
            }
            JsonSensorData1_g[QUEUE_SIZE - 1][0] = now_string();
            JsonSensorData1_g[QUEUE_SIZE - 1][1] = pcv;
        }
    }
    
    if (SlaveIDL == MB_US_SENSOR_2_ID && token == MB_US_SENSOR_2_TOKEN)
    {
        // Copy the data.
        for (uint16_t index = 0; index < 4; index++)
        {
            MessageContentL[index] = response[index+3];
        }

        // Convert the data to double.
        float DistanceL = midBigEndianToFloat(MessageContentL);

        if (DistanceL != 0.0)
        {
            // From DM to M.
            DistanceL = DM2M(DistanceL);

            LOG_I("Distance: %f\r\n", DistanceL);

            // Update model.
            Silo2_g->setDistance(DistanceL);

            // Turn ON/OFF outputs when levels are achieved.
            float pcv = Silo2_g->getPercentage();
            if (pcv >= SILO_2_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(1, 1);
            }
            if (pcv <= SILO_2_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(1, 0);
            }

            // Update chart data.
            for (int index = 0; index < QUEUE_SIZE - 1; index++)
            {
                JsonSensorData2_g[index] = JsonSensorData2_g[index + 1];
            }
            JsonSensorData2_g[QUEUE_SIZE - 1][0] = now_string();
            JsonSensorData2_g[QUEUE_SIZE - 1][1] = pcv;
        }
    }
    
    if (SlaveIDL == MB_US_SENSOR_3_ID && token == MB_US_SENSOR_3_TOKEN)
    {
        // Copy the data.
        for (uint16_t index = 0; index < 4; index++)
        {
            MessageContentL[index] = response[index+3];
        }

        // Convert the data to double.
        float DistanceL = midBigEndianToFloat(MessageContentL);

        if (DistanceL != 0.0)
        {
            // From DM to M.
            DistanceL = DM2M(DistanceL);

            LOG_I("Distance: %f\r\n", DistanceL);

            // Update model.
            Silo3_g->setDistance(DistanceL);

            // Turn ON/OFF outputs when levels are achieved.
            float pcv = Silo3_g->getPercentage();
            if (pcv >= SILO_3_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(2, 1);
            }
            if (pcv <= SILO_3_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(2, 0);
            }

            // Update chart data.
            for (int index = 0; index < QUEUE_SIZE - 1; index++)
            {
                JsonSensorData3_g[index] = JsonSensorData3_g[index + 1];
            }
            JsonSensorData3_g[QUEUE_SIZE - 1][0] = now_string();
            JsonSensorData3_g[QUEUE_SIZE - 1][1] = pcv;
        }
    }

    if (SlaveIDL == MB_US_SENSOR_4_ID && token == MB_US_SENSOR_4_TOKEN)
    {
        // Copy the data.
        for (uint16_t index = 0; index < 4; index++)
        {
            MessageContentL[index] = response[index+3];
        }

        // Convert the data to double.
        float DistanceL = midBigEndianToFloat(MessageContentL);

        if (DistanceL != 0.0)
        {
            // From DM to M.
            DistanceL = DM2M(DistanceL);

            LOG_I("Distance: %f\r\n", DistanceL);

            // Update model.
            Silo4_g->setDistance(DistanceL);

            // Turn ON/OFF outputs when levels are achieved.
            float pcv = Silo4_g->getPercentage();
            if (pcv >= SILO_4_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(3, 1);
            }
            if (pcv <= SILO_4_FILLED_VOLUME_MAX)
            {
                Made4Home.digitalWrite(3, 0);
            }

            // Update chart data.
            for (int index = 0; index < QUEUE_SIZE - 1; index++)
            {
                JsonSensorData4_g[index] = JsonSensorData4_g[index + 1];
            }
            JsonSensorData4_g[QUEUE_SIZE - 1][0] = now_string();
            JsonSensorData4_g[QUEUE_SIZE - 1][1] = pcv;
        }
    }
}

/**
 * @brief Define an onError handler function to receive error responses
 * 
 * @param error Error code.
 * @param token User-supplied token to identify the causing request.
 */
void cb_mb_error(Error error, uint32_t token) 
{
    if (token == MB_US_SENSOR_1_TOKEN)
    {

    }

    // ModbusError wraps the error code and provides a readable error message for it
    ModbusError ModbusErrorL(error);
    LOG_E("Error response: %02X - %s\n", (int)ModbusErrorL, (const char *)ModbusErrorL);
}
#endif

#if defined(ENABLE_WEB) || defined(ENABLE_MB)

/**
 * @brief Set up the sensor data.
 * 
 */
void setup_sensor_data()
{
    // Create default message.
    StaticJsonDocument<4> DefaultMsgL;
    DefaultMsgL[0] = "00:00:00";
    DefaultMsgL[1] = 0.0;

    // Clear the queue.
    JsonSensorData1_g.clear();

    // Fill the queue with default data.
    for (int index = 0; index < QUEUE_SIZE; index++)
    {
        JsonSensorData1_g[index] = DefaultMsgL;
    }
}

/**
 * @brief Clear a specific message.
 * 
 * @param index Message index.
 */
void clear_web_message(int index)
{
    JsonMessagesData_g[index].clear();
}

/**
 * @brief Set WEB message to screen.
 *
 * @param index Index of the message.
 * @param text Text of the message.
 * @param color Color of the message.
 * @param dt Display time of the message.
 * @param nox No X button flag of the message.
 */
void set_web_message(String text, String color, int dt, bool nox)
{
    if (GlobalMessageCounter_g > 100)
    {
        GlobalMessageCounter_g = 0;
    }
    JsonMessagesData_g[GlobalMessageCounter_g]["msg"] = text;
    JsonMessagesData_g[GlobalMessageCounter_g]["color"] = color;
    JsonMessagesData_g[GlobalMessageCounter_g]["dt"] = dt;
    JsonMessagesData_g[GlobalMessageCounter_g]["nox"] = nox;
    JsonMessagesData_g[GlobalMessageCounter_g]["id"] = String(mktime(&TimenowInfo_g)) + String(".") + String(GlobalMessageCounter_g);
    GlobalMessageCounter_g += 1;
}

#endif

#pragma endregion
