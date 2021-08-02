//std/stdl
#include <iostream>
#include <string>
#include <sstream> // std::stringstream
#include <map>
#include <math.h> // sqrt
#include <vector>
#include <memory> // std::unique_ptr

//ROOT
#include "TROOT.h" // gROOT
#include "TCanvas.h"
#include "TH1F.h"
#include "TChain.h"
#include "TFile.h"


void print_usage(char* argv[]) {

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << " Draw simple histograms of LCTuple file" << std::endl;
    std::cout << std::endl;
    std::cout << " Usage: " << argv[0] << " -i <file> [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "  -i|--input         Input LCTuple ROOT file [REQUIRED]" << std::endl;
    std::cout << "  -t|--tree          Name of TTree [OPTIONAL, default: MyLCTuple]" << std::endl;
    std::cout << "  -l|--logy          Use log scale for y-axes [OPTIONAL, default: false]" << std::endl;
    std::cout << "  -h|--help          Print this help message and exit" << std::endl;
    std::cout << std::endl;
    std::cout << "------------------------------------------------" << std::endl;
}

int main(int argc, char* argv[]) {

    // take in the command line arguments
    std::string input_file{""};
    std::string tree_name{"MyLCTuple"};
    bool do_logy{false};

    for (size_t i = 1; i < argc; i++) {
        if     (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) { input_file = argv[++i]; }
        else if(strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tree") == 0) { tree_name = argv[++i]; }
        else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--logy") == 0) { do_logy = true; }
        else if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { print_usage(argv); return 0; }
        else {
            std::cout << argv[0] << " Unknown command line argument provided: " << argv[i] << std::endl;
            return 1;
        }
    } // i

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

    // primary vertex related variables
    float pv_xpos = 0;
    float pv_ypos = 0;
    float pv_zpos = 0;

    // variables for mc particles
    int* mcpdg = new int[5000000]; // ROOT is dumb so just allocate a ton of memory for these arrays
    float* mc_px = new float[5000000];
    float* mc_py = new float[5000000];
    int n_mc_particles_in_event;

    // attach the local variables to those in the TTree
    tree->SetBranchAddress("vtxxx", &pv_xpos);
    tree->SetBranchAddress("vtyyy", &pv_ypos);
    tree->SetBranchAddress("vtzzz", &pv_zpos);
    tree->SetBranchAddress("mcpdg", mcpdg);
    tree->SetBranchAddress("nmcp", &n_mc_particles_in_event);
    tree->SetBranchAddress("mcmox", mc_px);
    tree->SetBranchAddress("mcmoy", mc_py);

    // instantiate the histograms that we want to make
    std::unique_ptr<TH1F> h_vtxxx = std::make_unique<TH1F>("h_vtxxx", "Reco PV x-position;PV x-position [#mum];Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_vtxxx = std::make_unique<TCanvas>("c_vtxxx");
    std::unique_ptr<TH1F> h_vtyyy = std::make_unique<TH1F>("h_vtyyy", "Reco PV y-position;PV y-position [#mum];Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_vtyyy = std::make_unique<TCanvas>("c_vtyyy");
    std::unique_ptr<TH1F> h_vtzzz = std::make_unique<TH1F>("h_vtzzz", "Reco PV z-position;PV z-position [mm];Entries/bin", 50, -10, 10);
    std::unique_ptr<TCanvas> c_vtzzz = std::make_unique<TCanvas>("c_vtzzz");
    std::unique_ptr<TH1F> h_mcpdg = std::make_unique<TH1F>("h_mcpdg", "MC PDG Id.;PDG Id.;Entries/bin", 50, -25, 25);
    std::unique_ptr<TCanvas> c_mcpdg = std::make_unique<TCanvas>("c_mcpdg");

    // histograms filled for those particles with pT > 1.0 GeV
    std::unique_ptr<TH1F> h_mcpdg_ptcut = std::make_unique<TH1F>("h_mcpdg_ptcut", "MC PDG Id. (particles with pT > 1.0 GeV);PDG Id.;Entries/bin", 50, -25, 25);
    std::unique_ptr<TCanvas> c_mcpdg_ptcut = std::make_unique<TCanvas>("c_mcpdg_ptcut");

    // loop over the events in the TTree and fill the histograms
    for(int ievent = 0; ievent < tree->GetEntries(); ++ievent) {

        if(ievent % 100 == 0) std::cout << " *** Processing event " << ievent << " ***" << std::endl;

        // load in the current event (fills the local variables with those of the current events')
        tree->GetEntry(ievent);

        h_vtxxx->Fill(pv_xpos * 1e6);
        h_vtyyy->Fill(pv_ypos * 1e6);
        h_vtzzz->Fill(pv_zpos * 1e3);

        // loop over the MC particles
        for(size_t iparticle = 0; iparticle < n_mc_particles_in_event; ++iparticle) {
            int pdgid = mcpdg[iparticle];

            // skip things we don't care about
            if(std::abs(pdgid) > 2000) continue;

            h_mcpdg->Fill(pdgid);

            // MC particle momentum of this particle
            double px = mc_px[iparticle];
            double py = mc_px[iparticle];
            double transverse_momentum = sqrt(px*px + py*py); // GeV
            if(transverse_momentum > 1.0) {
                h_mcpdg_ptcut->Fill(pdgid);
            }
        } // loop over iparticle
    } // loop over ievent

    // draw the histograms
    c_vtxxx->cd();
    h_vtxxx->Draw("hist");
    h_vtxxx->SetLineColor(kBlack);
    h_vtxxx->SetLineWidth(2);
    c_vtxxx->SetLogy(do_logy);
    c_vtxxx->SaveAs("h_vtxxx_none.png");

    c_vtyyy->cd();
    c_vtyyy->SetLogy(do_logy);
    h_vtyyy->Draw("hist");
    h_vtyyy->SetLineColor(kBlack);
    h_vtyyy->SetLineWidth(2);
    c_vtyyy->SaveAs("h_vtyyy_none.png");

    c_vtzzz->cd();
    c_vtzzz->SetLogy(do_logy);
    h_vtzzz->Draw("hist");
    h_vtzzz->SetLineColor(kBlack);
    h_vtzzz->SetLineWidth(2);
    c_vtzzz->SaveAs("h_vtzzz_none.png");

    c_mcpdg->cd();
    c_mcpdg->SetLogy(do_logy);
    h_mcpdg->Draw("hist");
    h_mcpdg->SetLineColor(kBlack);
    h_mcpdg->SetLineWidth(2);
    c_mcpdg->SaveAs("h_mcpdg_none.png");

    c_mcpdg_ptcut->cd();
    c_mcpdg_ptcut->SetLogy(do_logy);
    h_mcpdg_ptcut->Draw("hist");
    h_mcpdg_ptcut->SetLineColor(kBlack);
    h_mcpdg_ptcut->SetLineWidth(2);
    c_mcpdg_ptcut->SaveAs("h_mcpdg_ptcut.png");


    // cleanup
    delete [] mc_px;
    delete [] mc_py;
    delete [] mcpdg;

    return 0;
}
