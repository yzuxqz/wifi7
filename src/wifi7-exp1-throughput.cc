#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/wifi-module.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Wifi7Exp1Throughput");

struct FlowSummary
{
    double throughputMbps{0.0};
    double avgDelayMs{0.0};
    double avgJitterMs{0.0};
    double lossRate{0.0};
    uint64_t txPackets{0};
    uint64_t rxPackets{0};
};

static FlowSummary
CollectSingleFlowStats(FlowMonitorHelper& helper, const Ptr<FlowMonitor>& monitor)
{
    monitor->CheckForLostPackets();
    FlowSummary summary;

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(helper.GetClassifier());
    auto stats = monitor->GetFlowStats();

    for (const auto& [flowId, st] : stats)
    {
        auto tuple = classifier->FindFlow(flowId);
        if (tuple.protocol != 17)
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
        }
    }

    if (summary.txPackets > 0)
    {
        summary.lossRate =
            static_cast<double>(summary.txPackets - summary.rxPackets) / summary.txPackets;
    }
    return summary;
}

int
main(int argc, char* argv[])
{
    bool useWifi7 = true;
    double distance = 1.0;
    double simulationTime = 10.0;
    double appStart = 1.0;
    uint32_t payloadSize = 1400;
    uint16_t port = 5000;
    uint8_t nAntennas = 2;
    uint8_t nStreams = 2;
    int channelWidth = -1;
    int mcs = -1;
    uint32_t giNs = 800;
    std::string offeredLoad;
    std::string csvFile;

    CommandLine cmd(__FILE__);
    cmd.AddValue("useWifi7", "true 为 802.11be，false 为 802.11ax", useWifi7);
    cmd.AddValue("distance", "AP 与 STA 距离（米）", distance);
    cmd.AddValue("simulationTime", "仿真时长（秒）", simulationTime);
    cmd.AddValue("payloadSize", "UDP 负载大小（字节）", payloadSize);
    cmd.AddValue("channelWidth", "信道带宽（MHz），默认 ax=160，be=320", channelWidth);
    cmd.AddValue("mcs", "固定 MCS，默认 ax=11，be=13", mcs);
    cmd.AddValue("giNs", "Guard interval（ns）", giNs);
    cmd.AddValue("offeredLoad", "业务速率，例如 1500Mbps、3Gbps", offeredLoad);
    cmd.AddValue("csvFile", "可选 CSV 输出路径", csvFile);
    cmd.Parse(argc, argv);

    const WifiStandard standard =
        useWifi7 ? WIFI_STANDARD_80211be : WIFI_STANDARD_80211ax;
    const int widthMHz = (channelWidth > 0) ? channelWidth : (useWifi7 ? 320 : 160);
    const int mcsValue = (mcs >= 0) ? mcs : (useWifi7 ? 13 : 11);
    const std::string ssidName = useWifi7 ? "wifi7-exp1-eht" : "wifi7-exp1-he";
    const std::string mode = std::string(useWifi7 ? "EhtMcs" : "HeMcs") + std::to_string(mcsValue);
    const std::string controlMode = useWifi7 ? mode : "OfdmRate54Mbps";
    const std::string band = useWifi7 ? "BAND_6GHZ" : "BAND_5GHZ";
    const std::string channelSettings =
        "{0, " + std::to_string(widthMHz) + ", " + band + ", 0}";
    const std::string load = !offeredLoad.empty() ? offeredLoad : (useWifi7 ? "4Gbps" : "2Gbps");

    NodeContainer staNode;
    NodeContainer apNode;
    staNode.Create(1);
    apNode.Create(1);

    YansWifiChannelHelper channel;
    channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss("ns3::FriisPropagationLossModel");

    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());
    phy.SetErrorRateModel("ns3::TableBasedErrorRateModel");
    phy.Set("ChannelSettings", StringValue(channelSettings));
    phy.Set("Antennas", UintegerValue(nAntennas));
    phy.Set("MaxSupportedTxSpatialStreams", UintegerValue(nStreams));
    phy.Set("MaxSupportedRxSpatialStreams", UintegerValue(nStreams));
    phy.Set("TxPowerStart", DoubleValue(20.0));
    phy.Set("TxPowerEnd", DoubleValue(20.0));

    WifiHelper wifi;
    wifi.SetStandard(standard);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(mode),
                                 "ControlMode",
                                 StringValue(controlMode));

    WifiMacHelper mac;
    Ssid ssid = Ssid(ssidName);

    mac.SetType("ns3::StaWifiMac",
                "Ssid",
                SsidValue(ssid),
                "ActiveProbing",
                BooleanValue(false));
    NetDeviceContainer staDevice = wifi.Install(phy, mac, staNode);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevice = wifi.Install(phy, mac, apNode);

    Config::Set("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/HeConfiguration/GuardInterval",
                TimeValue(NanoSeconds(giNs)));

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    positionAlloc->Add(Vector(distance, 0.0, 0.0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(apNode);
    mobility.Install(staNode);

    InternetStackHelper stack;
    stack.Install(apNode);
    stack.Install(staNode);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer staIf = address.Assign(staDevice);
    Ipv4InterfaceContainer apIf = address.Assign(apDevice);

    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                InetSocketAddress(apIf.GetAddress(0), port));
    ApplicationContainer sinkApp = sinkHelper.Install(apNode.Get(0));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simulationTime + appStart));

    OnOffHelper onoff("ns3::UdpSocketFactory", Address());
    onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(apIf.GetAddress(0), port)));
    onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
    onoff.SetAttribute("DataRate", DataRateValue(DataRate(load)));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    ApplicationContainer clientApp = onoff.Install(staNode.Get(0));
    clientApp.Start(Seconds(appStart));
    clientApp.Stop(Seconds(simulationTime + appStart));

    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    std::cerr << "exp1-start standard=" << (useWifi7 ? "80211be" : "80211ax")
              << " width=" << widthMHz << " mcs=" << mcsValue << std::endl;
    Simulator::Stop(Seconds(simulationTime + appStart));
    Simulator::Run();
    std::cerr << "exp1-after-run" << std::endl;

    FlowSummary summary = CollectSingleFlowStats(flowHelper, monitor);

    Simulator::Destroy();
    std::cerr << "exp1-after-destroy" << std::endl;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "scenario,standard,distance_m,channel_width_mhz,mcs,throughput_mbps,"
                 "avg_delay_ms,avg_jitter_ms,loss_rate,tx_packets,rx_packets"
              << std::endl;
    std::cout << "exp1," << (useWifi7 ? "80211be" : "80211ax") << "," << distance << ","
              << widthMHz << "," << mcsValue << "," << summary.throughputMbps << ","
              << summary.avgDelayMs << "," << summary.avgJitterMs << "," << summary.lossRate
              << "," << summary.txPackets << "," << summary.rxPackets << std::endl;

    if (!csvFile.empty())
    {
        std::ofstream out(csvFile, std::ios::out | std::ios::trunc);
        out << "scenario,standard,distance_m,channel_width_mhz,mcs,throughput_mbps,"
               "avg_delay_ms,avg_jitter_ms,loss_rate,tx_packets,rx_packets\n";
        out << "exp1," << (useWifi7 ? "80211be" : "80211ax") << "," << distance << "," << widthMHz
            << "," << mcsValue << "," << summary.throughputMbps << "," << summary.avgDelayMs << ","
            << summary.avgJitterMs << "," << summary.lossRate << "," << summary.txPackets << ","
            << summary.rxPackets << "\n";
        std::cerr << "exp1-wrote-csv " << csvFile << std::endl;
    }

    return 0;
}
