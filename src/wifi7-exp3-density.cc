#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/neighbor-cache-helper.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-static-setup-helper.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wifi7Exp3Density");

struct AggregateStats
{
    double aggregateThroughputMbps{0.0};
    double avgDelayMs{0.0};
    double avgJitterMs{0.0};
    double lossRate{0.0};
    double fairnessIndex{0.0};
    uint64_t txPackets{0};
    uint64_t rxPackets{0};
};

static double
ComputeJainFairness(const std::vector<double>& values)
{
    if (values.empty())
    {
        return 0.0;
    }

    const double sum = std::accumulate(values.begin(), values.end(), 0.0);
    double squaredSum = 0.0;
    for (double value : values)
    {
        squaredSum += value * value;
    }

    if (squaredSum == 0.0)
    {
        return 0.0;
    }
    return (sum * sum) / (values.size() * squaredSum);
}

static void
ConfigureDlMuAckType(const std::string& dlAckType)
{
    if (dlAckType == "NO-OFDMA")
    {
        return;
    }
    if (dlAckType == "ACK-SU-FORMAT")
    {
        Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                           EnumValue(WifiAcknowledgment::DL_MU_BAR_BA_SEQUENCE));
        return;
    }
    if (dlAckType == "MU-BAR")
    {
        Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                           EnumValue(WifiAcknowledgment::DL_MU_TF_MU_BAR));
        return;
    }
    if (dlAckType == "AGGR-MU-BAR")
    {
        Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                           EnumValue(WifiAcknowledgment::DL_MU_AGGREGATE_TF));
        return;
    }

    NS_ABORT_MSG("Invalid DL ack sequence type");
}

int
main(int argc, char* argv[])
{
    bool useWifi7 = true;
    uint32_t nStations = 16;
    double simulationTime = 0.2;
    double distance = 1.0;
    uint32_t payloadSize = 1200;
    uint32_t guardIntervalNs = 800;
    int mcs = -1;
    bool enableMuScheduler = true;
    uint32_t muStations = 8;
    bool forceDlOfdma = true;
    bool enableUlOfdma = false;
    bool enableBsrp = false;
    bool useCentral26TonesRus = false;
    std::string dlAckType = "AGGR-MU-BAR";
    std::string csvFile;

    CommandLine cmd(__FILE__);
    cmd.AddValue("useWifi7", "true 为 802.11be，false 为 802.11ax", useWifi7);
    cmd.AddValue("nStations", "STA 数量，建议 8/16/32", nStations);
    cmd.AddValue("simulationTime", "业务仿真时长（秒）", simulationTime);
    cmd.AddValue("distance", "AP 到 STA 的半径距离（米）", distance);
    cmd.AddValue("payloadSize", "UDP 负载字节数", payloadSize);
    cmd.AddValue("guardIntervalNs", "GI（纳秒）", guardIntervalNs);
    cmd.AddValue("mcs", "固定 MCS，默认 ax=7，be=9", mcs);
    cmd.AddValue("enableMuScheduler", "是否启用 AP 端 MU/OFDMA 调度器", enableMuScheduler);
    cmd.AddValue("muStations", "单次 DL MU OFDMA 最多同时调度的 STA 数", muStations);
    cmd.AddValue("forceDlOfdma", "即使收益不高也强制尝试 DL OFDMA", forceDlOfdma);
    cmd.AddValue("enableUlOfdma", "是否在下行后继续触发上行 OFDMA", enableUlOfdma);
    cmd.AddValue("enableBsrp", "是否在上行 OFDMA 前发送 BSRP Trigger", enableBsrp);
    cmd.AddValue("useCentral26TonesRus", "是否允许分配 central 26-tone RU", useCentral26TonesRus);
    cmd.AddValue("dlAckType",
                 "DL OFDMA ACK 序列类型: NO-OFDMA/ACK-SU-FORMAT/MU-BAR/AGGR-MU-BAR",
                 dlAckType);
    cmd.AddValue("csvFile", "可选 CSV 输出路径", csvFile);
    cmd.Parse(argc, argv);

    ConfigureDlMuAckType(dlAckType);

    const WifiStandard standard = useWifi7 ? WIFI_STANDARD_80211be : WIFI_STANDARD_80211ax;
    const uint32_t channelWidth = useWifi7 ? 160 : 80;
    const std::string band = useWifi7 ? "BAND_6GHZ" : "BAND_5GHZ";
    const std::string channelSettings =
        "{0, " + std::to_string(channelWidth) + ", " + band + ", 0}";
    const int mcsValue = (mcs >= 0) ? mcs : (useWifi7 ? 9 : 7);
    const std::string mode = std::string(useWifi7 ? "EhtMcs" : "HeMcs") + std::to_string(mcsValue);
    const auto nonHtRefRateMbps =
        static_cast<uint32_t>(std::llround(HePhy::GetNonHtReferenceRate(mcsValue) / 1e6));
    const std::string controlMode =
        useWifi7 ? mode : ("OfdmRate" + std::to_string(nonHtRefRateMbps) + "Mbps");

    NodeContainer staNodes;
    NodeContainer apNode;
    staNodes.Create(nStations);
    apNode.Create(1);

    WifiHelper wifi;
    wifi.SetStandard(standard);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(mode),
                                 "ControlMode",
                                 StringValue(controlMode));

    auto spectrumChannel = CreateObject<MultiModelSpectrumChannel>();
    auto lossModel = CreateObject<LogDistancePropagationLossModel>();
    spectrumChannel->AddPropagationLossModel(lossModel);

    SpectrumWifiPhyHelper phy;
    phy.SetChannel(spectrumChannel);
    phy.SetErrorRateModel("ns3::TableBasedErrorRateModel");
    phy.Set("ChannelSettings", StringValue(channelSettings));

    WifiMacHelper mac;
    Ssid ssid = Ssid(useWifi7 ? "wifi7-exp3-eht" : "wifi7-exp3-he");

    mac.SetType("ns3::StaWifiMac",
                "Ssid",
                SsidValue(ssid),
                "ActiveProbing",
                BooleanValue(false),
                "MpduBufferSize",
                UintegerValue(64));
    NetDeviceContainer staDevices = wifi.Install(phy, mac, staNodes);

    if (enableMuScheduler && dlAckType != "NO-OFDMA")
    {
        mac.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                                  "NStations",
                                  UintegerValue(muStations),
                                  "ForceDlOfdma",
                                  BooleanValue(forceDlOfdma),
                                  "EnableUlOfdma",
                                  BooleanValue(enableUlOfdma),
                                  "EnableBsrp",
                                  BooleanValue(enableBsrp),
                                  "UseCentral26TonesRus",
                                  BooleanValue(useCentral26TonesRus));
    }
    mac.SetType("ns3::ApWifiMac",
                "EnableBeaconJitter",
                BooleanValue(false),
                "BeaconGeneration",
                BooleanValue(false),
                "Ssid",
                SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(phy, mac, apNode);

    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
                TimeValue(NanoSeconds(guardIntervalNs)));

    Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
    allocator->Add(Vector(0.0, 0.0, 0.0));
    for (uint32_t i = 0; i < nStations; ++i)
    {
        const double angle = (2.0 * 3.14159265358979323846 * i) / nStations;
        allocator->Add(Vector(distance * std::cos(angle), distance * std::sin(angle), 0.0));
    }

    MobilityHelper mobility;
    mobility.SetPositionAllocator(allocator);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNode);
    mobility.Install(staNodes);

    auto apWifiDev = DynamicCast<WifiNetDevice>(apDevice.Get(0));
    NS_ABORT_MSG_IF(!apWifiDev, "AP device is not a WifiNetDevice");
    WifiStaticSetupHelper::SetStaticAssociation(apWifiDev, staDevices);
    WifiStaticSetupHelper::SetStaticBlockAck(apWifiDev, staDevices, {0});

    InternetStackHelper stack;
    stack.Install(apNode);
    stack.Install(staNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.3.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staIf = address.Assign(staDevices);
    address.Assign(apDevice);

    NeighborCacheHelper neighborCache;
    neighborCache.PopulateNeighborCache();

    ApplicationContainer serverApps;
    ApplicationContainer clientApps;
    UdpServerHelper server(9);
    serverApps = server.Install(staNodes);
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(simulationTime + 0.001));

    const uint64_t dataRatePerStream =
        useWifi7 ? EhtPhy::GetDataRate(mcsValue,
                                       MHz_u{static_cast<double>(channelWidth)},
                                       NanoSeconds(guardIntervalNs),
                                       1)
                 : HePhy::GetDataRate(mcsValue,
                                      MHz_u{static_cast<double>(channelWidth)},
                                      NanoSeconds(guardIntervalNs),
                                      1);
    const double packetIntervalSeconds =
        (payloadSize * 8.0 * nStations) / static_cast<double>(dataRatePerStream);

    for (uint32_t i = 0; i < nStations; ++i)
    {
        UdpClientHelper client(staIf.GetAddress(i), 9);
        client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
        client.SetAttribute("Interval", TimeValue(Seconds(packetIntervalSeconds)));
        client.SetAttribute("PacketSize", UintegerValue(payloadSize));
        ApplicationContainer app = client.Install(apNode.Get(0));
        app.Start(Seconds(0.001));
        app.Stop(Seconds(simulationTime + 0.001));
        clientApps.Add(app);
    }

    Simulator::Schedule(Seconds(0), &Ipv4GlobalRoutingHelper::PopulateRoutingTables);
    Simulator::Stop(Seconds(simulationTime + 0.001));
    Simulator::Run();

    AggregateStats stats;
    std::vector<double> perStaThroughput;
    for (uint32_t i = 0; i < nStations; ++i)
    {
        auto udpServer = DynamicCast<UdpServer>(serverApps.Get(i));
        auto udpClient = DynamicCast<UdpClient>(clientApps.Get(i));
        NS_ABORT_MSG_IF(!udpServer || !udpClient, "Failed to cast UDP applications");

        const uint64_t rxPackets = udpServer->GetReceived();
        const uint64_t txBytes = udpClient->GetTotalTx();
        const uint64_t txPackets = payloadSize > 0 ? txBytes / payloadSize : 0;
        const double throughputMbps = (rxPackets * payloadSize * 8.0) / (simulationTime * 1e6);

        stats.rxPackets += rxPackets;
        stats.txPackets += txPackets;
        stats.aggregateThroughputMbps += throughputMbps;
        perStaThroughput.push_back(throughputMbps);
    }
    Simulator::Destroy();

    stats.fairnessIndex = ComputeJainFairness(perStaThroughput);
    if (stats.txPackets > 0)
    {
        stats.lossRate = static_cast<double>(stats.txPackets - stats.rxPackets) / stats.txPackets;
    }

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "scenario,standard,n_stations,channel_width_mhz,aggregate_throughput_mbps,"
                 "fairness_index,avg_delay_ms,avg_jitter_ms,loss_rate,tx_packets,rx_packets"
              << std::endl;
    std::cout << "exp3," << (useWifi7 ? "80211be" : "80211ax") << "," << nStations << ","
              << channelWidth << "," << stats.aggregateThroughputMbps << ","
              << stats.fairnessIndex << "," << stats.avgDelayMs << "," << stats.avgJitterMs << ","
              << stats.lossRate << "," << stats.txPackets << "," << stats.rxPackets << std::endl;

    if (!csvFile.empty())
    {
        std::ofstream out(csvFile, std::ios::out | std::ios::trunc);
        out << "scenario,standard,n_stations,channel_width_mhz,aggregate_throughput_mbps,"
               "fairness_index,avg_delay_ms,avg_jitter_ms,loss_rate,tx_packets,rx_packets\n";
        out << "exp3," << (useWifi7 ? "80211be" : "80211ax") << "," << nStations << ","
            << channelWidth << "," << stats.aggregateThroughputMbps << "," << stats.fairnessIndex
            << "," << stats.avgDelayMs << "," << stats.avgJitterMs << "," << stats.lossRate << ","
            << stats.txPackets << "," << stats.rxPackets << "\n";
    }

    return 0;
}
