# Wi-Fi 7 vs Wi-Fi 6 项目说明

## 1. 项目简介

本项目使用 `ns-3.47` 在 `Windows + WSL` 环境下，对 `Wi-Fi 7 (IEEE 802.11be)` 与 `Wi-Fi 6 (IEEE 802.11ax)` 做对比仿真。项目已经完成以下内容：

- 三个实验程序源码
- 仿真结果 `CSV`
- 结果汇总图
- 论文正文草稿
- 最终 `DOCX` 论文文件
- 生成图表和导出论文的 Python 脚本

当前目录已经整理为“可提交课程作业”的状态。

## 2. 目录结构

当前项目目录如下：

```text
wifi72/
├─ figures/
├─ results/
├─ scripts/
├─ src/
├─ PAPER_DRAFT.md
├─ README.md
├─ RESULTS_ANALYSIS.md
└─ WiFi7_vs_WiFi6_Final_Paper.docx
```

下面对每个目录和每个文件分别说明。

## 3. 根目录文件说明

### `README.md`

这是当前文件，也就是整个项目的总说明文档。

主要用途：

- 介绍项目目标
- 解释目录结构
- 说明每个文件的作用
- 帮助组员或老师快速理解项目内容

### `PAPER_DRAFT.md`

这是论文的 `Markdown` 正文源文件。

主要用途：

- 保存英文论文的主要内容
- 作为生成 `.docx` 文件的正文来源
- 便于后续继续修改实验流程、参数、代码片段和结果分析

如果后面要更新论文正文，优先修改这个文件，再重新导出 `DOCX`。

### `RESULTS_ANALYSIS.md`

这是结果分析草稿文件。

主要用途：

- 单独整理实验一、实验二、实验三的结果解释
- 给论文 `Results and Discussion` 提供文字素材
- 帮助后续扩写结论和演讲稿

这个文件更偏“分析笔记”，不是最终提交文件，但对后续修改论文很有帮助。

### `WiFi7_vs_WiFi6_Final_Paper.docx`

这是最终保留的论文文件，也是当前项目中最重要的交付物。

主要用途：

- 可直接用 `WPS` 打开
- 可直接继续修改
- 已包含实验结果图
- 已包含实验流程、参数设置、运行命令和核心代码片段

如果老师要求提交论文电子版，优先提交这个文件。

## 4. `src/` 目录说明

`src/` 目录存放三个实验对应的 `C++` 仿真程序源代码。

### `src/wifi7-exp1-throughput.cc`

这是实验一的源码文件。

对应实验：

- 峰值吞吐量与频宽增益实验

主要功能：

- 对比 `802.11ax` 与 `802.11be`
- 支持 `160 MHz` 与 `320 MHz` 带宽配置
- 支持固定 `MCS`
- 输出吞吐量、平均时延、抖动、丢包率

论文中对应内容：

- 实验一的参数设置
- 实验一的核心代码片段
- 实验一的吞吐量结果

### `src/wifi7-exp2-mlo-latency.cc`

这是实验二的源码文件。

对应实验：

- `MLO` 低时延与稳定性实验

主要功能：

- 支持 `Wi-Fi 6` 单链路模式
- 支持 `Wi-Fi 7` 的 `MLO / EMLSR` 模式
- 支持背景干扰流
- 输出平均时延、`P99` 时延、抖动、吞吐量和丢包率

论文中对应内容：

- 实验二的链路配置
- 背景干扰设置
- `MLO` 代码片段
- `P99 Delay` 对比结果

### `src/wifi7-exp3-density.cc`

这是实验三的源码文件。

对应实验：

- 高密度多用户效率与公平性实验

主要功能：

- 构建 `1 AP + N STA` 高密度下行场景
- 基于官方 `ns-3` OFDMA 模板启用 `RrMultiUserScheduler`
- 支持 `802.11ax` 与 `802.11be` 的固定 `MCS` 多用户对照
- 输出总吞吐量、公平性、丢包率等实验三核心指标

论文中对应内容：

- 实验三的高密度场景
- OFDMA 多用户调度配置
- 公平性与聚合吞吐分析

## 5. `results/` 目录说明

`results/` 目录保存所有仿真输出的 `CSV` 文件和汇总结果。

### 实验一结果文件

#### `results/exp1_ax.csv`

实验一早期生成的 `Wi-Fi 6` 结果文件。

用途：

- 早期调试和验证使用

#### `results/exp1_ax_fair.csv`

实验一最终采用的 `Wi-Fi 6` 对照结果。

用途：

- 用于论文中的正式结果对比
- 与 `exp1_be_fair.csv` 共同构成实验一主结果

#### `results/exp1_be_fair.csv`

实验一最终采用的 `Wi-Fi 7` 对照结果。

用途：

- 与 `exp1_ax_fair.csv` 一起用于论文实验一

### 实验二结果文件

#### `results/exp2_ax.csv`

实验二早期生成的 `Wi-Fi 6` 结果。

用途：

- 调试单链路模式是否正常运行

#### `results/exp2_ax_fair.csv`

实验二最终采用的 `Wi-Fi 6` 同参数对照结果。

用途：

- 论文实验二正式数据

#### `results/exp2_be_fair.csv`

实验二最终采用的 `Wi-Fi 7 MLO` 同参数对照结果。

用途：

- 论文实验二正式数据
- 用于 `P99 Delay` 对比

#### `results/exp2_be_paper.csv`

实验二在另一组轻量参数下得到的 `Wi-Fi 7` 可运行结果。

用途：

- 论文写作和调试过程中的辅助结果
- 不是最终主对照文件

### 实验三结果文件

#### `results/exp3_ax_16_fair.csv`

实验三早期阶段生成的 `Wi-Fi 6` 基线结果。

用途：

- 用于早期调试和分析“差异为什么不明显”

#### `results/exp3_be_16_fair.csv`

实验三早期阶段生成的 `Wi-Fi 7` 基线结果。

用途：

- 用于早期调试和分析“差异为什么不明显”

#### `results/exp3_ax_16_mu_template.csv`

这是实验三当前正式采用的 `Wi-Fi 6` 结果文件。

用途：

- 基于官方 OFDMA 模板的高密度 `16` 用户正式结果
- 用于论文实验三最终对照

关键结果：

- Aggregate throughput: `267.648 Mbps`
- Fairness index: `0.999899`
- Loss rate: `0.258511`

#### `results/exp3_be_16_mu_template.csv`

这是实验三当前正式采用的 `Wi-Fi 7` 结果文件。

用途：

- 与 `exp3_ax_16_mu_template.csv` 共同构成论文实验三最终结果

关键结果：

- Aggregate throughput: `778.224 Mbps`
- Fairness index: `0.999976`
- Loss rate: `0.190645`

### 汇总结果文件

#### `results/paper_summary.csv`

这是从多个正式结果 `CSV` 中提取出的论文汇总表。

主要用途：

- 统一整理实验一、二、三的关键指标
- 给画图脚本提供简洁的结果核对依据
- 方便手工做表格或插入论文

说明：

- 当前已经同步到实验三新的 OFDMA 正式结果

## 6. `figures/` 目录说明

`figures/` 目录存放论文图表。

### `figures/wifi7_vs_wifi6_summary.png`

这是当前项目的汇总结果图。

主要用途：

- 展示三组实验的主要对比结果
- 已经插入最终论文 `DOCX`
- 可直接用于 PPT 或论文正文

## 7. `scripts/` 目录说明

`scripts/` 目录存放自动化脚本。

### `scripts/plot_paper_results.py`

这是结果绘图脚本。

主要用途：

- 读取 `results/` 中当前正式采用的结果 `CSV`
- 自动绘制汇总图
- 输出 `figures/wifi7_vs_wifi6_summary.png`
- 输出 `results/paper_summary.csv`

适用场景：

- 当你重新跑了实验并更新 `CSV` 后，可以重新运行它来刷新图表

### `scripts/export_paper_docx.py`

这是论文导出脚本。

主要用途：

- 读取 `PAPER_DRAFT.md`
- 把论文正文转换成 `.docx`
- 自动插入结果图
- 支持将代码块按等宽字体写入文档

适用场景：

- 当你修改了论文正文后，可以重新导出新的 `.docx`

## 8. 文件之间的关系

可以把整个项目理解为以下流程：

1. `src/` 中的 `C++` 源码负责跑仿真
2. 仿真结果输出到 `results/` 中的多个 `CSV`
3. `scripts/plot_paper_results.py` 读取 `results/` 生成：
   - `figures/wifi7_vs_wifi6_summary.png`
   - `results/paper_summary.csv`
4. `PAPER_DRAFT.md` 提供论文正文内容
5. `scripts/export_paper_docx.py` 读取：
   - `PAPER_DRAFT.md`
   - `figures/wifi7_vs_wifi6_summary.png`
6. 最终导出：
   - `WiFi7_vs_WiFi6_Final_Paper.docx`

## 9. 推荐查看顺序

如果是老师或组员第一次看这个项目，建议按以下顺序查看：

1. 先看 `WiFi7_vs_WiFi6_Final_Paper.docx`
2. 再看 `README.md`
3. 再看 `figures/wifi7_vs_wifi6_summary.png`
4. 然后看 `results/paper_summary.csv`
5. 最后如果需要核查实现，再看 `src/` 下三个源码文件

## 10. 重新生成论文和图表的方法

### 重新画图

在 `WSL` 中运行：

```bash
python3 /mnt/c/Users/许钱洲/Desktop/wifi72/scripts/plot_paper_results.py
```

### 重新导出论文

在 `WSL` 中运行：

```bash
python3 /mnt/c/Users/许钱洲/Desktop/wifi72/scripts/export_paper_docx.py
```

## 11. 提交建议

课程作业最终建议提交以下两类内容：

### 论文文件

- `WiFi7_vs_WiFi6_Final_Paper.docx`

### 仿真模型代码

- `src/wifi7-exp1-throughput.cc`
- `src/wifi7-exp2-mlo-latency.cc`
- `src/wifi7-exp3-density.cc`

如果老师允许附加材料，也可以同时提交：

- `figures/wifi7_vs_wifi6_summary.png`
- `results/paper_summary.csv`

## 12. 备注

- 当前目录中的 `.docx` 只有一份最终论文，便于直接提交
- 如果 `WPS/Word` 正在打开论文，目录里可能会临时出现 `~$` 开头的锁文件，但这不是正式成果
- 如果后续继续修改论文，建议优先修改 `PAPER_DRAFT.md`，然后重新导出 `DOCX`
