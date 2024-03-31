/*

    MIT License

	Copyright (c) 2024 Orlin Dimitrov

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma region Options

#define ENABLE_DEBUG_PORT

#define ENABLE_WIFI

// #define ENABLE_ETH

#define ENABLE_WG

#define ENABLE_OTA

#define ENABLE_WEB

#pragma endregion

#pragma region Definitions

#if defined(ENABLE_DEBUG_PORT)

/**
 * @brief Serial port baudrate.
 * 
 */
#define SERIAL_BAUDRATE 115200

#endif

#if defined(ENABLE_WIFI)

/**
 * @brief WiFi SSID.
 * 
 */
#define WIFI_SSID "YOUR_SSID"

/**
 * @brief WiFi Password
 * 
 */
#define WIFI_PASS "YOUR_PASS"

#endif

#if defined(ENABLE_WG)

/**
 * @brief [Peer] Endpoint port.
 * 
 */
#define WG_PORT 13231

/**
 * @brief [Peer] Endpoint
 * 
 */
#define WG_ENDPOINT "127.0.0.1"

/**
 * @brief [Interface] PrivateKey
 * 
 */
#define WG_PRIVATE_KEY "WB+sHyr69b6j9JN8O7WTBWgyhcUIx0kDUy47+L6J159="

/**
 * @brief [Peer] PublicKey
 * 
 */
#define WG_PUBLIC_KEY "rTgXcLDEFHD20yEnyzrdeBM+BwBNooLNO7dFy1fgO3Q="

#endif

#if defined(ENABLE_OTA)

#define OTA_PASS "admin"

#endif

#if defined(ENABLE_WEB)

/**
 * @brief WEB server port.
 * 
 */
#define WEB_SERVER_PORT 80

/**
 * @brief HTTP access username.
 * 
 */
#define HTTP_USER "admin"

/**
 * @brief HTTP access password.
 * 
 */
#define HTTP_PASS "admin"

#endif

#if defined(ENABLE_WEB) || defined(ENABLE_MB)

#endif

#pragma endregion

#pragma region Model

#pragma endregion

#endif
