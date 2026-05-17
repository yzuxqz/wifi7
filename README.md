# Wi-Fi 7 vs Wi-Fi 6 Simulation Project

## Overview

This project uses `ns-3.47` to compare `Wi-Fi 7 (802.11be)` and `Wi-Fi 6 (802.11ax)` in three experiments:

- Experiment 1: peak throughput and bandwidth gain
- Experiment 2: delay stability in an `MLO` scenario
- Experiment 3: multi-user efficiency in a dense network

## Directory Structure

```text
wifi72/
├─ figures/
├─ results/
├─ scripts/
├─ src/
├─ README.md
└─ RESULTS_ANALYSIS.md
```

## File Description

### `src/`

- `wifi7-exp1-throughput.cc`: source code for Experiment 1; outputs throughput, delay, jitter, and packet loss.
- `wifi7-exp2-mlo-latency.cc`: source code for Experiment 2; compares single-link and `MLO/EMLSR` delay performance.
- `wifi7-exp3-density.cc`: source code for Experiment 3; builds a dense downlink scenario and outputs aggregate throughput, fairness, and packet loss.

### `results/`

- `exp1_ax_fair.csv`, `exp1_be_fair.csv`: final results for Experiment 1.
- `exp2_ax_fair.csv`, `exp2_be_fair.csv`: final results for Experiment 2.
- `exp3_ax_16_mu_template.csv`, `exp3_be_16_mu_template.csv`: final results for Experiment 3.
- `paper_summary.csv`: summary table of the three experiments.

### `figures/`

- `wifi7_vs_wifi6_summary.png`: summary figure for all experiments.

### `scripts/`

- `plot_paper_results.py`: generates the summary figure and `paper_summary.csv` from the final result files.

### Root Files

- `README.md`: project overview.
- `RESULTS_ANALYSIS.md`: result summary and discussion.

## Main Results

- Experiment 1: `802.11be` throughput is `1420.55 Mbps`, while `802.11ax` throughput is `1137.72 Mbps`.
- Experiment 2: both cases have similar throughput, while the `802.11be` case reduces `P99 delay` from `0.7 ms` to `0.5 ms`.
- Experiment 3: `802.11be` reaches `778.224 Mbps` aggregate throughput, while `802.11ax` reaches `267.648 Mbps`.

## Reproducing the Figure

Run the following command in `WSL`:

```bash
python3 /mnt/c/Users/许钱洲/Desktop/wifi72/scripts/plot_paper_results.py
```

## Suggested Submission Files

- the three source files in `src/`
- the seven final result `CSV` files in `results/`
- `figures/wifi7_vs_wifi6_summary.png`
- `README.md`
