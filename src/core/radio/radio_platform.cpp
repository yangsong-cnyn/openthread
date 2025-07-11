/*
 *    Copyright (c) 2019, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 *    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the radio platform callbacks into OpenThread and default/weak radio platform APIs.
 */

#include <openthread/instance.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>

#include "instance/instance.hpp"

using namespace ot;

//---------------------------------------------------------------------------------------------------------------------
// otPlatRadio callbacks

#if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

extern "C" void otPlatRadioReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    Instance     &instance = AsCoreType(aInstance);
    Mac::RxFrame *rxFrame  = static_cast<Mac::RxFrame *>(aFrame);

    VerifyOrExit(instance.IsInitialized());

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (rxFrame != nullptr)
    {
        rxFrame->SetRadioType(Mac::kRadioTypeIeee802154);
    }
#endif

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    if (instance.Get<Radio>().GetDiagMode())
    {
        instance.Get<Radio::Callbacks>().HandleDiagsReceiveDone(rxFrame, aError);
    }
    else
#endif
    {
        instance.Get<Radio::Callbacks>().HandleReceiveDone(rxFrame, aError);
    }

exit:
    return;
}

extern "C" void otPlatRadioTxStarted(otInstance *aInstance, otRadioFrame *aFrame)
{
    Instance     &instance = AsCoreType(aInstance);
    Mac::TxFrame &txFrame  = *static_cast<Mac::TxFrame *>(aFrame);

    VerifyOrExit(instance.IsInitialized());

#if OPENTHREAD_CONFIG_MULTI_RADIO
    txFrame.SetRadioType(Mac::kRadioTypeIeee802154);
#endif

    instance.Get<Radio::Callbacks>().HandleTransmitStarted(txFrame);

exit:
    return;
}

extern "C" void otPlatRadioTxDone(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    Instance     &instance = AsCoreType(aInstance);
    Mac::TxFrame &txFrame  = *static_cast<Mac::TxFrame *>(aFrame);
    Mac::RxFrame *ackFrame = static_cast<Mac::RxFrame *>(aAckFrame);

    VerifyOrExit(instance.IsInitialized());

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (ackFrame != nullptr)
    {
        ackFrame->SetRadioType(Mac::kRadioTypeIeee802154);
    }

    txFrame.SetRadioType(Mac::kRadioTypeIeee802154);
#endif

#if OPENTHREAD_CONFIG_DIAG_ENABLE
    if (instance.Get<Radio>().GetDiagMode())
    {
#if OPENTHREAD_RADIO
        uint8_t channel = txFrame.mInfo.mTxInfo.mRxChannelAfterTxDone;

        if (channel != aFrame->mChannel)
        {
            OT_ASSERT((otPlatRadioGetSupportedChannelMask(aInstance) & (1UL << channel)) != 0);
            IgnoreError(otPlatRadioReceive(aInstance, channel));
        }
#endif

        instance.Get<Radio::Callbacks>().HandleDiagsTransmitDone(txFrame, aError);
    }
    else
#endif
    {
        instance.Get<Radio::Callbacks>().HandleTransmitDone(txFrame, ackFrame, aError);
    }
exit:
    return;
}

extern "C" void otPlatRadioEnergyScanDone(otInstance *aInstance, int8_t aEnergyScanMaxRssi)
{
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.IsInitialized());
    instance.Get<Radio::Callbacks>().HandleEnergyScanDone(aEnergyScanMaxRssi);

exit:
    return;
}

extern "C" void otPlatRadioBusLatencyChanged(otInstance *aInstance)
{
    Instance &instance = AsCoreType(aInstance);

    VerifyOrExit(instance.IsInitialized());
    instance.Get<Radio::Callbacks>().HandleBusLatencyChanged();

exit:
    return;
}

#if OPENTHREAD_CONFIG_DIAG_ENABLE
extern "C" void otPlatDiagRadioReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    Mac::RxFrame *rxFrame = static_cast<Mac::RxFrame *>(aFrame);

#if OPENTHREAD_CONFIG_MULTI_RADIO
    if (rxFrame != nullptr)
    {
        rxFrame->SetRadioType(Mac::kRadioTypeIeee802154);
    }
#endif

    AsCoreType(aInstance).Get<Radio::Callbacks>().HandleDiagsReceiveDone(rxFrame, aError);
}

extern "C" void otPlatDiagRadioTransmitDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    Mac::TxFrame &txFrame = *static_cast<Mac::TxFrame *>(aFrame);
#if OPENTHREAD_RADIO
    uint8_t channel = txFrame.mInfo.mTxInfo.mRxChannelAfterTxDone;

    if (channel != aFrame->mChannel)
    {
        OT_ASSERT((otPlatRadioGetSupportedChannelMask(aInstance) & (1UL << channel)) != 0);
        IgnoreError(otPlatRadioReceive(aInstance, channel));
    }
#endif
#if OPENTHREAD_CONFIG_MULTI_RADIO
    txFrame.SetRadioType(Mac::kRadioTypeIeee802154);
#endif

    AsCoreType(aInstance).Get<Radio::Callbacks>().HandleDiagsTransmitDone(txFrame, aError);
}
#endif
#else // #if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

extern "C" void otPlatRadioReceiveDone(otInstance *, otRadioFrame *, otError) {}

extern "C" void otPlatRadioTxStarted(otInstance *, otRadioFrame *) {}

extern "C" void otPlatRadioTxDone(otInstance *, otRadioFrame *, otRadioFrame *, otError) {}

extern "C" void otPlatRadioEnergyScanDone(otInstance *, int8_t) {}

extern "C" void otPlatRadioBusLatencyChanged(otInstance *) {}

#endif // // #if OPENTHREAD_CONFIG_RADIO_LINK_IEEE_802_15_4_ENABLE

//---------------------------------------------------------------------------------------------------------------------
// Default/weak implementation of radio platform APIs

extern "C" OT_TOOL_WEAK void otPlatRadioSetAlternateShortAddress(otInstance *aInstance, otShortAddress aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddress);
}

extern "C" OT_TOOL_WEAK uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return Radio::kSupportedChannels;
}

extern "C" OT_TOOL_WEAK uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance)
{
    return otPlatRadioGetSupportedChannelMask(aInstance);
}

extern "C" OT_TOOL_WEAK const char *otPlatRadioGetVersionString(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return otGetVersionString();
}

extern "C" OT_TOOL_WEAK otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return OT_RADIO_STATE_INVALID;
}

extern "C" OT_TOOL_WEAK void otPlatRadioSetMacKey(otInstance             *aInstance,
                                                  uint8_t                 aKeyIdMode,
                                                  uint8_t                 aKeyId,
                                                  const otMacKeyMaterial *aPrevKey,
                                                  const otMacKeyMaterial *aCurrKey,
                                                  const otMacKeyMaterial *aNextKey,
                                                  otRadioKeyType          aKeyType)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyIdMode);
    OT_UNUSED_VARIABLE(aKeyId);
    OT_UNUSED_VARIABLE(aPrevKey);
    OT_UNUSED_VARIABLE(aCurrKey);
    OT_UNUSED_VARIABLE(aNextKey);
    OT_UNUSED_VARIABLE(aKeyType);
}

extern "C" OT_TOOL_WEAK void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aMacFrameCounter);
}

extern "C" OT_TOOL_WEAK void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    // Radio platforms that support `OT_RADIO_CAPS_TRANSMIT_SEC` should
    // provide this radio platform function.
    //
    // This function helps address an edge-case where OT stack may not
    // yet know the latest frame counter values used by radio platform
    // (e.g., due to enhanced acks processed by radio platform directly
    // or due to delay between RCP and host) and then setting the value
    // from OT stack may cause the counter value on radio to move back
    // (OT stack will set the counter after appending it in
    // `LinkFrameCounterTlv` on all radios when multi-radio links
    // feature is enabled).
    //
    // The weak implementation here is intended as a solution to ensure
    // temporary compatibility with radio platforms that may not yet
    // implement it. If this weak implementation is used, the edge-case
    // above may still happen.

    otPlatRadioSetMacFrameCounter(aInstance, aMacFrameCounter);
}

extern "C" OT_TOOL_WEAK uint64_t otPlatTimeGet(void) { return UINT64_MAX; }

extern "C" OT_TOOL_WEAK uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otPlatTimeGet();
}

extern "C" OT_TOOL_WEAK uint32_t otPlatRadioGetBusSpeed(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 0;
}

extern "C" OT_TOOL_WEAK uint32_t otPlatRadioGetBusLatency(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 0;
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
extern "C" OT_TOOL_WEAK otError otPlatRadioResetCsl(otInstance *aInstance)
{
    return otPlatRadioEnableCsl(aInstance, 0, Mac::kShortAddrInvalid, nullptr);
}
#endif

extern "C" OT_TOOL_WEAK uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return NumericLimits<uint8_t>::kMax;
}

extern "C" OT_TOOL_WEAK uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return NumericLimits<uint8_t>::kMax;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioGetFemLnaGain(otInstance *aInstance, int8_t *aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aGain);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioSetFemLnaGain(otInstance *aInstance, int8_t aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aGain);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance,
                                                                      uint8_t     aChannel,
                                                                      int8_t      aMaxPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aChannel);
    OT_UNUSED_VARIABLE(aMaxPower);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioSetRegion(otInstance *aInstance, uint16_t aRegionCode)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aRegionCode);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioGetRegion(otInstance *aInstance, uint16_t *aRegionCode)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aRegionCode);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK otError otPlatRadioReceiveAt(otInstance *aInstance,
                                                     uint8_t     aChannel,
                                                     uint32_t    aStart,
                                                     uint32_t    aDuration)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aChannel);
    OT_UNUSED_VARIABLE(aStart);
    OT_UNUSED_VARIABLE(aDuration);

    return kErrorNotImplemented;
}

extern "C" OT_TOOL_WEAK void otPlatRadioSetRxOnWhenIdle(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aEnable);
}

OT_TOOL_WEAK otError otPlatRadioSetChannelTargetPower(otInstance *, uint8_t, int16_t) { return kErrorNotImplemented; }
