// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "MeasurementBase.h"

/*
  Constructor/Destructors
*/

//********************************************************************
// 2nd Level Constructor (Inherits From MeasurementBase.h)
MeasurementBase::MeasurementBase() {
//********************************************************************

  scaleFactor = 1.0;
  filledMC = false;

};

//********************************************************************
// 2nd Level Destructor (Inherits From MeasurementBase.h)
MeasurementBase::~MeasurementBase() {
//********************************************************************

};


//********************************************************************
void MeasurementBase::SetFluxHistogram(std::string fluxFile, int minE, int maxE, double fluxNorm){
  //********************************************************************

  // Note this expects the flux bins to be given in terms of MeV
  LOG(SAM) << "Reading flux from file: " << fluxFile << std::endl;

  TGraph f(fluxFile.c_str(),"%lg %lg");

  this->fluxHist = new TH1D((this->measurementName+"_flux").c_str(), (this->measurementName+"; E_{#nu} (GeV)").c_str(),\
			    f.GetN()-1, minE, maxE);

  Double_t *yVal = f.GetY();

  for (int i = 0; i<fluxHist->GetNbinsX(); ++i)
    this->fluxHist->SetBinContent(i+1, yVal[i]*fluxNorm);

};

//********************************************************************
double MeasurementBase::TotalIntegratedFlux(std::string intOpt, double low, double high){
  //********************************************************************

  if(GetInput()->GetType() == kGiBUU){
    return 1.0;
  }

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = this->fluxHist->GetXaxis()->FindBin(low);
  int maxBin = this->fluxHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->fluxHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral;
};

//********************************************************************
double MeasurementBase::PredictedEventRate(std::string intOpt, double low, double high){
//********************************************************************

  // Set Energy Limits
  if (low == -9999.9)  low  = this->EnuMin;
  if (high == -9999.9) high = this->EnuMax;

  int minBin = this->eventHist->GetXaxis()->FindBin(low);
  int maxBin = this->eventHist->GetXaxis()->FindBin(high);

  // Get integral over custom range
  double integral = this->eventHist->Integral(minBin, maxBin+1, intOpt.c_str());

  return integral * 1E-38;
};


//********************************************************************
void MeasurementBase::SetupInputs(std::string inputfile){
//********************************************************************

  // Add this infile to the global manager
  if (FitPar::Config().GetParB("EventManager")){
    FitBase::AddInput(measurementName, inputfile);

    // Get a pointer to the input so we can grab flux stuff
    // Slightly Convoluted...
    input = FitBase::GetInput( FitBase::GetInputID(inputfile) );

  } else {
    input = new InputHandler(measurementName, inputfile);
  }

  this->fluxHist      = input->GetFluxHistogram();
  this->eventHist     = input->GetEventHistogram();
  this->xsecHist      = input->GetXSecHistogram();
  this->nevents       = input->GetNEvents();

  inputfilename = inputfile;
}

//***********************************************
int MeasurementBase::GetInputID(){
//***********************************************
  return FitBase::GetInputID(inputfilename);
}

//***********************************************
void MeasurementBase::Reconfigure(){
//***********************************************
  LOG(REC) << " Reconfiguring sample "<<this->measurementName<<std::endl;

  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;
  if (using_evtmanager) input_id = FitBase::GetInputID(inputfilename);
  cust_event = input->GetEventPointer();

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (input->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(input->GetSplineHead());
  }

  FitEvent* cust_event = input->GetEventPointer();
  int nevents = input->GetNEvents();
  int countwidth = (nevents/200);

  // Reset Signal Vectors
  this->X_VAR_VECT.clear();
  this->Y_VAR_VECT.clear();
  this->Z_VAR_VECT.clear();
  this->MODE_VECT.clear();
  this->INDEX_VECT.clear();

  size_t NSignal = 0;
  // MAIN EVENT LOOP
  for (int i = 0; i < nevents; i++){

    // Read in the TChain and Calc Kinematics
    if (using_evtmanager){
      cust_event = FitBase::EvtManager().GetEvent(input_id, i);
    } else {
      input->ReadEvent(i);

      cust_event->RWWeight = FitBase::GetRW()->CalcWeight(cust_event);
      cust_event->Weight   = cust_event->RWWeight*cust_event->InputWeight;

      Weight = cust_event->Weight;
    }

    Weight = cust_event->Weight;

    // Initialize
    X_VAR = 0.0;
    Y_VAR = 0.0;
    Z_VAR = 0.0;
    Signal = false;
    Mode = cust_event->Mode;

    // Extract Measurement Variables
    this->FillEventVariables(cust_event);
    Signal = this->isSignal(cust_event);

    // Push Back Signal
    if (Signal){
      this->X_VAR_VECT .push_back(X_VAR);
      this->Y_VAR_VECT .push_back(Y_VAR);
      this->Z_VAR_VECT .push_back(Z_VAR);
      this->MODE_VECT  .push_back(Mode);
      this->INDEX_VECT .push_back( (UInt_t)i);
      NSignal++;
    }

    // Fill Histogram Values
    this->FillHistograms();
    //    this->FillExtraHistograms();

    // Print Out
    if (LOG_LEVEL(REC) and countwidth and !(i % countwidth))
      LOG(REC) << "Reconfigured " << i <<" total events. [S,X,Y,Z,M,W] = ["
	       << Signal << ", "
	       << X_VAR  << ", "<< Y_VAR <<  ", "
	       << Z_VAR  << ", "<< Mode << ", "
	       << Weight << "] "<< std::endl;


  }

  // Finalise Histograms
  filledMC = true;
  this->ConvertEventRates();
}


//***********************************************
void MeasurementBase::ReconfigureFast(){
//***********************************************
  LOG(REC) << " Reconfiguring signal "<<this->measurementName<<std::endl;
  bool using_evtmanager = FitPar::Config().GetParB("EventManager");
  int input_id = -1;
  if (using_evtmanager){
    input_id = FitBase::GetInputID(inputfilename);
  } else {
    cust_event = input->GetEventPointer();
  }

  // Check if we Can't Signal Reconfigure
  if (!filledMC){
    this->Reconfigure();
    return;
  }

  // Reset Histograms
  this->ResetAll();

  // READ in spline head for this input
  if (input->GetType() == kEVTSPLINE){
    FitBase::GetRW()->ReadSplineHead(input->GetSplineHead());
  }

  // Get Pointer To Base Event (Just Generator Formats)
  int countwidth = (nevents / 20);

  // Setup Iterators
  std::vector<double>::iterator X = X_VAR_VECT.begin();
  std::vector<double>::iterator Y = Y_VAR_VECT.begin();
  std::vector<double>::iterator Z = Z_VAR_VECT.begin();
  std::vector<int>::iterator    M = MODE_VECT.begin();
  std::vector<UInt_t>::iterator I = INDEX_VECT.begin();

  // SIGNAL LOOP
  for (int i = 0; I != INDEX_VECT.end(); I++, i++){

    // Just Update Weight
    if (using_evtmanager){
      Weight = FitBase::EvtManager().GetEventWeight(input_id, (*I));
    } else {
      input->GetTreeEntry((*I));
      Weight = FitBase::GetRW()->CalcWeight(cust_event)	\
         	* cust_event->InputWeight;
    }

    X_VAR = (*X);
    Y_VAR = (*Y);
    Z_VAR = (*Z);
    Mode  = (*M);
    Signal = true;

    // Sort Histograms
    this->FillHistograms();

    // Get Next Iteration
    X++;
    Y++;
    Z++;
    M++;

    // Print Out
    if (LOG_LEVEL(REC) && (i) % countwidth == 0)
      LOG(REC) << "Reconfigured " << i <<" signal events. [X,Y,Z,M,W] = ["
	       << X_VAR  << ", " << Y_VAR << ", "
	       << Z_VAR  << ", " << Mode  << ", "
	       << Weight << "] " << std::endl;
  }

  // Finalise histograms
  filledMC = true;
  this->ConvertEventRates();
}

//***********************************************
void MeasurementBase::ConvertEventRates(){
//***********************************************

  this->ScaleEvents();
  this->ApplyNormScale( FitBase::GetRW()->GetSampleNorm( this->measurementName ) ) ;

}

//***********************************************
InputHandler* MeasurementBase::GetInput(){
//***********************************************

  if(FitPar::Config().GetParB("EventManager")){
    return FitBase::GetInput(FitBase::GetInputID(inputfilename));
  } else {
    return this->input;
  }
  return NULL;
};

//***********************************************
void MeasurementBase::Renormalise(){
//***********************************************

  // Called when the fitter has changed a measurements normalisation but not any reweight dials
  // Means we don't have to call the time consuming reconfigure when this happens.
  double norm = FitBase::GetRW()->GetDialValue( this->measurementName + "_norm" );

  if ((this->currentNorm == 0.0 and norm != 0.0) or not filledMC){
    this->ReconfigureFast();
    return;
  }

  if (this->currentNorm == norm) return;

  this->ApplyNormScale( 1.0 / this->currentNorm );
  this->ApplyNormScale( norm );

  return;
};


//***********************************************
void MeasurementBase::SetSignal(bool sig){
//***********************************************
  Signal = sig;
}

//***********************************************
void MeasurementBase::SetSignal(FitEvent* evt){
//***********************************************
  Signal = this->isSignal(evt);
}

//***********************************************
void MeasurementBase::SetWeight(double wght){
//***********************************************
  Weight = wght;
}

//***********************************************
void MeasurementBase::SetMode(int md){
//***********************************************
  Mode = md;
}
