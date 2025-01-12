/**
 * @file 
 * @brief Omicron cross-correlation analysis.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxProcess__
#define __OxProcess__

#include "OxEvent.h"

using namespace std;

/**
 * @brief Perform an Omicron cross-correlation analysis.
 * @details This class is designed to run a cross-correlation analysis between time-frequency maps produced with Omicron.
 *
 * After the OxProcess object is constructed, run the omicron cross-correlation analysis with Run().
 */
class OxProcess: public GwollumOptions{

 public:

  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the OxProcess class.
   * @details The list of options is defined. Options are identified by a TAG and a KEYWORD:
   * @verbatim
   TAG  KEYWORD  [PARAMETERS]
   @endverbatim
   *
   * @section oxprocess_options List of options
   * @subsection oxprocess_directory Analysis directory 
   * @verbatim
   PROC  DIRECTORY  [PARAMETER]
   @endverbatim
   * `[PARAMETER]` is the directory path (relative or absolute) to the analysis directory where result files will be saved.
   *
   * @subsection oxprocess_nperfile Number of analysis chunks per output file.
   * @verbatim
   PROC  NPERFILE  [PARAMETER]
   @endverbatim
   * `[PARAMETER]` is the number of analysis chunks (Omicron analysis window) saved in a file.
   *
   * @subsection oxprocess_spectropower Amplitude power-law for spectrograms
   * @verbatim
   PROC  SPECTROPOWER  [PARAMETER]
   @endverbatim
   * `[PARAMETER]` is the index for the amplitude power law used to build spectrograms, e.g. 2 for squared amplitudes. Note that only even numbers are accepted. Default value = 2.
   *
   * @subsection oxprocess_savemaps Save maps flags 
   * @verbatim
   PROC  SAVEMAPS  [PARAMETER]
   @endverbatim
   * With this option, it is possible to save time-frequency maps while processing the data. Set `[PARAMETER]` to a non zero value to save time-frequency maps.
   *
   * @subsection oxprocess_slicedelta Slice duration
   * @verbatim
   PROC  SLICEDELTA  [PARAMETERS]
   @endverbatim
   * Two parameters are expected to define the duration of a slice. At minimum, the slice duration is given by the maximum light travel time between the two detectors: \f$\pm \delta_\mathrm{det}\f$. The first parameter adds an extra contribution to account for a timing calibration uncertainty: \f$\pm \delta_\mathrm{cal}\f$. The second parameter is an another contribution which is used to veto events: \f$\pm \delta_\mathrm{veto}\f$. If an event is located in that slice with \f$|\delta\tau > \delta_\mathrm{det} + \delta_\mathrm{cal}\f$, it is rejected. Both parameters must be given in [s]. By default they are set to 0 s.
   *
   */
  OxProcess(void);

  /**
   * @brief Destructor of the OxProcess class.
   */
  virtual ~OxProcess(void);
  /**
     @}
  */

  /**
   * @brief Runs the cross-correlation analysis over a list of time segments.
   * @details Two Omicron objects are initialized using the input parameter files.
   * @warning The two input parameter files must have the same parameter set: see OxCorr::CheckOmicron().
   *
   * The list of input segments is processed sequentially. Each segment is processed by Omicron using the parameters defined in the Omicron option files.
   *
   * The data is analyzed twice. The second time, the Omicron analysis window is shifted by 50%. The results from the first (resp. second) analysis sequence is saved in a sub-directory named `x_even/` (resp. `x_odd/`).
   *
   * The Omicron and cross-correlation analyses of time-frequency maps is performed with the OxCorr class: OxCorr::ProcessOmicron() and OxCorr::ProcessCrossCorrelation().
   *
   * At any time, if one analysis step fails, the current segment is interrupted and the next segment in the list is considered.
   *
   * The analysis results are saved in ROOT files.
   *
   * @param[in] aInSeg List of time segments to process.
   * @param[in] aOptionFile1 Path to the option file for the first Omicron oject.
   * @param[in] aOptionFile2 Path to the option file for the second Omicron oject.
   */
  bool Run(Segments *aInSeg, const string aOptionFile1, const string aOptionFile2);

private:

  TFile *outfile;             ///< Output file.

  /**
   * @brief Opens and prepares a new output file.
   * @details The following elements are saved in the file:
   * - The GwollumOptions TTree for the Ox analysis.
   * - The `maps/` sub-directory.
   * - The first Omicron GwollumOptions TTree (in `omicron1/`).
   * - The second Omicron GwollumOptions TTree (in `omicron2/`).
   * @param[in] aAnalysis Analysis type: 0=even, 1=odd.
   * @param[in] aTimeStart GPS start time [s].
   * @param[in] aO1 First Omicron object.
   * @param[in] aO2 Second Omicron object.
   */
  bool NewOutputFile(const unsigned int aAnalysis, const unsigned aTimeStart,
                     Omicron *aO1, Omicron *aO2);

  ClassDef(OxProcess,0)
};

#endif


