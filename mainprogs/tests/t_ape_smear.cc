// $Id: t_ape_smear.cc,v 1.1 2003-10-08 01:59:00 edwards Exp $

#include <iostream>
#include <cstdio>

#include "chroma.h"

using namespace QDP;

int main(int argc, char *argv[])
{
  // Put the machine into a known state
  QDP_initialize(&argc, &argv);

  // Setup the layout
  const int foo[] = {4,4,4,4};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  XMLFileWriter xml("t_ape_smear.xml");
  push(xml, "t_ape_smear");

  push(xml,"lattis");
  Write(xml,Nd);
  Write(xml,Nc);
  Write(xml,nrow);
  pop(xml);

  //! Example of calling a plaquette routine
  /*! NOTE: the STL is *not* used to hold gauge fields */
  multi1d<LatticeColorMatrix> u(Nd);
  Double w_plaq, s_plaq, t_plaq, link;

  cerr << "Start gaussian\n";
  for(int m=0; m < u.size(); ++m)
    gaussian(u[m]);

  // Reunitarize the gauge field
  for(int m=0; m < u.size(); ++m)
    reunit(u[m]);

  // Try out the plaquette routine
  MesPlq(u, w_plaq, s_plaq, t_plaq, link);
  cout << "w_plaq = " << w_plaq << endl;
  cout << "link = " << link << endl;

  // Write out the results
  push(xml,"observables");
  Write(xml,w_plaq);
  Write(xml,link);
  pop(xml);


  // Now APE smear the gauge field
  int j_decay = Nd - 1;

  Real sm_fact = 2.5;   // typical parameter
  int sm_numb = 10;     // number of smearing hits

  int BlkMax = 100;    // max iterations in max-ing trace
  Real BlkAccu = 1.0e-5;  // accuracy of max-ing

  multi1d<LatticeColorMatrix> u_smr(Nd);
  u_smr = u;
  for(int i=0; i < sm_numb; ++i)
  {
    multi1d<LatticeColorMatrix> u_tmp(Nd);

    for(int mu = 0; mu < Nd; ++mu)
      if ( mu != j_decay )
	APE_Smear(u_smr, u_tmp[mu], mu, 0, sm_fact, BlkAccu, BlkMax, j_decay);
      else
	u_tmp[mu] = u_smr[mu];
    
    u_smr = u_tmp;
  }

  // Compute plaquette on smeared links
  MesPlq(u_smr, w_plaq, s_plaq, t_plaq, link);
  cout << "w_plaq = " << w_plaq << endl;
  cout << "link = " << link << endl;

  // Write out the results
  push(xml,"Smeared_observables");
  Write(xml,w_plaq);
  Write(xml,link);
  pop(xml);

  pop(xml);

  // Time to bolt
  QDP_finalize();

  exit(0);
}
