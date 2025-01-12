/**
 * @file 
 * @brief Omicron tiling structure.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Otile__
#define __Otile__

#include "Oqplane.h"
#include "Osequence.h"
#include <GwollumPlot.h>

using namespace std;

/**
 * @brief Computes a set of Q values.
 * @details This function returns a vector of Q values corresponding to a set of parameters:
 * @param[in] aQMin Minimal Q value.
 * @param[in] aQMax Maximal Q value.
 * @param[in] aMaximumMismatch Maximum mismatch between Q planes.
 */
vector <double> ComputeQs(const double aQMin, const double aQMax,
                          const double aMaximumMismatch);


/**
 * @brief Construct a time-frequency-Q tiling structure.
 * @details This class is designed to tile the 3-dimensional space in time, frequency and Q.
 * The tiling consists of logarithmically spaced Q-planes.
 * Each of these planes is divided in logarithmically spaced frequency bands.
 * Each of these bands are then linearly divided in time bins.
 * Once constructed, the planes can be used to apply a Q-transform.
 * @sa Oqplane. 
 *
 * This class also initializes an analysis sequence with the Osequence class from which it inherits.
 *
 * This class offers a graphical interface (GwollumPlot inheritance) and plotting functions to display the tiles and the data.
 */
class Otile: public Osequence, public GwollumPlot {

 public:

  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the Otile class.
   * @details The 3-dimensional tiling structure is constructed given the user parameters.
   * The parameter space is defined by a time range, a frequency range and a Q range. The user must specify a maximum mismatch value to guarantee a maximal fractional energy loss from one tile to the next.
   *
   * The analysis sequence is initialized: @sa Osequence::Osequence().
   * @param[in] aTimeRange Time range [s].
   * @param[in] aTimeOverlap Time overlap [s].
   * @param[in] aQMin Minimal Q value.
   * @param[in] aQMax Maximal Q value.
   * @param[in] aFrequencyMin Minimal frequency [Hz].
   * @param[in] aFrequencyMax Maximal frequency [Hz].
   * @param[in] aSampleFrequency Sampling frequency [Hz].
   * @param[in] aMaximumMismatch Maximum mismatch between tiles.
   * @param[in] aFullMapNt Full map time resolution (number of bins). 0 = full resolution. Use = 1 for a minimum resolution. This resolution cannot be changed anymore.
   * @param[in] aPlotStyle Plotting style.
   * @param[in] aVerbosity Verbosity level.
   */
  Otile(const unsigned int aTimeRange,
        const unsigned int aTimeOverlap,
	const double aQMin, 
	const double aQMax, 
	const double aFrequencyMin, 
	const double aFrequencyMax, 
	const unsigned int aSampleFrequency, 
	const double aMaximumMismatch,
        const unsigned int aFullMapNt=1,
	const string aPlotStyle="GWOLLUM", 
	const unsigned int aVerbosity=0);

  /**
   * @brief Destructor of the Otile class.
   */
  virtual ~Otile(void);
  /**
     @}
  */
  
  /**
   * @brief Returns the number of Q-planes.
   */
  inline unsigned int GetQN(void){ return nq; };

  /**
   * @brief Returns the Q value of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, 0 is returned.
   */
  inline double GetQ(const unsigned int aQindex){
    if(aQindex<nq) return qplanes[aQindex]->GetQ();
    return 0.0;
  };

  /**
   * @brief Returns the number of frequency bands of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   */
  inline unsigned int GetBandN(const unsigned int aQindex){
    if(aQindex<nq) return qplanes[aQindex]->GetBandN();
    return qplanes[nq-1]->GetBandN();
  };

  /**
   * @brief Returns the band central frequency [Hz] of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandFrequency(const unsigned int aQindex, const unsigned int aBandIndex){
    if(aQindex<nq) return qplanes[aQindex]->GetBandFrequency(aBandIndex);
    return qplanes[nq-1]->GetBandFrequency(aBandIndex);
  };

  /**
   * @brief Returns the number of tiles in a band of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   * @param[in] aBandIndex Band index (irrelevant for the full map).
   * @pre The band index must be valid for a Q plane.
   */
  inline unsigned int GetBandTileN(const unsigned int aQindex, const unsigned int aBandIndex){ 
    if(aQindex<nq) return qplanes[aQindex]->GetBandTileN(aBandIndex);
    if(FullMapNt>0) return FullMapNt;
    return qplanes[0]->GetBandTileN(qplanes[0]->GetBandN()-1);
  };

  /**
   * @brief Returns the SNR squared of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileSnrSq(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){ 
    return qplanes[aQindex]->GetTileSnrSq(aTimeTileIndex, aBandIndex);
  };

  /**
   * @brief Returns the amplitude of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileAmplitude(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){
    return qplanes[aQindex]->GetTileAmplitude(aTimeTileIndex, aBandIndex);
  };

  /**
   * @brief Returns the amplitude squared of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileAmplitudeSq(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){
    return qplanes[aQindex]->GetTileAmplitudeSq(aTimeTileIndex, aBandIndex);
  };

  /**
   * @brief Returns the maximum SNR squared estimated in a given Q plane.
   * @param[in] aQindex Q-plane index.
   */
  inline double GetSnrSqMax(const unsigned int aQindex){ return qplanes[aQindex]->GetSnrSqMax(); };

  /**
   * @brief Returns the total number of tiles.
   * @param[in] aPadding Number of seconds excluded on both sides of the time range.
   * @pre The padding value is not checked! Make sure it is compatible with the time range.
   */
  long unsigned int GetTileN(const double aPadding=0.0);

  /**
   * @brief Returns the list of frequency bands.
   * @details The returned array is of size GetBandN()+1 and contains the bin limits. 
   * @warning The returned array must be deleted by the user.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   */
  double* GetBands(const unsigned int aQindex);

  /**
   * @brief Sets the list of time windows for plots.
   * @details The full map containers are created.
   @note If a time window in the list is 0 or is larger than the tiling time range, it is forced to take the tiling time range value.
   * @param[in] aWindows List of time windows [s].
   */
  void SetPlotTimeWindows(vector <unsigned int> aWindows);
  
  /**
   * @brief Returns the list of time windows for plots.
   */
  inline vector <unsigned int> GetPlotTimeWindows(void){ return pwin; };

  /**
   * @brief Displays a canonical representation of a given Q-plane.
   * @param[in] aQindex Q-plane index: must be valid.
   */
  void DrawMapTiling(const unsigned int aQindex);

  /**
   * @brief Computes the noise power associated to the tiling structure.
   * @sa Oqplane::SetPower().
   * @param[in] aSpec1 First noise power spectrum used to whiten the data.
   * @param[in] aSpec2 Second noise power spectrum used to whiten the data.
   */
  inline void SetPower(Spectrum *aSpec1, Spectrum *aSpec2){
    for(unsigned int q=0; q<nq; q++) qplanes[q]->SetPower(aSpec1, aSpec2);
  };
  
  /**
   * @brief Projects a whitened data vector onto the Q planes.
   * @details The complex data vector is projected onto each Q-plane.
   * The data is provided through a fft object.
   * The fft::Forward() must be applied before calling this function.
   * @returns The number of tiles (excluding half the overlap on both sides) above the SNR threshold.
   * @sa Oqplane::ProjectData().
   * @param[in] aDataFft fft structure containing the data to project.
   */
  long unsigned int ProjectData(fft *aDataFft);

  /**
   * @brief Returns tile segments.
   * @details A tile segment is the tile start/stop.
   * Here, tiles are selected if the SNR is larger than a given threshold.
   * The threshold is given as a TH1D histogram binned in the tile frequency [Hz].
   * The bin content is the SNR threshold.
   * A negative bin content is considered as an infinite threshold.
   * Out-of-range frequencies are associated to an infinite threshold.
   * @param[in] aSnrThreshold SNR threshold as a function of frequency.
   * @param[in] aPadding Number of seconds excluded on both sides of the tiling structure when selecting tiles above the SNR threshold.
   * @note The user is in charge of deleting the returned Segments object.
   * @sa Oqplane::AddTileSegments()
   */
  Segments* GetTileSegments(TH1D *aSnrThreshold, const double aPadding);

  /**
   * @brief Saves tiles in a trigger structure.
   * @details Tiles with a SNR value above the SNR threshold are saved in the input trigger structure.
   * @sa Oqplane::SaveTriggers().
   *
   * The corresponding triggers Segments are also saved following the GWOLLUM convention for triggers.
   * If the `Sequence` algorithm is in use, the current timing is applied to the tiling.
   *
   * @sa SetSnrThr() and Osequence::SetSegments().
   *
   * @param[in] aTriggers TriggerBuffer object.
   * @returns true if the function was successful, false otherwise.
   */
  bool SaveTriggers(TriggerBuffer *aTriggers);

  /**
   * @brief Saves the maps for each Q-planes in output files.
   * @details The maps are saved in output files.
   * An additionnal map called "full map" is also saved.
   * It combines all tiles projected in the time-frequency plane.
   * @warning Maps are not saved if the maximum SNR within the first window is below the SNR map threshold: see SetSnrThr().
   *
   * @returns The maximum SNR value within the first window time range (-1.0 if this function fails)
   *
   * @param[in] aOutdir Output directory path to save the plots. It must exist.
   * @param[in] aName Name identifier used for titles.
   * @param[in] aFormat Output format string: usual graphical formats are supported.
   * @param[in] aTimeOffset Time offset applied to the window center [s].
   * @param[in] aThumb Produce thumbnails if set to true.
   */
  double SaveMaps(const string aOutdir, const string aName, const string aFormat,
                  const double aTimeOffset=0.0, const bool aThumb=false);

  /**
   * @brief Defines how to fill the maps.
   * @details Use a keyword to define the content of maps:
   * - "snr": fill with SNR values
   * - "amplitude": fill with amplitude values
   * - "phase": fill with phase values
   *
   * @param[in] aMapContentType Map content keyword.
   */
  inline void SetMapFill(const string aMapContentType="snr"){
    if(!aMapContentType.compare("amplitude")) mapfill="amplitude";
    else if(!aMapContentType.compare("phase")) mapfill="phase";
    else mapfill="snr";
    return;
  };

  /**
   * @brief Sets the map Z-axis range.
   * @details If aZmin>=aZmax, the Z-axis is automatically ranged.
   * @param[in] aZmin Minimum Z value.
   * @param[in] aZmax Maximum Z value.
   */
  inline void SetRangez(const double aZmin=-1.0, const double aZmax=-1.0){
    vrange[0] = aZmin;
    vrange[1] = aZmax;
  };

  /**
   * @brief Returns the map Z-axis range minimum.
   */
  inline double GetRangezMin(void){ return vrange[0]; };

  /**
   * @brief Sets a SNR threshold when saving maps and triggers.
   * @details The thresholds are applied when calling the SaveMaps() or SaveTriggers() functions.
   * @param[in] aSnrThr_map When calling SaveMaps(), a map is not saved if the loudest tile is below that threshold.
   * @param[in] aSnrThr_trigger Tiles with a SNR value below that threshold are not saved when calling SaveTriggers().
   */
  inline void SetSnrThr(const double aSnrThr_map=0.0, const double aSnrThr_trigger=2.0){
    SnrThr_map=aSnrThr_map;
    for(unsigned int q=0; q<nq; q++) qplanes[q]->SetSnrThr(aSnrThr_trigger);
  };
  
  /**
   * @brief Returns the current map fill type.
   * @sa SetMapFill().
   */
  inline string GetMapFill(void){ return mapfill; };

  /**
   * @brief Returns the current SNR threshold for maps.
   * @sa SetSnrThr().
   */
  inline double GetSnrMapThr(void){ return SnrThr_map; };

  /**
   * @brief Returns the current SNR threshold for triggers.
   * @sa SetSnrThr().
   */
  inline double GetSnrTriggerThr(void){ return qplanes[0]->GetSnrThr(); };

  /**
   * @brief Returns the minimum frequency of this tiling [Hz].
   * @details The minimum frequency of the lowest-Q plane is returned.
   */
  inline double GetFrequencyMin(void){ return qplanes[0]->GetFrequencyMin(); };

  /**
   * @brief Returns the highest frequency of this tiling [Hz].
   * @details The maximum frequency of the highest Q plane is returned.
   */
  inline double GetFrequencyMax(void){ return qplanes[nq-1]->GetFrequencyMax(); };

  /**
   * @brief Returns the maximum mismatch between tiles.
   */
  inline double GetMismatchMax(void){ return MaximumMismatch; };

  /**
   * @brief Returns the time range [s].
   */
  inline unsigned int GetTimeRange(void){ return qplanes[0]->GetTimeRange(); };

  /**
   * @brief Sets the chirp mass [solar mass].
   * @details Use a negative mass value to not draw the chirp track.
   * If the merger time is negative, the merger time is taken at the center of the  timing window.
   * @param[in] aMchirp Chirp mass in solar masses.
   * @param[in] aMergerTime Merger GPS time [s].
   */
  inline void SetChirp(const double aMchirp=-1.0, const double aMergerTime=-1.0){
    chirpt = aMergerTime;
    chirpm = aMchirp;
    chirp->SetParameters(-8.0*96.0/3.0/5.0*TMath::Power(TMath::Pi(),8.0/3.0)*TMath::Power(TMath::G()*chirpm*1.989e30/TMath::C()/TMath::C()/TMath::C(), 5.0/3.0),
    aMergerTime);
  }

  /**
   * @brief Returns the chirp mass [solar masses].
   */
  inline double GetChirpMass(void){ return chirpm; };

  /**
   * @brief Fills the Q-plane maps.
   */
  inline void FillMaps(void){
    for(unsigned int q=0; q<nq; q++)
      qplanes[q]->FillMap(mapfill, -(double)GetTimeRange()/2.0, (double)GetTimeRange()/2.0);
  };

  /**
   * @brief Fills the full map.
   * @details The full map combines all the Q-planes. For a given full map bin, the highest SNR ovelapping Q-plane tile is considered.
   * @param[in] aWindowIndex Window index, as defined with SetPlotTimeWindows().
   * @param[in] aTimeOffset Time offset [s].
   * @pre aWindowIndex must be a valid index.
   */
  void FillFullMap(const unsigned int aWindowIndex, const double aTimeOffset = 0.0);

  /**
   * @brief Returns a copy of the full map.
   * @warning Make sure the full map was built with FillFullMap().
   * @param[in] aWindowIndex Window index, as defined with SetPlotTimeWindows().
   * @pre aWindowIndex must be a valid index.
   */
  inline TH2D* GetFullMap(const unsigned int aWindowIndex){
    return (TH2D*)fullmap[aWindowIndex]->Clone();
  };

 private:

  unsigned int fVerbosity;      ///< Verbosity level.
  double MaximumMismatch;       ///< Maximum mismatch.
  Oqplane **qplanes;            ///< Q planes.
  unsigned int nq;              ///< number of q planes.
  double vrange[2];             ///< Map Z-axis range.
  double SnrThr_map;            ///< Map SNR threshold.
  string mapfill;               ///< Map fill type.
  unsigned int **t_snrmax;      ///< Loudest time tile (SNR).
  unsigned int **f_snrmax;      ///< Loudest frequency tile (SNR).
  TF1 *chirp;                   ///< Chirp track.
  double chirpm;                ///< Chirp mass [solar mass].
  double chirpt;                ///< Chirp merger time [s].
  vector <unsigned int> pwin;   ///< Plot time windows.

  // FULL MAP
  TH2D **fullmap;               ///< Full maps.
  unsigned int FullMapNt;       ///< Number of time bins in the full map (0 for full resolution).

  ClassDef(Otile,0)
};

#endif


