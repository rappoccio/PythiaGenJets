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

  bool verbose = false;

  char * configfile = argv[1];
  char * outfile = argv[2];
  unsigned int nEvents = atol(argv[3]);

  // Define the AK8 jet finder.
  double R = 0.8, ptmin = 170.0, lepfrac = 0.9;
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);

  // Create Pythia instance. Read config from a text file. 
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
  const Int_t kMaxJet = 3;                        // Stores leading 3 jets
  const Int_t kMaxParticle = 500;                 // and up to 500 of their constituents (pt ordered by jet)
  Int_t nJet;
  Float_t jet_pt[kMaxJet];
  Float_t jet_eta[kMaxJet];
  Float_t jet_phi[kMaxJet];
  Float_t jet_m[kMaxJet];
  Int_t   jet_nc[kMaxJet];
  Int_t nParticle;
  Int_t   jet_ndx[kMaxParticle];
  Int_t   particle_ndx[kMaxParticle];
  TTree * T = new TTree("T","ev1 Tree");         // Allocate the tree, but DO NOT DELETE IT since ROOT takes ownership magically. 
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
    if ( verbose ) 
      std::cout << "Generating event " << iEvent << std::endl;

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

    nJet = 0;
    if ( jets.size() > 0 ) {
      auto ibegin = jets.begin();
      auto iend = jets.end();      
      for ( auto ijet=ibegin;ijet!=iend;++ijet ) {
	auto constituents = ijet->constituents();

	// Get the fraction of the jet originating from leptons.
	// This is to remove jets that are comprised entirely of isolated leptons
	// such as Z->ll.
	// We ignore jets with >90% of their energy from leptons. 
	unsigned nlepton = 0;
	auto lepp4 = fastjet::PseudoJet();
	for ( auto icon = constituents.begin(); icon != constituents.end(); ++icon ) {
	  auto const & py8part = pythia.event[ icon->user_index() ];
	  if ( std::abs( py8part.id() ) > 10 && std::abs(py8part.id()) < 16)
	    lepp4 += *icon;
	}
	if ( lepp4.e() / ijet->e() > lepfrac){
	  continue;
	}

	if ( verbose ) 
	  std::cout << "getting constituents for jet with pt " << ijet->pt() << ", nc = " << ijet->constituents().size() << std::endl;
	if ( nJet < kMaxJet ) { 
	  
	  jet_pt[nJet]=ijet->perp();
	  jet_eta[nJet]=ijet->eta();
	  jet_phi[nJet]=ijet->phi();
	  jet_m[nJet]=ijet->m();
	  jet_nc[nJet] = constituents.size();
	  ++nJet;
	  
	  if ( constituents.size() > 0 && nParticle + constituents.size() < kMaxParticle ) {	    
	    auto jbegin = constituents.begin();
	    auto jend = constituents.end();
	    for ( auto iparticle=jbegin; iparticle != jend;++iparticle ){
	      
	      auto index = iparticle->user_index();
	      particle_ndx[nParticle] = index;
	      jet_ndx[nParticle] = ijet-ibegin;
	      ++nParticle;
	    }
	  } else {
	    std::cout << "In event " << iEvent << ", too many particles to write out, ignoring constituents from jet " << ijet-ibegin << std::endl;
	    // std::cout << "jets size: " << jets.size() << std::endl;
	    // std::cout << "     ijet: " << ijet - ibegin << std::endl;
	    // std::cout << "     nJet: " << nJet << std::endl;
	    // std::cout << "       nc: " << constituents.size() << std::endl;
	    // for ( auto prjet = ibegin; prjet != iend; ++prjet ) {
	    //   char buff[1000];
	    //   sprintf(buff, "   %6d %6.2f %6.2f %6.2f %6.2f", prjet - ibegin, prjet->perp(), prjet->eta(), prjet->phi(), prjet->m() );
	    //   std::cout << buff << std::endl;
	    // }
	    // char ci; cin >> ci;
	  }
	}
      }
      if ( verbose ) 
	std::cout << "About to write" << std::endl;
      if ( nJet > 0 && jet_pt[0] > 170 ) {
	// Fill the pythia event into the TTree.
	T->Fill();
      }
      if ( verbose ) 
	std::cout << "Done writing." << std::endl;
    }

  // End event loop.
  }

  // Statistics on event generation.
  pythia.stat();

  //  Write tree.
  T->Write();
  file->Close();
  // DO NOT delete T. 

  // Done.
  return 0;
}
