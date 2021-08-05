//std/stdl
#include <iostream>
#include <string>
#include <sstream> // std::stringstream
#include <map>
#include <vector>
#include <memory> // std::unique_ptr

//ROOT
#include "TROOT.h" // gROOT
#include "TCanvas.h"
#include "TH1F.h"
#include "TChain.h"
#include "TFile.h"

///////////////////////////////////////////////////////
// this script fills histograms using the
// TTree::Draw command
///////////////////////////////////////////////////////

struct HistoConfig {
    std::string x_label;
    std::string y_label;
    float bin_width = -1;
    float left_edge = 0;
    float right_edge = -1;
    float variable_scaling = 1.0;
};

////////////////////////////////////////////////////////
// variables available for plotting
std::map<std::string, HistoConfig> plot_vars {
    {"vtxxx", {/*x-label*/"PV x-position [#mum]", /*y-label*/"Entries/bin", /*bin-width*/0.2, /*left edge of histogram*/-5, /*right edge*/5, /*multiplier for the variable*/1.0e6}},
    {"vtyyy", {"PV y-position [#mum]", "Entries/bin", 0.2, -5, 5, 1.0e6}},
    {"vtzzz", {"PV z-position [mm]", "Entries/bin", 0.5, -12, 12, 1.0e3}},
    {"mcpdg", {"MC particle PDG id.", "Entries/bin", 1.0, -30, 30, 1.0}},
    {"mcvtx", {"MC x-position [#mum]", "Entries/bin", 0.2, -5, 5, 1.0e6}}  //new
    {"vtxxx-mcvtx", {"MC x-position [#mum]", "Entries/bin", 0.2, -5, 5, 1.0e6}}  //new
};

////////////////////////////////////////////////////////
// selections to apply
std::map<std::string, std::string> cut_map {
    {"none", "abs(mcpdg) < 1e6"},
    {"true-muon-only", "abs(mcpdg) == 13"},
};

void print_usage(char* argv[]) {

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << " Draw simple histograms of LCTuple file" << std::endl;
    std::cout << std::endl;
    std::cout << " Usage: " << argv[0] << " -i <file> [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "  -i|--input         Input LCTuple ROOT file [REQUIRED]" << std::endl;
    std::cout << "  -t|--tree          Name of TTree [OPTIONAL, default: MyLCTuple]" << std::endl;
    std::cout << "  -c|--cut           Cut selection [OPTIONAL, default: none]" << std::endl;
    std::cout << "  -v|--var           Specify a specific variable to plot [OPTIONAL, default: all available]" << std::endl;
    std::cout << "  -l|--logy          Use log scale for y-axes [OPTIONAL, default: false]" << std::endl;
    std::cout << "  -p|--print-config  Print the variables and cuts available for plotting and exit" << std::endl;
    std::cout << "  -h|--help          Print this help message and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

int main(int argc, char* argv[]) {

    // take in the command line arguments
    std::string input_file{"muonGun_lctuple.root"};
    std::string tree_name{"MyLCTuple"};
    std::string selected_cut{"none"};
    std::string selected_variable{""};
    bool do_logy{false};
    bool print_config_only{false};

    for (size_t i = 1; i < argc; i++) {
        if     (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) { input_file = argv[++i]; }
        else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0) { tree_name = argv[++i]; }
        else if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cut") == 0) { selected_cut = argv[++i]; }
        else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--var") == 0) { selected_variable = argv[++i]; }
        else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--logy") == 0) { do_logy = true; }
        else if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--print-vars") == 0) { print_config_only = true; }
        else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { print_usage(argv); return 0; }
        else {
            std::cout << argv[0] << " Unknown command line argument provided: " << argv[i] << std::endl;
            return 1;
        }
    } // i

    if (print_config_only) {
        std::cout << "Available variables for plotting from input LCTuple file:" << std::endl;
        size_t var_num = 0;
        size_t n_vars = plot_vars.size();
        for(auto [variable, config] : plot_vars) {
            std::cout << "  [" << ++var_num << "/" << n_vars << "] \"" << variable << "\"" << std::endl;
        }

        std::cout << "Available selections/cuts to apply:" << std::endl;
        size_t cut_num = 0;
        size_t n_cuts = cut_map.size();
        for(auto [name, cut_string] : cut_map) {
            std::cout << "  [" << ++cut_num << "/" << n_cuts << "] " << name << ": \"" << cut_string << "\"" << std::endl;
        }
        return 0;
    }

    // check that the input file exists
    std::unique_ptr<TFile> root_file = std::make_unique<TFile>(input_file.c_str());
    if (root_file->IsZombie()) {
        std::cout << "ERROR: Failed to open provided input file \"" << input_file << "\"" << std::endl;
        return 1;
    }

    // load the TTree from the input file
    std::unique_ptr<TChain> tree = std::make_unique<TChain>(tree_name.c_str());
    tree->Add(input_file.c_str());
    std::cout << "Loaded TTree \"" << tree_name << "\" with " << tree->GetEntries() << " events" << std::endl;

    // loop over variables and make 1D histograms
    size_t n_vars_to_plot = plot_vars.size();
    for (const auto [variable_name, histo_config] : plot_vars) {

        // create the canvas
        std::string canvas_name = "c_" + variable_name;
        std::unique_ptr<TCanvas> c = std::make_unique<TCanvas>(canvas_name.c_str());
        c->cd();

        // logarithmic y-axis scale?
        if(do_logy) {
            c->SetLogy(true);
        }

        // draw command
        std::string histo_name = "h_" + variable_name;
        std::stringstream draw_cmd;
        draw_cmd << variable_name << "*" << histo_config.variable_scaling <<  ">>" <<  histo_name;
        if (histo_config.bin_width > 0) {
            size_t n_bins = (histo_config.right_edge - histo_config.left_edge) / histo_config.bin_width;
            draw_cmd << "(" << n_bins << "," << histo_config.left_edge << "," <<  histo_config.right_edge << ")";
        }
        std::cout << draw_cmd.str() << std::endl;
        std::cout << cut_map.at(selected_cut) << std::endl;


        // fill the histogram
        tree->Draw(draw_cmd.str().c_str(), cut_map.at(selected_cut).c_str(), "HIST");

        // specify how the histogram looks
        TH1F* h = static_cast<TH1F*>(gROOT->FindObject(histo_name.c_str()));
        h->SetLineColor(kBlack);
        h->SetLineWidth(2);
        h->GetXaxis()->SetTitle(histo_config.x_label.c_str());
        h->GetYaxis()->SetTitle(histo_config.y_label.c_str());
        h->SetTitle("");

        // save the output as a png
        std::stringstream save_name;
        save_name << histo_name << "_" << selected_cut << ".png";
        c->SaveAs(save_name.str().c_str());
    }

    return 0;
}
