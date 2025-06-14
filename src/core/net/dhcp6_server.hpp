/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
 *   This file includes definitions for DHCPv6 Server.
 */

#ifndef DHCP6_SERVER_HPP_
#define DHCP6_SERVER_HPP_

#include "openthread-core-config.h"

#if OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE

#include "common/locator.hpp"
#include "common/non_copyable.hpp"
#include "common/notifier.hpp"
#include "mac/mac.hpp"
#include "mac/mac_types.hpp"
#include "net/dhcp6_types.hpp"
#include "net/udp6.hpp"
#include "thread/network_data_leader.hpp"

namespace ot {
namespace Dhcp6 {

#if OPENTHREAD_ENABLE_DHCP6_MULTICAST_SOLICIT
#error "OPENTHREAD_ENABLE_DHCP6_MULTICAST_SOLICIT requires DHCPv6 server on Border Router side to be enabled."
#endif

/**
 * @addtogroup core-dhcp6
 *
 * @brief
 *   This module includes definitions for DHCPv6 Server.
 *
 * @{
 */

class Server : public InstanceLocator, private NonCopyable
{
    friend class ot::Notifier;

public:
    /**
     * Initializes the object.
     *
     * @param[in]  aInstance     A reference to the OpenThread instance.
     */
    explicit Server(Instance &aInstance);

private:
    class PrefixAgent
    {
    public:
        /**
         * Indicates whether or not @p aAddress has a matching prefix.
         *
         * @param[in]  aAddress  The IPv6 address to compare.
         *
         * @retval TRUE if the address has a matching prefix.
         * @retval FALSE if the address does not have a matching prefix.
         */
        bool IsPrefixMatch(const Ip6::Address &aAddress) const { return aAddress.MatchesPrefix(GetPrefix()); }

        /**
         * Indicates whether or not this entry is valid.
         *
         * @retval TRUE if this entry is valid.
         * @retval FALSE if this entry is not valid.
         */
        bool IsValid(void) const { return mAloc.mValid; }

        /**
         * Sets the entry to invalid.
         */
        void Clear(void) { mAloc.mValid = false; }

        /**
         * Returns the 6LoWPAN context ID.
         *
         * @returns The 6LoWPAN context ID.
         */
        uint8_t GetContextId(void) const { return mAloc.mAddress.mFields.m8[15]; }

        /**
         * Returns the ALOC.
         *
         * @returns the ALOC.
         */
        Ip6::Netif::UnicastAddress &GetAloc(void) { return mAloc; }

        /**
         * Returns the IPv6 prefix.
         *
         * @returns The IPv6 prefix.
         */
        const Ip6::Prefix &GetPrefix(void) const { return mPrefix; }

        /**
         * Returns the IPv6 prefix.
         *
         * @returns The IPv6 prefix.
         */
        Ip6::Prefix &GetPrefix(void) { return mPrefix; }

        /**
         * Returns the IPv6 prefix as an IPv6 address.
         *
         * @returns The IPv6 prefix as an IPv6 address.
         */
        const Ip6::Address &GetPrefixAsAddress(void) const
        {
            return static_cast<const Ip6::Address &>(mPrefix.mPrefix);
        }

        /**
         * Sets the ALOC.
         *
         * @param[in]  aPrefix           The IPv6 prefix.
         * @param[in]  aMeshLocalPrefix  The Mesh Local Prefix.
         * @param[in]  aContextId        The 6LoWPAN Context ID.
         */
        void Set(const Ip6::Prefix &aPrefix, const Ip6::NetworkPrefix &aMeshLocalPrefix, uint8_t aContextId)
        {
            mPrefix = aPrefix;

            mAloc.InitAsThreadOrigin();
            mAloc.GetAddress().SetToAnycastLocator(aMeshLocalPrefix, (Ip6::Address::kAloc16Mask << 8) + aContextId);
            mAloc.mMeshLocal = true;
        }

    private:
        Ip6::Netif::UnicastAddress mAloc;
        Ip6::Prefix                mPrefix;
    };

    static constexpr uint16_t kNumPrefixes = OPENTHREAD_CONFIG_DHCP6_SERVER_NUM_PREFIXES;

    void  HandleNotifierEvents(Events aEvents);
    void  UpdateService(void);
    void  Start(void);
    void  Stop(void);
    void  AddPrefixAgent(const Ip6::Prefix &aIp6Prefix, const Lowpan::Context &aContext);
    Error AppendHeader(Message &aMessage, const TransactionId &aTransactionId);
    Error AppendClientIdOption(Message &aMessage, const ClientIdOption &aClientIdOption);
    Error AppendServerIdOption(Message &aMessage);
    Error AppendIaNaOption(Message &aMessage, uint32_t aIaid, const Mac::ExtAddress &aClientAddress);
    Error AppendStatusCodeOption(Message &aMessage, StatusCodeOption::Status aStatusCode);
    Error AppendIaAddressOptions(Message &aMessage, const Mac::ExtAddress &aClientAddress);
    Error AppendRapidCommitOption(Message &aMessage) { return RapidCommitOption::AppendTo(aMessage); }
    Error AppendVendorSpecificInformation(Message &aMessage);
    Error AppendIaAddressOption(Message &aMessage, const Ip6::Address &aPrefix, const Mac::ExtAddress &aClientAddress);
    void  HandleUdpReceive(Message &aMessage, const Ip6::MessageInfo &aMessageInfo);
    void  ProcessSolicit(Message &aMessage, const Ip6::Address &aDst, const TransactionId &aTransactionId);
    Error ProcessClientIdOption(const Message &aMessage, ClientIdOption &aClientIdOption);
    Error ProcessIaNaOption(const Message &aMessage, uint32_t &aIaid);
    void  ProcessIaAddressOption(const IaAddressOption &aAddressOption);
    Error ProcessElapsedTimeOption(const Message &aMessage);
    Error SendReply(const Ip6::Address   &aDst,
                    const TransactionId  &aTransactionId,
                    const ClientIdOption &aClientIdOption,
                    uint32_t              aIaid);

    using ServerSocket = Ip6::Udp::SocketIn<Server, &Server::HandleUdpReceive>;

    ServerSocket mSocket;

    PrefixAgent mPrefixAgents[kNumPrefixes];
    uint8_t     mPrefixAgentsCount;
    uint8_t     mPrefixAgentsMask;
};

/**
 * @}
 */

} // namespace Dhcp6
} // namespace ot

#endif // OPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE

#endif // DHCP6_SERVER_HPP_
