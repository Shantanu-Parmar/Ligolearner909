/**
 * @file 
 * @brief Manage Ox events.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxEvent__
#define __OxEvent__
#include "OxCorr.h"
#include "OxMeta.h"
#include "OxEventUtils.h"
#include "OxEventResults.h"

using namespace std;

/**
 * @brief Ox event management.
 * @details An Ox event is defined as a time slice in a cross-correlation map built with OxCorr. The parameters of an Ox event are managed with the OxEventParam class.
 *
 * This class can be used in two different ways:
 * 1. To build a list of events. In this case, the OxCorr engine is used to build cross-correlation maps from which Ox events are extracted: see Add(). Finally, events can be saved in a ROOT file:  see InitTree().
 * 2. To process a list of events. In this case, a list of ROOT files (generated in 1.) must be provided in the constructor. Then, the list of events is processed with ProcessEvents().
 */
class OxEvent: public OxCorr, public OxMeta, public GwollumPlot{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
   /**
   * @brief Constructor of the OxEvent class.
   * @details Two Omicron objects must be provided to initialize the cross-correlation engine (OxCorr). Optionnally, a ROOT file pattern can be provided to read Ox events from files.
   *
   * The cross-correlation slices are constructed here: see MakeSlices(). The contribution from calibration uncertainties must be provided: \f$\delta_\mathrm{cal}\f$. Another extra contribution to veto events can be added: \f$\delta_\mathrm{veto}\f$. See ComputeXi() for details.
   * @param[in] aName Name to uniquely identify the OxEvent object.
   * @param[in] aO1 Pointer to the first Omicron object.
   * @param[in] aO2 Pointer to the second Omicron object.
   * @param[in] aFilePattern ROOT file pattern. Use "" to build events.
   * @param[in] aDeltaCal Calibration time uncertainty [s]: \f$\delta_\mathrm{cal}\f$. This is only used for building events.
   * @param[in] aDeltaVeto Slice veto time window [s]: \f$\delta_\mathrm{veto}\f$. This is only used for building events.
   */ 
  OxEvent(const string aName,
          Omicron *aO1, Omicron *aO2, const string aFilePattern="",
          const double aDeltaCal=0.0, const double aDeltaVeto=0.0);

  /**
   * @brief Destructor of the OxEvent class.
   */
  virtual ~OxEvent(void);

  /**
     @}
  */

  /**
   * @brief Selects the slice index to simulate the zero lag.
   * @param[in] aSliceIndex Slice index (relative). If it is 0 or if the value is out of range, a random slice is selected.
   */
  void SetFakeZeroLag(const int aSliceIndex);

  /**
   * @brief Returns the number of events in the input ROOT files.
   * @note This function returns 0 when building events.
   */
  inline Long64_t GetEntries(void){ return event_chain->GetEntries(); };

  /**
   * @brief Loads an event from ROOT files.
   * @returns The function returns the number of bytes read from the input buffer.
   * @note The corresponding OxMeta entry is also loaded.
   * @param[in] aEntry Entry index.
   */
  int GetEntry(Long64_t aEntry);

  /**
   * @brief Returns the event parameter structure.
   * @details The event parameters are automatically loaded when calling an event with GetEntry().
   */
  inline OxEventParam* GetEventParameters(void){ return event_param; };

  /**
   * @brief Returns the event false-alarm rate [Hz].
   * @details The event false-alarm rate is derived from the background rank cumulative distribution.
   * @note This quantity can only be accessed after calling ProcessResults().
   * @sa OxEventResults::GetEventRate()
   */
  inline double GetEventFalseAlarmRate(void){
    return oxresults[OxEventType::oxevent_background]->GetEventRate(event_param->GetRank());
  };

  /**
   * @brief Returns the event inverse false-alarm rate [yrs].
   * @details The event inverse false-alarm rate is derived from the background rank cumulative distribution.
   * @note This quantity can only be accessed after calling ProcessResults().
   * @sa OxEventResults::GetEventRate()
   */
  inline double GetEventInverseFalseAlarmRate(void){
    return 1.0/oxresults[OxEventType::oxevent_background]->GetEventRate(event_param->GetRank())/O_SECONDS_IN_ONE_YEAR;
  };

  /**
   * @brief Processes all events in the input ROOT files.
   * @details Events are separated into 3 categories:
   * - Background events
   * - Zero-lag events
   * - Fake zero-lag events
   *
   * They are then saved in three OxEventResults object.
   */
  void ProcessEvents(void);

  /**
   * @brief Follows up the list of loudest events.
   * @details The Ox analysis is conducted over the list of loudest events. All results are posted in the output directory.
   * @param[in] aOutputDirectory Path to the output directory.
   * @param[in] aEventType Select the type of events.
   * @param[in] aN Maximum number of events to follow up in the list, starting with the loudest of all.
   * @param[in] aPowerLawIndex Power-law index. See OxCorr::ProcessCrossCorrelation().
   */
  void FollowUpLoudest(const string aOutputDirectory, const OxEventType aEventType, const unsigned int aN, const unsigned int aPowerLawIndex);

  /**
   * @brief Processes the analysis results.
   * @details The cumulative histograms are computed and all distributions are normalized to the number of events in the first histogram bin for the fake zero-lag.
   * @todo The normalization should be equivalent to a livetime normalization for a large number of events. This has to be checked. Indeed, the effect of hard cuts can create large fluctuations wrt the Gaussian case.
   * @sa OxEventResults::Process().
   */
  inline void ProcessResults(void){
    oxresults[OxEventType::oxevent_zerolag]->Process(oxresults[OxEventType::oxevent_fakezerolag]->GetEventN());
    oxresults[OxEventType::oxevent_fakezerolag]->Process(oxresults[OxEventType::oxevent_fakezerolag]->GetEventN());
    oxresults[OxEventType::oxevent_background]->Process(oxresults[OxEventType::oxevent_fakezerolag]->GetEventN());
  };
  
  /**
   * @brief Sets the detection threshold on the event rank.
   * @details An event is said to be detected if its rank is above a given threshold.
   * @param[in] aThreshold Minimum threshold on the event rank to detect an event.
   */
  inline void SetDetectionRankThreshold(const double aThreshold){ detection_rankthr = aThreshold; };
  
  /**
   * @brief Returns the detection threshold on the event rank.
   */
  inline double GetDetectionRankThreshold(void){ return detection_rankthr; };
  
  /**
   * @brief Sets the detection threshold as a threshold on the false-alarm rate threshold.
   * @details The threshold on the event rank is derived from the background distribution. This is an alternative to SetDetectionRankThreshold().
   * @warning The ProcessResults() function should be called first to finish the computation of the background distribution.
   * @param[in] aThreshold Detection false-alarm rate threshold [Hz].
  */
  inline void SetDetectionFarThreshold(const double aThreshold){
    detection_rankthr = oxresults[OxEventType::oxevent_background]->RateToRank(aThreshold);
  };
  
  /**
   * @brief Returns the detection threshold on the event false-alarm rate [Hz].
   * @warning The ProcessResults() function should be called first to finish the computation of the background distribution.
   */
  inline double GetDetectionFarThreshold(void){ return oxresults[OxEventType::oxevent_background]->GetEventRate(detection_rankthr); };
  
  /**
   * @brief Sets the number of loudest events to record.
   * @param[in] aZeroLagN Number of zero-lag events to record.
   * @param[in] aFakeZeroLagN Number of fake zero-lag events to record.
   * @param[in] aBackgroundN Number of background events to record.
   */
  inline void SetLoudestN(const unsigned int aZeroLagN, const unsigned int aBackgroundN, const unsigned int aFakeZeroLagN){
    oxresults[OxEventType::oxevent_zerolag]->SetLoudestN(aZeroLagN);
    oxresults[OxEventType::oxevent_fakezerolag]->SetLoudestN(aFakeZeroLagN);
    oxresults[OxEventType::oxevent_background]->SetLoudestN(aBackgroundN);
  }

  /**
   * @brief Creates an event TTree (`oxevent`) to save events.
   * @details This function is used to save events extracted from the cross-correlation maps: see Add().
   * @note The OxMeta tree is also created with OxMeta::InitTree()
   * @pre Call this function after opening a TFile.
   */
  void InitTree(void);

  /**
   * @brief Extracts events from a cross-correlation analysis.
   * @details When the cross-correlation is finished, call this function to extract the events (=slices) from the cross-correlation map. The \f$Xi\f$ value is calculated for each slice with ComputeXi(). Finally each slice is saved in the event tree.
   * @pre InitTree() should be called first.
   */
  bool Add(void);

  /**
   * @brief Returns the number of time bins in a slice.
   * @details The cross-correlation is measured as a function of the time delay between the two detectors (\f$\delta\tau\f$). The size of the bin is directly given by the Omicron parameters.
   * @sa MakeSlices() to get a detailed description of the slicing.
   */
  inline unsigned int GetNumberOfTimeBinsInOneSlice(void){ return slice_nbins; };

  /**
   * @brief Returns the number of slices.
   * @sa MakeSlices() to get a detailed description of the slicing.
   */
  inline unsigned int GetSliceN(void){ return slice_n; };

  /**
   * @brief Returns the maximum light travel time between the two detectors \f$\delta_\mathrm{det}\f$ [s].
   */
  inline double GetMaximumLightTravelTime(void){ return light_travel_time; };

  /**
   * @brief Returns the fake zero-lag slice index (relative).
   */
  inline int GetFakeZeroLagSliceIndex(void){
    return slice_fake0;
  };

  /**
   * @brief Returns the analysis results.
   * @param[in] aEventType Select the analysis type.
   */
  inline OxEventResults* GetResults(const OxEventType aEventType){
    return oxresults[aEventType];
  };

  /**
   * @brief Prints the analysis results.
   * @details Analysis plots are printed in the requested output directory as PNG and ROOT files:
   * - PrintRankPlot()
   * - PrintCumulativeRankPlot()
   * - PrintFrequencyPlot()
   * - PrintXiqPlot()
   * - PrintXiqMaxPlot()
   * - PrintBackgroundRankFrequencyPlot()
   * - PrintBackgroundSliceVeto2dPlot()
   * - PrintBackgroundSliceVeto1dPlot()
   * - PrintBackgroundTimeVariancePlot()
   * - PrintIfarPlot()
   *
   * @param[in] aOutputDirectory Path to the output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also print zero-lag results.
   */
  void PrintResults(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the event distribution of ranks in a png file and a ROOT file.
   * @details The plot is saved in `rk.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also print zero-lag results.
   */
  void PrintRankPlot(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the cumulative event rate distribution of ranks in a png file and a ROOT file.
   * @details The plot, is saved in `rk_cum.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also display the zero-lag results.
   */
  void PrintCumulativeRankPlot(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the background event rate distribution of frequencies in a png file and a ROOT file.
   * @details The plot is saved in `frequency.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also display the zero-lag results.
   */
  void PrintFrequencyPlot(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the event distribution of \f$\Xi\f$ spread across Q planes in a png file and a ROOT file.
   * @details The plot is saved in `Xiq.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also print zero-lag results.
   */
  void PrintXiqPlot(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the event distribution of \f$\Xi\f$ maximum spread across Q planes in a png file and a ROOT file.
   * @details The plot is saved in `Xiqmax.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also print zero-lag results.
   */
  void PrintXiqMaxPlot(const string aOutputDirectory, const bool aPlotZeroLag);

  /**
   * @brief Prints the background event rate distribution of ranks vs. frequencies in a png file and ROOT file.
   * @details The plot is saved in `rk_frequency.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   */
  void PrintBackgroundRankFrequencyPlot(const string aOutputDirectory);

  /**
   * @brief Prints the 2D background sliceveto plot in a png file and ROOT file.
   * @details The plot is saved in `slice_veto2d.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   */
  void PrintBackgroundSliceVeto2dPlot(const string aOutputDirectory);

  /**
   * @brief Prints the 1D background sliceveto plot in a png file and ROOT file.
   * @details The plot is saved in `slice_veto1d.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   */
  void PrintBackgroundSliceVeto1dPlot(const string aOutputDirectory);

  /**
   * @brief Prints the 1D background time variance plot in a png file and ROOT file.
   * @details The plot is saved in `time_variance.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   */
  void PrintBackgroundTimeVariancePlot(const string aOutputDirectory);

  /**
   * @brief Prints the inverse-false-alarm rate plot in a png file and a ROOT file.
   * @details The plot is saved in `ifar.png/.root`.
   * @param[in] aOutputDirectory Output directory.
   * @param[in] aPlotZeroLag Set this flag to true to also display the zero-lag results.
   */
  void PrintIfarPlot(const string aOutputDirectory, const bool aPlotZeroLag);

private:

  // EVENTS
  TTree *event_tree;                   ///< Event tree (DO NOT DELETE).
  TChain *event_chain;                 ///< Event chain.
  OxEventParam *event_param;           ///< Event parameters.
  double detection_rankthr;            ///< Detection threshold applied to the event rank.

  // SLICES
  double light_travel_time;            ///< Maximum light travel time between detectors [s].
  unsigned int slice_n;                ///< Number of slices.
  unsigned int slice_nbins;            ///< Number of time bins in a slice.
  unsigned int *slice_index_s;         ///< First time index of each slice.
  unsigned int **s_tmax;               ///< Container for ComputeXi(): time bin index of maximum \f$\xi\f$ for each frequency row and each Q plane.
  double **s_ximax;                    ///< Container for ComputeXi() - maximum \f$\xi\f$ along the frequency row / Q plane.
  double *s_Xi;                        ///< Container for ComputeXi() - \f$\Xi\f$.
  double **s_Xiq;                      ///< Container for ComputeXi() - \f$\Xi\f$ / Q plane.
  double **s_tvarq;                    ///< Container for ComputeXi() - time variance along the frequency direction / Q plane.
  double *s_deltatau;                  ///< Container for ComputeXi() - \f$\delta \tau\f$.
  unsigned int **s_nsliceveto;         ///< Container for ComputeXi() - number of frequency rows rejected by the slice veto / Q plane.
  double *s_frequency;                 ///< Container for ComputeXi() - frequency at maximum [Hz].
  int slice_fake0;                     ///< Relative slice index of fake zero lag.

  // RESULTS
  OxEventResults *oxresults[OxEventType::oxevent_n];///< Analysis results.

  /**
   * @brief TBC.
   */
  void ComputeXi(void);

  /**
   * @brief Slices the cross-correlation map.
   * @details A slice is a time window the size of which is at least \f$\pm (\delta_\mathrm{det} + \delta_\mathrm{cal} + \delta_\mathrm{veto})\f$. The maximum light travel time between the two detectors, \f$\delta_\mathrm{det}\f$, is automatically determined with the GetLightTravelTime() function from GWOLLUM. The calibration uncertainty \f$\delta_\mathrm{cal}\f$ must be provided in the constructor. An additional contribution, \f$\delta_\mathrm{veto}\f$, is used to veto events. This window size is chosen to include all correlated signals between the 2 detectors.
   *
   * The cross-correlation map, managed by the OxCorr class, is binned into \f$N_\tau\f$ time bins. The idea is to construct a sequence of slices with the following conditions:
   * - A slice must contain an even number of time bins.
   * - There must be an odd number of slices: \f$N_s = 2n_s+1\f$.
   * - Slices are indexed from \f$s=-n_s\f$ to \f$s=n_s\f$ (relative) or from \f$0\f$ to \f$N_s\f$.
   * - The zero-lag slice is indexed by \f$s=0\f$ (relative).
   * - The zero-lag slice must be centered on \f$\delta\tau=0\f$
   *
   * The number of time bins in a slice is the minimum number of time bins required to cover \f$\pm (\delta_\mathrm{det} + \delta_\mathrm{cal} + \delta_\mathrm{veto})\f$. It is returned by GetNumberOfTimeBinsInOneSlice().
   *
   * The number of slices \f$N_s\f$ is returned by GetSliceN().
   *
   * @note Some time bins may be left unused at both edges of the cross-corrlation map.
   *
   * The following image shows an example of slicing. The cross-correlation map is made of \f$N_\tau=48\f$ time bins. One slice contains 6 time bins, so there is \f$N_s=7\f$ slices. The zero-lag slice is positioned at the center and is indexed by 0. Three time bins at both edges do not belong to any slice.
   * @image html ox_slices.png "The cross-correlation map is sliced" width=700
   */
  void MakeSlices(void);

  /**
   * @brief Returns the duration of one slice [s].
   * @sa MakeSlices()
   */
  inline double GetSliceDuration(void){
    return (double)slice_nbins*OxCorr::GetTimeBinDuration();
  };

  /**
   * @brief Returns the cross-correlation \f$\delta \tau\f$ of a given slice.
   * @details This is the \f$\delta \tau\f$ value at the center of the slice
   * @param[in] aAbsoluteSliceIndex Absolute slice index.
   * @sa MakeSlices()
   */
  inline double GetSliceDeltaTau(const unsigned int aAbsoluteSliceIndex){
    return OxCorr::GetDeltaTau(slice_index_s[aAbsoluteSliceIndex] + slice_nbins/2);
  };

  /**
   * @brief Converts the relative slice index to an absolute slice index.
   * @details The absolute slice index runs from 0 to \f$N_s-1\f$.
   * The relative slice index runs from \f$-(N_s-1)/2\f$ to \f$+(N_s-1)/2\f$.
   * @param[in] aRelativeSliceIndex Relative slice index (0=zero-lag).
   */
  inline unsigned int RelativeToAbsoluteSliceIndex(const int aRelativeSliceIndex){
    return (unsigned int)(aRelativeSliceIndex + (int)((slice_n-1)/2));
  };

  /**
   * @brief Returns the absolute slice index for a given time bin.
   * @details The absolute slice index runs from 0 to \f$N_s-1\f$.
   * @warning This function can return an index which is out of range if the time bin does not belong to any slice.
   * @param[in] aTimeBinIndex Time bin index (starts at 0).
   */
  inline int GetAbsoluteSliceIndex(const unsigned int aTimeBinIndex){
    int ntbins_unused = (int)OxCorr::GetTimeBinsN()/2 - (int)(slice_n*slice_nbins)/2;
    if((int)aTimeBinIndex>=ntbins_unused)
      return ((int)aTimeBinIndex-ntbins_unused)/(int)slice_nbins;
    return -1;// for unused time bins at the beginning of the map
  };

  ClassDef(OxEvent,0)  
};


#endif


