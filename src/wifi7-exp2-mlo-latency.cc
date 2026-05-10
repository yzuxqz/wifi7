#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/eht-phy.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/neighbor-cache-helper.h"
#include "ns3/network-module.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-static-setup-helper.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wifi7Exp2MloLatency");

struct ForegroundStats
{
    double throughputMbps{0.0};
    double avgDelayMs{0.0};
    double avgJitterMs{0.0};
    double p99DelayMs{0.0};
    double lossRate{0.0};
    uint64_t txPackets{0};
    uint64_t rxPackets{0};
};

static double
GetPercentileFromHistogram(const Histogram& histogram, double percentile)
{
    uint64_t totalCount = 0;
    for (uint32_t bin = 0; bin < histogram.GetNBins(); ++bin)
    {
        totalCount += histogram.GetBinCount(bin);
    }

    if (totalCount == 0)
    {
        return 0.0;
    }

    const double target = percentile * static_cast<double>(totalCount);
    double cumulative = 0.0;
    for (uint32_t bin = 0; bin < histogram.GetNBins(); ++bin)
    {
        cumulative += histogram.GetBinCount(bin);
        if (cumulative >= target)
        {
            return histogram.GetBinStart(bin);
        }
    }
    return histogram.GetBinStart(histogram.GetNBins() - 1);
}

static ForegroundStats
CollectForegroundStats(FlowMonitorHelper& helper,
                       const Ptr<FlowMonitor>& monitor,
                       uint16_t foregroundPort)
{
    monitor->CheckForLostPackets();
    ForegroundStats summary;
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(helper.GetClassifier());

    for (const auto& [flowId, st] : monitor->GetFlowStats())
    {
        Ipv4FlowClassifier::FiveTuple tuple = classifier->FindFlow(flowId);
        if (tuple.protocol != 17 || tuple.destinationPort != foregroundPort)
        {
            continue;
        }

        summary.txPackets += st.txPackets;
        summary.rxPackets += st.rxPackets;

        if (st.timeLastRxPacket > st.timeFirstTxPacket)
        {
            summary.throughputMbps +=
                (st.rxBytes * 8.0) / (st.timeLastRxPacket - st.timeFirstTxPacket).GetSeconds() /
                1e6;
        }

        if (st.rxPackets > 0)
        {
            summary.avgDelayMs += (st.delaySum.GetSeconds() * 1000.0) / st.rxPackets;
            summary.avgJitterMs += (st.jitterSum.GetSeconds() * 1000.0) / st.rxPackets;
            summary.p99DelayMs =
                std::max(summary.p99DelayMs,
                         GetPercentileFromHistogram(st.delayHistogram, 0.99) * 1000.0);
        }
    }

    if (summary.txPackets > 0)
    {
        summary.lossRate =
            static_cast<double>(summary.txPackets - summary.rxPackets) / summary.txPackets;
    }
    return summary;
}

static void
InstallPositions(const NodeContainer& nodes, const std::vector<Vector>& positions)
{
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
    for (const auto& pos : positions)
    {
        allocator->Add(pos);
    }
    mobility.SetPositionAllocator(allocator);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
}

int
main(int argc, char* argv[])
{
    bool useMlo = true;
    bool useBackground = true;
    double simulationTime = 10.0;
    double appStart = 1.0;
    double distance = 3.0;
    uint32_t foregroundPacketSize = 200;
    double foregroundIntervalMs = 1.0;
    std::string backgroundRate = "600Mbps";
    uint32_t backgroundPacketSize = 1200;
    uint16_t foregroundPort = 6000;
    uint16_t backgroundPort = 7000;
    std::string csvFile;

    CommandLine cmd(__FILE__);
    cmd.AddValue("useMlo", "true 为 Wi-Fi 7 EMLSR/MLO，false 为 Wi-Fi 6 单链路", useMlo);
    cmd.AddValue("useBackground", "是否启用 5 GHz 背景干扰流", useBackground);
    cmd.AddValue("simulationTime", "仿真时长（秒）", simulationTime);
    cmd.AddValue("distance", "AP 与前景 STA 间距（米）", distance);
    cmd.AddValue("foregroundPacketSize", "前景小包大小（字节）", foregroundPacketSize);
    cmd.AddValue("foregroundIntervalMs", "前景发包间隔（毫秒）", foregroundIntervalMs);
    cmd.AddValue("backgroundRate", "背景干扰流速率", backgroundRate);
    cmd.AddValue("backgroundPacketSize", "背景流包大小（字节）", backgroundPacketSize);
    cmd.AddValue("csvFile", "可选 CSV 输出路径", csvFile);
    cmd.Parse(argc, argv);

    FlowMonitorHelper flowHelper;
    flowHelper.SetMonitorAttribute("DelayBinWidth", DoubleValue(0.0001));
    flowHelper.SetMonitorAttribute("JitterBinWidth", DoubleValue(0.0001));

    Ptr<MultiModelSpectrumChannel> link0Channel = CreateObject<MultiModelSpectrumChannel>();
    Ptr<LogDistancePropagationLossModel> link0Loss = CreateObject<LogDistancePropagationLossModel>();
    link0Channel->AddPropagationLossModel(link0Loss);

    Ptr<MultiModelSpectrumChannel> link1Channel = CreateObject<MultiModelSpectrumChannel>();
    Ptr<LogDistancePropagationLossModel> link1Loss = CreateObject<LogDistancePropagationLossModel>();
    link1Channel->AddPropagationLossModel(link1Loss);

    NodeContainer staNode;
    NodeContainer apNode;
    staNode.Create(1);
    apNode.Create(1);

    NetDeviceContainer staDevices;
    NetDeviceContainer apDevices;

    const Ssid ssid = Ssid(useMlo ? "wifi7-exp2-eht" : "wifi7-exp2-ax");

    if (useMlo)
    {
        WifiHelper wifi;
        wifi.SetStandard(WIFI_STANDARD_80211be);
        wifi.ConfigEhtOptions("EmlsrActivated", BooleanValue(true));

        wifi.SetRemoteStationManager(static_cast<uint8_t>(0),
                                     "ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     StringValue("EhtMcs13"),
                                     "ControlMode",
                                     StringValue("OfdmRate54Mbps"));
        wifi.SetRemoteStationManager(static_cast<uint8_t>(1),
                                     "ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     StringValue("EhtMcs13"),
                                     "ControlMode",
                                     StringValue("EhtMcs13"));

        WifiMacHelper mac;
        SpectrumWifiPhyHelper phy(2);
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.Set(0, "ChannelSettings", StringValue("{0, 80, BAND_5GHZ, 0}"));
        phy.Set(1, "ChannelSettings", StringValue("{0, 160, BAND_6GHZ, 0}"));
        phy.AddChannel(link0Channel, WIFI_SPECTRUM_5_GHZ);
        phy.AddChannel(link1Channel, WIFI_SPECTRUM_6_GHZ);

        mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
        mac.SetEmlsrManager("ns3::DefaultEmlsrManager",
                            "EmlsrLinkSet",
                            StringValue("0,1"),
                            "EmlsrPaddingDelay",
                            TimeValue(MicroSeconds(32)),
                            "EmlsrTransitionDelay",
                            TimeValue(MicroSeconds(128)),
                            "SwitchAuxPhy",
                            BooleanValue(true),
                            "AuxPhyTxCapable",
                            BooleanValue(true),
                            "AuxPhyChannelWidth",
                            UintegerValue(20));
        staDevices = wifi.Install(phy, mac, staNode);

        mac.SetType("ns3::ApWifiMac",
                    "EnableBeaconJitter",
                    BooleanValue(false),
                    "BeaconGeneration",
                    BooleanValue(false),
                    "Ssid",
                    SsidValue(ssid));
        apDevices = wifi.Install(phy, mac, apNode);

        Config::Set(
            "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
            TimeValue(NanoSeconds(800)));
        Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MpduBufferSize",
                    UintegerValue(512));
    }
    else
    {
        WifiHelper wifi;
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     StringValue("HeMcs11"),
                                     "ControlMode",
                                     StringValue("OfdmRate54Mbps"));

        WifiMacHelper mac;
        SpectrumWifiPhyHelper phy(1);
        phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
        phy.Set(0, "ChannelSettings", StringValue("{0, 160, BAND_5GHZ, 0}"));
        phy.AddChannel(link0Channel, WIFI_SPECTRUM_5_GHZ);

        mac.SetType("ns3::StaWifiMac",
                    "Ssid",
                    SsidValue(ssid),
                    "ActiveProbing",
                    BooleanValue(false));
        staDevices = wifi.Install(phy, mac, staNode);

        mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
        apDevices = wifi.Install(phy, mac, apNode);

        Config::Set(
            "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
            TimeValue(NanoSeconds(800)));
    }

    InstallPositions(apNode, {Vector(0.0, 0.0, 0.0)});
    InstallPositions(staNode, {Vector(distance, 0.0, 0.0)});

    NodeContainer bgSta;
    NodeContainer bgAp;
    NetDeviceContainer bgStaDevices;
    NetDeviceContainer bgApDevices;

    if (useBackground)
    {
        bgSta.Create(1);
        bgAp.Create(1);

        WifiHelper bgWifi;
        bgWifi.SetStandard(WIFI_STANDARD_80211ax);
        bgWifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                       "DataMode",
                                       StringValue("HeMcs11"),
                                       "ControlMode",
                                       StringValue("OfdmRate54Mbps"));

        WifiMacHelper bgMac;
        SpectrumWifiPhyHelper bgPhy(1);
        bgPhy.Set(0,
                  "ChannelSettings",
                  StringValue(useMlo ? "{0, 80, BAND_5GHZ, 0}" : "{0, 160, BAND_5GHZ, 0}"));
        bgPhy.AddChannel(link0Channel, WIFI_SPECTRUM_5_GHZ);

        const Ssid bgSsid = Ssid("wifi7-exp2-bg");
        bgMac.SetType("ns3::StaWifiMac",
                      "Ssid",
                      SsidValue(bgSsid),
                      "ActiveProbing",
                      BooleanValue(false));
        bgStaDevices = bgWifi.Install(bgPhy, bgMac, bgSta);

        bgMac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(bgSsid));
        bgApDevices = bgWifi.Install(bgPhy, bgMac, bgAp);

        InstallPositions(bgAp, {Vector(1.5, 2.0, 0.0)});
        InstallPositions(bgSta, {Vector(distance + 1.0, 2.0, 0.0)});
    }

    if (useMlo)
    {
        auto apDev = DynamicCast<WifiNetDevice>(apDevices.Get(0));
        NS_ASSERT(apDev);
        WifiStaticSetupHelper::SetStaticAssociation(apDev, staDevices);
        WifiStaticSetupHelper::SetStaticEmlsr(apDev, staDevices);
        WifiStaticSetupHelper::SetStaticBlockAck(apDev, staDevices, {0});
    }

    InternetStackHelper stack;
    stack.Install(apNode);
    stack.Install(staNode);
    if (useBackground)
    {
        stack.Install(bgAp);
        stack.Install(bgSta);
    }

    Ipv4AddressHelper address;
    address.SetBase("10.2.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staIf = address.Assign(staDevices);
    Ipv4InterfaceContainer apIf = address.Assign(apDevices);

    Ipv4InterfaceContainer bgStaIf;
    Ipv4InterfaceContainer bgApIf;
    if (useBackground)
    {
        address.SetBase("10.2.2.0", "255.255.255.0");
        bgStaIf = address.Assign(bgStaDevices);
        bgApIf = address.Assign(bgApDevices);
    }

    NeighborCacheHelper nbCache;
    nbCache.PopulateNeighborCache();

    UdpServerHelper fgServer(foregroundPort);
    ApplicationContainer fgServerApp = fgServer.Install(apNode.Get(0));
    fgServerApp.Start(Seconds(0.0));
    fgServerApp.Stop(Seconds(appStart + simulationTime));

    UdpClientHelper fgClient(apIf.GetAddress(0), foregroundPort);
    fgClient.SetAttribute("MaxPackets", UintegerValue(0xffffffffu));
    fgClient.SetAttribute("Interval", TimeValue(MilliSeconds(foregroundIntervalMs)));
    fgClient.SetAttribute("PacketSize", UintegerValue(foregroundPacketSize));
    ApplicationContainer fgClientApp = fgClient.Install(staNode.Get(0));
    fgClientApp.Start(Seconds(appStart));
    fgClientApp.Stop(Seconds(appStart + simulationTime));

    ApplicationContainer bgApps;
    if (useBackground)
    {
        PacketSinkHelper bgSink("ns3::UdpSocketFactory",
                                InetSocketAddress(bgApIf.GetAddress(0), backgroundPort));
        ApplicationContainer bgSinkApp = bgSink.Install(bgAp.Get(0));
        bgSinkApp.Start(Seconds(0.0));
        bgSinkApp.Stop(Seconds(appStart + simulationTime));

        OnOffHelper bgOnOff("ns3::UdpSocketFactory",
                            InetSocketAddress(bgApIf.GetAddress(0), backgroundPort));
        bgOnOff.SetAttribute("PacketSize", UintegerValue(backgroundPacketSize));
        bgOnOff.SetAttribute("DataRate", DataRateValue(DataRate(backgroundRate)));
        bgOnOff.SetAttribute("OnTime",
                             StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        bgOnOff.SetAttribute("OffTime",
                             StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        bgApps = bgOnOff.Install(bgSta.Get(0));
        bgApps.Start(Seconds(0.5));
        bgApps.Stop(Seconds(appStart + simulationTime));
    }

    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(appStart + simulationTime));
    Simulator::Run();

    ForegroundStats stats = CollectForegroundStats(flowHelper, monitor, foregroundPort);
    Simulator::Destroy();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "scenario,standard,mode,distance_m,background_rate,foreground_interval_ms,"
                 "throughput_mbps,avg_delay_ms,p99_delay_ms,avg_jitter_ms,loss_rate,"
                 "tx_packets,rx_packets"
              << std::endl;
    std::cout << "exp2," << (useMlo ? "80211be" : "80211ax") << ","
              << (useMlo ? "emlsr_5g80_6g160" : "single_5g160") << "," << distance << ","
              << (useBackground ? backgroundRate : "0bps") << "," << foregroundIntervalMs << ","
              << stats.throughputMbps << "," << stats.avgDelayMs << "," << stats.p99DelayMs << ","
              << stats.avgJitterMs << "," << stats.lossRate << "," << stats.txPackets << ","
              << stats.rxPackets << std::endl;

    if (!csvFile.empty())
    {
        std::ofstream out(csvFile, std::ios::out | std::ios::trunc);
        out << "scenario,standard,mode,distance_m,background_rate,foreground_interval_ms,"
               "throughput_mbps,avg_delay_ms,p99_delay_ms,avg_jitter_ms,loss_rate,"
               "tx_packets,rx_packets\n";
        out << "exp2," << (useMlo ? "80211be" : "80211ax") << ","
            << (useMlo ? "emlsr_5g80_6g160" : "single_5g160") << "," << distance << ","
            << (useBackground ? backgroundRate : "0bps") << "," << foregroundIntervalMs << ","
            << stats.throughputMbps << "," << stats.avgDelayMs << "," << stats.p99DelayMs << ","
            << stats.avgJitterMs << "," << stats.lossRate << "," << stats.txPackets << ","
            << stats.rxPackets << "\n";
    }

    return 0;
}
