/**
 * @file 
 * @brief Ox post-processing analysis.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxPost__
#define __OxPost__

#include "OxEvent.h"
#include "OxInj.h"

using namespace std;

/**
 * @brief Post-process the result of the Omicron cross-correlation analysis (OxProcess)
 * @details The background and zero-lag events are separated out. Plots are generated. A web report is produced. See Run().
 */
class OxPost: public GwollumOptions{

 public:

  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the OxPost class.
   * @details The list of options is defined. Options are identified by a TAG and a KEYWORD:
   * @verbatim
   TAG  KEYWORD  [PARAMETERS]
   @endverbatim
   *
   * @section oxpost_options List of options
   * @subsection oxpost_proc Analysis result files
   * @verbatim
   POST  PROC  [PARAMETERS]
   @endverbatim
   * `[PARAMETERS]` is a list of directory paths (relative or absolute) where analysis root files can be found. These root files must be produced with OxProcess::Run(). In particular, each directory must be structured in standard way, i.e two sub-directories must be found: `x_even/` and `x_odd/`.
   *
   * @subsection oxpost_post_directory Output directory
   * @verbatim
   POST  DIRECTORY  [PARAMETER]
   @endverbatim
   * `[PARAMETER]` is the path to the output directory where results (web page and plots) are posted.
   *
   * @subsection oxpost_post_vetosnr SNR veto
   * @verbatim
   POST  VETOSNR  [PARAMETERS]
   @endverbatim
   * It is possible to veto an event if the Omicron maximum SNR in the time-frequency maps is larger than a given threshold. Use this option to provide the SNR thresholds for the first and second detector. Use a negative value to de-activate the veto (default).
   *
   * @subsection oxpost_post_vetoxispread &Xi; spread veto
   * @verbatim
   POST  VETOXISPREAD  [PARAMETER]
   @endverbatim
   * It is possible to veto an event if the spread of &Xi;(Q) values exceeds the value provided with this option. Use a negative value to de-activate the veto (default).
   *
   * @subsection oxpost_post_vetofreqrej Frequency rejection veto
   * @verbatim
   POST  VETOFREQREJ  [PARAMETERS]
   @endverbatim
   * It is possible to veto an event if the number of frequency rows in a given Q plane rejected by the slice veto exceeds a given threshold. Use this option to set the thresholds for each Q plane. Each threshold value must be given as a fraction of frequency rows in percents. Use a negative value to de-activate the veto (default).
   *
   * @subsection oxpost_post_detectionthr Detection threshold
   * @verbatim
   POST  DETECTIONTHR  [PARAMETERS]
   @endverbatim
   * `[PARAMETERS]` defines the detection threshold. The interpretation of this option depends on the number of parameters:
   * - 2 parameters (or more): this is interpreted as two detection thresholds applied on the event rank for the EVEN (first parameter) and ODD (second parameter) analysis.
   * - 1 parameter: this is interpreted as a unique detection threshold on the false-alarm rate [Hz].
   * - 0 parameters: this is interpreted as setting a detection threshold at the level of the highest-ranked background event.
   *
   * @subsection oxpost_post_zerolag Flag to post the zero-lag results
   * @verbatim
   POST  ZEROLAG  [PARAMETERS]
   @endverbatim
   * Two integer numbers are expected:
   * - Set the first parameter to 0 to keep the zero lag hidden (default). Use a non-zero value to display the zero-lag results.
   * - Use the second parameter to select a slice (with its index) to simulate the zero lag. Use 0 to randomly select a slice (default) .
   *
   * @subsection oxpost_post_nloudestprint List of loudest events to print in the web report
   * @verbatim
   POST  NLOUDESTPRINT  [PARAMETERS]
   @endverbatim
   * This option specifies the number of loudest events to print in the final web report. Three numbers are expected:
   * - Number of zero-lag events
   * - Number of background events.
   * - Number of fake zero-lag events
   *
   * @subsection oxpost_post_nloudestfollow List of loudest events to follow up
   * @verbatim
   POST  NLOUDESTFOLLOW  [PARAMETERS]
   @endverbatim
   * This option specifies the number of loudest events to follow up by running an Ox analysis. Three numbers are expected:
   * - Number of zero-lag events
   * - Number of background events.
   * - Number of fake zero-lag events
   *
   * @note These numbers cannot be larger than the numbers specified with the @ref oxpost_post_nloudestprint "POST/NLOUDESTPRINT" option.
   *
   * @subsection oxpost_post_ffl1 First detector FFL
   * @verbatim
   POST  FFL1  [PARAMETER]
   @endverbatim
   * This option is useful to rerun an Omicron analysis over some events. Provide a path to the FFL file with the data of the first detector. If not provided, the FFL used in the analysis files is considered.
   *
   * @subsection oxpost_post_ffl2 Second detector FFL
   * @verbatim
   POST  FFL2  [PARAMETER]
   @endverbatim
   * This option is useful to rerun an Omicron analysis over some events. Provide a path to the FFL file with the data of the second detector. If not provided, the FFL used in the analysis files is considered.
   *
   * @subsection oxpost_post_injgen Coherent injections
   * @verbatim
   POST  INJGEN  [PARAMETER]
   @endverbatim
   * Provide a list of file patterns pointing to injection files. If not provided, the options contained in the analysis files are considered.
   *
   * @subsection oxpost_post_injbinsn Number of bins for injection parameter plots
   * @verbatim
   POST  INJBINSN  [PARAMETER]
   @endverbatim
   * If injections are performed, use this option to specify the number of bins to draw injection parameter histograms. By default = 20.
   *
   */
  OxPost(void);

  /**
   * @brief Destructor of the OxPost class.
   */
  virtual ~OxPost(void);
  /**
     @}
  */

  /**
   * @brief Post-processes the cross-correlation analysis results obtained with Process().
   * @details Input files are first loaded and options are analyzd with ReadDataFiles(). Two Omicron objects are intialized. 
   *
   * Each analysis round (even and odd) is post-processed:
   * - Events are collected with OxEvent::ProcessEvents().
   * - The loudest events are processed with OxEvent::ProcessLoudest().
   * - Results are processed with OxEvent::ProcessResults().
   *
   * If injections were performed, they are recovered with OxInj::Recover().
   * Finally analysis results are dumped in a web report with MakeHtml().
   */
  bool Run(void);
  
private:

  /**
   * @brief Reads a list of input ROOT files and extract options.
   * @details The list of root files in the ouput directories, `x_even/` and `x_odd/`, are scanned. The Omicron options as well as the Ox options are extracted from the list of analysis files. It is checked that the options are uniform across the dataset.
   *
   * Some of the Omicron options are modified for the post-processing. Option files are saved in the output directory.
   *
   * @returns true if all the extracted options are valid.
   * @param[out] aEvenFiles List of file patterns pointing to the EVEN analysis ROOT files.
   * @param[out] aOddFiles List of file patterns pointing to the ODD analysis ROOT files.
   * @param[in] aOutputDirectory Path to the output directory.
   */
  bool ReadDataFiles(string &aEvenFiles, string &aOddFiles, const string aOutputDirectory);

  /**
   * @brief Generates a HTML report with the Ox search results.
   * @details The two analysis rounds are displayed (detection plots, trigger distributions, list ofloudest events...).
   * If injections were performed, sensitivity results are displayed.
   * @param[in] aEvenAnalysis Pointer to the results of the Even analysis.
   * @param[in] aOddAnalysis Pointer to the results of the Odd analysis.
   * @param[in] aInjections Pointer to the injection object.
   */
  void MakeHtml(OxEvent *aEvenAnalysis, OxEvent *aOddAnalysis, OxInj *aInjections);

  
  ClassDef(OxPost,0)
};

#endif


