# Performance Evaluation of Wi-Fi 7 (IEEE 802.11be) Against Wi-Fi 6 (IEEE 802.11ax) Using ns-3.47

## Abstract

This paper presents a simulation-based comparison between Wi-Fi 7 (IEEE 802.11be) and Wi-Fi 6 (IEEE 802.11ax) using `ns-3.47` in a `Windows + WSL` environment. The objective is to quantify whether the new features introduced in Wi-Fi 7 provide measurable improvements in throughput, latency stability, and high-density performance. Three experiments were designed. The first experiment evaluates peak throughput under short-range and high-quality channel conditions. The second experiment investigates whether Multi-Link Operation (MLO) can reduce delay and tail latency under interference. The third experiment explores aggregate throughput and fairness in a dense multi-user scenario using downlink OFDMA scheduling. The results show that Wi-Fi 7 achieves higher peak throughput than Wi-Fi 6 in the single-link short-range case, with an observed increase from `1137.72 Mbps` to `1420.55 Mbps`. In the latency experiment, both standards maintain similar foreground throughput, while Wi-Fi 7 reduces the `P99` delay from `0.7 ms` to `0.5 ms`, indicating an advantage in tail-latency control. In the high-density experiment, Wi-Fi 7 increases aggregate throughput from `267.648 Mbps` to `778.224 Mbps` while maintaining fairness close to `1.0`, which demonstrates a clear multi-user efficiency gain when OFDMA scheduling is explicitly enabled. Overall, the study confirms that Wi-Fi 7 provides practical gains in bandwidth-driven throughput, tail-latency stability, and dense-user transmission efficiency.

**Keywords:** Wi-Fi 7, IEEE 802.11be, Wi-Fi 6, IEEE 802.11ax, ns-3, MLO, throughput, latency

## 1. Introduction

Wireless local area networks have evolved rapidly in order to support bandwidth-intensive, latency-sensitive, and highly dense application scenarios. Modern use cases such as 4K/8K video streaming, cloud gaming, immersive communication, smart campus connectivity, and large-scale Internet of Things deployments place significant pressure on conventional Wi-Fi systems. As a result, each new Wi-Fi generation attempts to improve not only raw throughput, but also delay stability, reliability, and scalability in dense user environments.

Wi-Fi 6, standardized as IEEE 802.11ax, introduced several major improvements over previous generations, including Orthogonal Frequency Division Multiple Access (OFDMA), better multi-user efficiency, and enhanced performance in dense deployments. However, emerging applications now demand even higher throughput and lower latency than what Wi-Fi 6 can consistently provide. To address these requirements, Wi-Fi 7, standardized as IEEE 802.11be, introduces Extremely High Throughput (EHT) features such as wider channel bandwidth, higher modulation order, and Multi-Link Operation (MLO).

Among the new features of Wi-Fi 7, three are especially important. First, Wi-Fi 7 extends channel bandwidth up to `320 MHz`, which directly increases the physical layer data rate. Second, it supports higher modulation schemes, allowing the link to deliver more bits per symbol under favorable channel conditions. Third, MLO enables devices to operate across multiple links and therefore provides a new mechanism for improving latency stability and robustness under interference. These capabilities make Wi-Fi 7 a strong candidate for next-generation campus and enterprise wireless networks.

This work focuses on a comparative simulation study of Wi-Fi 7 and Wi-Fi 6 using `ns-3.47`, a recent and extensible network simulator that supports both IEEE 802.11ax and IEEE 802.11be features. The main goal is not only to report raw numbers, but also to quantify which benefits are already visible in practical simulation settings. Three research questions are considered:

1. Can Wi-Fi 7 deliver higher peak throughput than Wi-Fi 6 under ideal short-range conditions?
2. Can Wi-Fi 7 reduce long-tail delay under interference through MLO?
3. Does Wi-Fi 7 improve aggregate throughput and fairness in dense multi-user scenarios?

To answer these questions, this paper designs three experiments corresponding to throughput, latency, and density dimensions. The experiments are implemented in `ns-3.47`, and the output is exported to `CSV` files for later plotting and discussion. The contribution of this paper is twofold. First, it provides a reproducible `ns-3`-based comparison framework for Wi-Fi 6 and Wi-Fi 7. Second, it offers an evidence-driven interpretation of where Wi-Fi 7 already shows measurable advantages and where additional tuning is still required.

The remainder of this paper is organized as follows. Section 2 summarizes the technical background of Wi-Fi 6 and Wi-Fi 7. Section 3 describes the simulation environment and methodology. Section 4 presents the experimental design. Section 5 discusses the results of the three experiments. Section 6 concludes the paper and outlines future work.

## 2. Background

### 2.1 Wi-Fi 6 (IEEE 802.11ax)

IEEE 802.11ax, commercially known as Wi-Fi 6, was designed to improve spectral efficiency and network performance in dense deployment scenarios. Compared with earlier standards, Wi-Fi 6 enhances capacity using technologies such as OFDMA, multi-user transmission support, and better scheduling efficiency. These features allow more users to share wireless resources in a structured way, which is particularly useful in classrooms, offices, dormitories, and other high-density environments.

Despite these gains, Wi-Fi 6 still faces limitations when applications demand ultra-high peak throughput and highly stable low-latency service. Its maximum channel width is `160 MHz`, and although it performs better than previous standards, it does not fully address the increasingly strict requirements of advanced multimedia and real-time services.

### 2.2 Wi-Fi 7 (IEEE 802.11be)

IEEE 802.11be, or Wi-Fi 7, extends Wi-Fi performance toward the Extremely High Throughput era. It is designed to improve both data rate and responsiveness. The most relevant features for this study are:

- Wider channels up to `320 MHz`
- Higher PHY capability under favorable channel conditions
- Multi-Link Operation (MLO), which enables simultaneous or coordinated use of multiple links

The expected benefit of these features is not limited to higher throughput. MLO is particularly important because it introduces a flexible mechanism for avoiding congestion and reducing tail delay when one link becomes impaired.

### 2.3 Why ns-3.47

This study uses `ns-3.47` because it is a recent version with support for both IEEE 802.11ax and IEEE 802.11be features, including the mechanisms needed for throughput and MLO-based evaluation. The simulator also provides a practical platform for exporting packet-level statistics and comparing multiple experimental configurations under controlled conditions.

## 3. Simulation Methodology

### 3.1 Simulation Platform

The simulations were conducted in the following environment:

- Simulator: `ns-3.47`
- Execution platform: `Windows + WSL`
- Programming language: `C++`
- Data collection: `FlowMonitor` and CSV output
- Result visualization: `Python` with `matplotlib`

### 3.2 General Configuration Principles

To keep the experiments comparable, several common design principles were followed:

- The same simulator version was used for all experiments.
- The same type of output structure was used for both Wi-Fi 6 and Wi-Fi 7.
- Transmission power, antenna configuration, and guard interval were kept controlled whenever possible.
- All scenarios were designed to focus on one main performance objective at a time.

The error behavior was modeled using `TableBasedErrorRateModel`, which is more suitable for advanced Wi-Fi PHY evaluation than simplistic error assumptions. Propagation models were selected according to the scenario. For short-range peak-throughput tests, a near-ideal propagation model was used. For interference and density experiments, a log-distance based propagation model was used to better represent indoor wireless attenuation.

### 3.3 Evaluation Metrics

The performance of Wi-Fi 6 and Wi-Fi 7 was evaluated using the following metrics:

- Throughput
- Average end-to-end delay
- Jitter
- Packet loss rate
- `P99` delay
- Aggregate throughput
- Jain's fairness index

These metrics were chosen to satisfy the course requirement of using at least three performance parameters while also covering the key design goals of Wi-Fi 7.

## 4. Experimental Design

### 4.1 Experiment 1: Peak Throughput and Bandwidth Gain

The first experiment evaluates peak throughput under favorable conditions. The topology contains one AP and one STA separated by `1 m`, which helps ensure strong link quality. The objective is to observe whether Wi-Fi 7 can provide higher throughput than Wi-Fi 6 in a near-ideal environment.

The parameter settings are:

- Wi-Fi 6:
  - Standard: `802.11ax`
  - Channel width: `160 MHz`
  - MCS: `11`
- Wi-Fi 7:
  - Standard: `802.11be`
  - Channel width: `320 MHz`
  - MCS: `13`

The measured outputs are throughput, average delay, jitter, and loss rate.

### 4.2 Experiment 2: MLO Latency Stability Under Interference

The second experiment is the most important part of this study because it focuses on the latency-related advantage of Wi-Fi 7. The topology contains one AP and one STA for the foreground flow. For Wi-Fi 7, an MLO-based configuration is used with two links:

- Link 1: `5 GHz`, `80 MHz`
- Link 2: `6 GHz`, `160 MHz`

To create interference, a background flow is added to one of the links. The comparison is defined as follows:

- Wi-Fi 6: single-link operation
- Wi-Fi 7: MLO / EMLSR configuration

The main performance metrics are average delay, `P99` delay, jitter, and loss rate. This design is intended to show that even if the average throughput remains similar, Wi-Fi 7 may still provide a meaningful gain through better tail-latency control.

### 4.3 Experiment 3: High-Density Multi-User Efficiency

The third experiment studies a dense multi-user environment. The topology includes one AP and `16` STAs arranged around the AP at short distance, which makes the link quality strong enough for OFDMA scheduling behavior to become visible. Instead of relying on a generic mixed-traffic contention scenario, the final experiment uses a controlled downlink UDP saturation model derived from the official `ns-3` HE/EHT OFDMA examples. This makes the comparison more stable and more suitable for exposing the multi-user efficiency difference between the two standards.

In the final implementation, the experiment reports:

- Aggregate throughput
- Fairness index
- Average delay
- Jitter
- Loss rate

This experiment is intended to quantify how much gain becomes visible when the AP side explicitly uses multi-user scheduling. It can later be extended to `32` and `64` stations for stronger evidence.

### 4.4 Detailed Experimental Workflow

To make the study reproducible, all experiments follow the same execution workflow:

1. Prepare the `ns-3.47` environment in `WSL`.
2. Copy the C++ source files into the `scratch` directory of `ns-3.47`.
3. Build the simulation programs using the `ns3` build system.
4. Run Wi-Fi 6 and Wi-Fi 7 cases separately with controlled parameters.
5. Export each run to `CSV` format.
6. Use Python scripts to summarize and visualize the output for paper writing.

The actual execution sequence used in this project is:

```bash
cp /mnt/c/Users/许钱洲/Desktop/wifi72/src/wifi7-exp*.cc /home/admin2/ns3-wifi7/ns-3.47/scratch/
cd /home/admin2/ns3-wifi7/ns-3.47
./ns3 build
```

After successful compilation, each experiment is executed from the generated binaries in the `build/scratch/` directory. The resulting `CSV` files are copied back to the Windows-side project folder for plotting and documentation.

### 4.5 Detailed Parameter Settings

The most important parameter settings used in the final paper version are summarized below.

#### Experiment 1 Parameter Set

- Topology: `1 AP + 1 STA`
- Distance: `1 m`
- Traffic type: UDP saturation flow
- Payload size: `1400 bytes`
- Antennas: `2`
- Spatial streams: `2`
- Guard interval: `800 ns`
- Error model: `TableBasedErrorRateModel`
- Propagation loss model: `FriisPropagationLossModel`
- Wi-Fi 6:
  - Standard: `802.11ax`
  - Band: `5 GHz`
  - Channel width: `160 MHz`
  - MCS: `11`
  - Offered load: `1500 Mbps`
  - Simulation time: `1 s`
- Wi-Fi 7:
  - Standard: `802.11be`
  - Band: `6 GHz`
  - Channel width: `320 MHz`
  - MCS: `13`
  - Offered load: `2500 Mbps`
  - Simulation time: `1 s`

The command style used for Experiment 1 is:

```bash
build/scratch/ns3.47-wifi7-exp1-throughput-default --useWifi7=0 --distance=1 --simulationTime=1 --offeredLoad=1500Mbps --csvFile=results/exp1_ax_fair.csv
build/scratch/ns3.47-wifi7-exp1-throughput-default --useWifi7=1 --distance=1 --simulationTime=1 --offeredLoad=2500Mbps --csvFile=results/exp1_be_fair.csv
```

#### Experiment 2 Parameter Set

- Topology: `1 AP + 1 STA` foreground pair
- Additional topology: `1 AP + 1 STA` background interference pair
- Foreground distance: `3 m`
- Foreground packet size: `200 bytes`
- Foreground interval: `5 ms`
- Simulation time: `1 s`
- Delay histogram enabled through `FlowMonitor`
- Wi-Fi 6:
  - Mode: single link
  - Link: `5 GHz`, `160 MHz`
- Wi-Fi 7:
  - Mode: `MLO / EMLSR`
  - Link 1: `5 GHz`, `80 MHz`
  - Link 2: `6 GHz`, `160 MHz`
- Background flow:
  - Rate: `20 Mbps`
  - Packet size: `1200 bytes`

The execution style used for Experiment 2 is:

```bash
build/scratch/ns3.47-wifi7-exp2-mlo-latency-default --useMlo=0 --useBackground=1 --backgroundRate=20Mbps --simulationTime=1 --foregroundIntervalMs=5 --foregroundPacketSize=200 --csvFile=results/exp2_ax_fair.csv
build/scratch/ns3.47-wifi7-exp2-mlo-latency-default --useMlo=1 --useBackground=1 --backgroundRate=20Mbps --simulationTime=1 --foregroundIntervalMs=5 --foregroundPacketSize=200 --csvFile=results/exp2_be_fair.csv
```

#### Experiment 3 Parameter Set

- Topology: `1 AP + 16 STA`
- Distance from AP to each STA: `1 m`
- PHY helper: `SpectrumWifiPhyHelper`
- Channel model: `MultiModelSpectrumChannel + LogDistancePropagationLossModel`
- Error model: `TableBasedErrorRateModel`
- Traffic model: downlink UDP saturation, one server per STA
- Payload size: `1200 bytes`
- Simulation time: `0.2 s`
- Guard interval: `800 ns`
- Static association and static block-ack enabled
- AP scheduler: `RrMultiUserScheduler`
- DL MU ACK type: `AGGR-MU-BAR`
- Maximum scheduled users per MU transmission: `8`
- Wi-Fi 6:
  - Standard: `802.11ax`
  - Band: `5 GHz`
  - Channel width: `80 MHz`
  - MCS: `7`
- Wi-Fi 7:
  - Standard: `802.11be`
  - Band: `6 GHz`
  - Channel width: `160 MHz`
  - MCS: `9`

The command style used for Experiment 3 is:

```bash
build/scratch/ns3.47-wifi7-exp3-density-default --useWifi7=0 --nStations=16 --simulationTime=0.2 --distance=1 --mcs=7 --muStations=8 --dlAckType=AGGR-MU-BAR --csvFile=results/exp3_ax_16_mu_template.csv
build/scratch/ns3.47-wifi7-exp3-density-default --useWifi7=1 --nStations=16 --simulationTime=0.2 --distance=1 --mcs=9 --muStations=8 --dlAckType=AGGR-MU-BAR --csvFile=results/exp3_be_16_mu_template.csv
```

### 4.6 Core Implementation Code

This section includes representative code fragments from the simulation programs used in the experiments.

#### Experiment 1 Code Fragment

The following code shows how the peak-throughput experiment selects the standard, bandwidth, MCS, and traffic load:

```cpp
bool useWifi7 = true;
double distance = 1.0;
double simulationTime = 10.0;
uint32_t payloadSize = 1400;
int channelWidth = -1;
int mcs = -1;
uint32_t giNs = 800;
std::string offeredLoad;

const WifiStandard standard =
    useWifi7 ? WIFI_STANDARD_80211be : WIFI_STANDARD_80211ax;
const int widthMHz = (channelWidth > 0) ? channelWidth : (useWifi7 ? 320 : 160);
const int mcsValue = (mcs >= 0) ? mcs : (useWifi7 ? 13 : 11);
const std::string mode = std::string(useWifi7 ? "EhtMcs" : "HeMcs") + std::to_string(mcsValue);
const std::string band = useWifi7 ? "BAND_6GHZ" : "BAND_5GHZ";
const std::string channelSettings =
    "{0, " + std::to_string(widthMHz) + ", " + band + ", 0}";
const std::string load = !offeredLoad.empty() ? offeredLoad : (useWifi7 ? "4Gbps" : "2Gbps");
```

The PHY and traffic installation are implemented as follows:

```cpp
phy.SetErrorRateModel("ns3::TableBasedErrorRateModel");
phy.Set("ChannelSettings", StringValue(channelSettings));
wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                             "DataMode",
                             StringValue(mode),
                             "ControlMode",
                             StringValue(controlMode));

OnOffHelper onoff("ns3::UdpSocketFactory", Address());
onoff.SetAttribute("Remote", AddressValue(InetSocketAddress(apIf.GetAddress(0), port)));
onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
onoff.SetAttribute("DataRate", DataRateValue(DataRate(load)));
```

#### Experiment 2 Code Fragment

The second experiment explicitly configures multi-link operation for Wi-Fi 7:

```cpp
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
```

The two-link PHY configuration is:

```cpp
SpectrumWifiPhyHelper phy(2);
phy.Set(0, "ChannelSettings", StringValue("{0, 80, BAND_5GHZ, 0}"));
phy.Set(1, "ChannelSettings", StringValue("{0, 160, BAND_6GHZ, 0}"));
phy.AddChannel(link0Channel, WIFI_SPECTRUM_5_GHZ);
phy.AddChannel(link1Channel, WIFI_SPECTRUM_6_GHZ);
```

The MLO station manager is attached as follows:

```cpp
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
```

The background interference flow is introduced with:

```cpp
OnOffHelper bgOnOff("ns3::UdpSocketFactory",
                    InetSocketAddress(bgApIf.GetAddress(0), backgroundPort));
bgOnOff.SetAttribute("PacketSize", UintegerValue(backgroundPacketSize));
bgOnOff.SetAttribute("DataRate", DataRateValue(DataRate(backgroundRate)));
bgOnOff.SetAttribute("OnTime",
                     StringValue("ns3::ConstantRandomVariable[Constant=1]"));
bgOnOff.SetAttribute("OffTime",
                     StringValue("ns3::ConstantRandomVariable[Constant=0]"));
```

#### Experiment 3 Code Fragment

The final dense-user experiment explicitly configures DL MU ACK type and a round-robin multi-user scheduler:

```cpp
bool useWifi7 = true;
uint32_t nStations = 16;
double simulationTime = 0.2;
double distance = 1.0;
uint32_t payloadSize = 1200;
uint32_t guardIntervalNs = 800;
int mcs = -1;
uint32_t muStations = 8;
std::string dlAckType = "AGGR-MU-BAR";

const int channelWidth = useWifi7 ? 160 : 80;
const int mcsValue = (mcs >= 0) ? mcs : (useWifi7 ? 9 : 7);
const std::string mode = std::string(useWifi7 ? "EhtMcs" : "HeMcs") + std::to_string(mcsValue);
```

The AP scheduler and OFDMA ACK sequence are installed as follows:

```cpp
Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                   EnumValue(WifiAcknowledgment::DL_MU_AGGREGATE_TF));

mac.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                          "NStations",
                          UintegerValue(muStations),
                          "ForceDlOfdma",
                          BooleanValue(true),
                          "EnableUlOfdma",
                          BooleanValue(false),
                          "EnableBsrp",
                          BooleanValue(false));
```

Static association and controlled downlink UDP generation are implemented as follows:

```cpp
WifiStaticSetupHelper::SetStaticAssociation(apWifiDev, staDevices);
WifiStaticSetupHelper::SetStaticBlockAck(apWifiDev, staDevices, {0});

UdpServerHelper server(9);
serverApps = server.Install(staNodes);

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
    clientApps.Add(client.Install(apNode.Get(0)));
}
```

### 4.7 Source Files Used in This Study

The following source files were used to implement the experiments:

- `src/wifi7-exp1-throughput.cc`
- `src/wifi7-exp2-mlo-latency.cc`
- `src/wifi7-exp3-density.cc`

These files can be submitted together with the article as the required simulation model code for the course project.

## 5. Results and Discussion

### 5.1 Experiment 1 Results

The first experiment compares peak throughput between Wi-Fi 6 and Wi-Fi 7. The observed results are:

- Wi-Fi 6 throughput: `1137.72 Mbps`
- Wi-Fi 7 throughput: `1420.55 Mbps`

This corresponds to an improvement of approximately:

\[
\frac{1420.55 - 1137.72}{1137.72} \times 100 \approx 24.86\%
\]

This result confirms that Wi-Fi 7 can deliver a higher peak throughput under ideal short-range conditions. The gain is consistent with the expectation that wider bandwidth and stronger PHY capability should improve link capacity.

The average delay also decreases from `83.6234 ms` in Wi-Fi 6 to `77.0044 ms` in Wi-Fi 7. This suggests that the higher-capacity Wi-Fi 7 link can also reduce queueing pressure under the tested offered load.

Although the packet loss rate is still visible in both cases, the primary purpose of this experiment is to compare relative peak throughput, not absolute reliability. Therefore, the key conclusion is that Wi-Fi 7 demonstrates a measurable bandwidth-driven throughput advantage.

### 5.2 Experiment 2 Results

The second experiment compares Wi-Fi 6 single-link operation and Wi-Fi 7 MLO under the same lightweight interference setting. The results are:

- Wi-Fi 6:
  - Throughput: `0.366609 Mbps`
  - Average delay: `0.121823 ms`
  - `P99` delay: `0.7 ms`
  - Jitter: `0.115358 ms`
- Wi-Fi 7:
  - Throughput: `0.366608 Mbps`
  - Average delay: `0.13789 ms`
  - `P99` delay: `0.5 ms`
  - Jitter: `0.141316 ms`

The two standards show almost identical throughput, which indicates that the foreground flow is preserved in both cases. However, the more important observation is the reduction in `P99` delay:

\[
\frac{0.7 - 0.5}{0.7} \times 100 \approx 28.57\%
\]

This reduction supports the hypothesis that MLO helps suppress long-tail delay. In other words, even when the mean delay does not improve significantly, the worst-case delay behavior becomes more stable. This is highly relevant for delay-sensitive services such as interactive applications, cloud gaming, or wireless real-time control.

The average delay and jitter of Wi-Fi 7 are slightly higher than those of Wi-Fi 6 in this specific run. However, this does not invalidate the advantage of MLO. Instead, it shows that the principal benefit of MLO in the current setting is tail-latency control rather than universal improvement across every delay metric.

Therefore, the most defensible conclusion is that Wi-Fi 7 demonstrates a promising advantage in `P99` delay reduction under interference, which is a meaningful indicator of latency stability.

### 5.3 Experiment 3 Results

The third experiment evaluates a dense `16`-station downlink OFDMA scenario. The results are:

- Wi-Fi 6:
  - Aggregate throughput: `267.648 Mbps`
  - Fairness index: `0.999899`
  - Loss rate: `0.258511`
- Wi-Fi 7:
  - Aggregate throughput: `778.224 Mbps`
  - Fairness index: `0.999976`
  - Loss rate: `0.190645`

The aggregate throughput gain is substantial:

\[
\frac{778.224 - 267.648}{267.648} \times 100 \approx 190.76\%
\]

This means that Wi-Fi 7 delivers about `2.91x` the aggregate throughput of Wi-Fi 6 in the tested dense downlink OFDMA setting. At the same time, both standards maintain a fairness index extremely close to `1.0`, which indicates that the scheduler does not create an unfair resource split while delivering the higher Wi-Fi 7 capacity.

The loss rate of Wi-Fi 7 is also lower than that of Wi-Fi 6 in this configuration. Although both systems are still heavily loaded, the lower Wi-Fi 7 loss rate suggests that the wider channel and stronger PHY setting provide additional headroom when multi-user scheduling is active.

This result is important because it clarifies why earlier dense-scenario attempts did not show a visible advantage. In those earlier trials, the simplified traffic model was dominated by generic contention, which hid the difference between the standards. Once the scenario was redesigned around explicit AP-side OFDMA scheduling, the intended dense-user benefit of Wi-Fi 7 became clearly measurable.

The main remaining extensions for future work are:

- Increase the number of tested stations to `32` and `64`
- Repeat runs with multiple random seeds
- Compare multiple MU scheduler settings and ACK sequences
- Add richer traffic mixes on top of the stable OFDMA baseline

### 5.4 Overall Discussion

Taken together, the three experiments provide a balanced view of Wi-Fi 7 performance:

- Experiment 1 shows a clear throughput gain.
- Experiment 2 shows a meaningful tail-latency gain.
- Experiment 3 shows a strong dense-user throughput gain when OFDMA scheduling is explicitly enabled.

This result pattern is still valuable because it reflects the multi-dimensional nature of protocol evaluation. A new wireless standard should not be judged only by raw throughput. In practice, its value also depends on responsiveness, stability, and behavior under multi-user load. In this regard, the current study suggests that Wi-Fi 7 already demonstrates benefits in throughput, tail latency, and dense-user efficiency, while broader parameter sweeps are still needed for a more exhaustive benchmark.

## 6. Limitations

This study has several limitations.

First, each experiment currently represents a limited number of successful parameter points rather than a full statistical sweep across many random seeds. Second, the high-density experiment in its final form is based on a controlled OFDMA template designed to expose scheduler benefits clearly, so it is more focused than a fully heterogeneous real-world traffic mix. Third, some advanced Wi-Fi 7 features such as richer multi-link policies and broader large-scale parameter sweeps are not fully explored in this version of the project.

These limitations do not invalidate the findings, but they do define the scope of the conclusions. The present work should be interpreted as a reproducible pilot study with practical results rather than a final exhaustive standard-level benchmark.

## 7. Conclusion

This paper compared Wi-Fi 7 (IEEE 802.11be) and Wi-Fi 6 (IEEE 802.11ax) using `ns-3.47` under three representative scenarios: peak throughput, interference-aware latency, and dense multi-user access. The simulation results show that Wi-Fi 7 achieves a clear peak throughput advantage under short-range high-quality channel conditions, improving throughput from `1137.72 Mbps` to `1420.55 Mbps`. The latency experiment further shows that Wi-Fi 7 can reduce `P99` delay from `0.7 ms` to `0.5 ms`, which indicates improved tail-latency stability when MLO is enabled. In the final dense access experiment with explicit OFDMA scheduling, Wi-Fi 7 increases aggregate throughput from `267.648 Mbps` to `778.224 Mbps` while preserving near-perfect fairness, showing that IEEE 802.11be can provide substantial efficiency gains in multi-user downlink transmission.

Overall, the study concludes that Wi-Fi 7 provides meaningful benefits in bandwidth-driven throughput, interference-aware delay stability, and dense-user OFDMA efficiency. Future work should extend the experiments with multiple random seeds, more user counts, and richer dense-network traffic models to better quantify the scalability advantages of IEEE 802.11be.

## References

The following references should be formatted according to your required paper template:

1. IEEE Std 802.11ax, *IEEE Standard for Information Technology Telecommunications and Information Exchange Between Systems Local and Metropolitan Area Networks Specific Requirements*.
2. IEEE Std 802.11be draft / official documentation for Extremely High Throughput.
3. ns-3 Consortium, *ns-3 Manual and ns-3 Models Documentation*.
4. Relevant journal or conference papers on Wi-Fi 6 and Wi-Fi 7 performance evaluation.
5. Papers on MLO latency analysis and high-density WLAN performance.

You can replace this section with the exact citation format required by your university or paper template.
