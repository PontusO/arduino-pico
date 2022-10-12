#pragma once

#define SS    (10U)

#include <LwipIntfDev.h>
#include <driver/nRF7002shim.h>

using nRF7002lwIP = LwipIntfDev<nRF7002>;
