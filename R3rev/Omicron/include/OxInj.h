/**
 * @file 
 * @brief Ox injections.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxInj__
#define __OxInj__
#include <TFile.h>
#include <InjRea.h>
#include <GwollumPlot.h>
#include "OxEvent.h"

using namespace std;

/**
 * @brief Manage injections for Ox analyses.
 * @details This class is used to recover injections in a set of Ox events. After constructing the object:
 * - Call Match() to associate Ox events to injections.
 * - Call Process() to detect injections, fixing the false-alarm rate threshold.
 * - Call Print() to print the results.
 */
class OxInj: public InjRea{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
  /**
   * @brief Constructor of the OxInj class.
   * @details The list of injections must be provided as a set of ROOT files generated with the GWOLLUM/InjGen class. Injections are loaded and they can be accessed with the GWOLLUM/InjRea class methods.
   *
   * In this constructor, parameter histograms and plots are initialized for each waveform name.
   * @param[in] aFilePattern File pattern pointing to ROOT files with injections (GWOLLUM/InjGen).
   * @param[in] aQN Number of Q planes.
   * @param[in] aNbins Number of bins to distribute the injections.
   */
  OxInj(const string aFilePattern, const unsigned int aQN, const unsigned int aNbins=20);

  /**
   * @brief Destructor of the OxInj class.
   */
  virtual ~OxInj(void);

  /**
     @}
  */

  /**
   * @brief Matches injections with Ox events.
   * @details The input pointer to the OxEvent object is saved internally. Then, events are scanned:
   * - An injection is set as valid (injection tag = true) if the injection time is found in the analysis times covered by the input events.
   * - The list of valid injections is saved (event tree entry, best rank value, and analysis index).
   *
   * @note This function can be called multiple times with different sets of events. For a given injection, the highest rank value is recorded.
   * @warning The geocentric time of the injection is considered. In other words, we neglect the time propagation to the detector sites.
   *
   * @warning Do not delete the OxEvent object as long as injections are being analyzed. In particular, the OxEvent objects should still exist when calling Process().
   *
   * @param[in] aEvent List of Ox events.
   */
  void Match(OxEvent *aEvent);
  
  /**
   * @brief Processes injections.
   * @details After injections have been matched to Ox events with Match(), call this function to detect injections and fill plots.
   *
   * An injection is detected if the associated event rank value is above the detection threshold. The detection threshold is internally managed by the OxEvent object saved in Match().
   */
  void Process(void);
  
  /**
   * @brief Prints injection results.
   * @details Injection distribution plots are printed as png files for each injection name:
   * - Injection amplitude (total and detected).
   * - Injection frequency (total and detected).
   * - Injection time (total and detected).
   * - Detection efficiency as a function of the injection amplitude.
   * - Event rank versus injection amplitude (total and detected).
   * - Event time versus injection amplitude (total and detected).
   * - Event frequency versus injection amplitude (total and detected).
   * - Event \f$\Xi\f$ spread versus injection amplitude (total and detected).
   * - Event \f$\Xi\f$ maximum spread versus injection amplitude (total and detected).
   * - Event slice veto versus injection amplitude for each Q plane(total and detected).
   * - Event time variance versus injection amplitude for each Q plane(total and detected).
   *
   * All the plots are also saved in a ROOT file.
   *
   * Also, the list of injections is printed in a text file.
   * @param[in] aOutDir Path to the output directory.
   * @param[in] aGwollumPlot Pointer to a GwollumPlot object used to print the plots.
   */
  void Print(const string aOutDir, GwollumPlot *aGwollumPlot);
  
  /**
   * @brief Returns the number of valid injections of a given name.
   * @param[in] aNameIndex Injection name index.
   * @pre The injection name index must be valid.
   */
  inline unsigned int GetValidN(const unsigned int aNameIndex){ return (unsigned int)h1_inj_amp_tot[aNameIndex]->GetEntries(); };
  
  /**
   * @brief Returns the number of detected injections of a given name.
   * @param[in] aNameIndex Injection name index.
   * @pre The injection name index must be valid.
   */
  inline unsigned int GetDetectedN(const unsigned int aNameIndex){ return (unsigned int)h1_inj_amp_det[aNameIndex]->GetEntries(); };
  
private:

  unsigned int Nq;                 ///< Number of Q planes.
  vector<OxEvent*> oxevents;       ///< Event lists - DO NOT DELETE.

  unsigned int *inj_event_index;   ///< Event list index.
  Long64_t *inj_evententry;        ///< List of event entries.
  double *inj_rk;                  ///< List of event ranks.

  // Injection parameters
  TH1D** h1_inj_amp_tot;          ///< Injection amplitude distribution /injection name.
  TH1D** h1_inj_amp_det;          ///< Detected injection amplitude distribution /injection name.
  TH1D** h1_inj_time_tot;         ///< Injection time distribution /injection name.
  TH1D** h1_inj_time_det;         ///< Detected time amplitude distribution /injection name.
  TH1D** h1_inj_freq_tot;         ///< Injection frequency distribution /injection name.
  TH1D** h1_inj_freq_det;         ///< Detected frequency amplitude distribution /injection name.

  // Event parameters
  TGraph** g_ev_rk_amp_tot;       ///< Event rank distribution / injection amplitude.
  TGraph** g_ev_rk_amp_det;       ///< Event rank distribution / injection amplitude.
  TGraph** g_ev_rk_time_tot;      ///< Event rank distribution / injection time.
  TGraph** g_ev_rk_time_det;      ///< Event rank distribution / injection time.
  TGraph** g_ev_rk_freq_tot;      ///< Event rank distribution / injection frequency.
  TGraph** g_ev_rk_freq_det;      ///< Event rank distribution / injection frequency.
  TGraph** g_ev_Xiq_amp_tot;       ///< Event \f$\Xi\f$ spread distribution / injection amplitude.
  TGraph** g_ev_Xiq_amp_det;       ///< Event \f$\Xi\f$ spread distribution / injection amplitude.
  TGraph** g_ev_Xiqmax_amp_tot;    ///< Event \f$\Xi\f$ maximum spread distribution / injection amplitude.
  TGraph** g_ev_Xiqmax_amp_det;    ///< Event \f$\Xi\f$ maximum spread distribution / injection amplitude.
  TGraph*** g_ev_sliceveto_amp_tot;///< Event slice veto / injection amplitude.
  TGraph*** g_ev_sliceveto_amp_det;///< Event slice veto / injection amplitude.
  TGraph*** g_ev_tvar_amp_tot;     ///< Event time variance distribution / injection amplitude.
  TGraph*** g_ev_tvar_amp_det;     ///< Event time variance distribution / injection amplitude.

  ClassDef(OxInj,0)  
};


#endif


