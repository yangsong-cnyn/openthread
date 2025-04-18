/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes compile-time configurations for the SRP (Service Registration Protocol) Server.
 */

#ifndef CONFIG_SRP_SERVER_H_
#define CONFIG_SRP_SERVER_H_

/**
 * @addtogroup config-srp-server
 *
 * @brief
 *   This module includes configuration variables for the SRP Server.
 *
 * @{
 */

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
 *
 * Define to 1 to enable SRP Server support.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_DEFAULT_ADDRESS_MODE
 *
 * Specifies the default address mode used by the SRP server.
 *
 * The address mode specifies how address and port number are determined by the SRP server and how this info is
 * published in the Thread Network Data.
 *
 * The value of this configuration should be from `otSrpServerAddressMode` enumeration.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_DEFAULT_ADDRESS_MODE
#define OPENTHREAD_CONFIG_SRP_SERVER_DEFAULT_ADDRESS_MODE OT_SRP_SERVER_ADDRESS_MODE_UNICAST
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MIN
 *
 * Specifies the min port number in the port range reserved for SRP server.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MIN
#define OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MIN 53535
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MAX
 *
 * Specifies the max port number in the port range reserved for SRP server.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MAX
#define OPENTHREAD_CONFIG_SRP_SERVER_UDP_PORT_MAX 53554
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
 *
 * Define to 1 to enable SRP server feature to save its own port in non-volatile settings.
 *
 * When enabled, the SRP server will save its port in the non-volatile settings. On a server
 * restart (e.g., due to a device reset) it will restore the port and change to a different one. The info is written to
 * Settings after the first service has been registered due to receiving an SRP Update.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_PORT_SWITCH_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_SERVICE_UPDATE_TIMEOUT
 *
 * Specifies the timeout value (in milliseconds) for the service update handler.
 *
 * The default timeout value is the sum of the maximum total mDNS probing delays
 * and a loose IPC timeout of 250ms. It is recommended that this configuration should
 * not use a value smaller than the default value here, if an Advertising Proxy is used
 * to handle the service update events.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_SERVICE_UPDATE_TIMEOUT
#define OPENTHREAD_CONFIG_SRP_SERVER_SERVICE_UPDATE_TIMEOUT ((4 * 250u) + 250u)
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_ADVERTISING_PROXY_ENABLE
 *
 * Define to 1 to enable Advertising Proxy for SRP Sever.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_ADVERTISING_PROXY_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_ADVERTISING_PROXY_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_SERVER_FAST_START_MODE_ENABLE
 *
 * The Fast Start Mode is designed for scenarios where a device, often a mobile device, needs to act as a provisional
 * SRP server (e.g., functioning as a temporary Border Router). The SRP server function is enabled only if no other
 * Border Routers (BRs) are already providing the SRP service within the Thread network. A common use case is a mobile
 * device joining a Thread network where it may be the first, or only, BR.  Importantly, Fast Start Mode allows the
 * device to quickly start its SRP server functionality upon joining the network, allowing other Thread devices to
 * quickly connect and register their services without the typical delays associated with standard Border Router
 * initialization (and SRP server startup).
 *
 * The Fast Start Mode can be enabled using `otSrpServerEnableFastStartMode()`. More details about behavior and
 * restrictions are provided in the documentation of this API.
 */
#ifndef OPENTHREAD_CONFIG_SRP_SERVER_FAST_START_MODE_ENABLE
#define OPENTHREAD_CONFIG_SRP_SERVER_FAST_START_MODE_ENABLE 0
#endif

/**
 * @}
 */

#endif // CONFIG_SRP_SERVER_H_
