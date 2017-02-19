#include "SampleList.h"

//! Functions to make it easier for samples to be created and handled.
namespace SampleUtils {

//! Create a given sample given its name, file, type, fakdata(fkdt) file and the
//! current rw engine and push it back into the list fChain.
MeasurementBase* CreateSample(std::string name, std::string file, 
  std::string type, std::string fkdt, FitWeight* rw){

  nuiskey samplekey;
  samplekey.AddS("name",name);
  samplekey.AddS("input",file);
  samplekey.AddS("type",type);

  return CreateSample(samplekey);
}

MeasurementBase* CreateSample(nuiskey samplekey){

  FitWeight* rw = FitBase::GetRW();
  std::string name = samplekey.GetS("name");
  std::string file = samplekey.GetS("input");
  std::string type = samplekey.GetS("type");
  std::string fkdt = "";
  /*
     ANL CCQE Samples
  */

  if (!name.compare("ANL_CCQE_XSec_1DEnu_nu") ||
      !name.compare("ANL_CCQE_XSec_1DEnu_nu_PRL31") ||
      !name.compare("ANL_CCQE_XSec_1DEnu_nu_PRD16")) {
    return (new ANL_CCQE_XSec_1DEnu_nu(name, file, rw, type, fkdt));
  } else if (!name.compare("ANL_CCQE_Evt_1DQ2_nu") ||
             !name.compare("ANL_CCQE_Evt_1DQ2_nu_PRL31") ||
             !name.compare("ANL_CCQE_Evt_1DQ2_nu_PRD16")) {
    return (new ANL_CCQE_Evt_1DQ2_nu(name, file, rw, type, fkdt));
    /*
      ANL CC1ppip samples
    */
  } else if (!name.compare("ANL_CC1ppip_XSec_1DEnu_nu")) {
    return (new ANL_CC1ppip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_XSec_1DQ2_nu")) {
    return (new ANL_CC1ppip_XSec_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DQ2_nu")) {
    return (new ANL_CC1ppip_Evt_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dppi_nu")) {
    return (new ANL_CC1ppip_Evt_1Dppi_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dthpr_nu")) {
    return (new ANL_CC1ppip_Evt_1Dthpr_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1ppip_Evt_1DcosmuStar_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1DcosthAdler_nu")) {
    return (
        new ANL_CC1ppip_Evt_1DcosthAdler_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1ppip_Evt_1Dphi_nu")) {
    return (new ANL_CC1ppip_Evt_1Dphi_nu(file, rw, type, fkdt));
    /*
      ANL CC1npip sample
    */
  } else if (!name.compare("ANL_CC1npip_XSec_1DEnu_nu")) {
    return (new ANL_CC1npip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1npip_Evt_1DQ2_nu")) {
    return (new ANL_CC1npip_Evt_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1npip_Evt_1Dppi_nu")) {
    return (new ANL_CC1npip_Evt_1Dppi_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1npip_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1npip_Evt_1DcosmuStar_nu(file, rw, type, fkdt));
    /*
      ANL CC1pi0 sample
    */
  } else if (!name.compare("ANL_CC1pi0_XSec_1DEnu_nu")) {
    return (new ANL_CC1pi0_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1pi0_Evt_1DQ2_nu")) {
    return (new ANL_CC1pi0_Evt_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC1pi0_Evt_1DcosmuStar_nu")) {
    return (new ANL_CC1pi0_Evt_1DcosmuStar_nu(file, rw, type, fkdt));
    /*
      ANL NC1npip sample
    */
  } else if (!name.compare("ANL_NC1npip_Evt_1Dppi_nu")) {
    return (new ANL_NC1npip_Evt_1Dppi_nu(file, rw, type, fkdt));
    /*
      ANL NC1ppim sample
    */
  } else if (!name.compare("ANL_NC1ppim_XSec_1DEnu_nu")) {
    return (new ANL_NC1ppim_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_NC1ppim_Evt_1DcosmuStar_nu")) {
    return (new ANL_NC1ppim_Evt_1DcosmuStar_nu(file, rw, type, fkdt));
    /*
      ANL CC2pi sample
    */
  } else if (!name.compare("ANL_CC2pi_1pim1pip_XSec_1DEnu_nu")) {
    return (new   ANL_CC2pi_1pim1pip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu")) {
    return (new   ANL_CC2pi_1pim1pip_Evt_1Dpmu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dppip_nu")) {
    return (new   ANL_CC2pi_1pim1pip_Evt_1Dppip_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dppim_nu")) {
    return (new   ANL_CC2pi_1pim1pip_Evt_1Dppim_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu")) {
    return (new   ANL_CC2pi_1pim1pip_Evt_1Dpprot_nu(file, rw, type, fkdt));

  } else if (!name.compare("ANL_CC2pi_1pip1pip_XSec_1DEnu_nu")) {
    return (new   ANL_CC2pi_1pip1pip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu")) {
    return (new   ANL_CC2pi_1pip1pip_Evt_1Dpmu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu")) {
    return (new   ANL_CC2pi_1pip1pip_Evt_1Dpneut_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu")) {
    return (new   ANL_CC2pi_1pip1pip_Evt_1DppipHigh_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu")) {
    return (new   ANL_CC2pi_1pip1pip_Evt_1DppipLow_nu(file, rw, type, fkdt));

  } else if (!name.compare("ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu")) {
    return (new   ANL_CC2pi_1pip1pi0_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu")) {
    return (new   ANL_CC2pi_1pip1pi0_Evt_1Dpmu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu")) {
    return (new   ANL_CC2pi_1pip1pi0_Evt_1Dppip_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu")) {
    return (new   ANL_CC2pi_1pip1pi0_Evt_1Dppi0_nu(file, rw, type, fkdt));
  } else if (!name.compare("ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu")) {
    return (new   ANL_CC2pi_1pip1pi0_Evt_1Dpprot_nu(file, rw, type, fkdt));

  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dpmu_antinu")) {
    return (
        new ArgoNeuT_CCInc_XSec_1Dpmu_antinu(file, rw, type, fkdt));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dpmu_nu")) {
    return (new ArgoNeuT_CCInc_XSec_1Dpmu_nu(file, rw, type, fkdt));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dthetamu_antinu")) {
    return (
        new ArgoNeuT_CCInc_XSec_1Dthetamu_antinu(file, rw, type, fkdt));
  } else if (!name.compare("ArgoNeuT_CCInc_XSec_1Dthetamu_nu")) {
    return (
        new ArgoNeuT_CCInc_XSec_1Dthetamu_nu(file, rw, type, fkdt));
    /*
      BNL Samples
    */
  } else if (!name.compare("BNL_CCQE_XSec_1DEnu_nu")) {
    return (new BNL_CCQE_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CCQE_Evt_1DQ2_nu")) {
    return (new BNL_CCQE_Evt_1DQ2_nu(file, rw, type, fkdt));
    /*
      BNL CC1ppip samples
    */
  } else if (!name.compare("BNL_CC1ppip_XSec_1DEnu_nu")) {
    return (new BNL_CC1ppip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CC1ppip_Evt_1DQ2_nu")) {
    return (new BNL_CC1ppip_Evt_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CC1ppip_Evt_1DcosthAdler_nu")) {
    return (
        new BNL_CC1ppip_Evt_1DcosthAdler_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CC1ppip_Evt_1Dphi_nu")) {
    return (new BNL_CC1ppip_Evt_1Dphi_nu(file, rw, type, fkdt));

    /*
      BNL CC1npip samples
    */
  } else if (!name.compare("BNL_CC1npip_XSec_1DEnu_nu")) {
    return (new BNL_CC1npip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CC1npip_Evt_1DQ2_nu")) {
    return (new BNL_CC1npip_Evt_1DQ2_nu(file, rw, type, fkdt));
    /*
      BNL CC1pi0 samples
    */
  } else if (!name.compare("BNL_CC1pi0_XSec_1DEnu_nu")) {
    return (new BNL_CC1pi0_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BNL_CC1pi0_Evt_1DQ2_nu")) {
    return (new BNL_CC1pi0_Evt_1DQ2_nu(file, rw, type, fkdt));

    /*
      FNAL Samples
    */
  } else if (!name.compare("FNAL_CCQE_Evt_1DQ2_nu")) {
    return (new FNAL_CCQE_Evt_1DQ2_nu(file, rw, type, fkdt));
    /*
      FNAL CC1ppip
    */
  } else if (!name.compare("FNAL_CC1ppip_XSec_1DEnu_nu")) {
    return (new FNAL_CC1ppip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("FNAL_CC1ppip_XSec_1DQ2_nu")) {
    return (new FNAL_CC1ppip_XSec_1DQ2_nu(file, rw, type, fkdt));
  } else if (!name.compare("FNAL_CC1ppip_Evt_1DQ2_nu")) {
    return (new FNAL_CC1ppip_Evt_1DQ2_nu(file, rw, type, fkdt));
    /*
      FNAL CC1ppim
    */
    //    } else if (!name.compare("FNAL_CC1ppim_XSec_1DEnu_antinu")) {
    //      return (new FNAL_CC1ppim_XSec_1DEnu_antinu(file, rw, type,
    //      fkdt));

    /*
      BEBC Samples
    */
  } else if (!name.compare("BEBC_CCQE_XSec_1DQ2_nu")) {
    return (new BEBC_CCQE_XSec_1DQ2_nu(name, file, rw, type, fkdt));
    /*
      BEBC CC1ppip samples
    */
  } else if (!name.compare("BEBC_CC1ppip_XSec_1DEnu_nu")) {
    return (new BEBC_CC1ppip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BEBC_CC1ppip_XSec_1DQ2_nu")) {
    return (new BEBC_CC1ppip_XSec_1DQ2_nu(file, rw, type, fkdt));
    /*
      BEBC CC1npip samples
    */
  } else if (!name.compare("BEBC_CC1npip_XSec_1DEnu_nu")) {
    return (new BEBC_CC1npip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BEBC_CC1npip_XSec_1DQ2_nu")) {
    return (new BEBC_CC1npip_XSec_1DQ2_nu(file, rw, type, fkdt));
    /*
      BEBC CC1pi0 samples
    */
  } else if (!name.compare("BEBC_CC1pi0_XSec_1DEnu_nu")) {
    return (new BEBC_CC1pi0_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("BEBC_CC1pi0_XSec_1DQ2_nu")) {
    return (new BEBC_CC1pi0_XSec_1DQ2_nu(file, rw, type, fkdt));
    /*
      BEBC CC1npim samples
    */
  } else if (!name.compare("BEBC_CC1npim_XSec_1DEnu_antinu")) {
    return (new BEBC_CC1npim_XSec_1DEnu_antinu(file, rw, type, fkdt));
  } else if (!name.compare("BEBC_CC1npim_XSec_1DQ2_antinu")) {
    return (new BEBC_CC1npim_XSec_1DQ2_antinu(file, rw, type, fkdt));
    /*
      BEBC CC1ppim samples
    */
  } else if (!name.compare("BEBC_CC1ppim_XSec_1DEnu_antinu")) {
    return (new BEBC_CC1ppim_XSec_1DEnu_antinu(file, rw, type, fkdt));
  } else if (!name.compare("BEBC_CC1ppim_XSec_1DQ2_antinu")) {
    return (new BEBC_CC1ppim_XSec_1DQ2_antinu(file, rw, type, fkdt));

    /*
      GGM CC1ppip samples
    */
  } else if (!name.compare("GGM_CC1ppip_XSec_1DEnu_nu")) {
    return (new GGM_CC1ppip_XSec_1DEnu_nu(file, rw, type, fkdt));
  } else if (!name.compare("GGM_CC1ppip_Evt_1DQ2_nu")) {
    return (new GGM_CC1ppip_Evt_1DQ2_nu(file, rw, type, fkdt));

    /*
      MiniBooNE Samples
    */
    /*
      CCQE
    */
  } else if (!name.compare("MiniBooNE_CCQE_XSec_1DQ2_nu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_1DQ2_nu")) {
    return (
        new MiniBooNE_CCQE_XSec_1DQ2_nu(samplekey));
  } else if (!name.compare("MiniBooNE_CCQE_XSec_1DQ2_antinu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_1DQ2_antinu") ||
             !name.compare("MiniBooNE_CCQE_CTarg_XSec_1DQ2_antinu")) {
    return (
        new MiniBooNE_CCQE_XSec_1DQ2_antinu(samplekey));

  } else if (!name.compare("MiniBooNE_CCQE_XSec_2DTcos_nu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_2DTcos_nu")) {
    return (
        new MiniBooNE_CCQE_XSec_2DTcos_nu(name, file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CCQE_XSec_2DTcos_antinu") ||
             !name.compare("MiniBooNE_CCQELike_XSec_2DTcos_antinu")) {
    return (
        new MiniBooNE_CCQE_XSec_2DTcos_antinu(name, file, rw, type, fkdt));

    /*
      MiniBooNE CC1pi+
    */
    // 1D
  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DEnu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DEnu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DQ2_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DQ2_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DTpi_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DTpi_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_1DTu_nu")) {
    return (new MiniBooNE_CC1pip_XSec_1DTu_nu(file, rw, type, fkdt));

    // 2D
  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DQ2Enu_nu")) {
    return (
        new MiniBooNE_CC1pip_XSec_2DQ2Enu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTpiCospi_nu")) {
    return (
        new MiniBooNE_CC1pip_XSec_2DTpiCospi_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTpiEnu_nu")) {
    return (
        new MiniBooNE_CC1pip_XSec_2DTpiEnu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTuCosmu_nu")) {
    return (
        new MiniBooNE_CC1pip_XSec_2DTuCosmu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pip_XSec_2DTuEnu_nu")) {
    return (
        new MiniBooNE_CC1pip_XSec_2DTuEnu_nu(file, rw, type, fkdt));

    /*
      MiniBooNE CC1pi0
    */
  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DEnu_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DEnu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DQ2_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DQ2_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1DTu_nu")) {
    return (new MiniBooNE_CC1pi0_XSec_1DTu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dcosmu_nu")) {
    return (
        new MiniBooNE_CC1pi0_XSec_1Dcosmu_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dcospi0_nu")) {
    return (
        new MiniBooNE_CC1pi0_XSec_1Dcospi0_nu(file, rw, type, fkdt));

  } else if (!name.compare("MiniBooNE_CC1pi0_XSec_1Dppi0_nu")) {
    return (
        new MiniBooNE_CC1pi0_XSec_1Dppi0_nu(file, rw, type, fkdt));

    /*
      MiniBooNE NCEL
    */
  } else if (!name.compare("MiniBooNE_NCEL_XSec_Treco_nu")) {
    ERR(FTL)
      << "MiniBooNE_NCEL_XSec_Treco_nu not implemented in current interface."
      << std::endl;
    throw 5;
    // return (new MiniBooNE_NCEL_XSec_Treco_nu(file, rw, type,
    // fkdt));

    /*
MINERvA Samples
    */
  } else if (!name.compare("MINERvA_CCQE_XSec_1DQ2_nu") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_20deg") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_nu_20deg_oldflux")) {
    return (
        new MINERvA_CCQE_XSec_1DQ2_nu(name, file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CCQE_XSec_1DQ2_antinu") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_20deg") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_antinu_20deg_oldflux")) {
    return (
        new MINERvA_CCQE_XSec_1DQ2_antinu(name, file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CCQE_XSec_1DQ2_joint_oldflux") ||
             !name.compare("MINERvA_CCQE_XSec_1DQ2_joint_20deg_oldflux") ||
	     !name.compare("MINERvA_CCQE_XSec_1DQ2_joint") ||
	     !name.compare("MINERvA_CCQE_XSec_1DQ2_joint_20deg")){

    return (new MINERvA_CCQE_XSec_1DQ2_joint(name, file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DEe_nue")) {
    return (new MINERvA_CC0pi_XSec_1DEe_nue(file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_nue")) {
    return (new MINERvA_CC0pi_XSec_1DQ2_nue(file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DThetae_nue")) {
    return (
        new MINERvA_CC0pi_XSec_1DThetae_nue(file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC0pi_XSec_1DQ2_nu_proton")) {
    return (
        new MINERvA_CC0pi_XSec_1DQ2_nu_proton(file, rw, type, fkdt));
    /*
      CC1pi+
    */
// DONE
  } else if (!name.compare("MINERvA_CC1pip_XSec_1DTpi_nu") ||
             !name.compare("MINERvA_CC1pip_XSec_1DTpi_nu_20deg")) {
    return (new MINERvA_CC1pip_XSec_1DTpi_nu(name, file, rw, type, fkdt));

// DONE
  } else if (!name.compare("MINERvA_CC1pip_XSec_1Dth_nu") ||
             !name.compare("MINERvA_CC1pip_XSec_1Dth_nu_20deg")) {
    return (new MINERvA_CC1pip_XSec_1Dth_nu(name, file, rw, type, fkdt));

    /*
      CCNpi+
    */
  // DONE
  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dth_nu") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2015") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_2016") ||
             !name.compare("MINERvA_CCNpip_XSec_1Dth_nu_20deg")) {
    return (new MINERvA_CCNpip_XSec_1Dth_nu(name, file, rw, type, fkdt));

 // Done
  } else if (!name.compare("MINERvA_CCNpip_XSec_1DTpi_nu") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2015") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_2016") ||
             !name.compare("MINERvA_CCNpip_XSec_1DTpi_nu_20deg")) {
    return (new MINERvA_CCNpip_XSec_1DTpi_nu(name, file, rw, type, fkdt));


// Done
  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dthmu_nu")) {
    return (new MINERvA_CCNpip_XSec_1Dthmu_nu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CCNpip_XSec_1Dpmu_nu")) {
    return (new MINERvA_CCNpip_XSec_1Dpmu_nu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CCNpip_XSec_1DQ2_nu")) {
    return (new MINERvA_CCNpip_XSec_1DQ2_nu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CCNpip_XSec_1DEnu_nu")) {
    return (new MINERvA_CCNpip_XSec_1DEnu_nu(file, rw, type, fkdt));

    /*
      CC1pi0
    */
  // Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2015") ||
             !name.compare("MINERvA_CC1pi0_XSec_1Dth_antinu_2016")) {
    return (
        new MINERvA_CC1pi0_XSec_1Dth_antinu(name, file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dppi0_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1Dppi0_antinu(file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DTpi0_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1DTpi0_antinu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DQ2_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1DQ2_antinu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dthmu_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1Dthmu_antinu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1Dpmu_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1Dpmu_antinu(file, rw, type, fkdt));

// Done
  } else if (!name.compare("MINERvA_CC1pi0_XSec_1DEnu_antinu")) {
    return (
        new MINERvA_CC1pi0_XSec_1DEnu_antinu(file, rw, type, fkdt));

    /*
      CCINC
    */
  } else if (!name.compare("MINERvA_CCinc_XSec_2DEavq3_nu")) {
    return (new MINERvA_CCinc_XSec_2DEavq3_nu(file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CCinc_XSec_1Dx_ratio_C12_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1Dx_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1Dx_ratio_Pb208_CH")) {
    return (
        new MINERvA_CCinc_XSec_1Dx_ratio(name, file, rw, type, fkdt));

  } else if (!name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_C12_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_Fe56_CH") ||
             !name.compare("MINERvA_CCinc_XSec_1DEnu_ratio_Pb208_CH")) {
    return (
        new MINERvA_CCinc_XSec_1DEnu_ratio(name, file, rw, type, fkdt));

    /*
T2K Samples
    */

  } else if (!name.compare("T2K_CC0pi_XSec_2DPcos_nu") ||
	     !name.compare("T2K_CC0pi_XSec_2DPcos_nu_I") ||
	     !name.compare("T2K_CC0pi_XSec_2DPcos_nu_II")) {
    return (new T2K_CC0pi_XSec_2DPcos_nu(name, file, rw, type));

    /*
      T2K CC1pi+ CH samples
    */
    // Comment these out for now because we don't have the proper data
    /*
    
  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dpmu_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dpmu_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dppi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dppi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1DQ2_nu")) {
    return (new T2K_CC1pip_CH_XSec_1DQ2_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dq3_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dq3_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthmupi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dthmupi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthpi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dthpi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1Dthq3pi_nu")) {
    return (new T2K_CC1pip_CH_XSec_1Dthq3pi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_CH_XSec_1DWrec_nu")) {
    return (new T2K_CC1pip_CH_XSec_1DWrec_nu(file, rw, type, fkdt));
    */

    /*
      T2K CC1pi+ H2O samples
    */
  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1DEnuDelta_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1DEnuDelta_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1DEnuMB_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1DEnuMB_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcosmu_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1Dcosmu_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcosmupi_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1Dcosmupi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dcospi_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1Dcospi_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dpmu_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1Dpmu_nu(file, rw, type, fkdt));

  } else if (!name.compare("T2K_CC1pip_H2O_XSec_1Dppi_nu")) {
    return (new   T2K_CC1pip_H2O_XSec_1Dppi_nu(file, rw, type, fkdt));

    /*
      T2K CC0pi + np CH samples
    */
  } else if (!name.compare("T2K_CC0pinp_STV_XSec_1Ddpt_nu")) {
    return (new T2K_CC0pinp_STV_XSec_1Ddpt_nu(file, rw, type, fkdt));

    /*
K2K Samples
    */
    /*
      NC1pi0
    */
  } else if (!name.compare("K2K_NC1pi0_Evt_1Dppi0_nu")) {
    return (new K2K_NC1pi0_Evt_1Dppi0_nu(file, rw, type, fkdt));

    /*
Fake Studies
    */

  } else if (name.find("ExpMultDist_CCQE_XSec_1D") != std::string::npos &&
             name.find("_FakeStudy") != std::string::npos) {
    return (
        new ExpMultDist_CCQE_XSec_1DVar_FakeStudy(name, file, rw, type, fkdt));

  } else if (name.find("ExpMultDist_CCQE_XSec_2D") != std::string::npos &&
             name.find("_FakeStudy") != std::string::npos) {
    return (
        new ExpMultDist_CCQE_XSec_2DVar_FakeStudy(name, file, rw, type, fkdt));

  } else if (name.find("GenericFlux_") != std::string::npos) {
    return (new GenericFlux_Tester(name, file, rw, type, fkdt));

  } else if (name.find("MCStudy_KaonPreSelection") != std::string::npos) {
    return (new MCStudy_KaonPreSelection(name, file, rw, type, fkdt));

  } else if (name.find("MuonValidation_") != std::string::npos) {
    return (new MCStudy_MuonValidation(name, file, rw, type, fkdt));
        
  } else {
    ERR(FTL) << "Error: No such sample: " << name << std::endl;
    exit(-1);
    return NULL;
  }

  // Return NULL if no sample loaded.
  return NULL;
}
}
