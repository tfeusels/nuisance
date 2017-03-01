// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "StatusMessage.h"

#include "ComparisonRoutines.h"

/*
  Constructor/Destructor
*/
//************************
void ComparisonRoutines::Init() {
  //************************

  fInputFile = "";
  fInputRootFile = NULL;

  fOutputFile = "";
  fOutputRootFile = NULL;

  fStrategy = "Compare";
  fRoutines.clear();

  fCardFile = "";

  fFakeDataInput = "";

  fSampleFCN = NULL;

  fAllowedRoutines = ("Compare");
};

//*************************************
ComparisonRoutines::~ComparisonRoutines() {
  //*************************************
};



/*
  Input Functions
*/
//*************************************
ComparisonRoutines::ComparisonRoutines(int argc, char* argv[]) {
//*************************************

  // Initialise Defaults
  Init();
  nuisconfig configuration = Config::Get();
  std::string cardfile = "";
  int maxevents = -1;
  int errorcount = Config::Get().GetParI("ERROR");
  int verbocount = Config::Get().GetParI("VERBOSITY");
  std::vector<std::string> xmlcmds;
  std::vector<std::string> configargs;

  // Make easier to handle arguments.
  std::vector<std::string> args = GeneralUtils::LoadCharToVectStr(argc, argv);
  ParserUtils::ParseArgument(args, "-c", fCardFile, true);
  ParserUtils::ParseArgument(args, "-o", fOutputFile, false, false);
  ParserUtils::ParseArgument(args, "-n", maxevents, false, false);
  ParserUtils::ParseArgument(args, "-f", fStrategy, false, false);
  ParserUtils::ParseSplitArgument(args, "-i", xmlcmds);
  ParserUtils::ParseArgument(args, "-q", configargs);
  ParserUtils::ParseCounter(args, "e", errorcount);
  ParserUtils::ParseCounter(args, "v", verbocount);
  ParserUtils::CheckBadArguments(args);

  // Add extra defaults if none given
  if (fCardFile.empty() and xmlcmds.empty()){
    ERR(FTL) << "No input supplied!" << std::endl;
    throw;
  }

  if (fOutputFile.empty() and !fCardFile.empty()) {
    fOutputFile = fCardFile + ".root";
    ERR(WRN) << "No output supplied so saving it to: " << fOutputFile << std::endl;

  } else if (fOutputFile.empty()) {
    ERR(FTL) << "No output file supplied!" << std::endl;
    throw;
  }

  // Setup this configuration
  fCompKey = Config::Get().CreateNode("nuiscomp");
  fCompKey.AddS("cardfile", fCardFile);
  fCompKey.AddS("outputfile", fOutputFile);
  fCompKey.AddS("strategy", fStrategy);

  // Load XML Cardfile
  configuration.LoadConfig( fCompKey.GetS("cardfile"),"");

  // Add CMD XML Structs
  for (size_t i = 0; i < xmlcmds.size(); i++){
    // std::cout << "Adding XML Line " << xmlcmds[i] << std::endl;
    configuration.AddXMLLine(xmlcmds[i]);
  }

  // Add Config Args
  for (size_t i = 0; i < configargs.size(); i++){
    configuration.OverrideConfig(configargs[i]);
  }

  // Add Error Verbo Lines
  FitPar::log_verb = verbocount;
  LOG_VERB(verbocount);
  ERR_VERB(errorcount);

  // Finish configuration XML
  configuration.FinaliseConfig(fCompKey.GetS("outputfile") + ".xml");

  // Proper Setup
  fOutputRootFile = new TFile(fCompKey.GetS("outputfile").c_str(), "RECREATE");
  SetupComparisonsFromXML();

  SetupRWEngine();
  SetupFCN();

  return;
};

//*************************************
void ComparisonRoutines::SetupComparisonsFromXML() {
//*************************************

  LOG(FIT) << "Setting up nuiscomp" << std::endl;

  // Setup Parameters ------------------------------------------
  std::vector<nuiskey> parkeys = Config::QueryKeys("parameter");
  if (!parkeys.empty()){
    LOG(FIT) << "Number of parameters :  " << parkeys.size() << std::endl;
  }

  for (size_t i = 0; i < parkeys.size(); i++) {
    nuiskey key = parkeys.at(i);

    // Check for type,name,nom
    if (!key.Has("type")) {
      ERR(FTL) << "No type given for parameter " << i << std::endl;
      throw;
    } else if (!key.Has("name")) {
      ERR(FTL) << "No name given for parameter " << i << std::endl;
      throw;
    } else if (!key.Has("nom")) {
      ERR(FTL) << "No nominal given for parameter " << i << std::endl;
      throw;
    }

    // Get Inputs
    std::string partype = key.GetS("type");
    std::string parname = key.GetS("name");
    double parnom  = key.GetD("nom");
    double parlow  = parnom - 1;
    double parhigh = parnom + 1;
    double parstep = 1;
    std::string parstate = key.GetS("state");

    // Extra limits
    if (key.Has("low")) {
      parlow  = key.GetD("low");
      parhigh = key.GetD("high");
      parstep = key.GetD("step");

      LOG(FIT) << "Read " << partype << " : "
               << parname << " = "
               << parnom << " : "
               << parlow << " < p < " << parhigh 
               << " : " << parstate << std::endl;
    } else {
      LOG(FIT) << "Read " << partype << " : "
               << parname << " = "
               << parnom << " : "
               << parstate << std::endl;
    }

    // Convert if required
    if (parstate.find("ABS") != std::string::npos) {
      parnom  = FitBase::RWAbsToSigma( partype, parname, parnom  );
      parlow  = FitBase::RWAbsToSigma( partype, parname, parlow  );
      parhigh = FitBase::RWAbsToSigma( partype, parname, parhigh );
      parstep = FitBase::RWAbsToSigma( partype, parname, parstep );
    } else if (parstate.find("FRAC") != std::string::npos) {
      parnom  = FitBase::RWFracToSigma( partype, parname, parnom  );
      parlow  = FitBase::RWFracToSigma( partype, parname, parlow  );
      parhigh = FitBase::RWFracToSigma( partype, parname, parhigh );
      parstep = FitBase::RWFracToSigma( partype, parname, parstep );
    }

    // Push into vectors
    fParams.push_back(parname);

    fTypeVals[parname]  = FitBase::ConvDialType(partype);;
    fCurVals[parname]   = parnom;
    fStateVals[parname] = parstate;

  }

  // Setup Samples ----------------------------------------------
  std::vector<nuiskey> samplekeys =  Config::QueryKeys("sample");
  if (!samplekeys.empty()){
    LOG(FIT) << "Number of samples : " << samplekeys.size() << std::endl;
  }

  for (size_t i = 0; i < samplekeys.size(); i++) {
    nuiskey key = samplekeys.at(i);

    // Get Sample Options
    std::string samplename = key.GetS("name");
    std::string samplefile = key.GetS("input");

    std::string sampletype =
      key.Has("type") ? key.GetS("type") : "DEFAULT";

    double samplenorm =
      key.Has("norm") ? key.GetD("norm") : 1.0;

    // Print out
    LOG(FIT) << "Read sample info " << i << " : " 
               << samplename << std::endl
               << "\t\t input -> " << samplefile  << std::endl
               << "\t\t state -> " << sampletype << std::endl
               << "\t\t norm  -> " << samplenorm << std::endl;

    // If FREE add to parameters otherwise continue
    if (sampletype.find("FREE") == std::string::npos) {
      continue;
    }

    // Form norm dial from samplename + sampletype + "_norm";
    std::string normname = samplename + sampletype + "_norm";

    // Check normname not already present
    if (fTypeVals.find("normname") != fTypeVals.end()) {
      continue;
    }

    // Add new norm dial to list if its passed above checks
    fParams.push_back(normname);

    fTypeVals[normname] = kNORM;
    fStateVals[normname] = sampletype;
    fCurVals[normname] = samplenorm;

  }

  // Setup Fake Parameters -----------------------------
  std::vector<nuiskey> fakekeys = Config::QueryKeys("fakeparameter");
  if (!fakekeys.empty()){
    LOG(FIT) << "Number of fake parameters : " << fakekeys.size() << std::endl; 
  }

  for (size_t i = 0; i < fakekeys.size(); i++) {
    nuiskey key = fakekeys.at(i);

    // Check for type,name,nom
    if (!key.Has("name")) {
      ERR(FTL) << "No name given for fakeparameter " << i << std::endl;
      throw;
    } else if (!key.Has("nom")) {
      ERR(FTL) << "No nominal given for fakeparameter " << i << std::endl;
      throw;
    }

    // Get Inputs
    std::string parname = key.GetS("name");
    double parnom  = key.GetD("nom");
   
    // Push into vectors
    fFakeVals[parname] = parnom;
  }
}

//*************************************
void ComparisonRoutines::SetupRWEngine() {
//*************************************

  LOG(FIT) << "Setting up FitWeight Engine" << std::endl;
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];
    FitBase::GetRW()->IncludeDial(name, fTypeVals.at(name));
  }

  return;
}

//*************************************
void ComparisonRoutines::SetupFCN() {
  //*************************************

  LOG(FIT) << "Building the SampleFCN" << std::endl;
  if (fSampleFCN) delete fSampleFCN;
  FitPar::Config().out = fOutputRootFile;
  fOutputRootFile->cd();
  fSampleFCN = new JointFCN(fOutputRootFile);
  SetFakeData();

  return;
}

//*************************************
void ComparisonRoutines::SetFakeData() {
//*************************************

  if (fFakeDataInput.empty()) return;

  if (fFakeDataInput.compare("MC") == 0) {
    LOG(FIT) << "Setting fake data from MC starting prediction." << std::endl;
    UpdateRWEngine(fFakeVals);

    FitBase::GetRW()->Reconfigure();
    fSampleFCN->ReconfigureAllEvents();
    fSampleFCN->SetFakeData("MC");

    UpdateRWEngine(fCurVals);

    LOG(FIT) << "Set all data to fake MC predictions." << std::endl;
  } else {
    LOG(FIT) << "Setting fake data from: " << fFakeDataInput << std::endl;
    fSampleFCN->SetFakeData(fFakeDataInput);
  }

  return;
}

/*
  Fitting Functions
*/
//*************************************
void ComparisonRoutines::UpdateRWEngine(
  std::map<std::string, double>& updateVals) {
  //*************************************

  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string name = fParams[i];

    if (updateVals.find(name) == updateVals.end()) continue;
    FitBase::GetRW()->SetDialValue(name, updateVals.at(name));
  }

  FitBase::GetRW()->Reconfigure();
  return;
}

//*************************************
void ComparisonRoutines::Run() {
//*************************************

  LOG(FIT) << "Running ComparisonRoutines : " << fStrategy << std::endl;

  if (FitPar::Config().GetParB("save_nominal")) {
    SaveNominal();
  }

  // Parse given routines
  fRoutines = GeneralUtils::ParseToStr(fStrategy,",");
  if (fRoutines.empty()){
    ERR(FTL) << "Trying to run ComparisonRoutines with no routines given!" << std::endl;
    throw;
  }

  for (UInt_t i = 0; i < fRoutines.size(); i++) {
    std::string routine = fRoutines.at(i);

    LOG(FIT) << "Routine: " << routine << std::endl;
    if (!routine.compare("Compare")) {
      UpdateRWEngine(fCurVals);
      GenerateComparison();
      PrintState();
      SaveCurrentState();
    }
  }



  return;
}

//*************************************
void ComparisonRoutines::GenerateComparison() {
  //*************************************
  LOG(FIT) << "Generating Comparison." << std::endl;
  // Main Event Loop from event Manager
  fSampleFCN->ReconfigureAllEvents();
  return;

}

//*************************************
void ComparisonRoutines::PrintState() {
  //*************************************
  LOG(FIT) << "------------" << std::endl;

  // Count max size
  int maxcount = 0;
  for (UInt_t i = 0; i < fParams.size(); i++) {
    maxcount = max(int(fParams[i].size()), maxcount);
  }

  // Header
  LOG(FIT) << " #    " << left << setw(maxcount) << "Parameter "
           << " = " << setw(10) << "Value"
           << " +- " << setw(10) << "Error"
           << " " << setw(8) << "(Units)"
           << " " << setw(10) << "Conv. Val"
           << " +- " << setw(10) << "Conv. Err"
           << " " << setw(8) << "(Units)" << std::endl;

  // Parameters
  for (UInt_t i = 0; i < fParams.size(); i++) {
    std::string syst = fParams.at(i);

    std::string typestr = FitBase::ConvDialType(fTypeVals[syst]);
    std::string curunits = "(sig.)";
    double curval = fCurVals[syst];
    double curerr = 0.0;

    if (fStateVals[syst].find("ABS") != std::string::npos) {
      curval = FitBase::RWSigmaToAbs(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
                FitBase::RWSigmaToAbs(typestr, syst, 0.0));
      curunits = "(Abs.)";
    } else if (fStateVals[syst].find("FRAC") != std::string::npos) {
      curval = FitBase::RWSigmaToFrac(typestr, syst, curval);
      curerr = (FitBase::RWSigmaToFrac(typestr, syst, curerr) -
                FitBase::RWSigmaToFrac(typestr, syst, 0.0));
      curunits = "(Frac)";
    }

    std::string convunits = "(" + FitBase::GetRWUnits(typestr, syst) + ")";
    double convval = FitBase::RWSigmaToAbs(typestr, syst, curval);
    double converr = (FitBase::RWSigmaToAbs(typestr, syst, curerr) -
                      FitBase::RWSigmaToAbs(typestr, syst, 0.0));

    std::ostringstream curparstring;

    curparstring << " " << setw(3) << left << i << ". " << setw(maxcount)
                 << syst << " = " << setw(10) << curval << " +- " << setw(10)
                 << curerr << " " << setw(8) << curunits << " " << setw(10)
                 << convval << " +- " << setw(10) << converr << " " << setw(8)
                 << convunits;

    LOG(FIT) << curparstring.str() << endl;
  }

  LOG(FIT) << "------------" << std::endl;
  double like = fSampleFCN->GetLikelihood();
  LOG(FIT) << std::left << std::setw(46) << "Likelihood for JointFCN: " << like << endl;
  LOG(FIT) << "------------" << std::endl;
}

/*
  Write Functions
*/
//*************************************
void ComparisonRoutines::SaveCurrentState(std::string subdir) {
//*************************************

  LOG(FIT) << "Saving current full FCN predictions" << std::endl;

  // Setup DIRS
  TDirectory* curdir = gDirectory;
  if (!subdir.empty()) {
    TDirectory* newdir = (TDirectory*)gDirectory->mkdir(subdir.c_str());
    newdir->cd();
  }

  fSampleFCN->Write();

  // Change back to current DIR
  curdir->cd();

  return;
}

//*************************************
void ComparisonRoutines::SaveNominal() {
  //*************************************

  fOutputRootFile->cd();

  LOG(FIT) << "Saving Nominal Predictions (be cautious with this)" << std::endl;
  FitBase::GetRW()->Reconfigure();
  GenerateComparison();
  SaveCurrentState("nominal");
};


