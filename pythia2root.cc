// main92.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program.
// Modified by Rene Brun and Axel Naumann to put the Pythia::event
// into a TTree.

// Header file to access Pythia 8 program elements.
#include "Pythia8/Pythia.h"
#include "fastjet/ClusterSequence.hh"

// ROOT, for saving Pythia events as trees in a file.
#include "TTree.h"
#include "TFile.h"

using namespace Pythia8;

int main(int argc, char ** argv) {

  if ( argc < 4 ) {
    std::cout << "usage: " << argv[0] << " config_file root_file n_events" << std::endl;
    return 0;
  }

  char * configfile = argv[1];
  char * outfile = argv[2];
  unsigned int nEvents = atol(argv[3]);

  // Define the AK8 jet finder.
  double R = 0.8, ptmin = 170.0;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // Create Pythia instance and set it up to generate hard QCD processes
  // above pTHat = 20 GeV for pp collisions at 14 TeV.
  Pythia pythia;
  std::ifstream config( configfile );
  while (!config.eof() ) {
    std::string line;
    std::getline( config, line );
    if ( line[0] != '!' && line != "" && line != "\n" ){
      pythia.readString(line);
    }
  }
  pythia.init();

  // Set up the ROOT TFile and TTree.
  TFile *file = TFile::Open(outfile,"recreate");
  Event *event = &pythia.event;
  const Int_t kMaxJet = 5;
  const Int_t kMaxParticle = 100;
  Int_t nJet;
  Float_t jet_pt[kMaxJet];
  Float_t jet_eta[kMaxJet];
  Float_t jet_phi[kMaxJet];
  Float_t jet_m[kMaxJet];
  Int_t   jet_nc[kMaxJet];
  Int_t nParticle;
  Int_t   jet_ndx[kMaxParticle];
  Int_t   particle_ndx[kMaxParticle];
  TTree *T = new TTree("T","ev1 Tree");
  T->Branch("event",&event);
  T->Branch("nJet",    &nJet,  "nJet/I");
  T->Branch("nParticle",    &nParticle,  "nParticle/I");
  T->Branch("jet_pt",  &jet_pt,  "jet_pt[nJet]/F");
  T->Branch("jet_eta", &jet_eta, "jet_eta[nJet]/F");
  T->Branch("jet_phi", &jet_phi, "jet_phi[nJet]/F");
  T->Branch("jet_m",   &jet_m,   "jet_m[nJet]/F");
  T->Branch("jet_nc",  &jet_nc,  "jet_nc[nJet]/I");
  T->Branch("particle_ndx", &particle_ndx, "particle_ndx[nParticle]/I");   // Index of particles clustered. 
  T->Branch("jet_ndx", &jet_ndx, "jet_ndx[nParticle]/I");                  // Jet this particle came from. 

 // Begin event loop. Generate event; skip if generation aborted.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (!pythia.next()) continue;

    nJet=0;
    nParticle=0;
    for ( auto x : jet_pt ) x=-1.0;
    for ( auto x : jet_eta ) x=-1.0;
    for ( auto x : jet_phi ) x=-1.0;
    for ( auto x : jet_m ) x=-1.0;
    for ( auto x : jet_nc ) x=-1;
    for ( auto x : particle_ndx ) x=-1;
    for ( auto x : jet_ndx ) x=-1;

    // Create AK8 jets with pt > 170 GeV
    std::vector<fastjet::PseudoJet> fj_particles;
    for (int i = 0; i < event->size(); ++i){      
      if (pythia.event[i].isFinal() ) {
	auto const & p = pythia.event[i];
	fj_particles.emplace_back( p.px(), p.py(), p.pz(), p.e()  );
	fj_particles.back().set_user_index( i );
      }
    }
    fastjet::ClusterSequence cs(fj_particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(cs.inclusive_jets(ptmin));
    nJet = jets.size();
    if ( jets.size() > 0 ) {
      auto ibegin=jets.begin();
      for ( auto ijet=ibegin;ijet!=jets.end();++ijet ) {
	if ( ijet - ibegin < kMaxJet ) {
	  jet_pt[ijet - ibegin]=ijet->perp();
	  jet_eta[ijet - ibegin]=ijet->eta();
	  jet_phi[ijet - ibegin]=ijet->phi();
	  jet_m[ijet - ibegin]=ijet->m();
	  jet_nc[ijet - ibegin] = ijet->constituents().size();
	  if ( ijet->constituents().size() > 0 ) {
	    auto jbegin = ijet->constituents().begin();
	    for ( auto iparticle=jbegin; iparticle != ijet->constituents().end();++iparticle ){
	      if ( iparticle-jbegin < kMaxParticle ) {
		auto index = iparticle->user_index();
		particle_ndx[nParticle] = index;     // remember : postincrement means nParticle=0 first, then increments to 1
		jet_ndx[nParticle++] = ijet-ibegin;    //            so for the first one we don't increment, the second we postincrment
	      }
	    }
	  }
	}
      }
      if ( jets[0].perp() > 170 ) {
	// Fill the pythia event into the TTree.
	T->Fill();
      }
    }

  // End event loop.
  }

  // Statistics on event generation.
  pythia.stat();

  //  Write tree.
  T->Print();
  T->Write();
  delete file;

  // Done.
  return 0;
}
