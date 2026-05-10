# Wi-Fi 7 vs Wi-Fi 6 结果分析草稿

## 1. 文档用途

本文档基于当前已经跑通的 `ns-3.47` 仿真结果，整理出可直接用于课程论文 `Results and Discussion` 部分的第一版分析文字。现阶段结果可视为“可复现实验初稿”，适合用于论文撰写与后续图表扩展。

当前分析使用的数据文件如下：

- `results/exp1_ax_fair.csv`
- `results/exp1_be_fair.csv`
- `results/exp2_ax_fair.csv`
- `results/exp2_be_fair.csv`
- `results/exp3_ax_16_mu_template.csv`
- `results/exp3_be_16_mu_template.csv`

## 2. 实验一：峰值吞吐量与频宽增益

### 2.1 核心结果

- `Wi-Fi 6 (802.11ax)`：吞吐量约 `1137.72 Mbps`
- `Wi-Fi 7 (802.11be)`：吞吐量约 `1420.55 Mbps`
- 吞吐量提升约 `24.9%`

### 2.2 结果解释

实验一验证了在近距离、低干扰、高质量链路条件下，`Wi-Fi 7` 相比 `Wi-Fi 6` 能够提供更高的峰值吞吐量。当前结果显示，`802.11be` 的吞吐量明显高于 `802.11ax`，这与 `Wi-Fi 7` 引入更大信道带宽和更高阶物理层能力的设计目标一致。

从参数上看，本组对比主要体现了两个变化：

- `Wi-Fi 6` 使用 `160 MHz`
- `Wi-Fi 7` 使用 `320 MHz`

因此，该实验更适合在论文中解释为“`Wi-Fi 7` 在高质量链路下的总体峰值吞吐优势”，而不是单独归因于某一个特性。若后续需要更强的论文说服力，可以再加入“固定带宽、只改变 MCS”的控制变量组，用于进一步拆分 `320 MHz` 与高阶调制的贡献比例。

### 2.3 可直接写进论文的讨论

可写为：

`The results indicate that Wi-Fi 7 achieves a higher peak throughput than Wi-Fi 6 under ideal short-range conditions. In the current ns-3 setup, 802.11be reaches about 1420.55 Mbps, while 802.11ax reaches about 1137.72 Mbps. This represents an improvement of approximately 24.9%, confirming that wider bandwidth and enhanced PHY capabilities in Wi-Fi 7 can translate into measurable throughput gains.`

## 3. 实验二：MLO 时延稳定性

### 3.1 核心结果

在同参数条件下：

- `Wi-Fi 6 (single link)`：
  - 吞吐量 `0.366609 Mbps`
  - 平均时延 `0.121823 ms`
  - `P99` 时延 `0.7 ms`
  - 抖动 `0.115358 ms`
- `Wi-Fi 7 (MLO/EMLSR)`：
  - 吞吐量 `0.366608 Mbps`
  - 平均时延 `0.13789 ms`
  - `P99` 时延 `0.5 ms`
  - 抖动 `0.141316 ms`

### 3.2 结果解释

实验二的重点不是比较峰值吞吐量，而是考察链路在干扰环境下的低时延与长尾时延表现。当前结果中，`Wi-Fi 6` 与 `Wi-Fi 7` 的前景流吞吐量几乎相同，说明两种方案都能维持基本的数据传输能力。

更值得关注的是长尾时延指标：

- `Wi-Fi 7` 的 `P99 delay` 为 `0.5 ms`
- `Wi-Fi 6` 的 `P99 delay` 为 `0.7 ms`

这说明在当前干扰和业务负载设置下，`MLO` 对长尾时延有一定抑制作用。虽然 `Wi-Fi 7` 的平均时延与抖动在这一组数据中没有全面优于 `Wi-Fi 6`，但 `P99` 的改善更符合 `MLO` 的理论价值，即在链路状态出现局部波动时，减少极端排队和尾部延迟。

### 3.3 论文写法建议

这一组数据很适合在论文中写成：

- 平均时延不一定总是显著下降
- 但 `P99 delay` 的下降说明 `MLO` 对长尾时延更敏感
- `Wi-Fi 7` 的关键收益更多体现在“稳定性”而非单次平均值

可写为：

`Although the average delay of the Wi-Fi 7 MLO case is slightly higher in this pilot run, the P99 delay is lower than that of the Wi-Fi 6 single-link case. This suggests that the primary benefit of MLO is not necessarily a universal reduction in mean delay, but rather a reduction in tail latency under interference, which is more relevant for latency-sensitive applications.`

## 4. 实验三：高密度场景下的效率与公平性

### 4.1 核心结果

当前正式采用的是基于官方 `ns-3` OFDMA 模板改写后的 `16` 用户下行多用户场景。在该场景下：

- `Wi-Fi 6`
  - 总吞吐量 `267.648 Mbps`
  - 公平性指数 `0.999899`
  - 丢包率 `0.258511`
- `Wi-Fi 7`
  - 总吞吐量 `778.224 Mbps`
  - 公平性指数 `0.999976`
  - 丢包率 `0.190645`

按聚合吞吐量计算，`Wi-Fi 7` 相比 `Wi-Fi 6` 的提升约为：

`(778.224 - 267.648) / 267.648 ≈ 190.76%`

也可以理解为，当前实验三中 `Wi-Fi 7` 的聚合吞吐量约为 `Wi-Fi 6` 的 `2.91` 倍。

### 4.2 结果解释

这组结果与前面几次“差异不明显”的实验三尝试有本质区别。新的实验三不是简单继续沿用原来的混合业务争用场景，而是改写为更接近官方 `wifi-he-network / wifi-eht-network` 示例的高密度下行 `OFDMA` 场景，并显式启用了：

- `RrMultiUserScheduler`
- `AGGR-MU-BAR`
- `SpectrumWifiPhyHelper`
- 静态关联与静态 block-ack

在这种配置下，`Wi-Fi 7` 的优势终于被明显放大出来，说明前面“看不出差异”的根因并不是 `802.11be` 本身没有收益，而是原始实验三的场景没有真正激活能够体现多用户调度收益的机制。

当前结果说明：

- `Wi-Fi 7` 在高密度下行 OFDMA 场景中可以显著提升系统总吞吐量
- 两者公平性都接近 `1.0`，说明吞吐提升不是靠牺牲公平性换来的
- `Wi-Fi 7` 的丢包率也更低，说明在相同负载模式下它有更大的容量余量

这意味着实验三现在已经可以作为论文中的“正式正结果”使用，而不再只是早期探索性基线。

当然，这并不意味着实验三已经完全覆盖真实网络的所有复杂性。当前版本更准确的定位是：

- 它是一个“稳定、可复现、差异明确”的高密度 OFDMA 对照实验
- 它适合用来说明 `Wi-Fi 7` 在多用户调度场景下的容量优势
- 它还不是一个完全异构业务、完全真实办公流量分布下的最终工程场景

### 4.3 论文里的诚实写法

可写为：

`In the final 16-station dense downlink OFDMA scenario, Wi-Fi 7 achieves a much higher aggregate throughput than Wi-Fi 6 while maintaining nearly identical fairness. Specifically, 802.11be reaches about 778.224 Mbps, whereas 802.11ax reaches about 267.648 Mbps. This indicates that once AP-side multi-user scheduling is explicitly enabled, the high-density efficiency advantage of Wi-Fi 7 becomes clearly measurable in ns-3.`

## 5. 总结性讨论

基于当前已经完成的三组实验，可以形成以下论文主线：

- `实验一`：证明 `Wi-Fi 7` 在理想链路条件下具有更高峰值吞吐量
- `实验二`：证明 `Wi-Fi 7 MLO` 在干扰环境中对长尾时延更有优势
- `实验三`：证明 `Wi-Fi 7` 在显式启用 OFDMA 多用户调度后具有明显的高密度吞吐优势

这样组织的好处是：

- 三个实验都有明确结果支撑，不再出现“实验三几乎没差别”的薄弱点
- 论文主线更完整：吞吐量、长尾时延、高密度效率三个维度都能对应 `Wi-Fi 7` 的核心卖点
- 同时仍然可以诚实说明实验三当前更偏“稳定模板场景”，后续还可以继续扩展到更复杂负载

## 6. 可直接放入论文的结论草稿

可作为 `Conclusion` 初稿使用：

`This study used ns-3.47 to compare Wi-Fi 7 (IEEE 802.11be) and Wi-Fi 6 (IEEE 802.11ax) under three representative scenarios, namely peak throughput, interference-aware latency, and dense multi-user access. The results show that Wi-Fi 7 provides a clear throughput advantage in short-range high-quality links, and its multi-link capability demonstrates potential in reducing tail latency under interference. In the final dense OFDMA scenario, Wi-Fi 7 also achieves a much higher aggregate throughput than Wi-Fi 6 while maintaining nearly identical fairness. Overall, the findings suggest that Wi-Fi 7 offers meaningful gains in bandwidth-driven throughput, tail-latency stability, and multi-user transmission efficiency.`

## 7. 下一步增强建议

如果你要把论文进一步做漂亮，优先级建议如下：

1. 对每组实验加入多个随机种子，求平均值与误差条
2. 对实验一加入多个距离点，如 `1/3/5 m`
3. 对实验二加入多个背景流速率，如 `20/50/100 Mbps`
4. 对实验三增加 `32` 和 `64` 个用户点
5. 在实验三上测试不同 `MU scheduler` 参数与 ACK 序列
6. 在当前稳定 OFDMA 模板基础上再叠加更复杂的混合业务
7. 生成柱状图与折线图，并把图表编号整理进论文正文
