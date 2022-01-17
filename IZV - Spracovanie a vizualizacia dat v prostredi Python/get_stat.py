#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import matplotlib.pyplot as plt
import argparse
from matplotlib import colors
import os
import sys

from download import DataDownloader


def arg_parse():
    """
    Processes command line arguments that are after parsing sent to the plot_stat function.
    """
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument(
        "--fig_location",
        action="store",
        type=str,
        help="If set, the graph is saved to the path specified by this argument.",
    )
    parser.add_argument(
        "--show_figure",
        action="store_true",
        help="If set, the graph is displayed in the window",
    )
    args = parser.parse_args()
    if not args.fig_location and not args.show_figure:
        print("Nothing to do.\n")
        parser.print_help()
        sys.exit(0)

    plot_stat(
        data_source=DataDownloader().get_dict(),
        fig_location=args.fig_location,
        show_figure=args.show_figure,
    )


def plot_stat(data_source, fig_location=None, show_figure=False):
    """
    Processes data obtained from DataDownloader class and visualizes them
    in two graphs - absolute graph - logaritmic scale and relative with linear scale.

    Attributes:
        data_source     Dictionary obtained from DataDownloader class.
        fig_location    If set, the graph image is saved to the path specified by this argument.
        show_figure     If set, the graph is displayed in the window.
    """
    regions = sorted(list(DataDownloader.regions.keys()))
    causes = [
        "Prerušovaná žltá",
        "Semafor mimo prevádzku",
        "Dopravné značky",
        "Prenosné dopravné značky",
        "Nevyznačená",
        "Žiadna úprava",
    ]
    p24_values = [0, 1, 2, 3, 4, 5]
    absolute = np.zeros(len(regions) * 6, dtype=int).reshape(6, len(regions))

    for region_i, region in enumerate(regions):
        reg_indices = np.where(data_source["region"] == region)
        reg_p24 = np.take(data_source["p24"], reg_indices)
        for value in p24_values:
            absolute[value, region_i] = len(np.where(reg_p24 == value)[0])
    # shift np.array as we want first row as last
    absolute = np.roll(absolute, -1, axis=0)
    fig, (ax_abs, ax_rel) = plt.subplots(2, figsize=(17, 8.5))
    fig.subplots_adjust(hspace=0.4)

    pos_abs = ax_abs.imshow(absolute, norm=colors.LogNorm())
    ax_abs.set_title("Absolutne")
    ax_abs.set_xticks(np.arange(len(regions)))
    ax_abs.set_yticks(np.arange(len(causes)))
    ax_abs.set_xticklabels(regions)
    ax_abs.set_yticklabels(causes)
    ca_abs = fig.add_axes(
        [
            ax_abs.get_position().x1 + 0.04,
            ax_abs.get_position().y0,
            0.0175,
            ax_abs.get_position().height,
        ]
    )
    absolute_cbar = plt.colorbar(pos_abs, cax=ca_abs)
    absolute_cbar.set_label("Počet nehôd", rotation=90, labelpad=10)

    # calculate relative values
    row_sum = np.sum(absolute, axis=1)
    relative = (100 * absolute) / row_sum[:, np.newaxis]
    relative = np.where(relative == .0, np.nan, relative)
    
    # set white color on each 0% value
    pos_rel = ax_rel.imshow(relative, cmap="plasma", vmin=.0, vmax=100.0)
    ax_rel.set_title("Relatívne voči príčine")
    ax_rel.set_xticks(np.arange(len(regions)))
    ax_rel.set_yticks(np.arange(len(causes)))
    ax_rel.set_xticklabels(regions)
    ax_rel.set_yticklabels(causes)
    ca_rel = fig.add_axes(
        [
            ax_rel.get_position().x1 + 0.04,
            ax_rel.get_position().y0,
            0.0175,
            ax_rel.get_position().height,
        ]
    )

    relative_cbar = plt.colorbar(pos_rel, cax=ca_rel, ticks=np.linspace(0.0, 100.0, num=6))
    relative_cbar.set_label(
        "Podiel nehôd pre danú príčinu [%]", rotation=90, labelpad=10
    )
    if fig_location:
        if fig_location.startswith("/"):
            fig_location = fig_location[1:]
        if "/" in fig_location:
            dirs = fig_location.split("/")[:-1]
            path = ""
            for dir in dirs:
                if not path:
                    path = os.path.join(f"{os.getcwd()}/{dir}")
                else:
                    path = os.path.join(f"{path}/{dir}")
                if not os.path.isdir(path):
                    os.makedirs(path)
        plt.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    arg_parse()
