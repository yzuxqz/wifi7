from __future__ import annotations

import csv
from pathlib import Path
from typing import Dict, List, Optional

import matplotlib.pyplot as plt


BASE_DIR = Path(__file__).resolve().parent.parent
RESULTS_DIR = BASE_DIR / "results"
FIGURES_DIR = BASE_DIR / "figures"


def read_single_row(csv_path: Path) -> Optional[Dict[str, str]]:
    if not csv_path.exists():
        return None

    with csv_path.open("r", encoding="utf-8", newline="") as f:
        rows = list(csv.DictReader(f))
    return rows[0] if rows else None


def to_float(row: Dict[str, str], key: str) -> float:
    return float(row.get(key, "0") or 0)


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def add_value_labels(ax: plt.Axes, bars, fmt: str = "{:.3f}") -> None:
    for bar in bars:
        value = bar.get_height()
        ax.text(
            bar.get_x() + bar.get_width() / 2.0,
            value,
            fmt.format(value),
            ha="center",
            va="bottom",
            fontsize=9,
        )


def save_summary_csv(summary_rows: List[Dict[str, str]]) -> Path:
    out_path = RESULTS_DIR / "paper_summary.csv"
    fieldnames = [
        "experiment",
        "standard",
        "metric",
        "value",
        "unit",
        "source_file",
    ]
    with out_path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(summary_rows)
    return out_path


def plot_exp1(ax_row: List[plt.Axes], ax: Dict[str, str], be: Dict[str, str], summary_rows: List[Dict[str, str]]) -> None:
    standards = ["Wi-Fi 6", "Wi-Fi 7"]
    throughput = [to_float(ax, "throughput_mbps"), to_float(be, "throughput_mbps")]
    avg_delay = [to_float(ax, "avg_delay_ms"), to_float(be, "avg_delay_ms")]

    bars1 = ax_row[0].bar(standards, throughput, color=["#4C78A8", "#F58518"])
    ax_row[0].set_title("Experiment 1: Throughput")
    ax_row[0].set_ylabel("Mbps")
    add_value_labels(ax_row[0], bars1, "{:.2f}")

    bars2 = ax_row[1].bar(standards, avg_delay, color=["#4C78A8", "#F58518"])
    ax_row[1].set_title("Experiment 1: Average Delay")
    ax_row[1].set_ylabel("ms")
    add_value_labels(ax_row[1], bars2, "{:.2f}")

    summary_rows.extend(
        [
            {
                "experiment": "exp1",
                "standard": "80211ax",
                "metric": "throughput_mbps",
                "value": ax["throughput_mbps"],
                "unit": "Mbps",
                "source_file": "exp1_ax_fair.csv",
            },
            {
                "experiment": "exp1",
                "standard": "80211be",
                "metric": "throughput_mbps",
                "value": be["throughput_mbps"],
                "unit": "Mbps",
                "source_file": "exp1_be_fair.csv",
            },
            {
                "experiment": "exp1",
                "standard": "80211ax",
                "metric": "avg_delay_ms",
                "value": ax["avg_delay_ms"],
                "unit": "ms",
                "source_file": "exp1_ax_fair.csv",
            },
            {
                "experiment": "exp1",
                "standard": "80211be",
                "metric": "avg_delay_ms",
                "value": be["avg_delay_ms"],
                "unit": "ms",
                "source_file": "exp1_be_fair.csv",
            },
        ]
    )


def plot_exp2(ax_row: List[plt.Axes], ax: Dict[str, str], be: Dict[str, str], summary_rows: List[Dict[str, str]]) -> None:
    standards = ["Wi-Fi 6", "Wi-Fi 7"]
    avg_delay = [to_float(ax, "avg_delay_ms"), to_float(be, "avg_delay_ms")]
    p99_delay = [to_float(ax, "p99_delay_ms"), to_float(be, "p99_delay_ms")]
    jitter = [to_float(ax, "avg_jitter_ms"), to_float(be, "avg_jitter_ms")]

    x = range(len(standards))
    width = 0.25

    bars1 = ax_row[0].bar([v - width for v in x], avg_delay, width=width, label="Avg Delay", color="#4C78A8")
    bars2 = ax_row[0].bar(list(x), p99_delay, width=width, label="P99 Delay", color="#F58518")
    bars3 = ax_row[0].bar([v + width for v in x], jitter, width=width, label="Jitter", color="#54A24B")
    ax_row[0].set_title("Experiment 2: Delay Metrics")
    ax_row[0].set_ylabel("ms")
    ax_row[0].set_xticks(list(x), standards)
    ax_row[0].legend()
    add_value_labels(ax_row[0], bars1, "{:.3f}")
    add_value_labels(ax_row[0], bars2, "{:.3f}")
    add_value_labels(ax_row[0], bars3, "{:.3f}")

    throughput = [to_float(ax, "throughput_mbps"), to_float(be, "throughput_mbps")]
    bars4 = ax_row[1].bar(standards, throughput, color=["#4C78A8", "#F58518"])
    ax_row[1].set_title("Experiment 2: Foreground Throughput")
    ax_row[1].set_ylabel("Mbps")
    add_value_labels(ax_row[1], bars4, "{:.3f}")

    for standard, row, source in [
        ("80211ax", ax, "exp2_ax_fair.csv"),
        ("80211be", be, "exp2_be_fair.csv"),
    ]:
        for metric, unit in [
            ("avg_delay_ms", "ms"),
            ("p99_delay_ms", "ms"),
            ("avg_jitter_ms", "ms"),
            ("throughput_mbps", "Mbps"),
        ]:
            summary_rows.append(
                {
                    "experiment": "exp2",
                    "standard": standard,
                    "metric": metric,
                    "value": row[metric],
                    "unit": unit,
                    "source_file": source,
                }
            )


def plot_exp3(ax_row: List[plt.Axes], ax: Dict[str, str], be: Dict[str, str], summary_rows: List[Dict[str, str]]) -> None:
    standards = ["Wi-Fi 6", "Wi-Fi 7"]
    throughput = [to_float(ax, "aggregate_throughput_mbps"), to_float(be, "aggregate_throughput_mbps")]
    fairness = [to_float(ax, "fairness_index"), to_float(be, "fairness_index")]
    loss_rate = [to_float(ax, "loss_rate"), to_float(be, "loss_rate")]

    bars1 = ax_row[0].bar(standards, throughput, color=["#4C78A8", "#F58518"])
    ax_row[0].set_title("Experiment 3: Dense OFDMA Throughput")
    ax_row[0].set_ylabel("Mbps")
    add_value_labels(ax_row[0], bars1, "{:.3f}")

    x = range(len(standards))
    width = 0.35
    bars2 = ax_row[1].bar([v - width / 2 for v in x], fairness, width=width, label="Fairness", color="#54A24B")
    bars3 = ax_row[1].bar([v + width / 2 for v in x], loss_rate, width=width, label="Loss Rate", color="#E45756")
    ax_row[1].set_title("Experiment 3: Fairness and Loss")
    ax_row[1].set_ylabel("Index / Ratio")
    ax_row[1].set_xticks(list(x), standards)
    ax_row[1].legend()
    add_value_labels(ax_row[1], bars2, "{:.3f}")
    add_value_labels(ax_row[1], bars3, "{:.3f}")

    for standard, row, source in [
        ("80211ax", ax, "exp3_ax_16_mu_template.csv"),
        ("80211be", be, "exp3_be_16_mu_template.csv"),
    ]:
        for metric, unit in [
            ("aggregate_throughput_mbps", "Mbps"),
            ("fairness_index", "index"),
            ("loss_rate", "ratio"),
        ]:
            summary_rows.append(
                {
                    "experiment": "exp3",
                    "standard": standard,
                    "metric": metric,
                    "value": row[metric],
                    "unit": unit,
                    "source_file": source,
                }
            )


def main() -> None:
    ensure_dir(FIGURES_DIR)

    exp1_ax = read_single_row(RESULTS_DIR / "exp1_ax_fair.csv")
    exp1_be = read_single_row(RESULTS_DIR / "exp1_be_fair.csv")
    exp2_ax = read_single_row(RESULTS_DIR / "exp2_ax_fair.csv")
    exp2_be = read_single_row(RESULTS_DIR / "exp2_be_fair.csv")
    exp3_ax = read_single_row(RESULTS_DIR / "exp3_ax_16_mu_template.csv")
    exp3_be = read_single_row(RESULTS_DIR / "exp3_be_16_mu_template.csv")

    missing = []
    for name, row in [
        ("exp1_ax_fair.csv", exp1_ax),
        ("exp1_be_fair.csv", exp1_be),
        ("exp2_ax_fair.csv", exp2_ax),
        ("exp2_be_fair.csv", exp2_be),
        ("exp3_ax_16_mu_template.csv", exp3_ax),
        ("exp3_be_16_mu_template.csv", exp3_be),
    ]:
        if row is None:
            missing.append(name)

    if missing:
        raise FileNotFoundError("Missing result files: " + ", ".join(missing))

    plt.style.use("default")
    fig, axes = plt.subplots(3, 2, figsize=(14, 14))
    fig.suptitle("Wi-Fi 7 vs Wi-Fi 6 Experimental Results", fontsize=16)

    summary_rows: List[Dict[str, str]] = []
    plot_exp1(list(axes[0]), exp1_ax, exp1_be, summary_rows)
    plot_exp2(list(axes[1]), exp2_ax, exp2_be, summary_rows)
    plot_exp3(list(axes[2]), exp3_ax, exp3_be, summary_rows)

    for row in axes:
        for ax in row:
            ax.grid(axis="y", linestyle="--", alpha=0.35)

    fig.tight_layout()
    figure_path = FIGURES_DIR / "wifi7_vs_wifi6_summary.png"
    fig.savefig(figure_path, dpi=300, bbox_inches="tight")
    plt.close(fig)

    summary_path = save_summary_csv(summary_rows)
    print(f"Saved figure: {figure_path}")
    print(f"Saved summary CSV: {summary_path}")


if __name__ == "__main__":
    main()
