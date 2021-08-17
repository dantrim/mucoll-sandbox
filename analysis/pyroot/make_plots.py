#!/usr/bin/env python

###########################################
#
# Simple PyROOT-based histogramming
# 
# Basic usage:
#  $ python make_plots.py -i myFile.root
#
# Full list of available options:
#  $ python make_plots.py --help
#
###########################################

from argparse import ArgumentParser
from pathlib import Path
import os
import sys
import numpy
import ROOT

# constants/globals
DEFAULT_TTREE_NAME = "MyLCTuple"
VERBOSE = False

def get_variables() :
    """
    Return a Python dict containing the names of the variables to plot
    as the keys and another dict containing the configuration for
    the histograms that will be used for plotting the variable.
    Configuration items refer to things like the binning, axis labels, etc...
    """

    variable_dict = {
            "vtxxx": {"n_bins": 50, "x_low": -5, "x_high": 5, "x_label": "Reco PV x-position [#mum]", "y_label": "Entries/bin", "multiplier": 1e6},
            "mcvtx": {"n_bins": 50, "x_low": -5, "x_high": 5, "x_label": "MC x-position [#mum]", "y_label": "Entries/bin", "multiplier": 1e6},
            "vtyyy": {"n_bins": 50, "x_low": -5, "x_high": 5, "x_label": "Reco PV y-position [#mum]", "y_label": "Entries/bin", "multiplier": 1e6},
            "mcpdg": {"n_bins": 50, "x_low": -25, "x_high": 25, "x_label": "MC particle PDG Id", "y_label": "Entries/bin"},
            "vtxxx - mcvtx": {"n_bins": 50, "x_low": -10, "x_high": 10, "x_label": "PV x residual", "y_label": "Entries/bin", "multiplier": 1e6}
    }
 
    return variable_dict

def get_selections() :
    """
    Return a Python dict containing the names of the selections to apply
    to the events as the key and the values being the TCut string defining
    the selection to apply.
    """

    cut_dict = {
        "none": "",
        "true-muon": "abs(mcpdg)==13",
        "mc-pT-cut": "sqrt(mcmox*mcmox + mcmoy*mcmoy) > 1.0",
        "muon": "(mcpdg)==13",
        "true-muon-ptcut": "(mcpdg)=13 && (sqrt(mcmox*mcmox + mcmoy*mcmoy) > 1.0)"
    }

    return cut_dict



def make_plot(tree, variable_name, selection_name, do_logy = False) :

    ##
    ## get the configuration for this plot
    ##


    histogram_config = get_variables()[variable_name]
    selection_string = get_selections()[selection_name]

    ##
    ## create the TCanvas onto which we'll draw the histogram
    ##
    canvas_name = f"c_{variable_name}"
    if VERBOSE :
        print(f"\t\tcanvas name: {canvas_name}")
    canvas = ROOT.TCanvas(canvas_name)
    canvas.cd()

    ##
    ## create the name of the histogram that we'll fill
    ##
    histogram_name = f"h_{variable_name}"
    n_bins = histogram_config["n_bins"]
    x_low = histogram_config["x_low"]
    x_high = histogram_config["x_high"]

    histogram_definition = f"{histogram_name}({n_bins}, {x_low}, {x_high})"
    if VERBOSE :
        print(f"\t\thistogram_definition: {histogram_definition}")

    ##
    ## create the TTree::Draw string, which streams the selected variable
    ## from the TTree into the histogram using the following syntax:
    ## <variable_name> >> <histogram_name>, e.g.
    ##    MyLCTuple->Draw("mcpdg>>h_foo")
    ##
    if "multiplier" in histogram_config :
        variable_scaling = histogram_config["multiplier"]
        draw_cmd = f"{variable_name}*{variable_scaling}>>{histogram_definition}"
    else :
        draw_cmd = f"{variable_name}>>{histogram_definition}"
    if VERBOSE :
        print(f"\t\tdraw_cmd: {draw_cmd}")

    ##
    ## create the TCut from the selection string, which is used
    ## inside of the TTree::Draw command as the second argument, e.g.:
    ## for a selection of "sqrt(mcmox*mcmoyx + mcmoy*mcmoy) > 1.0", the 
    ## TCut string will be used in the TTree::Draw command as follows:
    ##    MyLCTuple->Draw("mcpdg>>h_foo", "sqrt(mcmox*mcmox + mcmoy*mcmoy) > 1.0")
    ##
    tcut_string = selection_string
    if VERBOSE :
        print(f"\t\ttcut_string: {tcut_string}")

    ##
    ## Now fill the histogram with the variable and with the selection applied
    ## to the events by calling the TTree::Draw function
    ##

    tree.Draw(draw_cmd, tcut_string, "HIST")

    

    ##
    ## Set some other asthetic features of the drawn histogram
    ##

    # grab the built histogram from ROOT by asking for it by name
    hist = ROOT.gROOT.FindObject(histogram_name)
    if hist is None :
        print(f"ERROR: Failed to find histogram named \"{histogram_name}\"")
        sys.exit(1)

    # set some aesthetics (colors, line widths, etc)
    hist.SetLineColor(ROOT.kBlack)
    hist.SetLineWidth(2)

    # set axis labels and titles, etc
    x_axis_label = histogram_config["x_label"]
    y_axis_label = histogram_config["y_label"]
    histogram_title = f"{variable_name} : {selection_name}"
    hist.GetXaxis().SetTitle(x_axis_label)
    hist.GetYaxis().SetTitle(y_axis_label)
    hist.SetTitle(histogram_title)

    ##
    ## Now save the plotted histogram as an image file (.png)
    ##
    output_name = f"plot_{variable_name}-{selection_name}.png"
    canvas.SaveAs(output_name)
    
def make_plots(tree, variable_name = "all", selection_name = "none", do_logy = False) :
    print(f"Making plots...")

    variables_dict = get_variables()
    if variable_name == "all" :
        variables = sorted(variables_dict.keys())
    else :
        variables = [variable_name]
    n_variables = len(variables)

    for ivariable, variable_name in enumerate(variables) :
        print(f"  [{ivariable+1:>2}/{n_variables:>2}] Plotting {variable_name}")
        make_plot(tree, variable_name, selection_name, do_logy)
        if VERBOSE :
            print(80 * '-')
   
def main() :

    ##
    ## define the command-line-interface and options
    ##
    parser = ArgumentParser(
            description = "Make 1D histograms from TTree input contained in ROOT file"
    )
    parser.add_argument("-i", "--input-file", type = str, required = True,
            help = "Input ROOT file")
    parser.add_argument("-t", "--tree-name", type = str,
            default = DEFAULT_TTREE_NAME,
            help = f"The name of the TTree in the input ROOT file (default: {DEFAULT_TTREE_NAME})"
    )
    parser.add_argument("--logy", default = False, action = "store_true",
            help = "Flag to set logarithmic y-axes for the plots"
    )
    parser.add_argument("-v", "--variable", default = "all", type = str,
            help = "Specify a specific variable to plot (must be defined in the variable dict function)"
    )
    parser.add_argument("-s", "--selection", default = "none", type = str,
            help = "Specify a selection to apply (must be defined in the selections dict function)"
    )
    parser.add_argument("--list", default = False, action = "store_true",
            help = "Print the variables available for plotting and exit"
    )
    parser.add_argument("-d", "--debug", action = "store_true", default = False,
            help = "Make more verbose output for debugging"
    )
    args = parser.parse_args()

    if args.list :
        ##
        ## print the variables and exit
        ##
        print(70 * "-")
        print("Variables available for plotting:")
        variables_dict = get_variables()
        n_variables = len(variables_dict)
        for ivariable, variable_name in enumerate(variables_dict) :
            print(f"  [{ivariable+1:>2}/{n_variables:>2}] \"{variable_name}\"")
        print(35 * "- ")
        print("Selections avaible to apply to events:")
        selections_dict = get_selections()
        n_selections = len(selections_dict)
        for iselection, selection_name in enumerate(selections_dict) :
            print(f"  [{iselection+1:>2}/{n_selections:>2}] {selection_name}: \"{selections_dict[selection_name]}\"")
        print(70 * "-")
        sys.exit(0)

    ##
    ## check that the input file exists
    ##
    p_input = Path(args.input_file)
    if not p_input.exists() or not p_input.is_file() :
        print(f"ERROR: Provided input file \"{args.input_file}\" does not exist, exiting!")
        sys.exit(1)

    ##
    ## confirm that the user-specified variable name is defined in the
    ## variables dictionary defined above
    ##
    if args.variable != "all" and args.variable not in get_variables() :
        print(f"ERROR: Requested variable \"{args.variable}\" is not specified in defined variables dict")
        sys.exit(1)

    ##
    ## get the TTree
    ##
    tree = ROOT.TChain(args.tree_name)
    tree.AddFile(args.input_file)

    n_entries = tree.GetEntries()
    if n_entries <= 0 :
        print(f"ERROR: TTree with name \"{args.tree_name}\" is empty in provided input file \"{args.input_file}\"")
        sys.exit(1)

    global VERBOSE
    if args.debug :
        VERBOSE = True

    print(70 * "-")
    print(f"Input file              : {os.path.abspath(args.input_file)}")
    print(f"TTree name              : {args.tree_name}")
    print(f"Total number of events  : {n_entries}")
    print(f"Variable to plot        : \"{args.variable}\"")
    print(f"Selection               : {args.selection} = \"{get_selections()[args.selection]}\"")
    print(f"Debug output?           : {VERBOSE}")
    print(70 * "-")

    make_plots(tree, args.variable, args.selection, args.logy)
    
if __name__ == "__main__" :
    main()