#ifdef __GENIE_ENABLED__
//____________________________________________________________________________
/*!

\program gevgen

\brief   A simple 'generic' GENIE v+A event generation driver (gevgen).

	 It handles:
 	 a) event generation for a fixed init state (v+A) at fixed energy, or
         b) event generation for simple fluxes (specified either via some
            functional form, tabular text file or a ROOT histogram) and for
            simple 'geometries' (a target mix with its corresponding weights)

         See the GENIE manual for other apps handling experiment-specific
         event generation cases using the outputs of detailed neutrino flux
         simulations and realistic detector geometry descriptions.

         Syntax :
           gevgen [-h]
                  [-r run#]
                   -n nev
                   -e energy (or energy range)
                   -p neutrino_pdg
                   -t target_pdg
                  [-f flux_description]
                  [-w]
                  [--seed random_number_seed]
                  [--cross-sections xml_file]
                  [--event-generator-list list_name]
                  [--tune genie_tune]
                  [--message-thresholds xml_file]
                  [--unphysical-event-mask mask]
                  [--event-record-print-level level]
                  [--mc-job-status-refresh-rate  rate]
                  [--cache-file root_file]

         Options :
           [] Denotes an optional argument.
           -h
              Prints-out help on using gevgen and exits.
           -n
              Specifies the number of events to generate.
           -r
              Specifies the MC run number.
           -e
              Specifies the neutrino energy.
	      If what follows the -e option is a comma separated pair of values
              it will be interpreted as an energy range for the flux specified
              via the -f option (see below).
           -p
              Specifies the neutrino PDG code.
           -t
              Specifies the target PDG code (pdg format: 10LZZZAAAI) _or_ a target
              mix (pdg codes with corresponding weights) typed as a comma-separated
              list of pdg codes with the corresponding weight fractions in brackets,
              eg code1[fraction1],code2[fraction2],...
              For example, to use a target mix of 95% O16 and 5% H type:
              `-t 1000080160[0.95],1000010010[0.05]'.
           -f
              Specifies the neutrino flux spectrum.
              It can be any of:
	      -- A function:
                 eg ` -f x*x+4*exp(-x)'
              -- A vector file:
                 The vector file should contain 2 columns corresponding to
                 energy,flux (see $GENIE/data/flux/ for few examples).
              -- A 1-D ROOT histogram (TH1D):
                 The general syntax is `-f /full/path/file.root,object_name'
           -w
              Forces generation of weighted events.
              This option is relevant only if a neutrino flux is specified.
              Note that 'weighted' refers to the selection of the primary
              flux neutrino + target that were forced to interact. A weighting
              scheme for the generated kinematics of individual processes can
              still be in effect if enabled..
              ** Only use that option if you understand what it means **
           --seed
              Random number seed.
           --cross-sections
              Name (incl. full path) of an XML file with pre-computed
              cross-section values used for constructing splines.
           --event-generator-list
              List of event generators to load in event generation drivers.
              [default: "Default"].
           --tune
              Specifies a GENIE comprehensive neutrino interaction model tune.
              [default: "Default"].
           --message-thresholds
              Allows users to customize the message stream thresholds.
              The thresholds are specified using an XML file.
              See $GENIE/config/Messenger.xml for the XML schema.
           --unphysical-event-mask
              Allows users to specify a 16-bit mask to allow certain types of
              unphysical events to be written in the output file.
              [default: all unphysical events are rejected]
           --event-record-print-level
              Allows users to set the level of information shown when the event
              record is printed in the screen. See GHepRecord::Print().
           --mc-job-status-refresh-rate
              Allows users to customize the refresh rate of the status file.
           --cache-file
              Allows users to specify a cache file so that the cache can be
              re-used in subsequent MC jobs.

	***  See the User Manual for more details and examples. ***

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         University of Liverpool & STFC Rutherford Appleton Lab

\created October 05, 2004

\cpright Copyright (c) 2003-2016, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <cstdlib>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#if defined(HAVE_FENV_H) && defined(HAVE_FEENABLEEXCEPT)
#include <fenv.h> // for `feenableexcept`
#endif

#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <TVector3.h>
#include <TH1.h>
#include <TF1.h>

#include "Conventions/XmlParserStatus.h"
#include "Conventions/GBuild.h"
#include "Conventions/Controls.h"
#include "Conventions/Units.h"

#include "EVGCore/EventRecord.h"
#include "EVGDrivers/GFluxI.h"
#include "EVGDrivers/GEVGDriver.h"
//#include "EVGDrivers/GMCJDriver.h"
#include "GNUISANCEMCJDriver.h"
#include "EVGDrivers/GMCJMonitor.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "Ntuple/NtpWriter.h"
#include "Ntuple/NtpMCFormat.h"
#include "Numerical/RandomGen.h"
#include "Numerical/Spline.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "Utils/AppInit.h"
#include "Utils/RunOpt.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"
#include "Utils/PrintUtils.h"
#include "Utils/SystemUtils.h"
#include "Utils/CmdLnArgParser.h"

//#include "FitBase/GNUISANCEFlux.h"
#include "GNUISANCEFlux.h"

//#ifdef __GENIE_FLUX_DRIVERS_ENABLED__
//#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#define __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
#include "GNUISANCEFlux.h"
#include "FluxDrivers/GMonoEnergeticFlux.h"
#include "Geo/PointGeomAnalyzer.h"
//#endif
//#endif

using std::string;
using std::vector;
using std::map;
using std::ostringstream;

using namespace genie;
using namespace genie::controls;
using namespace genie::flux;

void GetCommandLineArgs (int argc, char ** argv);
void Initialize         (void);
void PrintSyntax        (void);

#ifdef __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
void            GenerateEventsUsingFluxOrTgtMix();
GeomAnalyzerI * GeomDriver              (void);
GFluxI *        FluxDriver              (void);
GFluxI *        MonoEnergeticFluxDriver (void);
GFluxI *        TH1FluxDriver           (void);
string          ConvertTargetIDs        (string);
string          ConvertFluxIDs          (string);
void            ListTargetIDs(void);
void            ListFluxIDs(void);
#endif

void GenerateEventsAtFixedInitState (void);

//Default options (override them using the command line arguments):
int           kDefOptNevents   = 0;       // n-events to generate
NtpMCFormat_t kDefOptNtpFormat = kNFGHEP; // ntuple format
Long_t        kDefOptRunNu     = 0;       // default run number

//User-specified options:
int             gOptNevents;      // n-events to generate
double          gOptNuEnergy;     // neutrino E, or min neutrino energy in spectrum
double          gOptNuEnergyRange;// energy range in input spectrum
int             gOptNuPdgCode;    // neutrino PDG code
map<int,double> gOptTgtMix;       // target mix (each with its relative weight)
Long_t          gOptRunNu;        // run number
string          gOptFlux;         //
bool            gOptWeighted;     //
bool            gOptUsingFluxOrTgtMix = false;
long int        gOptRanSeed;      // random number seed
string          gOptInpXSecFile;  // cross-section splines


vector<int>     gOptNuPDGs;   // Beam PDGS
vector<TH1D*>   gOptNuFluxs;  // Beam Fluxes
vector<int>     gOptTgtPDGs;  // Target PDGS
int             gOptNumberNucleons; // Total number of target nucleons

//____________________________________________________________________________
int main(int argc, char ** argv)
{
  GetCommandLineArgs(argc,argv);
  Initialize();

  // throw on NaNs and Infs...
#if defined(HAVE_FENV_H) && defined(HAVE_FEENABLEEXCEPT)
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif
  //
  // Generate neutrino events
  //

  if(gOptUsingFluxOrTgtMix) {
#ifdef __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
	GenerateEventsUsingFluxOrTgtMix();
#else
  LOG("gevgen", pERROR)
    << "\n   To be able to generate neutrino events from a flux and/or a target mix"
    << "\n   you need to add the following config options at your GENIE installation:"
    << "\n   --enable-flux-drivers  --enable-geom-drivers \n" ;
#endif
  } else {
     GenerateEventsAtFixedInitState();
  }
  return 0;
}
//____________________________________________________________________________
void Initialize()
{
  // Initialization of random number generators, cross-section table,
  // messenger thresholds, cache file
  utils::app_init::MesgThresholds(RunOpt::Instance()->MesgThresholdFiles());
  utils::app_init::CacheFile(RunOpt::Instance()->CacheFile());
  utils::app_init::RandGen(gOptRanSeed);
  utils::app_init::XSecTable(gOptInpXSecFile, false);

  // Set GHEP print level
  GHepRecord::SetPrintLevel(RunOpt::Instance()->EventRecordPrintLevel());
}
//____________________________________________________________________________
void GenerateEventsAtFixedInitState(void)
{
  int neutrino = gOptNuPdgCode;
  int target   = gOptTgtMix.begin()->first;
  double Ev    = gOptNuEnergy;
  TLorentzVector nu_p4(0.,0.,Ev,Ev); // px,py,pz,E (GeV)

  // Create init state
  InitialState init_state(target, neutrino);

  // Create/config event generation driver
  GEVGDriver evg_driver;
  evg_driver.SetEventGeneratorList(RunOpt::Instance()->EventGeneratorList());
  evg_driver.SetUnphysEventMask(*RunOpt::Instance()->UnphysEventMask());
  evg_driver.Configure(init_state);

  // Initialize an Ntuple Writer
  NtpWriter ntpw(kDefOptNtpFormat, gOptRunNu);
  ntpw.Initialize();

  // Create an MC Job Monitor
  GMCJMonitor mcjmonitor(gOptRunNu);
  mcjmonitor.SetRefreshRate(RunOpt::Instance()->MCJobStatusRefreshRate());

  LOG("gevgen", pNOTICE)
    << "\n ** Will generate " << gOptNevents << " events for \n"
    << init_state << " at Ev = " << Ev << " GeV";

  // Generate events / print the GHEP record / add it to the ntuple
  int ievent = 0;
  while (ievent < gOptNevents) {
     LOG("gevgen", pNOTICE)
        << " *** Generating event............ " << ievent;

     // generate a single event
     EventRecord * event = evg_driver.GenerateEvent(nu_p4);

     if(!event) {
        LOG("gevgen", pNOTICE)
          << "Last attempt failed. Re-trying....";
        continue;
     }

     LOG("gevgen", pNOTICE)
	<< "Generated Event GHEP Record: " << *event;

     // add event at the output ntuple, refresh the mc job monitor & clean up
     ntpw.AddEventRecord(ievent, event);
     mcjmonitor.Update(ievent,event);
     ievent++;
     delete event;
  }

  // Save the generated MC events
  ntpw.Save();
}
//____________________________________________________________________________

#ifdef __CAN_GENERATE_EVENTS_USING_A_FLUX_OR_TGTMIX__
//............................................................................
void GenerateEventsUsingFluxOrTgtMix(void)
{
  // Get flux and geom drivers
  GFluxI *        flux_driver = FluxDriver();
  GeomAnalyzerI * geom_driver = GeomDriver();

  // Create the monte carlo job driver
  GNUISANCEMCJDriver * mcj_driver = new GNUISANCEMCJDriver;
  mcj_driver->SetEventGeneratorList(RunOpt::Instance()->EventGeneratorList());
  mcj_driver->SetUnphysEventMask(*RunOpt::Instance()->UnphysEventMask());
  mcj_driver->UseFluxDriver(flux_driver);
  mcj_driver->UseGeomAnalyzer(geom_driver);
  mcj_driver->Configure();
  mcj_driver->UseSplines();
  if(!gOptWeighted)
	mcj_driver->ForceSingleProbScale();

  // Initialize an Ntuple Writer to save GHEP records into a TTree
  NtpWriter ntpw(kDefOptNtpFormat, gOptRunNu);
  ntpw.Initialize();

  // Now Create flux histograms
  TH1D* totalflux = static_cast<GNUISANCEFlux*>(flux_driver)->GetTotalSpectrum();
  vector<TH1D*> fluxinputs = static_cast<GNUISANCEFlux*>(flux_driver)->GetSpectrum();
  totalflux->SetNameTitle("nuisance_flux","NUISANCE Total Flux");

  TH1D* totalevent = (TH1D*) totalflux->Clone();
  totalevent->SetNameTitle("nuisance_events", "NUISANCE Total Events");
  //  totalevent->Reset();

  TH1D* totalxsec = (TH1D*)totalflux->Clone();
  totalxsec->SetNameTitle("nuisance_xsec", "NUISANCE Total XSec");
  totalxsec->Reset();

  // Save the cross-section histograms
  for (UInt_t i = 0; i < gOptNuPDGs.size(); i++){
    for (UInt_t j = 0; j < gOptTgtPDGs.size(); j++){

      int npdg = gOptNuPDGs[i];
      TH1D* fluxhist = gOptNuFluxs[i];

      int tpdg = gOptTgtPDGs[j];

      // Determine the total number of these targets by using the tgt ratios backwards
      double mixfrac = gOptTgtMix[tpdg];

      int tnucl = tpdg % 10000 / 10; // Total Single Target Nucleons
      double enucl = double(mixfrac * double(gOptNumberNucleons)); // Expected nucleons from weight fractions

      double tfrac = double(enucl) / double(tnucl); // Scaling factor.

      // Get Initial state driver
      InitialState init_state(tpdg, npdg);
      GEVGPool* gpool = mcj_driver->GetConfigPool();
      GEVGDriver * evgdriver = gpool->FindDriver(init_state);
      const Spline * totxsecspl = evgdriver->XSecSumSpline();

      // make new empty xsec hist
      TH1D* xsechist = (TH1D*) totalflux->Clone();
      xsechist->SetNameTitle((init_state.AsString()+"_xsec").c_str(),init_state.AsString().c_str());
      xsechist->Reset();

      TH1D* eventhist = (TH1D*) totalflux->Clone();
      eventhist->SetNameTitle((init_state.AsString()+"_events").c_str(),init_state.AsString().c_str());
      eventhist->Reset();

      std::cout << "Total Frac for " << init_state.AsString() << " = " << tfrac << std::endl;
      std::cout << "TNUCL = " << tnucl << std::endl;
      std::cout << "ENUCL = " << enucl << std::endl;
      std::cout << "MIXFRAC = " << mixfrac << std::endl;
      std::cout << "TPDG = " << tpdg << std::endl;

      // Fill xsec hist
      for (int k = 0; k < totalflux->GetNbinsX(); k++){
	double avgxsec = 0.0;


	int res = 1000;
	for (int l = 0; l < res; l++){
	  double E = fluxhist->GetXaxis()->GetBinLowEdge(k+1) + (double(l)*fluxhist->GetXaxis()->GetBinWidth(k+1) / double(res));
	  double xsec = totxsecspl->Evaluate( E ) / (1E-38 * genie::units::cm2);
	  avgxsec += xsec;
	}
	avgxsec /= double(res);

	//	double E = fluxhist->GetXaxis()->GetBinCenter(k+1);
	//	avgxsec = totxsecspl->Evaluate( E ) / (1E-38 * genie::units::cm2);

	xsechist->SetBinContent(k+1, avgxsec);
	eventhist->SetBinContent(k+1, avgxsec * fluxhist->GetBinContent(k+1));

      }

      // Scale totals by target fractions
      xsechist->Scale(tfrac);
      eventhist->Scale(tfrac);

      // Add to totals
      totalxsec->Add(xsechist);
      //totalevent->Add(eventhist);

      std::cout << "Total XSec Hist for " << init_state.AsString() << " = " << xsechist->Integral("width") << std::endl;

      // Scale by nnucleons
      //      xsechist->Scale(1.0 / double(gOptNumberNucleons));
      //      eventhist->Scale(1.0 / double(gOptNumberNucleons));

      // Write our new xsec histto file
      fluxhist->SetNameTitle((init_state.AsString()+"_flux").c_str(),init_state.AsString().c_str());
      fluxhist->Write();
      xsechist->Write();
      eventhist->Write();

      delete xsechist;
      delete eventhist;

      //      sleep(10);
    }
  }

  // Determine NUISANCE Style Histograms
  totalevent->SetNameTitle("nuisance_events", "NUISANCE Total Events");
  totalevent->Multiply(totalxsec);
  totalevent->Scale(1.0 / double(gOptNumberNucleons));
  std::cout << "GOPTNumberNucleons = " << gOptNumberNucleons << std::endl;
  std::cout << "Inclusive XSec Per Nucleon = " << totalevent->Integral("width") * 1E-38 / totalflux->Integral("width") << std::endl;
  std::cout << "XSec Hist Integral = " << totalxsec->Integral("width") << std::endl;
  //  sleep(20);

  //  totalxsec->Scale(1.0/double(gOptNumberNucleons));
  //  totalevent->Scale(1.0/double(gOptNumberNucleons));

  // Create an MC Job Monitor
  GMCJMonitor mcjmonitor(gOptRunNu);
  mcjmonitor.SetRefreshRate(RunOpt::Instance()->MCJobStatusRefreshRate());

  // Generate events / print the GHEP record / add it to the ntuple
  int ievent = 0;
  while ( ievent < gOptNevents) {

     LOG("gevgen", pNOTICE) << " *** Generating event............ " << ievent;

     // generate a single event for neutrinos coming from the specified flux
     EventRecord * event = mcj_driver->GenerateEvent();

     LOG("gevgen", pNOTICE) << "Generated Event GHEP Record: " << *event;

     // add event at the output ntuple, refresh the mc job monitor & clean-up
     ntpw.AddEventRecord(ievent, event);
     mcjmonitor.Update(ievent,event);
     ievent++;
     delete event;
  }

  // Save the generated MC events
  ntpw.Save();


  delete flux_driver;
  delete geom_driver;
  delete mcj_driver;;
}
//____________________________________________________________________________
GeomAnalyzerI * GeomDriver(void)
{
// create a trivial point geometry with the specified target or target mix

  GeomAnalyzerI * geom_driver = new geometry::PointGeomAnalyzer(gOptTgtMix);
  return geom_driver;
}
//____________________________________________________________________________
GFluxI * FluxDriver(void)
{
// create & configure one of the generic flux drivers
//
  GFluxI * flux_driver = 0;

  if(gOptNuEnergyRange<0) flux_driver = MonoEnergeticFluxDriver();
  else flux_driver = TH1FluxDriver();

  return flux_driver;
}
//____________________________________________________________________________
GFluxI * MonoEnergeticFluxDriver(void)
{
//
//
  flux::GMonoEnergeticFlux * flux =
              new flux::GMonoEnergeticFlux(gOptNuEnergy, gOptNuPdgCode);
  GFluxI * flux_driver = dynamic_cast<GFluxI *>(flux);
  return flux_driver;
}
//____________________________________________________________________________
GFluxI * TH1FluxDriver(void)
{

  // GeVGEN NUISANCE Can only Read flux files from ROOT
  flux::GNUISANCEFlux * flux = new flux::GNUISANCEFlux;

  // Initial beam directions
  TVector3 bdir (0,0,1);
  TVector3 bspot(0,0,0);

  flux->SetNuDirection      (bdir);
  flux->SetBeamSpot         (bspot);
  flux->SetTransverseRadius (-1);

  // Flux inputs
  int flux_entries = 100000;

  double emin = gOptNuEnergy;
  double emax = gOptNuEnergy+gOptNuEnergyRange;
  double de   = gOptNuEnergyRange;

  // check whether the input flux is a file or a functional form
  bool input_is_root_file = gOptFlux.find(".root") != string::npos &&
                            gOptFlux.find(",") != string::npos;

  if (!input_is_root_file){
    std::cout << "ONLY ROOT INPUTS ALLOWED" << std::endl;
    throw;
  }

  //
  // ** extract specified flux histogram from the input root file
  //
  vector<string> fv = utils::str::Split(gOptFlux,",");

  assert(fv.size()>=2);
  assert( !gSystem->AccessPathName(fv[0].c_str()) );

  // First entry is the root file
  LOG("gevgen", pNOTICE) << "Getting input flux from root file: " << fv[0];
  TFile * flux_file = new TFile(fv[0].c_str(),"read");

  // Later entries are flux inputs
  for (UInt_t i = 1; i < fv.size(); i++){
    string fluxid = fv[i];

    // Parse out the beam pdg and flux name
    string flux_with_pdg = fluxid;
    string::size_type open_bracket  = flux_with_pdg.find("[");
    string::size_type close_bracket = flux_with_pdg.find("]");
    string::size_type ibeg = 0;
    string::size_type iend = open_bracket;
    string::size_type jbeg = open_bracket+1;
    string::size_type jend = close_bracket-1;

    string fluxname = flux_with_pdg.substr(ibeg,iend);
    int    fluxpdg  = atoi(flux_with_pdg.substr(jbeg,jend).c_str());

    // Get histogram
    LOG("gevgen", pNOTICE) << "Flux name: " << fluxname << " pdg = " << fluxpdg;
    TH1D * hst = (TH1D *)flux_file->Get(fluxname.c_str());
    assert(hst);

    LOG("gevgen", pNOTICE) << hst->GetEntries();


    // Copy in the flux histogram from the root file and remove bins outside the emin,emax range
    TH1D* spectrum = (TH1D*)hst->Clone();
    spectrum->SetNameTitle("spectrum","neutrino_flux");
    spectrum->SetDirectory(0);
    for(int ibin = 1; ibin <= hst->GetNbinsX(); ibin++) {
      if(spectrum->GetBinCenter(ibin) > emax ||
    	 spectrum->GetBinCenter(ibin) < emin){
	spectrum->SetBinContent(ibin, 0);
      }
    }

    LOG("gevgen", pNOTICE) << spectrum->GetEntries();


    // Add energy spectrum to the flux driver.
    flux->AddEnergySpectrum( fluxpdg, spectrum );

    gOptNuPDGs.push_back(fluxpdg);
    gOptNuFluxs.push_back(spectrum);

  }
  //  flux->AddAllFluxes();

  // Close inputs
  flux_file->Close();
  delete flux_file;

  // Return flux driver.
  GFluxI * flux_driver = dynamic_cast<GFluxI *>(flux);
  return flux_driver;
}

///____________________________________________________________________________
void ListTargetIDs(){

  // Keep in sync with ConvertTargetIDs
  LOG("gevgen", pNOTICE) << "Possible Target IDs: \n"
			 << "\n H  : " << ConvertTargetIDs("H")
                         << "\n C  : " << ConvertTargetIDs("C")
			 << "\n CH  : " << ConvertTargetIDs("CH")
			 << "\n CH2 : " << ConvertTargetIDs("CH2")
			 << "\n H2O : " << ConvertTargetIDs("H2O")
			 << "\n Fe  : " << ConvertTargetIDs("Fe")
			 << "\n Pb  : " << ConvertTargetIDs("Pb")
			 << "\n D2  : " << ConvertTargetIDs("D2")
			 << "\n D2-free : " << ConvertTargetIDs("D2-free");
}


//____________________________________________________________________________
string ConvertTargetIDs(string id){

  if (!id.compare("H")) return "1000010010";
  else if  (!id.compare("C")) return "1000060120";
  else if  (!id.compare("CH"))  return "13,1000060120[0.9231],1000010010[0.0769]";
  else if  (!id.compare("CH2")) return "14,1000060120[0.8571],1000010010[0.1429]";
  else if  (!id.compare("H2O")) return "18,1000080160[0.8888],1000010010[0.1111]";
  else if  (!id.compare("Fe"))  return "1000260560";
  else if  (!id.compare("Pb"))  return "1000822070";
  else if  (!id.compare("D2"))  return "1000010020";
  else if  (!id.compare("D2-free")) return "2,1000010010[0.5],1000000010[0.5]";
  else return "";

};

///____________________________________________________________________________
void ListFluxIDs(){

  // Keep in sync with ConvertTargetIDs
  LOG("gevgen", pNOTICE) << "Possible Flux IDs: \n"
                         << "\n MINERvA_fhc_numu  : " << ConvertFluxIDs("MINERvA_fhc_numu")
			 << "\n MINERvA_fhc_numunumubar  : " << ConvertFluxIDs("MINERvA_fhc_numunumubar")
                         << "\n MINERvA_fhc_nue  : " << ConvertFluxIDs("MINERvA_fhc_nue")
                         << "\n MINERvA_fhc_nuenuebar  : " << ConvertFluxIDs("MINERvA_fhc_nuenuebar")
			 << "\n MINERvA_fhc_all  : " << ConvertFluxIDs("MINERvA_fhc_all")

			 << "\n MINERvA_rhc_numubar  : " << ConvertFluxIDs("MINERvA_rhc_numubar")
			 << "\n MINERvA_rhc_numubarnumu  : " << ConvertFluxIDs("MINERvA_rhc_numubarnumu")
			 << "\n MINERvA_rhc_nuebar  : " << ConvertFluxIDs("MINERvA_rhc_nuebar")
			 << "\n MINERvA_rhc_nuebarnue  : " << ConvertFluxIDs("MINERvA_rhc_nuebarnue")
			 << "\n MINERvA_rhc_all  : " << ConvertFluxIDs("MINERvA_rhc_all")

			 << "\n ANL_fhc_numu : " << ConvertFluxIDs("ANL_fhc_numu")
			 << "\n BNL_fhc_numu : " << ConvertFluxIDs("BNL_fhc_numu")
			 << "\n BNL_fhc_numu_ALT1986 : " << ConvertFluxIDs("BNL_fhc_numu_ALT1986")
			 << "\n BNL_fhc_numu_ALT1981 : " << ConvertFluxIDs("BNL_fhc_numu_ALT1981")
			 << "\n BEBC_fhc_numu : " << ConvertFluxIDs("BEBC_fhc_numu")
			 << "\n FNAL_fhc_numu : " << ConvertFluxIDs("FNAL_fhc_numu")
			 << "\n FNAL_rhc_numub : " << ConvertFluxIDs("FNAL_rhc_numub")
			 << "\n GGM_fhc_numu : " << ConvertFluxIDs("GGM_fhc_numu");
    
}


//____________________________________________________________________________
string ConvertFluxIDs(string id){

  char * const var = getenv("NUISANCE");
  if (!var) {
    std::cout << "Cannot find top level directory! Set the NUISANCE environmental variable" << std::endl;
    exit(-1);
  }
  string topnuisancedir = string(var);
  string fluxfolder = topnuisancedir + "/data/flux/";
  string inputs = "";

  if (!id.compare("MINERvA_fhc_numu")) inputs="minerva_flux.root,numu_fhc[14]";
  else if (!id.compare("MINERvA_fhc_numunumubar")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14]";
  else if (!id.compare("MINERvA_fhc_numu")) inputs="minerva_flux.root,nue_fhc[12]";
  else if (!id.compare("MINERvA_fhc_nuenuebar")) inputs="minerva_flux.root,nue_fhc[12],nuebar_fhc[-12]";
  else if (!id.compare("MINERvA_fhc_all")) inputs="minerva_flux.root,numu_fhc[14],numubar_fhc[-14],nue_fhc[12],nuebar_fhc[-12]";

  else if (!id.compare("MINERvA_rhc_numubar")) inputs="minerva_flux.root,numubar_rhc[-14]";
  else if (!id.compare("MINERvA_rhc_numubarnumu")) inputs="minerva_flux.root,numubar_rhc[-14],numu_rhc[14]";
  else if (!id.compare("MINERvA_rhc_nuebar")) inputs="minerva_flux.root,nuebar_rhc[-12]";
  else if (!id.compare("MINERvA_rhc_nuebarnue")) inputs="minerva_flux.root,nuebar_rhc[-12],nue_rhc[12]";
  else if (!id.compare("MINERvA_rhc_all")) inputs="minerva_flux.root,numu_rhc[14],numubar_rhc[-14],nue_rhc[12],nuebar_rhc[-12]";

  else if (!id.compare("ANL_fhc_numu"))         inputs="ANL_1977_2horn_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu"))         inputs="BNL_NuInt02_rescan.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1986")) inputs="BNL_1986_flux-ALTERNATIVE.root,numu_flux[14]";
  else if (!id.compare("BNL_fhc_numu_ALT1981")) inputs="BNL_CCQE_1981_rescan-ALTERNATIVE.root,numu_flux[14]";

  else if (!id.compare("BEBC_fhc_numu"))   inputs="BEBC_Wachsmuth_numubar_table.root,numu_flux[14]";
  else if (!id.compare("FNAL_fhc_numu"))   inputs="FNAL_CCinc_1982_nu_MCadj.root,numu_flux[14]";
  else if (!id.compare("FNAL_rhc_numub"))  inputs="FNAL_coh_1993_anu.root,numu_flux[-14]";
  else if (!id.compare("GGM_fhc_numu"))    inputs="GGM_nu_flux_1979_rescan.root,numu_flux[14]";
  else return "";

  return fluxfolder + inputs;

};



//............................................................................
#endif


//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("gevgen", pINFO) << "Parsing command line arguments";

  // Common run options. Set defaults and read.
  RunOpt::Instance()->EnableBareXSecPreCalc(true);
  RunOpt::Instance()->ReadFromCommandLine(argc,argv);

  // Parse run options for this app

  CmdLnArgParser parser(argc,argv);

  // help?
  bool help = parser.OptionExists('h');
  if(help) {
      PrintSyntax();
      exit(0);
  }

  // number of events
  if( parser.OptionExists('n') ) {
    LOG("gevgen", pINFO) << "Reading number of events to generate";
    gOptNevents = parser.ArgAsInt('n');
  } else {
    LOG("gevgen", pINFO)
       << "Unspecified number of events to generate - Using default";
    gOptNevents = kDefOptNevents;
  }

  // run number
  if( parser.OptionExists('r') ) {
    LOG("gevgen", pINFO) << "Reading MC run number";
    gOptRunNu = parser.ArgAsLong('r');
  } else {
    LOG("gevgen", pINFO) << "Unspecified run number - Using default";
    gOptRunNu = kDefOptRunNu;
  }

  // flux functional form
  bool using_flux = false;
  if( parser.OptionExists('f') ) {
    LOG("gevgen", pINFO) << "Reading flux function";
    gOptFlux = parser.ArgAsString('f');

    // Convert for known strings.
    string fluxid = ConvertFluxIDs(gOptFlux);
    if (!fluxid.empty()) gOptFlux = fluxid;

    using_flux = true;
  }

  if(parser.OptionExists('s')) {
    LOG("gevgen", pWARN)
      << "-s option no longer available. Please read the revised code documentation";
    gAbortingInErr = true;
    exit(1);
  }


  // generate weighted events option (only relevant if using a flux)
  gOptWeighted = parser.OptionExists('w');

  // neutrino energy
  if( parser.OptionExists('e') ) {
    LOG("gevgen", pINFO) << "Reading neutrino energy";
    string nue = parser.ArgAsString('e');

    // is it just a value or a range (comma separated set of values)
    if(nue.find(",") != string::npos) {
       // split the comma separated list
       vector<string> nurange = utils::str::Split(nue, ",");
       assert(nurange.size() == 2);
       double emin = atof(nurange[0].c_str());
       double emax = atof(nurange[1].c_str());
       assert(emax>emin && emin>=0);
       gOptNuEnergy      = emin;
       gOptNuEnergyRange = emax-emin;
       if(!using_flux) {
          LOG("gevgen", pWARN)
             << "No flux was specified but an energy range was input!";
          LOG("gevgen", pWARN)
	     << "Events will be generated at fixed E = " << gOptNuEnergy << " GeV";
	  gOptNuEnergyRange = -1;
       }
    } else {
       gOptNuEnergy       = atof(nue.c_str());
       gOptNuEnergyRange = -1;
    }
  } else {
    LOG("gevgen", pFATAL) << "Unspecified neutrino energy - Exiting";
    PrintSyntax();
    exit(1);
  }

  // neutrino PDG code
  if( parser.OptionExists('p') && !parser.OptionExists('f')) {
    LOG("gevgen", pINFO) << "Reading neutrino PDG code";
    gOptNuPdgCode = parser.ArgAsInt('p');
  } else if (!parser.OptionExists('p') && !parser.OptionExists('f')){
    LOG("gevgen", pFATAL) << "Unspecified neutrino PDG code or Flux Inputs - Exiting";
    PrintSyntax();
    exit(1);
  }

  // target mix (their PDG codes with their corresponding weights)
  bool using_tgtmix = false;
  if( parser.OptionExists('t') ) {
    LOG("gevgen", pINFO) << "Reading target mix";
    string stgtmix = parser.ArgAsString('t');
    gOptTgtMix.clear();

    // Check for ID Strings
    string tgtids = ConvertTargetIDs(stgtmix);
    if (!tgtids.empty()){
      stgtmix = tgtids;
    }

    // Parse Targets
    vector<string> tgtmix = utils::str::Split(stgtmix,",");
    if(tgtmix.size()==1) {
         int    pdg = atoi(tgtmix[0].c_str());
         double wgt = 1.0;
	 gOptTgtPDGs.push_back(pdg);
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));

	 // For single target number of free nucleons
	 // automatic
	 gOptNumberNucleons = pdg % 10000/ 10;

    } else {
      using_tgtmix = true;
      vector<string>::const_iterator tgtmix_iter = tgtmix.begin();

      // For multiple targets N nucleons must be specified first.
      gOptNumberNucleons = atoi((*tgtmix_iter).c_str());
      tgtmix_iter++;

      // Now remainder is the full target list
      for( ; tgtmix_iter != tgtmix.end(); ++tgtmix_iter) {
         string tgt_with_wgt = *tgtmix_iter;
         string::size_type open_bracket  = tgt_with_wgt.find("[");
         string::size_type close_bracket = tgt_with_wgt.find("]");
         string::size_type ibeg = 0;
         string::size_type iend = open_bracket;
         string::size_type jbeg = open_bracket+1;
         string::size_type jend = close_bracket-1;
         int    pdg = atoi(tgt_with_wgt.substr(ibeg,iend).c_str());
         double wgt = atof(tgt_with_wgt.substr(jbeg,jend).c_str());
         LOG("Main", pNOTICE)
            << "Adding to target mix: pdg = " << pdg << ", wgt = " << wgt;
	 gOptTgtPDGs.push_back(pdg);
         gOptTgtMix.insert(map<int, double>::value_type(pdg, wgt));
      }//tgtmix_iter
    }//>1

  } else {
    LOG("gevgen", pFATAL) << "Unspecified target PDG code - Exiting";
    PrintSyntax();
    exit(1);
  }

  gOptUsingFluxOrTgtMix = using_flux || using_tgtmix;

  // random number seed
  if( parser.OptionExists("seed") ) {
    LOG("gevgen", pINFO) << "Reading random number seed";
    gOptRanSeed = parser.ArgAsLong("seed");
  } else {
    LOG("gevgen", pINFO) << "Unspecified random number seed - Using default";
    gOptRanSeed = -1;
  }

  // input cross-section file
  if( parser.OptionExists("cross-sections") ) {
    LOG("gevgen", pINFO) << "Reading cross-section file";
    gOptInpXSecFile = parser.ArgAsString("cross-sections");
  } else {
    LOG("gevgen", pINFO) << "Unspecified cross-section file";
    gOptInpXSecFile = "";
  }

  //
  // print-out the command line options
  //
  LOG("gevgen", pNOTICE)
     << "\n"
     << utils::print::PrintFramedMesg("gevgen job configuration");
  LOG("gevgen", pNOTICE)
     << "MC Run Number: " << gOptRunNu;
  if(gOptRanSeed != -1) {
     LOG("gevgen", pNOTICE)
       << "Random number seed: " << gOptRanSeed;
  } else {
     LOG("gevgen", pNOTICE)
       << "Random number seed was not set, using default";
  }
  LOG("gevgen", pNOTICE)
       << "Number of events requested: " << gOptNevents;
  if(gOptInpXSecFile.size() > 0) {
     LOG("gevgen", pNOTICE)
       << "Using cross-section splines read from: " << gOptInpXSecFile;
  } else {
     LOG("gevgen", pNOTICE)
       << "No input cross-section spline file";
  }
  LOG("gevgen", pNOTICE)
       << "Flux: " << gOptFlux;
  LOG("gevgen", pNOTICE)
       << "Generate weighted events? " << gOptWeighted;
  if(gOptNuEnergyRange>0) {
     LOG("gevgen", pNOTICE)
        << "Neutrino energy: ["
        << gOptNuEnergy << ", " << gOptNuEnergy+gOptNuEnergyRange << "]";
  } else {
     LOG("gevgen", pNOTICE)
        << "Neutrino energy: " << gOptNuEnergy;
  }
  LOG("gevgen", pNOTICE)
      << "Neutrino code (PDG): " << gOptNuPdgCode;
  LOG("gevgen", pNOTICE)
      << "Target code (PDG) & weight fraction (in case of multiple targets): ";
  map<int,double>::const_iterator iter;
  for(iter = gOptTgtMix.begin(); iter != gOptTgtMix.end(); ++iter) {
      int    tgtpdgc = iter->first;
      double wgt     = iter->second;
      LOG("gevgen", pNOTICE)
          << " >> " <<  tgtpdgc << " (weight fraction = " << wgt << ")";
  }
  LOG("gevgen", pNOTICE) << "\n";

  LOG("gevgen", pNOTICE) << *RunOpt::Instance();

}
//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gevgen", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "\n      gevgen [-h]"
    << "\n              [-r run#]"
    << "\n               -n nev"
    << "\n               -e energy (or energy range) "
    << "\n               -p neutrino_pdg"
    << "\n               -t target_pdg "
    << "\n              [-f flux_description]"
    << "\n              [-w]"
    << "\n              [--seed random_number_seed]"
    << "\n              [--cross-sections xml_file]"
    << "\n              [--event-generator-list list_name]"
    << "\n              [--message-thresholds xml_file]"
    << "\n              [--unphysical-event-mask mask]"
    << "\n              [--event-record-print-level level]"
    << "\n              [--mc-job-status-refresh-rate  rate]"
    << "\n              [--cache-file root_file]"
    << "\n\n" ;
  ListTargetIDs();
  ListFluxIDs();


}
//____________________________________________________________________________
#endif
