/*
  This file is based on the WiFiEspAT library for Arduino
  https://github.com/jandrassy/WiFiEspAT
  Copyright 2019 Juraj Andrassy

  Modified to work with Arduino-Pico board support package by
  Pontus Oldberg 2022

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "WiFi.h"
#include "utility/EspAtDrv.h"

#define WIFIESPAT_INTERNAL_AP_LIST_SIZE   5

// these statics are removed by compiler, if not used
char WiFiClass::fwVersion[15] = {0};
char WiFiClass::ssid[33] = {0};
char WiFiClass::name[33] = {0}; // hostname
WiFiApData WiFiClass::apDataInternal[WIFIESPAT_INTERNAL_AP_LIST_SIZE];

#if 1
bool WiFiClass::init(Stream& serial, int8_t resetPin) {
  return init(&serial, resetPin);
}

bool WiFiClass::init(Stream* serial, int8_t resetPin) {
  bool ok = EspAtDrv.init(serial, resetPin);
  state = ok ? WL_IDLE_STATUS : WL_NO_MODULE;
  return ok;
}
#endif

WiFiClass::WiFiClass() {
}

/*
    Get firmware version
*/
const char* WiFiClass::firmwareVersion() {
  EspAtDrv.firmwareVersion(&WiFiClass::fwVersion[0]);
  return &WiFiClass::fwVersion[0];
}

void WiFiClass::mode(WiFiMode_t m) {
    _calledESP = true;
    switch (m) {
    case WiFiMode_t::WIFI_OFF:
        end();
        break;
    case WiFiMode_t::WIFI_AP:
        _modeESP = WiFiMode_t::WIFI_AP;
        break;
    case WiFiMode_t::WIFI_STA:
        _modeESP = WiFiMode_t::WIFI_STA;
        break;
    case WiFiMode_t::WIFI_AP_STA:
        _modeESP = WiFiMode_t::WIFI_STA;
        break;
    }
}


/*  Start WiFi connection for OPEN networks

    param ssid: Pointer to the SSID string.
*/
int WiFiClass::begin(const char* ssid) {
  bool ok = EspAtDrv.joinAP(ssid, NULL, NULL);
  state = ok ? WL_CONNECTED : WL_CONNECT_FAILED;
  return state;
}

#if defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(WIFIESPAT2)
/*  Start WiFi connection with passphrase
    the most secure supported mode will be automatically selected

    param ssid: Pointer to the SSID string.
    param passphrase: Passphrase. Valid characters in a passphrase
          must be between ASCII 32-126 (decimal).
*/
int WiFiClass::begin(const char* ssid, const char *passphrase) {
  bool ok = EspAtDrv.joinAP(ssid, passphrase, NULL);
  state = ok ? WL_CONNECTED : WL_CONNECT_FAILED;
  return state;
}

uint8_t WiFiClass::beginAP(const char *ssid) {
  uint8_t encoding = 4; // WPA_WPA2_PSK
  apMaxConn = 0; // clear the ap params info cahche
  return EspAtDrv.beginSoftAP(ssid, NULL, 10, encoding, 2, false) ? WL_AP_LISTENING : WL_AP_FAILED;
}

uint8_t WiFiClass::beginAP(const char *ssid, uint8_t channel) {
  uint8_t encoding = 4; // WPA_WPA2_PSK
  apMaxConn = 0; // clear the ap params info cahche
  return EspAtDrv.beginSoftAP(ssid, NULL, channel, encoding, 2, false) ? WL_AP_LISTENING : WL_AP_FAILED;
}

uint8_t WiFiClass::beginAP(const char *ssid, const char* passphrase, uint8_t channel) {
  uint8_t encoding = 4; // WPA_WPA2_PSK
  apMaxConn = 0; // clear the ap params info cahche
  return EspAtDrv.beginSoftAP(ssid, passphrase, channel, encoding, 2, false) ? WL_AP_LISTENING : WL_AP_FAILED;
}

uint8_t WiFiClass::beginAP(const char *ssid, const char* passphrase) {
  uint8_t encoding = 4; // WPA_WPA2_PSK
  apMaxConn = 0; // clear the ap params info cahche
  return EspAtDrv.beginSoftAP(ssid, passphrase, 10, encoding, 2, false) ? WL_AP_LISTENING : WL_AP_FAILED;
}
#endif

bool WiFiClass::connected() {
  return (state == WL_CONNECTED);
}

/*  Change Ip configuration settings disabling the dhcp client

    param local_ip: 	Static ip configuration
*/
void WiFiClass::config(IPAddress local_ip) {
  EspAtDrv.staStaticIp(local_ip, INADDR_ANY, INADDR_ANY);
}

/*  Change Ip configuration settings disabling the dhcp client

    param local_ip: 	Static ip configuration
    param dns_server:     IP configuration for DNS server 1
*/
void WiFiClass::config(IPAddress local_ip, IPAddress dns_server) {
  EspAtDrv.staStaticIp(local_ip, INADDR_ANY, INADDR_ANY);
  setDNS(dns_server);
}

/*  Change Ip configuration settings disabling the dhcp client

    param local_ip: 	Static ip configuration
    param dns_server:     IP configuration for DNS server 1
    param gateway : 	Static gateway configuration
*/
void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
  EspAtDrv.staStaticIp(local_ip, gateway, INADDR_ANY);
  setDNS(dns_server);
}

/*  Change Ip configuration settings disabling the dhcp client

    param local_ip: 	Static ip configuration
    param dns_server:     IP configuration for DNS server 1
    param gateway: 	Static gateway configuration
    param subnet:		Static Subnet mask
*/
void WiFiClass::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
  EspAtDrv.staStaticIp(local_ip, gateway, subnet);
  setDNS(dns_server);
}

/*  Change DNS Ip configuration

    param dns_server1: ip configuration for DNS server 1
*/
void WiFiClass::setDNS(IPAddress dns_server1) {
  EspAtDrv.staDNS(dns_server1, INADDR_ANY);
}

/*  Change DNS Ip configuration

    param dns_server1: ip configuration for DNS server 1
    param dns_server2: ip configuration for DNS server 2

*/
void WiFiClass::setDNS(IPAddress dns_server1, IPAddress dns_server2) {
  EspAtDrv.staDNS(dns_server1, dns_server2);
}

/*  Set the hostname used for DHCP requests

    param name: hostname to set

*/
void WiFiClass::setHostname(const char* name) {
    EspAtDrv.setHostname(name);
}
const char *WiFiClass::getHostname() {
  if (!EspAtDrv.hostnameQuery(&WiFiClass::name[0])) {
    WiFiClass::name[0] = 0;
  }
  return &WiFiClass::name[0];
}

/*
    Disconnect from the network

    return: one value of wl_status_t enum
*/
int WiFiClass::disconnect(bool wifi_off) {
  if (EspAtDrv.quitAP(wifi_off)) {
    state = WL_DISCONNECTED;
  }
  return state;
}

void WiFiClass::end(void) {
  disconnect();
}

/*
    Get the interface MAC address.

    return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
*/
uint8_t* WiFiClass::macAddress(uint8_t* mac) {
  if (!EspAtDrv.staMacQuery(mac))
    return nullptr;
  return mac;
}

/*
    Get the interface IP address.

    return: Ip address value
*/
IPAddress WiFiClass::localIP() {
  IPAddress ip;
  IPAddress gw;
  IPAddress mask;
  EspAtDrv.staIpQuery(ip, gw, mask);
  return ip;
}

/*
    Get the interface subnet mask address.

    return: subnet mask address value
*/
IPAddress WiFiClass::subnetMask() {
  IPAddress ip;
  IPAddress gw;
  IPAddress mask;
  EspAtDrv.staIpQuery(ip, gw, mask);
  return mask;
}

/*
    Get the gateway ip address.

    return: gateway ip address value
*/
IPAddress WiFiClass::gatewayIP() {
  IPAddress ip;
  IPAddress gw;
  IPAddress mask;
  EspAtDrv.staIpQuery(ip, gw, mask);
  return gw;
}

/*
    Return the current SSID associated with the network

    return: ssid string
*/
const String &WiFiClass::SSID() {
  uint8_t bssid[6] = {0};
  uint8_t ch = 0;
  int32_t rssi = 0;
  EspAtDrv.apQuery(ssid, bssid, ch, rssi);
  _ssid = String(ssid);
  return _ssid;
}

/*
    Return the current BSSID associated with the network.
    It is the MAC address of the Access Point

    return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
*/
uint8_t* WiFiClass::BSSID(uint8_t* bssid) {
  uint8_t ch = 0;
  int32_t rssi = 0;
  EspAtDrv.apQuery(nullptr, bssid, ch, rssi);
  return bssid;
}

/*
 * TODO: Need to get the current channel from the connection result.
 */
int WiFiClass::channel() {
  if (connected()) {
    return -2;
  }
  return -1;
}


/*
    Return the current RSSI /Received Signal Strength in dBm)
    associated with the network

    return: signed value
*/
int32_t WiFiClass::RSSI() {
  uint8_t bssid[6] = {0};
  uint8_t ch = 0;
  int32_t rssi = 0;
  EspAtDrv.apQuery(nullptr, bssid, ch, rssi);
  return rssi;
}

/*
    Return the Encryption Type associated with the network

    return: one value of wl_enc_type enum

    TODO: Need to get the encryption typed from the connection result
*/
uint8_t WiFiClass::encryptionType() {
  if (connected()) {
    return 2;
  }
  return 1;
}

//TODO - this can be in the class
static uint64_t _to64(uint8_t b[8]) {
    uint64_t x = 0;
    for (int i = 0; i < 6; i++) {
        x <<= 8LL;
        x |= b[i] & 255;
    }
    return x;
}

/*
    Start scan WiFi networks available

    return: Number of discovered networks

    We're not supporting asyncronous scans yet.
*/
int8_t WiFiClass::scanNetworks(bool async) {
  return scanNetworks(apDataInternal, WIFIESPAT_INTERNAL_AP_LIST_SIZE);
}

int8_t WiFiClass::scanNetworks(WiFiApData* _apData, uint8_t _apDataSize) {
  apData = _apData;
  apDataSize = _apDataSize;
  apDataLength = EspAtDrv.listAP(apData, apDataSize);
  return apDataLength;
}

int8_t WiFiClass::scanComplete() {
  return apDataLength;
}

void WiFiClass::scanDelete() {
    apDataLength = 0;
}

/*
    Return the SSID discovered during the network scan.

    param networkItem: specify from which network item want to get the information

    return: ssid string of the specified item on the networks scanned list
*/
const char* WiFiClass::SSID(uint8_t networkItem) {
  if (networkItem >= apDataLength)
    return nullptr;
  return apData[networkItem].ssid;
}

/*
    Return the encryption type of the networks discovered during the scanNetworks

    param networkItem: specify from which network item want to get the information

    return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
*/
uint8_t WiFiClass::encryptionType(uint8_t networkItem) {
  if (networkItem >= apDataLength)
    return ENC_TYPE_UNKNOWN;
  return mapAtEnc2ArduinoEnc(apData[networkItem].enc);
}

uint8_t* WiFiClass::BSSID(uint8_t networkItem, uint8_t* bssid) {
  if (networkItem >= apDataLength)
    return nullptr;
  memcpy(bssid, apData[networkItem].bssid, 6);
  return bssid;
}

uint8_t WiFiClass::channel(uint8_t networkItem) {
  if (networkItem >= apDataLength)
    return 0;
  return apData[networkItem].channel;
}

/*
    Return the RSSI of the networks discovered during the scanNetworks

    param networkItem: specify from which network item want to get the information

    return: signed value of RSSI of the specified item on the networks scanned list
*/
int32_t WiFiClass::RSSI(uint8_t networkItem) {
  if (networkItem >= apDataLength)
    return -9999;
  return apData[networkItem].rssi;
}

/*
    Return Connection status.

    return: one of the value defined in wl_status_t
*/
uint8_t WiFiClass::status() {
  if (state == WL_NO_MODULE)
    return state;
  int res = EspAtDrv.staStatus();
  switch (res) {
    case -1:
      switch (EspAtDrv.getLastErrorCode()) {
        case EspAtDrvError::NOT_INITIALIZED:
        case EspAtDrvError::AT_NOT_RESPONDIG:
          state = WL_NO_MODULE;
          break;
        default: // some temporary error?
          break; // no change
      }
      break;
    case 2:
    case 3:
    case 4:
      state = WL_CONNECTED;
      break;
    case 0: // inactive
    case 1: // idle
    case 5: // STA disconnected
      switch (state) {
        case WL_CONNECT_FAILED:
          break; // no change
        case WL_CONNECTED:
          state = WL_CONNECTION_LOST;
          break;
        default:
          state = WL_DISCONNECTED;
      }
  }
  return state;
}

/*
    Return The deauthentication reason code.

    return: the deauthentication reason code
*/
uint8_t WiFiClass::reasonCode() {
    // TODO: Get result code from connection result
    return WL_NO_SHIELD;
}

/**
    Resolve the given hostname to an IP address.
    @param aHostname     Name to be resolved
    @param aResult       IPAddress structure to store the returned IP address
    @return 1 if aIPAddrString was successfully converted to an IP address,
            else 0
*/

int WiFiClass::hostByName(const char* aHostname, IPAddress& aResult, int timeout_ms) {
  // TODO: Implement timeout timer.
  (void) timeout_ms;
  return EspAtDrv.resolve(aHostname, aResult);
}

// TODO
unsigned long WiFiClass::getTime() {
    return millis();
}

void WiFiClass::lowPowerMode() {
}

void WiFiClass::noLowPowerMode() {
}

int WiFiClass::ping(const char* hostname, uint8_t ttl) {
  (void) ttl;
  IPAddress ip;
  if (!hostByName(hostname, ip)) {
    return WL_PING_UNKNOWN_HOST;
  }
  return ping(ip, ttl);
}

int WiFiClass::ping(const String &hostname, uint8_t ttl) {
  (void) ttl;
  return ping(hostname.c_str(), ttl);
}

int WiFiClass::ping(IPAddress host, uint8_t ttl) {
  // TODO: Implement ttl
  (void) ttl;
  char s[16];
  EspAtDrv.ip2str(host, s);
  return ping(s);
}

void WiFiClass::setTimeout(unsigned long timeout) {
    _timeout = timeout;
}

void WiFiClass::setFeedWatchdogFunc(FeedHostProcessorWatchdogFuncPointer func) {
    (void) func;
}

void WiFiClass::feedWatchdog() {
}

uint8_t WiFiClass::mapAtEnc2ArduinoEnc(uint8_t encryptionType) {
  if (encryptionType == 0) { // WIFI_AUTH_OPEN
    encryptionType = ENC_TYPE_NONE;
  } else if (encryptionType == 1) { // WIFI_AUTH_WEP
    encryptionType = ENC_TYPE_WEP;
  } else if (encryptionType == 2) { // WIFI_AUTH_WPA_PSK
    encryptionType = ENC_TYPE_TKIP;
  } else if (encryptionType == 3 || encryptionType == 4) { // WIFI_AUTH_WPA2_PSK || WIFI_AUTH_WPA_WPA2_PSK
    encryptionType = ENC_TYPE_CCMP;
  } else {
    // unknown?
    encryptionType = ENC_TYPE_UNKNOWN;
  }
  return encryptionType;
}

WiFiClass WiFi;
