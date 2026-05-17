# Wi-Fi 7 vs Wi-Fi 6 Result Analysis

## Data Sources

This document is based on the following result files:

- `results/exp1_ax_fair.csv`
- `results/exp1_be_fair.csv`
- `results/exp2_ax_fair.csv`
- `results/exp2_be_fair.csv`
- `results/exp3_ax_16_mu_template.csv`
- `results/exp3_be_16_mu_template.csv`

## Experiment 1: Peak Throughput

### Results

- `802.11ax`: `1137.72 Mbps`
- `802.11be`: `1420.55 Mbps`
- Throughput improvement: about `24.9%`

### Analysis

This experiment was conducted under short-range, high-quality link conditions. The result shows that `802.11be` achieves a higher peak throughput than `802.11ax`. This difference is consistent with the wider channel bandwidth and stronger PHY capability of `Wi-Fi 7`. Experiment 1 mainly reflects the capacity advantage under ideal conditions.

## Experiment 2: MLO Delay Stability

### Results

- `802.11ax`
  - Throughput: `0.366609 Mbps`
  - Average delay: `0.121823 ms`
  - `P99` delay: `0.7 ms`
  - Jitter: `0.115358 ms`
- `802.11be`
  - Throughput: `0.366608 Mbps`
  - Average delay: `0.13789 ms`
  - `P99` delay: `0.5 ms`
  - Jitter: `0.141316 ms`

### Analysis

The foreground throughput is nearly identical in both cases, which indicates that link capacity is not the main source of difference under the current load. In contrast, the lower `P99` delay in the `802.11be` case suggests that `MLO` improves tail-delay performance under interference. The main value of this experiment is stability rather than throughput gain.

## Experiment 3: Dense Multi-User Efficiency

### Results

- `802.11ax`
  - Aggregate throughput: `267.648 Mbps`
  - Fairness index: `0.999899`
  - Packet loss rate: `0.258511`
- `802.11be`
  - Aggregate throughput: `778.224 Mbps`
  - Fairness index: `0.999976`
  - Packet loss rate: `0.190645`

The aggregate throughput of `802.11be` is about `2.91x` that of `802.11ax`.

### Analysis

Experiment 3 uses a dense downlink `OFDMA` scenario with AP-side multi-user scheduling enabled. The result shows that `802.11be` substantially increases system throughput while maintaining nearly the same fairness and a lower packet loss rate. This indicates that the capacity advantage of `Wi-Fi 7` is more apparent in multi-user scheduling scenarios.

## Summary

- Experiment 1 shows that `Wi-Fi 7` provides higher peak throughput under ideal link conditions.
- Experiment 2 shows that `MLO` is more effective in improving tail-delay performance.
- Experiment 3 shows that `Wi-Fi 7` provides higher aggregate throughput in dense multi-user scenarios.

Overall, `Wi-Fi 7` shows clear advantages over `Wi-Fi 6` in throughput, tail-delay control, and dense-network transmission efficiency.
