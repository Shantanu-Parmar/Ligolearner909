/**
 * @file 
 * @brief Manage Ox analysis results.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxEventResults__
#define __OxEventResults__

#include <TGraphErrors.h>
#include "OxEventUtils.h"
#include "Oomicron.h"

using namespace std;

/**
 * @brief Ox analysis results.
 * @details This class is designed to collect Ox events and present results of an Ox analysis. The class must be initialized with an OxEventParam object from which the event parameters will be extracted. Everytime an event is loaded, call SaveEvent() to save the event parameters. Multiple plots are then filled. After all events have been loaded, call Process() to process the results. Finally, Get functions are available to acess result plots. 
 */
class OxEventResults{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
   /**
   * @brief Constructor of the OxEventResults class.
   * @details All the analysis containers/plots are created. An OxEventParam object must be linked to save events in the result structures.
   * @param[in] aName Name to uniquely identify the OxEventResults object.
   * @param[in] aParam Ox event parameters.
   */ 
  OxEventResults(const string aName, OxEventParam* aParam);

  /**
   * @brief Destructor of the OxEventResults class.
   */
  virtual ~OxEventResults(void);

  /**
     @}
  */
  
  /**
   * @brief Resets all the analysis results.
   */
  void Reset(void);

  /**
   * @brief Saves an event.
   * @details Call this function to save an event and its parameters in the result structures. The OxEventParam object linked in the constructor is used to extract the event parameters.
   * @param[in] aStartTime Event GPS start time [s].
   * @param[in] aEndTime Event GPS end time [s].
   */
  void SaveEvent(const unsigned int aStartTime, const unsigned int aEndTime);

  /**
   * @brief Processes the results.
   * @details When all the events are saved with SaveEvent(), call this function to process the results:
   * - Cumulative distribution histograms are computed.
   * - All the histograms are normalized to contain a reference number of events.
   * - All histograms are given statistical error bars.
   *
   * @param[in] aEventsNorm Reference number of events to normalize all histograms.
   */
  void Process(const unsigned int aEventsNorm);

  /**
   * @brief Returns the event rank corresponding to a given event rate.
   * @details The event cumulative rank distribution is interpolated to associate a rank value to a given event rate value.
   *
   * If the requested event rate is too high, the lower end of the rank distribution is returned as an effective value. If the event rate cannot be reached (too low), the highest known rank value is returned.
   * @warning The event cumulative rank distribution is used for the interpolation. Therefore the Process() function must be called first.
   * @param[in] aRate Requested event rate value [Hz].
  */
  double RateToRank(const double aRate);
  
  /**
   * @brief Sets the number of loudest events to record.
   * @note The list of loudest events is reset if it already exists: all rank values are set to -1.
   * @param[in] aN Number of events to record.
   */
  inline void SetLoudestN(const unsigned int aN=0){
    g_loudest->Set(aN);
    for(int p=0; p<g_loudest->GetN(); p++) g_loudest->SetPoint(p, -1.0, -1.0);
  };

  /**
   * @brief Returns the number of events saved until now.
   * @details This is the number of events collected until now with the SaveEvent() function. It only includes events inside the range of the rank distribition histogram, excluding under/overflows.
   */
  inline unsigned int GetEventN(void){
    return (unsigned int)h1_rk->GetSumOfWeights();
  };

  /**
   * @brief Returns the event rate [Hz] above a given rank value.
   * @pre Results must be processed first (Process()).
   * @param[in] aRank Event rank value.
   */
  inline double GetEventRate(const double aRank){
    if(segments->GetLiveTime()>0.0) return h1_rk_cum->Interpolate(aRank)/segments->GetLiveTime();
    return 0.0;
  };

  /**
   * @brief Returns the number of loudest events.
   * @note This is a maximum number. The list of actual events can be shorter.
   */
  inline unsigned int GetLoudestN(void){ return (unsigned int)g_loudest->GetN(); };

  /**
   * @brief Returns the event TTree index in the list of loudest events.
   * @returns  It returns -1 if that event does not exist.
   * @param[out] aRank The rank value for that event.
   * @param[in] aLoudestIndex Index in the list of loudest events.
   */
  inline Long64_t GetLoudest(double &aRank, const unsigned int aLoudestIndex){
    if(aLoudestIndex>=GetLoudestN()) return -1;
    aRank = g_loudest->GetX()[aLoudestIndex];
    return (Long64_t)g_loudest->GetY()[aLoudestIndex];
  };

  /**
   * @brief Returns the analysis segments.
   * @warning DO NOT DELETE.
   */
  inline Segments* GetSegments(void){ return segments; };

  /**
   * @brief Returns the cumulative livetime [s].
   */
  inline unsigned int GetCumulativeLiveTime(void){ return livetime; };

  /**
   * @brief Returns the 1D histogram of the event rank distribution.
   * @warning DO NOT DELETE.
   */
  inline TH1D* GetRankPlot(void){ return h1_rk; };

  /**
   * @brief Returns the cumulative 1D histogram of the event rank distribution.
   * @warning DO NOT DELETE.
   */
  inline TH1D* GetCumulativeRankPlot(void){ return h1_rk_cum; };

  /**
   * @brief Returns the 1D histogram of the event frequency distribution.
   * @warning DO NOT DELETE.
   */
  inline TH1D* GetFrequencyPlot(void){ return h1_frequency; };

  /**
   * @brief Returns the 1D histogram of the spread of \f$\Xi\f$ values across Q planes.
   * @warning DO NOT DELETE.
   */
  inline TH1D* GetXiqPlot(void){ return h1_Xiq; };

  /**
   * @brief Returns the 1D histogram of the maximum spread of \f$\Xi\f$ values across Q planes.
   * @warning DO NOT DELETE.
   */
  inline TH1D* GetXiqMaxPlot(void){ return h1_Xiq_max; };

  /**
   * @brief Returns the 2D histogram of the event rank vs. frequency distribution.
   * @warning DO NOT DELETE.
   */
  inline TH2D* GetRankFrequencyPlot(void){ return h2_rk_frequency; };

  /**
   * @brief Returns the 2D histogram of the event slce veto.
   * @warning DO NOT DELETE.
   */
  inline TH2D* GetSliceVeto2dPlot(void){ return h2_sliceveto; };

  /**
   * @brief Returns the 1D histogram of the event slice veto.
   * @warning DO NOT DELETE.
   * @param[in] aQindex Q plane index: must be valid!
   */
  inline TH1D* GetSliceVeto1dPlot(const unsigned int aQindex){ return h1_sliceveto[aQindex]; };

  /**
   * @brief Returns the 1D histogram of the time variance along frequency rows.
   * @warning DO NOT DELETE.
   * @param[in] aQindex Q plane index: must be valid!
   */
  inline TH1D* GetTimeVariancePlot(const unsigned int aQindex){ return h1_tvar[aQindex]; };

  /**
   * @brief Returns the inverse false-alarm rate TGraph for the loudest events.
   * @details The list of loudest events is scanned. For each event, the false-alarm rate is computed. It is converted to an inverse false-alarm rate in [yr]. Then the cumulative distribution is built.
   * @note If the loudest events have a rank value higher than the the loudest background event, the inverse false-alarm rate is set to 0.
   * @warning The returned TGraph must be deleted by the user.
   * @param[in] aBackground Background distribution to evaluate the event false-alarm rate.
   */
  TGraphErrors* GetIfarPlot(OxEventResults *aBackground);

private:

  // ANALYSIS CONTAINERS
  OxEventParam *param;           ///< Ox event parameters - do not delete.
  Segments *segments;            ///< Analysis segments.
  unsigned int livetime;         ///< Analysis cumulative livetime [s].
  TH1D *h1_rk;                   ///< Rank distribution.
  TH1D *h1_rk_cum;               ///< Rank cumulative distribution.
  TH1D *h1_frequency;            ///< Frequency distribution [Hz].
  TH1D *h1_Xiq;                  ///< \f$\Xi\f$ spread across Q planes distribution.
  TH1D *h1_Xiq_max;              ///< \f$\Xi\f$ maximum spread across Q planes distribution.
  TH1D **h1_sliceveto;           ///< Number of frequency rows rejected by the slice veto / Q plane.
  TH1D **h1_tvar;                ///< Time variance along frequency rows / Q plane.
  TH2D *h2_rk_frequency;         ///< Rank vs frequency [Hz].
  TH2D *h2_sliceveto;            ///< Number of frequency rows rejected by the slice veto.
  TGraphErrors* g_loudest;       ///< List of loudest events (X=rank, Y=entry).
  
  ClassDef(OxEventResults,0)  
};


#endif


