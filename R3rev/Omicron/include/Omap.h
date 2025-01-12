/**
 * @file 
 * @brief Omicron multi-resolution time-frequency map.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Omap__
#define __Omap__
#include "Oconfig.h"
#include <TRandom.h>
#include <TMath.h>
#include <TFile.h>
#include <TH2D.h>

using namespace std;

/**
 * @brief Applies a time offset to a map.
 * @param[in] aMap Map.
 * @param[in] aOffset Time offset [s].
 */
void MapApplyOffset(TH2D *aMap, const double aOffset);

/**
 * @brief Create a time-frequency map for the Q-transform.
 * @details This class is designed to create and use a multi-resolution time-frequency map.
 * The time-frequency map is represented by a 2-dimensional histogram with a TH2D object.
 * The vertical axis is binned logarithmically in frequency [Hz].
 * Each frequency band is simply called a band.
 * Each band is binned linearly in time (horizontal axis), and the number of bins in a band is a power of two.
 */
class Omap{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
  /**
   * @brief Constructor of the Omap class.
   * @details An empty time-frequency map is created.
   * @warning The parameters must verify some conditions to perform a Q-transform, or else they will be automatically adjusted:
   * - The time range \f$T\f$ must be an even number of seconds.
   * - The time range \f$T\f$ must be at least 4 seconds.
   * - The minimum frequency must be larger than \f$\frac{4Q}{2\pi \times T}\f$.
   * - The maximum frequency must be smaller than \f$\frac{f_s/2}{1+\sqrt{11}/Q}\f$.
   * 
   * @param[in] aQ Q factor \f$Q\f$.
   * @param[in] aSampleFrequency Sampling frequency [Hz] \f$f_s\f$.
   * @param[in] aFrequencyMin Minimum frequency [Hz].
   * @param[in] aFrequencyMax Maximum frequency [Hz].
   * @param[in] aTimeRange Time range \f$T\f$ [s]. The map is centered on 0.
   * @param[in] aMaximumMismatch Maximum mismatch between 2 consecutive tiles.
   */
  Omap(const double aQ, const unsigned int aSampleFrequency,
       const double aFrequencyMin, const double aFrequencyMax,
       const unsigned int aTimeRange, const double aMaximumMismatch);

  /**
   * @brief Destructor of the Omap class.
   */
  virtual ~Omap(void);

  /**
     @}
  */


  /**
   * @brief Prints the map parameters.
   */
  void PrintParameters(void);

  /**
   * @brief Returns the Q factor.
   */
  inline double GetQ(void){ return q; };

  /**
   * @brief Returns the map time minimum [s].
   */
  inline double GetTimeMin(void){ 
    return tfmap->GetXaxis()->GetBinLowEdge(1);
  };
  
  /**
   * @brief Returns the map time maximum [s].
   */
  inline double GetTimeMax(void){ 
    return tfmap->GetXaxis()->GetBinUpEdge(tfmap->GetNbinsX());
  };
  
  /**
   * @brief Returns the map time range [s].
   */
  inline unsigned int GetTimeRange(void){
    return (unsigned int)(GetTimeMax()-GetTimeMin());
  };
  
  /**
   * @brief Returns the map frequency minimum [Hz].
   */
  inline double GetFrequencyMin(void){ 
    return tfmap->GetYaxis()->GetBinLowEdge(1); 
  };

  /**
   * @brief Returns the map frequency maximum [Hz].
   */
  inline double GetFrequencyMax(void){ 
    return tfmap->GetYaxis()->GetBinUpEdge(tfmap->GetNbinsY()); 
  };

  /**
   * @brief Returns the number of frequency bands in the map.
   */
  inline unsigned int GetBandN(void){ 
    return tfmap->GetNbinsY();
  };

  /**
   * @brief Returns the number of tiles in the map.
   */
  inline long unsigned int GetTileN(void){ 
    return Ntiles;
  };

  /**
   * @brief Returns the number of tiles in the map.
   * @param[in] aPadding Number of seconds excluded on both sides of the time range.
   * @pre The padding value is not checked! Make sure it is compatible with the map time range.
   */
  long unsigned int GetTileN(const double aPadding);

  /**
   * @brief Returns the band index for a given frequency.
   * @param[in] aFrequency Frequency [Hz].
   */
  inline int GetBandIndex(const double aFrequency){
    return tfmap->GetYaxis()->FindBin(aFrequency)-1;
  };
  
  /**
   * @brief Returns the band central frequency [Hz].
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandFrequency(const unsigned int aBandIndex){
    return tfmap->GetYaxis()->GetBinCenterLog(aBandIndex+1);
  };

  /**
   * @brief Returns the band frequency start [Hz].
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandStart(const unsigned int aBandIndex){ 
    return tfmap->GetYaxis()->GetBinLowEdge(aBandIndex+1);
  };

  /**
   * @brief Returns the band frequency end [Hz].
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandEnd(const unsigned int aBandIndex){ 
    return tfmap->GetYaxis()->GetBinUpEdge(aBandIndex+1);
  };
  
  /**
   * @brief Returns the band width [Hz].
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandWidth(const unsigned int aBandIndex){ 
    return tfmap->GetYaxis()->GetBinWidth(aBandIndex+1);
  };

  /**
   * @brief Returns the list of frequency bands.
   * @details The returned array is of size GetBandN()+1 and contains the bin limits. 
   * @warning The returned array must be deleted by the user.
   */
  double* GetBands(void);

  /**
   * @brief Returns the tile duration [s].
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileDuration(const unsigned int aBandIndex){
    return tfmap->GetXaxis()->GetBinWidth(1)*bandMultiple[aBandIndex];
  };

  /**
   * @brief Returns the number of tiles in a band.
   * @param[in] aBandIndex Band index.
   */
  inline unsigned int GetBandTileN(const unsigned int aBandIndex){ 
    return (unsigned int)tfmap->GetNbinsX()/bandMultiple[aBandIndex];
  };

  /**
   * @brief Returns the start time of a tile [s].
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileTimeStart(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return tfmap->GetXaxis()->GetBinLowEdge(aTimeTileIndex*bandMultiple[aBandIndex]+1);
  };

  /**
   * @brief Returns the end time of a tile [s].
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileTimeEnd(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return tfmap->GetXaxis()->GetBinUpEdge((aTimeTileIndex+1)*bandMultiple[aBandIndex]);
  };

  /**
   * @brief Returns the central time of a tile [s].
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileTime(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return GetTileTimeStart(aTimeTileIndex,aBandIndex)/2.0 + GetTileTimeEnd(aTimeTileIndex,aBandIndex)/ 2.0;
  };

  /**
   * @brief Returns the index of a tile in a band.
   * @warning The returned index might be out-of range.
   * @param[in] aBandIndex Band index.
   * @param[in] aTime Time [s].
   */
  inline int GetTimeTileIndex(const unsigned int aBandIndex, const double aTime){
    return (int)floor((aTime-GetTimeMin())/GetTileDuration(aBandIndex));
  };

  /**
   * @brief Returns the content of a tile.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   */
  inline double GetTileContent(const unsigned int aTimeTileIndex, const unsigned int aBandIndex){
    return tfmap->GetBinContent(aTimeTileIndex*bandMultiple[aBandIndex]+1,aBandIndex+1);
  };

  /**
   * @brief Sets the content of a tile.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @param[in] aBandIndex Band index.
   * @param[in] aContent Content.
   */
  inline void SetTileContent(const unsigned int aTimeTileIndex, const unsigned int aBandIndex, const double aContent){
    int tstart=aTimeTileIndex*bandMultiple[aBandIndex]+1;
    int tend=tstart+bandMultiple[aBandIndex];
    for(int t=tstart; t<tend; t++) tfmap->SetBinContent(t,aBandIndex+1,aContent);
  };

   /**
   * @brief Writes the time-frequency map to a ROOT file.
   * @returns The function returns the total number of bytes written to the file. It returns 0 if the object cannot be written.
   * @param[in] aFile Pointer to an open TFile.
   * @param[in] aName Map name.
   */
  int Write(TFile *aFile, const string aName){
    aFile->cd();
    return tfmap->Write(aName.c_str());
  }
 
protected:
  
  TH2D *tfmap;               ///< Time-frequency map.

private:

  double q;                  ///< Q factor.      
  long unsigned int Ntiles;  ///< Number of tiles in the tf map.
  unsigned int *bandMultiple;///< Band multiple (time resolution).

  friend class Otile;
  
  ClassDef(Omap,0)  
};


#endif


