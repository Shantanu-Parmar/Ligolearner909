/**
 * @file 
 * @brief Ox metadata.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxMeta__
#define __OxMeta__
#include <ReadTriggerMetaData.h>
#include "Oomicron.h"
#include <TChain.h>

using namespace std;

/**
 * @brief Metadata for Ox events.
 * @details When running an Ox analysis, the metadata associated to events are saved in a TTree. One set of metadata is associated to one cross-correlation map.
 *
 * After opening a ROOT file, call InitTree to attach the metadata TTree.
 *
 * This class can also be used to read metadata from a chain of TTrees.
 */
class OxMeta: public TChain{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
  /**
   * @brief Constructor of the OxMeta class.
   * @details If a ROOT file pattern is given, all the "oxmeta" TTrees are chained. If not, all the metadata are set to default values.
   * @param[in] aFilePattern ROOT file pattern (optional).
   */
  OxMeta(const string aFilePattern="");

  /**
   * @brief Destructor of the OxMeta class.
   */
  virtual ~OxMeta(void);

  /**
     @}
  */

  /**
   * @brief Creates the metadata TTree.
   * @pre Call this function after opening a TFile, so that the metadata TTree ia owned by the file.
   */
  void InitTree(void);

  /**
   * @brief Returns the current calibration uncertainty \f$\delta_\mathrm{cal}\f$ [s].
   */
  inline double GetCalibrationUncertainty(void){ return meta_delta_cal; };

  /**
   * @brief Returns the current veto time \f$\delta_\mathrm{veto}\f$ [s].
   */
  inline double GetVetoTime(void){ return meta_delta_veto; };

  /**
   * @brief Returns the current GPS time.
   * @returns The time is given at the center of the analysis window.
   */
  inline unsigned int GetTime(void){
    return (meta_start_time+meta_end_time)/2;
  };

  /**
   * @brief Returns the current GPS start time [s].
   */
  unsigned int GetStartTime(void){ return meta_start_time; };

  /**
   * @brief Returns the current GPS end time [s].
   */
  unsigned int GetEndTime(void){ return meta_end_time; };
  
  /**
   * @brief Returns the maximum SNR squared in the first detector.
   */
  inline double GetSnrSq1(void){
    return meta_snrsq1;
  };

  /**
   * @brief Returns the maximum SNR squared in the second detector.
   */
  inline double GetSnrSq2(void){
    return meta_snrsq2;
  };

protected:

  // METADATA
  TTree *meta_tree;              ///< Metadata tree (DO NOT DELETE).
  double meta_delta_cal;         ///< Calibration time uncertainty [s].
  double meta_delta_veto;        ///< Slice veto time contribution [s].
  unsigned int meta_start_time;  ///< GPS start time [s].
  unsigned int meta_end_time;    ///< GPS end time [s].
  double meta_snrsq1;            ///< Maximum SNR squared for detector 1 (from metadata tree).
  double meta_snrsq2;            ///< Maximum SNR squared for detector 2 (from metadata tree).

  ClassDef(OxMeta,0)  
};


#endif


