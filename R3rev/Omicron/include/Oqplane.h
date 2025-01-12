/**
 * @file 
 * @brief Omicron Q-plane.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Oqplane__
#define __Oqplane__

#include <TriggerBuffer.h>
#include <Spectrum.h>
#include "Omap.h"

using namespace std;

/**
 * @brief Create a time-frequency Q-plane.
 * @details This class is designed to create and use a time-frequency Q-plane defined by a Q value.
 * The time-frequency map is described using the Omap class.
 * In addition, this class creates a bisquare window for each frequency row which is used to performed the Q-transform.
 * The Q-transform of whitened data is performed with ProjectData().
 * The result of the Q-transform can be used to fill the time-frequency map (FillMap()) and/or to fill a trigger structure (SaveTriggers()).
 */
class Oqplane: public Omap {

public:
  
  /**
   * @name Constructors and destructors
   @{
  */

  /**
   * @brief Constructor of the Oqplane class.
   * @details An empty time-frequency map is created with Omap::Omap().
   * In addition, for each frequency band, a fast fourier transform is initialized,
   * as well as bisquare windows.
   * @param[in] aQ Q factor \f$Q\f$.
   * @param[in] aSampleFrequency Sampling frequency [Hz] \f$f_s\f$.
   * @param[in] aFrequencyMin Minimum frequency [Hz].
   * @param[in] aFrequencyMax Maximum frequency [Hz].
   * @param[in] aTimeRange Time range \f$T\f$ [s]. The map is centered on 0.
   * @param[in] aMaximumMismatch Maximum mismatch between 2 consecutive tiles.
   */
  Oqplane(const double aQ, const unsigned int aSampleFrequency, 
          const double aFrequencyMin, const double aFrequencyMax, 
	  const unsigned int aTimeRange, const double aMaximumMismatch);

  /**
   * @brief Destructor of the Oqplane class.
   */
  virtual ~Oqplane(void);

  /**
     @}
  */

  /**
   * @brief Projects a whitened data vector onto the Q-plane.
   * @details The input whitened data vector is shifted in frequency to match the frequency bands.
   * Then it is multiplied by the band bisquare window.
   * Finally the inverse Fourier transform is performed for each frequency row to obtain SNR values for each tile.
   *
   * The number of tiles with a SNR above the threshold set with SetSnrThr() is counted.
   * Tiles on both edges of the time range can be excluded using a padding parameter.
   * @returns The number of tiles above the SNR threshold.
   * @note This function does not fill the time-frequency map.
   * To fill the map, call FillMap().
   * @param[in] aDataFft Whitened data vector in the fourier domain.
   * @param[in] aPadding Number of seconds excluded on both sides of the time range when counting tiles above the SNR threshold.
   * @pre The padding value is not checked! Make sure it is compatible with the map time range.
   */
  long unsigned int ProjectData(fft *aDataFft, const double aPadding=0.0);

  /**
   * @brief Fills the Q-plane map.
   * @details The Q-plane tiles are filled with:
   * - the SNR if "snr" is specified as a content type.
   * - the amplitude if "amplitude" is specified as a content type.
   * - the phase if "phase" is specified as a content type.
   * - a demo tiling for any other type.
   *
   * @param[in] aContentType Content type.
   * @param[in] aTimeStart Time after which to fill the map [s].
   * @param[in] aTimeEnd Time before which to fill the map [s].
   */
  void FillMap(const string aContentType, const double aTimeStart, const double aTimeEnd);

  /**
   * @brief Adds tile segments to a Segments structure.
   * @details The tiling structure is scanned.
   * The tile start and end times are added as a segment to the input segment structure if the tile SNR is above a given threshold.
   *
   * The SNR threshold is given as a TH1D histogram binned in tile frequency.
   * The bin content is the SNR threshold.
   * @note For bins with a negative content, the SNR threshold is considered to be infinite.
   *
   * @note Tiles with a frequency outside the histogram frequency range are excluded.
   *
   * @param[in,out] aSegments Segments structure in which tile segments will be added. This object must be created before calling this function.
   * @param[in] aSnrThreshold SNR threshold histogram.
   * @param[in] aT0 Reference time to center the time-frequency map [s].
   * @param[in] aPadding Number of seconds excluded on both sides of the tiling structure when selecting tiles above the SNR threshold.
   */
  void AddTileSegments(Segments *aSegments, TH1D *aSnrThreshold,
                       const double aT0, const double aPadding);

  /**
   * @brief Fills a trigger structure with tiles above the SNR threshold.
   * @details By construction, the time-frequency map is centered on 0.
   * This central time can be shifted to a physical time with 'aT0'.
   * Moreover a time selection is applied using a segment list.
   * Only tiles with a central time (after the time shift) inside these segments are saved.
   * @param[in] aTriggers Trigger structure (must be valid!)
   * @param[in] aT0 Reference time to center the time-frequency map [s].
   * @param[in] aSeg Segment list to select the tiles to save.
   */
  bool SaveTriggers(TriggerBuffer *aTriggers, const double aT0, Segments* aSeg);

  /**
   * @brief Gets the tile SNR threshold.
   */
  inline double GetSnrThr(void){ return SnrThr; };

  /**
   * @brief Sets the tile SNR threshold.
   * @param[in] aSnrThr SNR threshold.
   */
  inline void SetSnrThr(const double aSnrThr=2.0){ SnrThr=aSnrThr; };

  /**
   * @brief Returns the SNR squared estimated in a tile.
   * @sa ProjectData().
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileSnrSq(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return TMath::Max(bandFFT[aBandIndex]->GetNorm2_t(aTimeTileIndex)-2.0,0.0);
  };

  /**
   * @brief Returns the maximum SNR squared estimated in the entire Q plane.
   */
  inline double GetSnrSqMax(void){ return SnrSqMax; };

  /**
   * @brief Returns the amplitude squared estimated in a tile.
   * @warning This function returns meaningful data only if the data noise power spectrum was previously set with SetPower().
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileAmplitudeSq(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return GetTileSnrSq(aTimeTileIndex, aBandIndex)*bandNoiseAmplitude[aBandIndex]*bandNoiseAmplitude[aBandIndex];
  };

  /**
   * @brief Returns the amplitude estimated in a tile.
   * @warning This function returns meaningful data only if the data noise power spectrum was previously set with SetPower().
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileAmplitude(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return TMath::Sqrt(GetTileSnrSq(aTimeTileIndex, aBandIndex))*bandNoiseAmplitude[aBandIndex];
  };

  /**
   * @brief Computes the noise power associated to each frequency band.
   * @details For each frequency band, the Spectrum object is integrated over the frequency range.
   * The power is weighted by the bisquare window.
   * @param[in] aSpec1 First noise power spectrum used to whiten the data.
   * @param[in] aSpec2 Second noise power spectrum used to whiten the data.
   */
  void SetPower(Spectrum *aSpec1, Spectrum *aSpec2);
  
private:
  
  // TRIGGER SELECTION
  double SnrThr;                    ///< SNR threshold to save tiles.
  double SnrSqMax;                  ///< Maximum SNR squared in the Q plane.

  // FREQUENCY BANDS
  unsigned int *bandWindowSize;     ///< Band bisquare window size.
  double **bandWindow_r;            ///< Band bisquare windows (real).
  double **bandWindow_i;            ///< Band bisquare windows (imaginary).
  double *bandNoiseAmplitude;       ///< Band noise amplitude.
  fft **bandFFT;                    ///< Band ffts.
    
  ClassDef(Oqplane,0)  
};


#endif


