/**
 * @file 
 * @brief Ox cross-correlation engine.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxCorr__
#define __OxCorr__
#include <FFT.h>
#include <TH2.h>
#include "Oomicron.h"

using namespace std;

/**
 * @brief Ox cross-correlation engine.
 * @details This class is using the Omicron engine to process two data streams and generate spectrograms. Then, the one-dimensional cross-correlation of the two spectrograms is performed in the Fourier domain along the time direction.
 */
class OxCorr{

public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
  /**
   * @brief Constructor of the OxCorr class.
   * @details FFT plans are created. They cover the duration of the spectrograms excluding half the Omicron overlap at both sides. Time-frequency maps are also created to compute the cross-correlation.
   * @warning The two input Omicron objects must have the same parameter set: see CheckOmicron().
   * @param[in] aName Name to uniquely identify the OxCorr object.
   * @param[in] aO1 Pointer to the first Omicron object.
   * @param[in] aO2 Pointer to the second Omicron object.
   */
  OxCorr(const string aName, Omicron *aO1, Omicron *aO2);

  /**
   * @brief Destructor of the OxCorr class.
   */
  virtual ~OxCorr(void);

  /**
     @}
  */

  /**
   * @brief Runs the two Omicron processes over a given chunk.
   * @details The two Omicron analyses are performed:
   * - Omicron::DefineNewChunk()
   * - Omicron::NewChannel()
   * - Omicron::LoadData()
   * - Omicron::Project()
   *
   * @returns true if all the processing steps were successful.
   * @param[in] aTimeStart Chunk GPS time start [s].
   * @param[in] aResetPsd Flag to reset the PSD estimation.
   */
  bool ProcessOmicron(const unsigned int aTimeStart, const bool aResetPsd);

  /**
   * @brief Runs the cross-correlation analysis over the current Omicron chunk.
   * @details After the Omicron analysis is finished (ProcessOmicron()), this function runs the cross-correlation analysis over the current chunk. The cross-correlation is applied on power-lawed amplitude spectrograms: \f$A^{2\alpha}(Q, \phi, t)\f$, where \f$\alpha\f$ is the power-law index.
   *
   * Each Q plane, is Fourier-transformed one frequency row at a time. The cross-correlation is performed in the Fourier domain. After Fourier-transforming back the data, the cross-correlation map is filled.
   * @returns true if all the analysis was successful.
   * @param[in] aPowerLawIndex Power-law index.
   */
  bool ProcessCrossCorrelation(const unsigned int aPowerLawIndex);

  /**
   * @brief Writes maps in a ROOT file.
   * @details After the cross-correlation analysis is finished (ProcessCrossCorrelation()), this function writes the Omicron maps as well as the cross-correlation map in a ROOT file.
   * @returns true if all the maps were saved.
   * @pre Before calling this function, a valid ROOT file must be opened and set to the right directory.
   * @param[in] aId ID used to name maps.
   */
  bool WriteMaps(const unsigned int aId);

  /**
   * @brief Returns the class status.
   */
  inline bool GetStatus(void){ return status; };

  /**
   * @brief Returns the number of time bins for the cross-correlation analysis.
   */
  inline unsigned int GetTimeBinsN(void){ return fft1->GetSize_t(); };

  /**
   * @brief Returns the duration of one time bin in the cross-correlation map [s].
   */
  inline double GetTimeBinDuration(void){ return dx[0]->GetXaxis()->GetBinWidth(1); };

  /**
   * @brief Returns the cross-correlation analysis duration [s].
   */
  inline unsigned int GetAnalysisDuration(void){ return (unsigned int)(d1[0]->GetXaxis()->GetBinUpEdge(d1[0]->GetNbinsX())-d1[0]->GetXaxis()->GetBinLowEdge(1)); };

  /**
   * @brief Returns the cross-correlation \f$\delta \tau\f$ of a given time bin [s].
   * @note The cross-correlation histogram X axis is binned from \f$-(\tau_{max}-\tau_{min})/2\f$ to \f$+(\tau_{max}-\tau_{min})/2\f$. As a result, the requested \f$\delta \tau\f$ is given by the bin low edge.
   * @param[in] aTimeBinIndex Time bin index (starts at 0).
   */
  inline double GetDeltaTau(const unsigned int aTimeBinIndex){ return dx[0]->GetXaxis()->GetBinLowEdge(aTimeBinIndex+1); };

  /**
   * @brief Returns the time bin index for a given \f$\delta \tau\f$.
   * @note The time bin index starts at 0.
   * @param[in] aDeltaTau \f$\delta \tau\f$ [s]
   */
  inline unsigned int GetTimeBinIndex(const double aDeltaTau){
    return (unsigned int)((aDeltaTau - dx[0]->GetXaxis()->GetBinLowEdge(1))/(double)GetAnalysisDuration()*(double)(dx[0]->GetNbinsX()));
  };

  /**
   * @brief Returns the pointer to the Omicron object for the first detector.
   */  
  inline Omicron* GetOmicron1(void){ return O1; };

  /**
   * @brief Returns the pointer to the Omicron object for the second detector.
   */  
  inline Omicron* GetOmicron2(void){ return O2; };

protected:
  
  bool status;           ///< Class status.
  string name;           ///< Name identifier.
  Omicron *O1;           ///< First Omicron object - do not delete.
  Omicron *O2;           ///< Second Omicron object - do not delete.
  TH2D **dx;             ///< Cross-correlation time-frequency maps.

private:

  fft *fft1;             ///< X FFT plan for detector 1.
  fft *fft2;             ///< X FFT plan for detector 2.
  TH2D **d1;             ///< Time-frequency maps for detector 1.
  TH2D **d2;             ///< Time-frequency maps for detector 2.

  /**
   * @brief Checks the consistency between the 2 Omicron objects.
   * @details Conditions to meet:
   * - The Omicron objects must be valid
   * - There must be exactly 1 channel per Omicron object.
   * - The analysis timing must be the same.
   * - The tiling structure must be the same.
   * - The sampling frequency must be the same.
   *
   * @returns true if the omicron objects are consistent.
   */
  bool CheckOmicron(void);

  ClassDef(OxCorr,0)  
};


#endif


