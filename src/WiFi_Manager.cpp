#include "WiFi_Manager.h"

WiFi_Manager *WiFi_Manager::instance = nullptr;

WiFi_Manager::WiFi_Manager(
  char (&server)[MQTT_SERVER_MAX_LEN],
  char (&port)[MQTT_PORT_MAX_LEN],
  char (&username)[MQTT_USERNAME_MAX_LEN],
  char (&password)[MQTT_PASSWORD_MAX_LEN],
  char (&client_id)[MQTT_CLIENT_ID_MAX_LEN],
  char (&topic)[MQTT_TOPIC_MAX_LEN],
  uint8_t config_portal_timeout,
  uint8_t connect_timeout
) :
  _MQTT_Server(server),
  _MQTT_Port(port),
  _MQTT_Username(username),
  _MQTT_Password(password),
  _MQTT_Client_ID(client_id),
  _MQTT_Topic(topic),

  _config_portal_timeout(config_portal_timeout),
  _connect_timeout(connect_timeout),

  _Custom_MQTT_Server("MQTT_Server", "MQTT Server", server, MQTT_SERVER_MAX_LEN),
  _Custom_MQTT_Port("MQTT_Port", "MQTT Port", port, MQTT_PORT_MAX_LEN),
  _Custom_MQTT_Username("MQTT_Username", "MQTT Username", username, MQTT_USERNAME_MAX_LEN),
  _Custom_MQTT_Password("MQTT_Password", "MQTT Password", password, MQTT_PASSWORD_MAX_LEN),
  _Custom_MQTT_Client_ID("MQTT_Client_ID", "MQTT Client ID", client_id, MQTT_CLIENT_ID_MAX_LEN),
  _Custom_MQTT_Topic("MQTT_Topic", "MQTT Topic", topic, MQTT_TOPIC_MAX_LEN)
{
  instance = this;
}

void WiFi_Manager::begin(const char *ap_name, const char *ap_password)
{
  strlcpy(_AP_Name, ap_name, sizeof(_AP_Name));
  strlcpy(_AP_Password, ap_password, sizeof(_AP_Password));

  load_config();
  
  _WiFi_Manager.setSaveConfigCallback(save_config_callback);

  _WiFi_Manager.setConfigPortalTimeout(_config_portal_timeout);
  _WiFi_Manager.setConnectTimeout(_connect_timeout);

  if (!_WiFi_Manager.autoConnect(_AP_Name, _AP_Password))
  {
    Serial.println("Failed connect to WiFi");
    Serial.println("Restarting...");
    Serial.println();
    ESP.restart();
  }
  
  Serial.println();
  Serial.println("WiFi Connected");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}

void WiFi_Manager::load_config()
{
  _prefs.begin("MQTT_conf", true);
  _prefs.getString("MQTT_Server", _MQTT_Server).toCharArray(_MQTT_Server, sizeof(_MQTT_Server));
  _prefs.getString("MQTT_Port", _MQTT_Port).toCharArray(_MQTT_Port, sizeof(_MQTT_Port));
  _prefs.getString("MQTT_Username", _MQTT_Username).toCharArray(_MQTT_Username, sizeof(_MQTT_Username));
  _prefs.getString("MQTT_Password", _MQTT_Password).toCharArray(_MQTT_Password, sizeof(_MQTT_Password));
  _prefs.getString("MQTT_Client_ID", _MQTT_Client_ID).toCharArray(_MQTT_Client_ID, sizeof(_MQTT_Client_ID));
  _prefs.getString("MQTT_Topic", _MQTT_Topic).toCharArray(_MQTT_Topic, sizeof(_MQTT_Topic));
  _prefs.end();
  
  _Custom_MQTT_Server.setValue(_MQTT_Server, MQTT_SERVER_MAX_LEN);
  _Custom_MQTT_Port.setValue(_MQTT_Port, MQTT_PORT_MAX_LEN);
  _Custom_MQTT_Username.setValue(_MQTT_Username, MQTT_USERNAME_MAX_LEN);
  _Custom_MQTT_Password.setValue(_MQTT_Password, MQTT_PASSWORD_MAX_LEN);
  _Custom_MQTT_Client_ID.setValue(_MQTT_Client_ID, MQTT_CLIENT_ID_MAX_LEN);
  _Custom_MQTT_Topic.setValue(_MQTT_Topic, MQTT_TOPIC_MAX_LEN);
  
  _WiFi_Manager.addParameter(&_Custom_MQTT_Server);
  _WiFi_Manager.addParameter(&_Custom_MQTT_Port);
  _WiFi_Manager.addParameter(&_Custom_MQTT_Username);
  _WiFi_Manager.addParameter(&_Custom_MQTT_Password);
  _WiFi_Manager.addParameter(&_Custom_MQTT_Client_ID);
  _WiFi_Manager.addParameter(&_Custom_MQTT_Topic);

  Serial.println();
  Serial.println("CONFIGURATION LOADED");
  Serial.printf("MQTT Server       : %s\n", _MQTT_Server);
  Serial.printf("MQTT Port         : %s\n", _MQTT_Port);
  Serial.printf("MQTT Username     : %s\n", _MQTT_Username);
  Serial.printf("MQTT Password     : %s\n", _MQTT_Password);
  Serial.printf("MQTT Client ID    : %s\n", _MQTT_Client_ID);
  Serial.printf("MQTT Topic        : %s\n", _MQTT_Topic);
}

void WiFi_Manager::save_config()
{
  strlcpy(_MQTT_Server, _Custom_MQTT_Server.getValue(), sizeof(_MQTT_Server));
  strlcpy(_MQTT_Port, _Custom_MQTT_Port.getValue(), sizeof(_MQTT_Port));
  strlcpy(_MQTT_Username, _Custom_MQTT_Username.getValue(), sizeof(_MQTT_Username));
  strlcpy(_MQTT_Password, _Custom_MQTT_Password.getValue(), sizeof(_MQTT_Password));
  strlcpy(_MQTT_Client_ID, _Custom_MQTT_Client_ID.getValue(), sizeof(_MQTT_Client_ID));
  strlcpy(_MQTT_Topic, _Custom_MQTT_Topic.getValue(), sizeof(_MQTT_Topic));

  _prefs.begin("MQTT_conf", false);
  _prefs.putString("MQTT_Server", _MQTT_Server);
  _prefs.putString("MQTT_Port", _MQTT_Port);
  _prefs.putString("MQTT_Username", _MQTT_Username);
  _prefs.putString("MQTT_Password", _MQTT_Password);
  _prefs.putString("MQTT_Client_ID", _MQTT_Client_ID);
  _prefs.putString("MQTT_Topic", _MQTT_Topic);
  _prefs.end();
  
  Serial.println();
  Serial.println("CONFIGURATION SAVED");
  Serial.printf("MQTT Server       : %s\n", _MQTT_Server);
  Serial.printf("MQTT Port         : %s\n", _MQTT_Port);
  Serial.printf("MQTT Username     : %s\n", _MQTT_Username);
  Serial.printf("MQTT Password     : %s\n", _MQTT_Password);
  Serial.printf("MQTT Client ID    : %s\n", _MQTT_Client_ID);
  Serial.printf("MQTT Topic        : %s\n", _MQTT_Topic);
}

void WiFi_Manager::reset_config()
{
  Serial.println();
  Serial.println("CONFIGURATION RESET");

  _prefs.begin("MQTT_conf", false);
  _prefs.clear();
  _prefs.end();

  _WiFi_Manager.resetSettings();

  Serial.println("Restarting...");
  Serial.println();
  ESP.restart();
}