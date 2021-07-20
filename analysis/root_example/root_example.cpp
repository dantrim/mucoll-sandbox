//std/stl
#include <iostream>
#include <string>

//ROOT
#include "TCanvas.h"
#include "TH1F.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TFile.h"

int main() {
    TLorentzVector v(1,2,3,4);
    v.Print();
    return 0;
}
