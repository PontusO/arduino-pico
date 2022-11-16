#pragma once

#include "wl_definitions.h"
#include "wl_types.h"

#if !defined(WIFIESPAT2)
#include "picoWiFiClass.h"
#else
#include "espatWiFiClass.h"
#endif
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiClientSecure.h"
#include "WiFiServerSecure.h"
#include "WiFiUdp.h"

#include "WiFiMulti.h"

#include "WiFiNTP.h"
