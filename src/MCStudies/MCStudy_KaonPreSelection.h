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

#ifndef MCStudy_KaonPreSelection_H_SEEN
#define MCStudy_KaonPreSelection_H_SEEN
#include "Measurement1D.h"

//********************************************************************
class MCStudy_KaonPreSelection : public Measurement1D {
//********************************************************************

public:

  MCStudy_KaonPreSelection(std::string name, std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile);
  virtual ~MCStudy_KaonPreSelection() {};

  //! Grab info from event
  void FillEventVariables(FitEvent *event);

  //! Define this samples signal
  bool isSignal(FitEvent *nvect);

  //! Write Files
  void Write(std::string drawOpt);

 private:
  
  double fEventScaleFactor;
  TTree* fEventTree;
  int nlep;
  int nkplus;
  int nkaon;
  double kplusmom;
  double kaonmom;
  bool fKaonLogging;
};

#endif
