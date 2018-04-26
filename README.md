# PythiaGenJets

Simple [PYTHIA8](http://home.thep.lu.se/Pythia/) program that uses [fastjet](http://fastjet.fr) to generate events, precluster them with the anti-kt algorithm with R = 0.8, and write events with the leading jet satisfying pt > 170 GeV to a [ROOT](http://root.cern.ch) [TTree](https://root.cern.ch/doc/v608/classTTree.html).

The example generates Z+jets events where the Z decays to electron or muon pairs. The example event size is around 45 kB/event. 


## Instructions:

```
make pythia2root
./pythia2root 
```
Output:
```
usage: ./pythia2root config_file root_file n_events
```

Example:
```
./pythia2root test_run_all.cfg test.root 1000
```


## Output TTree structure

The output TTree uses the default PYTHIA8 "Event" for the gen particle information. It also stores the AK8 jets and the indices of their constituents in a 

## Citations:

### fastjet:

```
%\cite{Cacciari:2011ma}
\bibitem{Cacciari:2011ma}
  M.~Cacciari, G.~P.~Salam and G.~Soyez,
  %``FastJet user manual,''
  Eur.\ Phys.\ J.\ C {\bf 72} (2012) 1896
  [arXiv:1111.6097 [hep-ph]].
  %%CITATION = ARXIV:1111.6097;%%

%\cite{hep-ph/0512210}
\bibitem{hep-ph/0512210}
  M.~Cacciari and G.~P.~Salam,
  %``Dispelling the $N^{3}$ myth for the $k_t$ jet-finder,''
  Phys.\ Lett.\ B\ {\bf 641} (2006) 57
  [hep-ph/0512210].
  %%CITATION = PHLTA,B641,57;%%
  
  ```
  
  ### anti-kt:
  
  ```
  @article{Cacciari:2008gp,
      author         = "Cacciari, Matteo and Salam, Gavin P. and Soyez, Gregory",
      title          = "{The Anti-k(t) jet clustering algorithm}",
      journal        = "JHEP",
      volume         = "04",
      year           = "2008",
      pages          = "063",
      doi            = "10.1088/1126-6708/2008/04/063",
      eprint         = "0802.1189",
      archivePrefix  = "arXiv",
      primaryClass   = "hep-ph",
      reportNumber   = "LPTHE-07-03",
      SLACcitation   = "%%CITATION = ARXIV:0802.1189;%%"
}
```
  
  ### pythia:
  
  ```
  @article{Sjostrand:2006za,
      author         = "Sjostrand, Torbjorn and Mrenna, Stephen and Skands, Peter
                        Z.",
      title          = "{PYTHIA 6.4 Physics and Manual}",
      journal        = "JHEP",
      volume         = "05",
      year           = "2006",
      pages          = "026",
      doi            = "10.1088/1126-6708/2006/05/026",
      eprint         = "hep-ph/0603175",
      archivePrefix  = "arXiv",
      primaryClass   = "hep-ph",
      reportNumber   = "FERMILAB-PUB-06-052-CD-T, LU-TP-06-13",
      SLACcitation   = "%%CITATION = HEP-PH/0603175;%%"
}

@article{Sjostrand:2007gs,
      author         = "Sjostrand, Torbjorn and Mrenna, Stephen and Skands, Peter
                        Z.",
      title          = "{A Brief Introduction to PYTHIA 8.1}",
      journal        = "Comput. Phys. Commun.",
      volume         = "178",
      year           = "2008",
      pages          = "852-867",
      doi            = "10.1016/j.cpc.2008.01.036",
      eprint         = "0710.3820",
      archivePrefix  = "arXiv",
      primaryClass   = "hep-ph",
      reportNumber   = "CERN-LCGAPP-2007-04, LU-TP-07-28,
                        FERMILAB-PUB-07-512-CD-T",
      SLACcitation   = "%%CITATION = ARXIV:0710.3820;%%"
}


@article{Sjostrand:2014zea,
      author         = "Sjöstrand, Torbjörn and Ask, Stefan and Christiansen,
                        Jesper R. and Corke, Richard and Desai, Nishita and Ilten,
                        Philip and Mrenna, Stephen and Prestel, Stefan and
                        Rasmussen, Christine O. and Skands, Peter Z.",
      title          = "{An Introduction to PYTHIA 8.2}",
      journal        = "Comput. Phys. Commun.",
      volume         = "191",
      year           = "2015",
      pages          = "159-177",
      doi            = "10.1016/j.cpc.2015.01.024",
      eprint         = "1410.3012",
      archivePrefix  = "arXiv",
      primaryClass   = "hep-ph",
      reportNumber   = "LU-TP-14-36, MCNET-14-22, CERN-PH-TH-2014-190,
                        FERMILAB-PUB-14-316-CD, DESY-14-178, SLAC-PUB-16122",
      SLACcitation   = "%%CITATION = ARXIV:1410.3012;%%"
}


```
