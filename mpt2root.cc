// main92.cc is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is a simple test program.
// Modified by Rene Brun and Axel Naumann to put the Pythia::event
// into a TTree.

// Header file to access Pythia 8 program elements.
#include "Pythia8/Pythia.h"
#include "Pythia8/Basics.h"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/contrib/Nsubjettiness.hh" // In external code, this should be fastjet/contrib/Nsubjettiness.hh
#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/contrib/NjettinessPlugin.hh"


// ROOT, for saving Pythia events as trees in a file.
#include "TTree.h"
#include "TFile.h"

using namespace Pythia8;

class CompareIndex {
public:
  CompareIndex( fastjet::PseudoJet j ) : i_(j.user_index()){
  }
  CompareIndex( unsigned int j = 0 ) : i_(j){
  }

  bool operator() ( fastjet::PseudoJet const & j ) const { return j.user_index() == i_; }
  bool operator() ( unsigned int j ) const { return j == i_; }

protected : 
  unsigned int i_; 
};

int main(int argc, char ** argv) {

  if ( argc < 4 ) {
    std::cout << "usage: " << argv[0] << " config_file root_file n_events <optional: seed (-1 = default, 0=use time, or input your own)> " << std::endl;
    return 0;
  }


  // Define the AK4 jet finder.
  double R = 0.4, ptmin = 20.0, lepfrac = 0.9;
  bool exclude_leptons_from_jets = true; 
  fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, R);
  fastjet::JetDefinition mpt_def(fastjet::kt_algorithm, 1000);

  bool verbose = false;

  char * configfile = argv[1];
  char * outfile = argv[2];
  unsigned int nEvents = atol(argv[3]);
  long seed = -1; 
  if ( argc > 5 ) {
    seed = atol(argv[4]);
  }
  
  double z_cut = 0.10;
  double beta  = 0.0;
  fastjet::contrib::SoftDrop sd(beta, z_cut);

  // Create Pythia instance. Read config from a text file. 
  Pythia pythia;
  char buff[1000];
  sprintf(buff, "Random:seed = %d", seed);
  pythia.readString("Random:setSeed = on");
  pythia.readString(buff);
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
  const Int_t kMaxJet = 10;                       // Stores leading 10 jets
  const Int_t kMaxGen = 5000;                     // and 1000 of the generator particles
  ULong64_t eventNum = 0;                         // need to store an event number for uproot access
  Float_t mpt_pt=0.;
  Float_t mpt_phi=0.;
  Float_t mpt_eta=0.;
  Float_t mpt_ptsd=0.;
  Float_t mpt_phisd=0.;

  Int_t nJet=0;
  Float_t jet_pt[kMaxJet];
  Float_t jet_eta[kMaxJet];
  Float_t jet_phi[kMaxJet];
  Float_t jet_m[kMaxJet];
  Float_t jet_msd[kMaxJet];

  Int_t   nGen=0;
  Int_t   gen_orig[kMaxGen]; // original index for debugging
  Float_t gen_pt[kMaxGen];
  Float_t gen_eta[kMaxGen];
  Float_t gen_phi[kMaxGen];
  Float_t gen_m[kMaxGen];
  Int_t   gen_id[kMaxGen];
  Int_t   gen_flags[kMaxGen];
  Int_t   gen_status[kMaxGen];
  Int_t   gen_mother1[kMaxGen];
  Int_t   gen_mother2[kMaxGen];
  Int_t   gen_daughter1[kMaxGen];
  Int_t   gen_daughter2[kMaxGen];
  Int_t   gen_col[kMaxGen];
  Float_t   gen_vxx[kMaxGen];
  Float_t   gen_vyy[kMaxGen];
  Float_t   gen_vzz[kMaxGen];
  Float_t   gen_tau[kMaxGen];

  TTree * T = new TTree("T","ev1 Tree");         // Allocate the tree, but DO NOT DELETE IT since ROOT takes ownership magically.
  T->Branch("eventNum",    &eventNum,  "eventNum/l");
  T->Branch("mpt_pt",  &mpt_pt,  "mpt_pt/F");
  T->Branch("mpt_phi",  &mpt_phi,  "mpt_phi/F");
  T->Branch("mpt_ptsd",  &mpt_ptsd,  "mpt_ptsd/F");
  T->Branch("mpt_phisd",  &mpt_phisd,  "mpt_phisd/F");
  T->Branch("nJet",    &nJet,  "nJet/I");
  T->Branch("jet_pt",  &jet_pt,  "jet_pt[nJet]/F");
  T->Branch("jet_eta", &jet_eta, "jet_eta[nJet]/F");
  T->Branch("jet_phi", &jet_phi, "jet_phi[nJet]/F");
  T->Branch("jet_m",   &jet_m,   "jet_m[nJet]/F");
  T->Branch("jet_msd",   &jet_msd,   "jet_msd[nJet]/F");
  T->Branch("nGen",    &nGen,  "nGen/I");
  T->Branch("gen_pt",        &gen_pt,  "gen_pt[nGen]/F");
  T->Branch("gen_eta",       &gen_eta, "gen_eta[nGen]/F");
  T->Branch("gen_phi",       &gen_phi, "gen_phi[nGen]/F");
  T->Branch("gen_m",         &gen_m,   "gen_m[nGen]/F");
  T->Branch("gen_flags",     &gen_flags,     "gen_flags[nGen]/I");
  T->Branch("gen_id",        &gen_id,        "gen_id[nGen]/I"       );
  T->Branch("gen_status",    &gen_status,    "gen_status[nGen]/I"   );
  T->Branch("gen_mother1",   &gen_mother1,   "gen_mother1[nGen]/I"  );
  T->Branch("gen_mother2",   &gen_mother2,   "gen_mother2[nGen]/I"  );
  T->Branch("gen_daughter1", &gen_daughter1, "gen_daughter1[nGen]/I");
  T->Branch("gen_daughter2", &gen_daughter2, "gen_daughter2[nGen]/I");
  T->Branch("gen_col",       &gen_col,       "gen_col[nGen]/I"      );
  T->Branch("gen_vxx",       &gen_vxx,       "gen_vxx[nGen]/F"      );
  T->Branch("gen_vyy",       &gen_vyy,       "gen_vyy[nGen]/F"      );
  T->Branch("gen_vzz",       &gen_vzz,       "gen_vzz[nGen]/F"      );
  T->Branch("gen_tau",       &gen_tau,       "gen_tau[nGen]/F"      );
  
  
 // Begin event loop. Generate event; skip if generation aborted.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    eventNum = iEvent; 
    nGen = nJet = 0;
    if (!pythia.next()) continue;
    if ( verbose ) 
      std::cout << "Generating event " << iEvent << std::endl;

    mpt_pt = mpt_phi = mpt_ptsd = mpt_phisd = 0.; 
    for ( auto x : jet_pt ) x=0.0;
    for ( auto x : jet_eta ) x=0.0;
    for ( auto x : jet_phi ) x=0.0;
    for ( auto x : jet_m ) x=0.0;
    for ( auto x : jet_msd ) x=0.0;

    for ( auto x : gen_pt ) x=0.0;
    for ( auto x : gen_eta ) x=0.0;
    for ( auto x : gen_phi ) x=0.0;
    for ( auto x : gen_m ) x=0.0;
    for ( auto x : gen_flags ) x = 0;
    for ( auto x : gen_id ) x = 0;
    for ( auto x : gen_status ) x = 0;
    for ( auto x : gen_mother1 ) x = 0;
    for ( auto x : gen_mother2 ) x = 0;
    for ( auto x : gen_daughter1 ) x = 0;
    for ( auto x : gen_daughter2 ) x = 0;
    for ( auto x : gen_col ) x = 0;
    for ( auto x : gen_vxx ) x = 0.;
    for ( auto x : gen_vyy ) x = 0.;
    for ( auto x : gen_vzz ) x = 0.;
    for ( auto x : gen_tau ) x = 0.;

    // Dump the PYTHIA8 content.     
    // Create AK8 jets with pt > 170 GeV
    std::vector<fastjet::PseudoJet> fj_particles;
    std::map<int,int> constituentmap;
    for (int i = 0; i < event->size(); ++i){
      auto const & p = pythia.event[i];
      if ( (p.isFinalPartonLevel() || p.isResonance()) && p.idAbs() != 21  ) {
	if ( verbose ) {
	  char buff[1000];
	  sprintf( buff, "  ndx=%6d, id=%6d, status=%6d, p4=(%6.4f,%6.2f,%6.2f,%6.4f)", i, p.id(), p.status(), p.pT(), p.eta(), p.phi(), p.m() );
	  std::cout << buff << std::endl; 
	}
	gen_pt[nGen] = p.pT();
	gen_eta[nGen] = p.eta();
	gen_phi[nGen] = p.phi();
	gen_m[nGen] = p.m();
	gen_orig[nGen] = i;
	gen_id[nGen] =         p.id();
	gen_flags[nGen] =      p.isHadron() << 3 | p.isFinal() << 2 | p.isFinalPartonLevel() << 1 | p.isVisible() << 0; 
	gen_status[nGen] =     p.status();    
	gen_mother1[nGen] =    p.mother1();   
	gen_mother2[nGen] =    p.mother2();   
	gen_daughter1[nGen] =  p.daughter1(); 
	gen_daughter2[nGen] =  p.daughter2(); 
	gen_col[nGen] =        p.col();       
	gen_vxx[nGen] =        p.xProd();       
	gen_vyy[nGen] =        p.yProd();       
	gen_vzz[nGen] =        p.zProd();       
	gen_tau[nGen] =        p.tau();
	++nGen;
	if ( nGen >= kMaxGen ){
	  std::cout << "too many particles in event " << iEvent << ", storing first " << kMaxGen << std::endl;
	  break;
	}
      } else if ( p.isFinal() && std::abs(p.eta()) < 5. ) {
	auto imother = p.mother1();
	auto mother = pythia.event[imother];
	if ( verbose && mother.idAbs() == 23 ) {
	  std::cout << "Daughter of Z boson at index " << p.index() << std::endl;
	}
	if ( !exclude_leptons_from_jets || (mother.idAbs() != 23 && mother.idAbs() != 24) ) {
	  fj_particles.emplace_back( p.px(), p.py(), p.pz(), p.e()  );
	  fj_particles.back().set_user_index( i );
	}
      }
    }
    if ( verbose) std::cout << "About to cluster" << std::endl;
    fastjet::ClusterSequence cs(fj_particles, jet_def);
    std::vector<fastjet::PseudoJet> jets = fastjet::sorted_by_pt(cs.inclusive_jets(ptmin));

    fastjet::ClusterSequence cs_mpt(fj_particles, mpt_def);
    std::vector<fastjet::PseudoJet> mpts = fastjet::sorted_by_pt(cs_mpt.inclusive_jets(1));
    if ( verbose ) std::cout << " ------ number of mpts : " << mpts.size() << std::endl;
    if ( mpts.size() > 0 ) {
      auto mpt =  mpts[0];
      auto mpt_sd = sd( mpts[0] );
      mpt_pt    = mpt.pt();
      mpt_phi   = mpt.phi();
      mpt_ptsd  = mpt_sd.pt();
      mpt_phisd = mpt_sd.phi();
      if ( verbose ) std::cout << " mpt : pt,phi,ptsd,phisd = " << mpt_pt << ", " << mpt_phi << ", " << mpt_ptsd << ", " << mpt_phisd << std::endl;
    }

    if ( verbose ) std::cout << "About to loop over jets" << std::endl;
    nJet = 0;
    if ( jets.size() > 0 ) {
      auto ibegin = jets.begin();
      auto iend = jets.end();
      for ( auto ijet=ibegin;ijet!=iend;++ijet ) {
	if ( verbose ) std::cout << "processing jet " << ijet - ibegin << std::endl;

	auto sd_jet =  sd(*ijet);
	if ( nJet < kMaxJet ) { 
	  
	  jet_pt[nJet]=ijet->perp();
	  jet_eta[nJet]=ijet->eta();
	  jet_phi[nJet]=ijet->phi();
	  jet_m[nJet]=ijet->m();	  
	  jet_msd[nJet] = sd_jet.m();
	  ++nJet;
	}
      }
    } // end check if jets.size() > 0
    if ( verbose ) 
      std::cout << "About to write" << std::endl;
    // Fill the pythia event into the TTree.
    T->Fill();
    
    if ( verbose ) 
      std::cout << "Done writing." << std::endl;
    
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
