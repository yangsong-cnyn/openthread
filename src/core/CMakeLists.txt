#
#  Copyright (c) 2019, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

set(COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/src/core
)

set(COMMON_SOURCES
    api/backbone_router_api.cpp
    api/backbone_router_ftd_api.cpp
    api/ble_secure_api.cpp
    api/border_agent_api.cpp
    api/border_router_api.cpp
    api/border_routing_api.cpp
    api/channel_manager_api.cpp
    api/channel_monitor_api.cpp
    api/child_supervision_api.cpp
    api/coap_api.cpp
    api/coap_secure_api.cpp
    api/commissioner_api.cpp
    api/crypto_api.cpp
    api/dataset_api.cpp
    api/dataset_ftd_api.cpp
    api/dataset_updater_api.cpp
    api/diags_api.cpp
    api/dns_api.cpp
    api/dns_server_api.cpp
    api/error_api.cpp
    api/heap_api.cpp
    api/history_tracker_api.cpp
    api/icmp6_api.cpp
    api/instance_api.cpp
    api/ip6_api.cpp
    api/jam_detection_api.cpp
    api/joiner_api.cpp
    api/link_api.cpp
    api/link_metrics_api.cpp
    api/link_raw_api.cpp
    api/logging_api.cpp
    api/mdns_api.cpp
    api/mesh_diag_api.cpp
    api/message_api.cpp
    api/multi_radio_api.cpp
    api/nat64_api.cpp
    api/netdata_api.cpp
    api/netdata_publisher_api.cpp
    api/netdiag_api.cpp
    api/network_time_api.cpp
    api/ping_sender_api.cpp
    api/radio_stats_api.cpp
    api/random_crypto_api.cpp
    api/random_noncrypto_api.cpp
    api/server_api.cpp
    api/sntp_api.cpp
    api/srp_client_api.cpp
    api/srp_client_buffers_api.cpp
    api/srp_server_api.cpp
    api/tasklet_api.cpp
    api/tcp_api.cpp
    api/tcp_ext_api.cpp
    api/thread_api.cpp
    api/thread_ftd_api.cpp
    api/trel_api.cpp
    api/udp_api.cpp
    api/verhoeff_checksum_api.cpp
    backbone_router/backbone_tmf.cpp
    backbone_router/bbr_leader.cpp
    backbone_router/bbr_local.cpp
    backbone_router/bbr_manager.cpp
    backbone_router/multicast_listeners_table.cpp
    backbone_router/ndproxy_table.cpp
    border_router/dhcp6_pd_client.cpp
    border_router/infra_if.cpp
    border_router/routing_manager.cpp
    coap/coap.cpp
    coap/coap_message.cpp
    coap/coap_secure.cpp
    common/appender.cpp
    common/binary_search.cpp
    common/crc.cpp
    common/data.cpp
    common/error.cpp
    common/frame_builder.cpp
    common/frame_data.cpp
    common/heap.cpp
    common/heap_data.cpp
    common/heap_string.cpp
    common/log.cpp
    common/message.cpp
    common/notifier.cpp
    common/offset_range.cpp
    common/preference.cpp
    common/random.cpp
    common/settings.cpp
    common/string.cpp
    common/tasklet.cpp
    common/time_ticker.cpp
    common/timer.cpp
    common/tlvs.cpp
    common/trickle_timer.cpp
    common/uptime.cpp
    crypto/aes_ccm.cpp
    crypto/aes_ecb.cpp
    crypto/crypto_platform.cpp
    crypto/hkdf_sha256.cpp
    crypto/hmac_sha256.cpp
    crypto/mbedtls.cpp
    crypto/sha256.cpp
    crypto/storage.cpp
    diags/factory_diags.cpp
    instance/instance.cpp
    mac/channel_mask.cpp
    mac/data_poll_handler.cpp
    mac/data_poll_sender.cpp
    mac/link_raw.cpp
    mac/mac.cpp
    mac/mac_filter.cpp
    mac/mac_frame.cpp
    mac/mac_header_ie.cpp
    mac/mac_links.cpp
    mac/mac_types.cpp
    mac/sub_mac.cpp
    mac/sub_mac_callbacks.cpp
    mac/sub_mac_csl_receiver.cpp
    mac/sub_mac_wed.cpp
    mac/wakeup_tx_scheduler.cpp
    meshcop/announce_begin_client.cpp
    meshcop/border_agent.cpp
    meshcop/commissioner.cpp
    meshcop/dataset.cpp
    meshcop/dataset_manager.cpp
    meshcop/dataset_manager_ftd.cpp
    meshcop/dataset_updater.cpp
    meshcop/energy_scan_client.cpp
    meshcop/extended_panid.cpp
    meshcop/joiner.cpp
    meshcop/joiner_router.cpp
    meshcop/meshcop.cpp
    meshcop/meshcop_leader.cpp
    meshcop/meshcop_tlvs.cpp
    meshcop/network_name.cpp
    meshcop/panid_query_client.cpp
    meshcop/secure_transport.cpp
    meshcop/tcat_agent.cpp
    meshcop/timestamp.cpp
    net/checksum.cpp
    net/dhcp6_client.cpp
    net/dhcp6_server.cpp
    net/dhcp6_types.cpp
    net/dns_client.cpp
    net/dns_dso.cpp
    net/dns_platform.cpp
    net/dns_types.cpp
    net/dnssd.cpp
    net/dnssd_server.cpp
    net/icmp6.cpp
    net/ip4_types.cpp
    net/ip6.cpp
    net/ip6_address.cpp
    net/ip6_filter.cpp
    net/ip6_headers.cpp
    net/ip6_mpl.cpp
    net/mdns.cpp
    net/nat64_translator.cpp
    net/nd6.cpp
    net/nd_agent.cpp
    net/netif.cpp
    net/sntp_client.cpp
    net/socket.cpp
    net/srp_advertising_proxy.cpp
    net/srp_client.cpp
    net/srp_server.cpp
    net/tcp6.cpp
    net/tcp6_ext.cpp
    net/udp6.cpp
    radio/ble_secure.cpp
    radio/radio.cpp
    radio/radio_callbacks.cpp
    radio/radio_platform.cpp
    radio/trel_interface.cpp
    radio/trel_link.cpp
    radio/trel_packet.cpp
    radio/trel_peer.cpp
    radio/trel_peer_discoverer.cpp
    thread/address_resolver.cpp
    thread/announce_begin_server.cpp
    thread/announce_sender.cpp
    thread/anycast_locator.cpp
    thread/child.cpp
    thread/child_supervision.cpp
    thread/child_table.cpp
    thread/csl_tx_scheduler.cpp
    thread/discover_scanner.cpp
    thread/dua_manager.cpp
    thread/energy_scan_server.cpp
    thread/indirect_sender.cpp
    thread/key_manager.cpp
    thread/link_metrics.cpp
    thread/link_metrics_types.cpp
    thread/link_quality.cpp
    thread/lowpan.cpp
    thread/mesh_forwarder.cpp
    thread/mesh_forwarder_ftd.cpp
    thread/mesh_forwarder_mtd.cpp
    thread/mle.cpp
    thread/mle_ftd.cpp
    thread/mle_tlvs.cpp
    thread/mle_types.cpp
    thread/mlr_manager.cpp
    thread/neighbor.cpp
    thread/neighbor_table.cpp
    thread/network_data.cpp
    thread/network_data_leader.cpp
    thread/network_data_leader_ftd.cpp
    thread/network_data_local.cpp
    thread/network_data_notifier.cpp
    thread/network_data_publisher.cpp
    thread/network_data_service.cpp
    thread/network_data_tlvs.cpp
    thread/network_data_types.cpp
    thread/network_diagnostic.cpp
    thread/network_diagnostic_tlvs.cpp
    thread/panid_query_server.cpp
    thread/radio_selector.cpp
    thread/router.cpp
    thread/router_table.cpp
    thread/src_match_controller.cpp
    thread/thread_link_info.cpp
    thread/thread_netif.cpp
    thread/time_sync_service.cpp
    thread/tmf.cpp
    thread/uri_paths.cpp
    thread/version.cpp
    utils/channel_manager.cpp
    utils/channel_monitor.cpp
    utils/flash.cpp
    utils/heap.cpp
    utils/history_tracker.cpp
    utils/jam_detector.cpp
    utils/link_metrics_manager.cpp
    utils/mesh_diag.cpp
    utils/otns.cpp
    utils/parse_cmdline.cpp
    utils/ping_sender.cpp
    utils/power_calibration.cpp
    utils/slaac_address.cpp
    utils/srp_client_buffers.cpp
    utils/verhoeff_checksum.cpp
)

set(RADIO_COMMON_SOURCES
    api/diags_api.cpp
    api/error_api.cpp
    api/instance_api.cpp
    api/link_raw_api.cpp
    api/logging_api.cpp
    api/random_noncrypto_api.cpp
    api/tasklet_api.cpp
    common/binary_search.cpp
    common/error.cpp
    common/frame_builder.cpp
    common/log.cpp
    common/random.cpp
    common/string.cpp
    common/tasklet.cpp
    common/timer.cpp
    common/uptime.cpp
    crypto/aes_ccm.cpp
    crypto/aes_ecb.cpp
    crypto/crypto_platform.cpp
    crypto/storage.cpp
    diags/factory_diags.cpp
    instance/instance.cpp
    mac/link_raw.cpp
    mac/mac_frame.cpp
    mac/mac_header_ie.cpp
    mac/mac_types.cpp
    mac/sub_mac.cpp
    mac/sub_mac_callbacks.cpp
    mac/sub_mac_csl_receiver.cpp
    mac/sub_mac_wed.cpp
    radio/radio.cpp
    radio/radio_callbacks.cpp
    radio/radio_platform.cpp
    thread/link_quality.cpp
    utils/parse_cmdline.cpp
    utils/power_calibration.cpp
)

set(OT_VENDOR_EXTENSION "" CACHE STRING "specify a C++ source file built as part of OpenThread core library")
if(OT_VENDOR_EXTENSION)
  target_compile_definitions(ot-config INTERFACE "OPENTHREAD_ENABLE_VENDOR_EXTENSION=1")
  list(APPEND COMMON_SOURCES ${OT_VENDOR_EXTENSION})
endif()

if(OT_FTD)
    include(ftd.cmake)
endif()

if(OT_MTD)
    include(mtd.cmake)
endif()

if(OT_RCP)
    include(radio.cmake)
    if (OT_APP_CLI)
        include(radio_cli.cmake)
    endif()
endif()

set_property(SOURCE api/instance_api.cpp
    APPEND PROPERTY COMPILE_DEFINITIONS "PACKAGE_VERSION=\"${OT_PACKAGE_VERSION}\""
)
