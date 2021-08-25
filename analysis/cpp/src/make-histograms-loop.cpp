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
    std::string input_file{"muonGun_lctuple.root"};
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

    /*
      Here we declare local variables that will be used to hold the data
      for each of the variables we wish to read from the input
      TTree file. There must be one locally declared variable for
      each of the variables in the TTree that we wish to read.
      */


    // reco-level primary vertex related variables
    float pv_xpos = 0;
    float pv_ypos = 0;
    float pv_zpos = 0;


    // truth-level primary vertex proxies (we take the vertex positions from the truth muons from the muon gun as a proxy)
     float truth_pv_xpos = 0;
     float truth_pv_ypos = 0;
     float truth_pv_zpos = 0;

     // mc particle related variables
     const size_t mc_particle_array_size = 5000000;
     int* mcpdg = new int[mc_particle_array_size]; // pdg id
     int* mcgst = new int[mc_particle_array_size]; // MC generator status (1 == generated particle, 0 = decay ??)
     float* mc_px = new float[mc_particle_array_size]; // x momentum
     float* mc_py = new float[mc_particle_array_size]; // y momentum
     float* mc_vtx_xpos = new float[mc_particle_array_size]; // x coordinate of MC particle vertex
     float* mc_vtx_ypos = new float[mc_particle_array_size]; // y coordinate of MC particle vertex
     float* mc_vtx_zpos = new float[mc_particle_array_size]; // z coordinate of MC particle vertex
     int n_mc_particles_in_event = 0; // the number of MC particles in the event

     float mu_vtx_x_pos = 0;
     float antimu_vtx_x_pos = 0;
     float mu_vtx_y_pos = 0;
     float antimu_vtx_y_pos = 0;
     float mu_vtx_z_pos = 0;
     float antimu_vtx_z_pos = 0;


    // attach the local variables to those in the TTree
    tree->SetBranchAddress("nmcp" , &n_mc_particles_in_event);
    tree->SetBranchAddress("vtxxx", &pv_xpos);
    tree->SetBranchAddress("mcvtx", mc_vtx_xpos);

    tree->SetBranchAddress("vtyyy", &pv_ypos);
    tree->SetBranchAddress("mcvty", mc_vtx_ypos);

    tree->SetBranchAddress("vtyyy", &pv_zpos);
    tree->SetBranchAddress("mcvty", mc_vtx_zpos);

    // we need to load the "nmcp" varialbe so that we know how many MC particles there are in an event
    //tree->SetBranchAddress("vtzzz", &pv_zpos);
    tree->SetBranchAddress("mcpdg", mcpdg);
    tree->SetBranchAddress("mcgst", mcgst);
    //tree->SetBranchAddress("mcmox", mc_px);
    //tree->SetBranchAddress("mcmoy", mc_py);

    // instantiate the histograms that we want to make (x)
    std::unique_ptr<TH1F> h_vtxxx = std::make_unique<TH1F>("h_vtxxx", "Reco PV x-position;PV x-position ;Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_vtxxx = std::make_unique<TCanvas>("c_vtxxx");
    //mcvtx_muon: truth level vertex xposition 
    std::unique_ptr<TH1F> h_mcvtx_muon = std::make_unique<TH1F>("h_mcvtx_muon", "Muon Truth-level Vertex x-position;Vertex x-position;Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_mcvtx_muon = std::make_unique<TCanvas>("c_mcvtx_muon");
    //mu_mcvtx_diff: difference between th MC vertex position of muon and antimuon 
    std::unique_ptr<TH1F> h_mu_mcvtx_diff = std::make_unique<TH1F>("h_mu_mcvtx_diff", "Difference between Truth-level Muon and Anti-Muon Production Vertices;#Delta Vertex x-position [#mum];Entries/bin", 20, -0.2, 0.2);
    std::unique_ptr<TCanvas> c_mu_mcvtx_diff = std::make_unique<TCanvas>("c_mu_mcvtx_diff");
    //mc_muon_count_x: # of muons and anti-muons with generator status (=1) in the event (1.5 muon, -1.5 antimuon). 
    std::unique_ptr<TH1F> h_mc_muon_count_x = std::make_unique<TH1F>("h_mc_muon_count_x", "Number of MC (anti-)muons per event -X;Number of (anti-)muons;Entries/bin", 10, -5, 5);
    std::unique_ptr<TCanvas> c_mc_muon_count_x = std::make_unique<TCanvas>("c_mc_muon_count_x");

    std::unique_ptr<TH1F> h_pvxresidual = std::make_unique<TH1F>("h_pvxresidual", "PV X Residual;Residual;Entries/bin", 100, -2.5, 2.5);
    std::unique_ptr<TCanvas> c_pvxresidual = std::make_unique<TCanvas>("c_pvxresidual");
    
    // instantiate the histograms that we want to make (y)
    std::unique_ptr<TH1F> h_vtyyy = std::make_unique<TH1F>("h_vtyyy", "Reco PV y-position;PV y-position [#mum];Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_vtyyy = std::make_unique<TCanvas>("c_vtyyy");

    std::unique_ptr<TH1F> h_mcvty_muon = std::make_unique<TH1F>("h_mcvty_muon", "Muon Truth-level Vertex y-position;Vertex y-position;Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_mcvty_muon = std::make_unique<TCanvas>("c_mcvty_muon");

    std::unique_ptr<TH1F> h_mu_mcvty_diff = std::make_unique<TH1F>("h_mu_mcvty_diff", "Difference between Truth-level Muon and Anti-Muon Production Vertices;#Delta Vertex y-position [#mum];Entries/bin", 20, -0.2, 0.2);
    std::unique_ptr<TCanvas> c_mu_mcvty_diff = std::make_unique<TCanvas>("c_mu_mcvty_diff");

    std::unique_ptr<TH1F> h_mc_muon_count_y = std::make_unique<TH1F>("h_mc_muon_count_y", "Number of MC (anti-)muons per event -Y;Number of (anti-)muons;Entries/bin", 10, -5, 5);
    std::unique_ptr<TCanvas> c_mc_muon_count_y = std::make_unique<TCanvas>("c_mc_muon_count_y");

    std::unique_ptr<TH1F> h_pvyresidual = std::make_unique<TH1F>("h_pvyresidual", "PV Y Residual;Residual;Entries/bin", 50, -2.5, 2.5);
    std::unique_ptr<TCanvas> c_pvyresidual = std::make_unique<TCanvas>("c_pvyresidual");

    // instantiate the histograms that we want to make (z)
    std::unique_ptr<TH1F> h_vtzzz = std::make_unique<TH1F>("h_vtzzz", "Reco PV z-position;PV z-position ;Entries/bin", 50, -20, 20); //TODO
    std::unique_ptr<TCanvas> c_vtzzz = std::make_unique<TCanvas>("c_vtzzz");

    std::unique_ptr<TH1F> h_mcvtz_muon = std::make_unique<TH1F>("h_mcvtz_muon", "Muon Truth-level Vertex z-position;Vertex z-position;Entries/bin", 50, -5, 5);
    std::unique_ptr<TCanvas> c_mcvtz_muon = std::make_unique<TCanvas>("c_mcvtz_muon");

    std::unique_ptr<TH1F> h_mu_mcvtz_diff = std::make_unique<TH1F>("h_mu_mcvtz_diff", "Difference between Truth-level Muon and Anti-Muon Production Vertices;#Delta Vertex y-position [#mum];Entries/bin", 20, -0.2, 0.2);
    std::unique_ptr<TCanvas> c_mu_mcvtz_diff = std::make_unique<TCanvas>("c_mu_mcvtz_diff");

    std::unique_ptr<TH1F> h_mc_muon_count_z = std::make_unique<TH1F>("h_mc_muon_count_z", "Number of MC (anti-)muons per event -Z;Number of (anti-)muons;Entries/bin", 10, -5, 5);
    std::unique_ptr<TCanvas> c_mc_muon_count_z = std::make_unique<TCanvas>("c_mc_muon_count_z");

    std::unique_ptr<TH1F> h_pvzresidual = std::make_unique<TH1F>("h_pvzresidual", "PV Z Residual;Residual;Entries/bin", 50, -2.5, 2.5);
    std::unique_ptr<TCanvas> c_pvzresidual = std::make_unique<TCanvas>("c_pvzresidual");

    // std::unique_ptr<TH1F> h_vtzzz = std::make_unique<TH1F>("h_vtzzz", "Reco PV z-position;PV z-position [mm];Entries/bin", 50, -10, 10);
    // std::unique_ptr<TCanvas> c_vtzzz = std::make_unique<TCanvas>("c_vtzzz");
    // std::unique_ptr<TH1F> h_mcpdg = std::make_unique<TH1F>("h_mcpdg", "MC PDG Id.;PDG Id.;Entries/bin", 50, -30, 30);
    // std::unique_ptr<TCanvas> c_mcpdg = std::make_unique<TCanvas>("c_mcpdg");
    


    // // histograms filled for those particles with pT > 1.0 GeV
    // std::unique_ptr<TH1F> h_mcpdg_ptcut = std::make_unique<TH1F>("h_mcpdg_ptcut", "MC PDG Id. (particles with pT > 1.0 GeV);PDG Id.;Entries/bin", 40, -20, 20);
    // std::unique_ptr<TCanvas> c_mcpdg_ptcut = std::make_unique<TCanvas>("c_mcpdg_ptcut");

    // loop over the events in the TTree and fill the histograms
    for(int ievent = 0; ievent < tree->GetEntries(); ++ievent) {

        if(ievent % 100 == 0) std::cout << " *** Processing event " << ievent << " ***" << std::endl;

        // load in the current event (fills the local variables with those of the current events')
        tree->GetEntry(ievent);

        h_vtxxx->Fill(pv_xpos * 1e6);
        h_vtyyy->Fill(pv_ypos * 1e6);
        h_vtzzz->Fill(pv_zpos * 1e3);
        //h_mcvtx->Fill(pv_xpos * 1e6);

        // loop over the MC particles
        float mu_vtx_x_pos = 0;
        float antimu_vtx_x_pos = 0;
        float pvxresidual = 0;

        float mu_vtx_y_pos = 0;
        float antimu_vtx_y_pos = 0;
        float pvyresidual = 0;

        float mu_vtx_z_pos = 0;
        float antimu_vtx_z_pos = 0;
        float pvzresidual = 0;

        unsigned n_muons_in_event = 0;
         for(size_t iparticle = 0; iparticle < n_mc_particles_in_event; ++iparticle) {
             int pdgid = mcpdg[iparticle];
             int generator_status = mcgst[iparticle];

             // skip things we don't care about (they are hadrons/baryons/etc)
             if(std::abs(pdgid) > 2000) continue;

             // let's only grab the muons with generator status == 1


             if(std::abs(pdgid) == 13 && generator_status == 1) {
                    if (pdgid == 13){
                     mu_vtx_x_pos = mc_vtx_xpos[iparticle] * 1e6;
                     h_mcvtx_muon->Fill(mu_vtx_x_pos);
                     h_mc_muon_count_x->Fill(1.5);

                     mu_vtx_y_pos = mc_vtx_ypos[iparticle] * 1e6;
                     h_mcvty_muon->Fill(mu_vtx_y_pos);
                     h_mc_muon_count_y->Fill(1.5);

                     mu_vtx_z_pos = mc_vtx_zpos[iparticle] * 1e3;
                     h_mcvtz_muon->Fill(mu_vtx_z_pos);
                     h_mc_muon_count_z->Fill(1.5);

                 } 
                    else if(pdgid == -13) {
                     antimu_vtx_x_pos = mc_vtx_xpos[iparticle] * 1e6;
                     h_mc_muon_count_x->Fill(-1.5);
                     antimu_vtx_y_pos = mc_vtx_ypos[iparticle] * 1e6;
                     h_mc_muon_count_y->Fill(-1.5);
                     antimu_vtx_z_pos = mc_vtx_zpos[iparticle] * 1e3;
                     h_mc_muon_count_z->Fill(-1.5);
                 }
             }

             //h_mcpdg->Fill(pdgid);

             //// MC particle momentum of this particle
             //double px = mc_px[iparticle];
             //double py = mc_px[iparticle];
             //double transverse_momentum = sqrt(px*px + py*py); // GeV
             //if(transverse_momentum > 1.0) {
             //    h_mcpdg_ptcut->Fill(pdgid);
             //}
         } // loop over iparticle

         //h_mc_muon_count->Fill(n_muons_in_event);
         float delta_mu_vtx_x = mu_vtx_x_pos - antimu_vtx_x_pos;
         h_mu_mcvtx_diff->Fill(delta_mu_vtx_x);
         pvxresidual = (pv_xpos - mu_vtx_x_pos)*1e6;
         
         float delta_mu_vtx_y = mu_vtx_y_pos - antimu_vtx_y_pos;
         h_mu_mcvty_diff->Fill(delta_mu_vtx_y);
         pvyresidual = (pv_ypos - mu_vtx_y_pos)*1e6;

         float delta_mu_vtx_z = mu_vtx_z_pos - antimu_vtx_z_pos;
         h_mu_mcvty_diff->Fill(delta_mu_vtx_z);
         pvzresidual = (pv_zpos - mu_vtx_z_pos)*1e6;



         // std::cout << "event " << ievent << "  residual = " << pvxresidual << std::endl;

         h_pvxresidual->Fill(pvxresidual);
         h_pvyresidual->Fill(pvyresidual);
         h_pvzresidual->Fill(pvzresidual);

         } // loop over ievent

     // draw the histograms
     c_vtxxx->cd();
     h_vtxxx->Draw("hist");
     h_vtxxx->SetLineColor(kBlack);
     h_vtxxx->SetLineWidth(2);
     c_vtxxx->SetLogy(do_logy);
     c_vtxxx->SaveAs("h_vtxxx_none.png");

     c_mcvtx_muon->cd();
     h_mcvtx_muon->Draw("hist");
     h_mcvtx_muon->SetLineColor(kBlack);
     h_mcvtx_muon->SetLineWidth(2);
     c_mcvtx_muon->SetLogy(do_logy);
     c_mcvtx_muon->SaveAs("h_mcvtx_muon.png");

     c_mc_muon_count_x->cd();
     h_mc_muon_count_x->Draw("hist");
     h_mc_muon_count_x->SetLineColor(kBlack);
     h_mc_muon_count_x->SetLineWidth(2);
     c_mc_muon_count_x->SetLogy(do_logy);
     c_mc_muon_count_x->SaveAs("h_mc_muon_count_x.png");

     c_mu_mcvtx_diff->cd();
     h_mu_mcvtx_diff->Draw("hist");
     h_mu_mcvtx_diff->SetLineColor(kBlack);
     h_mu_mcvtx_diff->SetLineWidth(2);
     c_mu_mcvtx_diff->SetLogy(do_logy);
     c_mu_mcvtx_diff->SaveAs("h_mu_mcvtx_diff.png");

    
     c_pvxresidual->cd();
     h_pvxresidual->Draw("hist");
     h_pvxresidual->SetLineColor(kBlack);
     h_pvxresidual->SetLineWidth(2);
     c_pvxresidual->SetLogy(do_logy);
     c_pvxresidual->SaveAs("h_pvxresidual.png");

     c_vtyyy->cd();
     h_vtyyy->Draw("hist");
     h_vtyyy->SetLineColor(kBlack);
     h_vtyyy->SetLineWidth(2);
     c_vtyyy->SetLogy(do_logy);
     c_vtyyy->SaveAs("h_vtyyy_none.png");

     c_mcvty_muon->cd();
     h_mcvty_muon->Draw("hist");
     h_mcvty_muon->SetLineColor(kBlack);
     h_mcvty_muon->SetLineWidth(2);
     c_mcvty_muon->SetLogy(do_logy);
     c_mcvty_muon->SaveAs("h_mcvty_muon.png");

     c_mc_muon_count_y->cd();
     h_mc_muon_count_y->Draw("hist");
     h_mc_muon_count_y->SetLineColor(kBlack);
     h_mc_muon_count_y->SetLineWidth(2);
     c_mc_muon_count_y->SetLogy(do_logy);
     c_mc_muon_count_y->SaveAs("h_mc_muon_count_y.png");

     c_mu_mcvty_diff->cd();
     h_mu_mcvty_diff->Draw("hist");
     h_mu_mcvty_diff->SetLineColor(kBlack);
     h_mu_mcvty_diff->SetLineWidth(2);
     c_mu_mcvty_diff->SetLogy(do_logy);
     c_mu_mcvty_diff->SaveAs("h_mu_mcvty_diff.png");

    
     c_pvyresidual->cd();
     h_pvyresidual->Draw("hist");
     h_pvyresidual->SetLineColor(kBlack);
     h_pvyresidual->SetLineWidth(2);
     c_pvyresidual->SetLogy(do_logy);
     c_pvyresidual->SaveAs("h_pvyresidual.png");

     c_vtzzz->cd();
     h_vtzzz->Draw("hist");
     h_vtzzz->SetLineColor(kBlack);
     h_vtzzz->SetLineWidth(2);
     c_vtzzz->SetLogy(do_logy);
     c_vtzzz->SaveAs("h_vtzzz_none.png");

     c_mcvtz_muon->cd();
     h_mcvtz_muon->Draw("hist");
     h_mcvtz_muon->SetLineColor(kBlack);
     h_mcvtz_muon->SetLineWidth(2);
     c_mcvtz_muon->SetLogy(do_logy);
     c_mcvtz_muon->SaveAs("h_mcvtz_muon.png");

     c_mc_muon_count_z->cd();
     h_mc_muon_count_z->Draw("hist");
     h_mc_muon_count_z->SetLineColor(kBlack);
     h_mc_muon_count_z->SetLineWidth(2);
     c_mc_muon_count_z->SetLogy(do_logy);
     c_mc_muon_count_z->SaveAs("h_mc_muon_count_z.png");

     c_mu_mcvtz_diff->cd();
     h_mu_mcvtz_diff->Draw("hist");
     h_mu_mcvtz_diff->SetLineColor(kBlack);
     h_mu_mcvtz_diff->SetLineWidth(2);
     c_mu_mcvtz_diff->SetLogy(do_logy);
     c_mu_mcvtz_diff->SaveAs("h_mu_mcvtz_diff.png");

    
     c_pvzresidual->cd();
     h_pvzresidual->Draw("hist");
     h_pvzresidual->SetLineColor(kBlack);
     h_pvzresidual->SetLineWidth(2);
     c_pvzresidual->SetLogy(do_logy);
     c_pvzresidual->SaveAs("h_pvzresidual.png");
    
    

    // c_mcvtx->cd();
    // h_mcvtx->Draw("hist");
    // h_mcvtx->SetLineColor(kBlack);
    // h_mcvtx->SetLineWidth(2);
    // c_mcvtx->SetLogy(do_logy);
    // c_mcvtx->SaveAs("h_mcvtx_none.png");

    // c_vtyyy->cd();
    // c_vtyyy->SetLogy(do_logy);
    // h_vtyyy->Draw("hist");
    // h_vtyyy->SetLineColor(kBlack);
    // h_vtyyy->SetLineWidth(2);
    // c_vtyyy->SaveAs("h_vtyyy_none.png");

    // c_vtzzz->cd();
    // c_vtzzz->SetLogy(do_logy);
    // h_vtzzz->Draw("hist");
    // h_vtzzz->SetLineColor(kBlack);
    // h_vtzzz->SetLineWidth(2);
    // c_vtzzz->SaveAs("h_vtzzz_none.png");

    // c_mcpdg->cd();
    // c_mcpdg->SetLogy(do_logy);
    // h_mcpdg->Draw("hist");
    // h_mcpdg->SetLineColor(kBlack);
    // h_mcpdg->SetLineWidth(2);
    // c_mcpdg->SaveAs("h_mcpdg_none.png");

    // c_mcpdg_ptcut->cd();
    // c_mcpdg_ptcut->SetLogy(do_logy);
    // h_mcpdg_ptcut->Draw("hist");
    // h_mcpdg_ptcut->SetLineColor(kBlack);
    // h_mcpdg_ptcut->SetLineWidth(2);
    // c_mcpdg_ptcut->SaveAs("h_mcpdg_ptcut.png");


    // cleanup
    delete [] mc_px;
    delete [] mc_py;
    delete [] mcpdg;

    return 0;
}
