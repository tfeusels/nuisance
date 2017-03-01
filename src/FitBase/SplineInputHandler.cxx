#include "SplineInputHandler.h"

SplineInputHandler::SplineInputHandler(std::string const& handle, std::string const& rawinputs) {

	// Run a joint input handling
	fName = handle;
	jointinput = false;
	jointindexswitch = 0;

	// Setup the TChain
	fFitEventTree = new TChain("nuisance_events");

	// Add to TChain
	fFitEventTree->Add( rawinputs.c_str() );

	// Open File for histogram access
	int evtcounter = 0;
	TFile* inp_file = new TFile(rawinputs.c_str(), "READ");

	// Get Flux/Event hist
	TH1D* fluxhist  = (TH1D*)inp_file->Get(
	                      (PlotUtils::GetObjectWithName(inp_file, "nuisance_fluxhist")).c_str());
	TH1D* eventhist = (TH1D*)inp_file->Get(
	                      (PlotUtils::GetObjectWithName(inp_file, "nuisance_eventhist")).c_str());

	// Get N events
	TTree* neuttree = (TTree*)inp_file->Get("nuisance_events");
	int nevents = neuttree->GetEntries();

	// Push into individual input vectors
	jointfluxinputs.push_back( (TH1D*) fluxhist->Clone() );
	jointeventinputs.push_back( (TH1D*) eventhist->Clone() );

	jointindexlow.push_back(evtcounter);
	jointindexhigh.push_back(evtcounter + nevents);
	evtcounter += nevents;

	// Add to the total flux/event hist
	if (!fFluxHist) fFluxHist = (TH1D*) fluxhist->Clone();
	else fFluxHist->Add(fluxhist);

	if (!fEventHist) fEventHist = (TH1D*) eventhist->Clone();
	else fEventHist->Add(eventhist);


	// Setup NEvents and the FitEvent
	fNEvents = fFitEventTree->GetEntries();
	fEventType = kNEWSPLINE;
	fNUISANCEEvent = new FitEvent();
	fNUISANCEEvent->SetBranchAddress(fFitEventTree);

	// Setup Reader
	fSplRead = new SplineReader();
	fSplRead->Read( (TTree*)inp_file->Get("spline_reader") );
	fNUISANCEEvent->fSplineRead = this->fSplRead;

	// Setup Friend Spline TTree
	fSplTree = (TTree*)inp_file->Get("spline_tree");
	// fSplTree->Add( rawinputs.c_str() );
	// fSplineCoeff = new double[400];
	fSplTree->SetBranchAddress( "SplineCoeff", fSplineCoeff );
	// fSplTree->GetEntry(0);
	// std::cout << " Branch Address " << fSplineCoeff << std::endl;

	fNUISANCEEvent->fSplineCoeff = this->fSplineCoeff;

	// Normalise event histograms for relative flux contributions.
	for (size_t i = 0; i < jointeventinputs.size(); i++) {
		TH1D* eventhist = (TH1D*) jointeventinputs.at(i)->Clone();

		double scale = double(fNEvents) / fEventHist->Integral("width");
		scale *= eventhist->Integral("width");
		scale /= double(jointindexhigh[i] - jointindexlow[i]);

		jointindexscale .push_back(scale);
	}

	fEventHist->SetNameTitle((fName + "_EVT").c_str(), (fName + "_EVT").c_str());
	fFluxHist->SetNameTitle((fName + "_FLUX").c_str(), (fName + "_FLUX").c_str());

};


FitEvent* SplineInputHandler::GetNuisanceEvent(const UInt_t entry) {

	// Make sure events setup
	if (entry >= fNEvents) return NULL;

	// Reset all variables before tree read
	fNUISANCEEvent->ResetEvent();

	// Read NUISANCE Tree
	// std::cout << "Getting entry " << entry << std::endl;
	fFitEventTree->GetEntry(entry);
	
	// std::cout << "Event Coeff = "  << fSplTree << " " << fSplineCoeff << " " << fNUISANCEEvent->fSplineCoeff << std::endl;
	fSplTree->GetEntry(entry);
	// std::cout << "Showing " << entry << std::endl;
	// fSplTree->Show(entry);

	fNUISANCEEvent->eventid = entry;

	// Run Initial, FSI, Final, Other ordering.
	fNUISANCEEvent-> OrderStack();

	// Setup Input scaling for joint inputs
	if (jointinput) {
		fNUISANCEEvent->InputWeight *= GetInputWeight(entry);
	} else {
		fNUISANCEEvent->InputWeight *= 1.0;
	}

	return fNUISANCEEvent;
}


double SplineInputHandler::GetInputWeight(int entry) {

	// Find Switch Scale
	while ( entry < jointindexlow[jointindexswitch] ||
	        entry >= jointindexhigh[jointindexswitch] ) {
		jointindexswitch++;

		// Loop Around
		if (jointindexswitch == jointindexlow.size()) {
			jointindexswitch = 0;
		}
	}
	return jointindexscale[jointindexswitch];
};


BaseFitEvt* SplineInputHandler::GetBaseEvent(const UInt_t entry) {

	// Make sure events setup
	if (entry >= fNEvents) return NULL;

	// Read entry from TTree to fill NEUT Vect in BaseFitEvt;
	fFitEventTree->GetEntry(entry);
	// fSplTree->GetEntry(entry);

	fBaseEvent->eventid = entry;

	// Set joint scaling if required
	if (jointinput) {
		fBaseEvent->InputWeight *= GetInputWeight(entry);
	} else {
		fBaseEvent->InputWeight *= 1.0;
	}

	return fBaseEvent;
}

void SplineInputHandler::Print() {}


