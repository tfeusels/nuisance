#ifndef NEUTINPUTHANDLER_H
#define NEUTINPUTHANDLER_H
#include "InputHandler2.h"
#include "TargetUtils.h"
#ifdef __NEUT_ENABLED__
#include "neutpart.h"
#include "neutvect.h"
#endif

#include "PlotUtils.h"

class NEUTInputHandler : public InputHandlerBase {
public:

	NEUTInputHandler(std::string const& handle, std::string const& rawinputs);
	~NEUTInputHandler(){};

	FitEvent* GetNuisanceEvent(const UInt_t entry);
	int GetNeutParticleStatus(NeutPart* part);
	void CalcNUISANCEKinematics();
	double GetInputWeight(int entry);
	BaseFitEvt* GetBaseEvent(const UInt_t entry);
	void Print();

	std::vector<TH1D*> jointfluxinputs;
	std::vector<TH1D*> jointeventinputs;
	std::vector<int> jointindexlow;
	std::vector<int> jointindexhigh;
	size_t jointindexswitch;
	bool jointinput;
	std::vector<double> jointindexscale;

	bool save_extra;
	TChain* fNEUTTree;
	NeutVect* fNeutVect;  
};

class NEUTGeneratorInfo : GeneratorInfoBase {
public:
  NEUTGeneratorInfo(){};
  ~NEUTGeneratorInfo(){};
  void AddBranchesToTree(TTree* tn);

  const static UInt_t kMaxParticles = 400;
  int fNEUT_ParticleStatusCode[kMaxParticles];
  int fNEUT_ParticleAliveCode[kMaxParticles];

};

#endif