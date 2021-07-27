#!/usr/bin/env python

import sys
from pathlib import Path
from argparse import ArgumentParser

import matplotlib.pyplot as plt
import matplotlib
import numpy as np

import uproot
import mplhep as hep
import hist
hep.style.use(hep.style.ATLAS)

TO_MM = 1.0e3
TO_UM = 1.0e6
TO_NM = 1.0e9

def make_pv_plots(tree) -> None :
    """
    Dump plots related to the primary vertex contained in the
    input ROOT LCTuple file.

    Args:
        tree : ROOT file
    """

    plot_vars = ["vtxxx", "vtyyy", "vtzzz"]
    plot_titles = ["Primary vertex x-position", "Primary vertex y-position", "Primary vertex z-position"]
    x_labels = [r"x [$\mu$m]", r"y [$\mu$m]", r"z [mm]"]
    y_labels = ["Entries" for _ in x_labels]
    var_multipliers = [TO_UM, TO_UM, TO_MM]

    x_bins = [
            np.arange(-3.0, 3.1, 0.1),
            np.arange(-3.0, 3.1, 0.1),
            np.arange(-10.0, 10.1, 0.2)
            ]


    ##
    ## 2D plot of x- and y-positions of PV
    ##
    fig, ax = plt.subplots(1,1)
    ax.tick_params(direction = "in", which = "both", top = True, bottom = True, left = True, right = True)

    h2 = hist.Hist(hist.axis.Regular(len(x_bins[0]), x_bins[0][0], x_bins[0][-1], label="x [$\mu$m]"),
                    hist.axis.Regular(len(x_bins[1]), x_bins[1][0], x_bins[1][-1], label="y [$\mu$m]"))
    h2.fill(tree["vtxxx"].array(library="np") * var_multipliers[0], tree["vtyyy"].array(library="np") * var_multipliers[1])
    hep.hist2dplot(h2, labels = None, cbar = False, ax = ax, norm = matplotlib.colors.LogNorm())
    ax.set_title("Primary vertex x-pos. versus y-pos.")
    fig.show()
    input("Press enter to continue")
    

    ##
    ## Overlay x and y position 1D histograms
    ##
    fig, ax = plt.subplots(1,1)
    ax.tick_params(direction = "in", which = "both", top = True, bottom = True, left = True, right = True)
    ax.set_xlabel(r"Transverse PV position [$\mu$m]")
    ax.set_ylabel("Entries / bin")

    vtx = tree.arrays(["vtxxx", "vtyyy"], library = "np")

    hx, bx = np.histogram(vtx["vtxxx"] * 1.0e6, x_bins[0])
    hy, by = np.histogram(vtx["vtyyy"] * 1.0e6, x_bins[0])

    hep.histplot(hx, bx, ax = ax, color = "r", lw = 2, label = "x")
    hep.histplot(hy, by, ax = ax, color = "b", lw = 2, label = "y")
    ax.legend(loc = "best")
    ax.set_title("Dimuon gun - Primary Vertex Reconstruction", size = 14)
    fig.show()
    input("Press enter to continue")

    ##
    ## cycle through PV position variables and make 1D histograms
    ##
    for ivar, var_name in enumerate(plot_vars) :

        ##
        ## setup the canvas
        ##
        fig, ax = plt.subplots(1,1)
        ax.tick_params(direction = "in", which = "both", top = True, bottom = True, left = True, right = True)
        ax.set_xlabel(x_labels[ivar])
        ax.set_ylabel(y_labels[ivar])
        ax.set_title(plot_titles[ivar])

        var_data = tree[var_name].array(library = "np")
        var_data *= var_multipliers[ivar]
        h, b = np.histogram(tree[var_name], x_bins[ivar])

        hep.histplot(h, b, ax = ax, color = "k")
        fig.show()
        input("Press enter to continue")

def main() :

    parser = ArgumentParser()
    parser.add_argument("input_file", metavar = "input-file", help = "Input LCTuple ROOT file")
    parser.add_argument("-t", "--tree", help = "Tree name", default = "MyLCTuple")
    parser.add_argument("-p", "--path", help = "Path to tree inside of input file", default = "/")
    args = parser.parse_args()

    pf = Path(args.input_file)
    if not pf.exists() or not pf.is_file() :
        raise RuntimeError(f"Provided input file {args.input_file} not found")

    with uproot.open(args.input_file) as rfile :
        tree_path = f"{args.path}{args.tree}"
        tree = rfile[tree_path]
        make_pv_plots(tree)

if __name__ == "__main__" :
    main()
