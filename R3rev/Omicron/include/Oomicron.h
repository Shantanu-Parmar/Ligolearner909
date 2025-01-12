/**
 * @file 
 * @brief Omicron processing.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Oomicron__
#define __Oomicron__

#include <IO.h>
#include "Otile.h"
#include "Oinject.h"
#include <Date.h>
#include <InjEct.h>
#include <ffl.h>

using namespace std;

/**
 * @brief Process data with the Omicron algorithm.
 * @details This class is designed to offer various methods to conduct an Omicron analysis.
 * The Omicron object must be initialized with an option file: see ReadOptions().
 *
 * After construction, the Omicron methods should be called sequentially to perform the analysis. Here is a typical sequence:
 * - InitSegments() defines the data segments to process.
 * - MakeDirectories() creates a specific directory structure for the output (optional).
 * - NewChannel() loads a new channel (loop #1).
 * - NewChunk() loads a new chunk of data (loop #2).
 * - LoadData() loads a data vector for this chunk and this channel from a FFL file (in loop #1/2)
 * - Condition() conditions the data vector (in loop #1/2)
 * - Project() projects data onto the tiling structure (in loop #1/2)
 * - WriteOutput() writes output data products to disk (in loop #1/2)
 * - ResetSequence() to go back to the first chunk (in loop #1)
 *
 * @author Florent Robinet
 */
class Omicron: public GwollumOptions {

 public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the Omicron class.
   * @details This constructor initializes all the components to run Omicron: data structures, data streams, spectra, tiling, maps, triggers, injections, monitoring, etc.
   *
   * An option file is required to define all the parameters to run Omicron. For more details about the Omicron configuration, see ReadOptions().
   *
   * When the one-channel optimization flag is active, channels must be processed completely one by one with NewChannel(). Indeed, only one channel spectrum container is created and is used for one channel after the other. With this mode, it is impossible to process all channels one chunk at a time. @sa NewChannel().
   *
   * @param[in] aOptionFile Path to the option file.
   * @param[in] aOneChannel One-channel optimization flag.
   * @param[in] aGpsRef Reference time to initiate structures.
   * @param[in] aStrict Strict mode: when set to true, the status of the Omicron object is set to false if options are incorrectly provided.
   */
  Omicron(const string aOptionFile, const bool aOneChannel, const unsigned int aGpsRef=0, const bool aStrict=false);
  
  /**
   * Destructor of the Omicron class.
   */
  virtual ~Omicron(void);
  /**
     @}
  */
  
  /**
   * @brief Resets the time sequence of chunks.
   * @details The sequence is initialized to the start at the first chunk. Note that the segments set with InitSegments() remain the same.
   * @sa Otile::ResetSequence().
   */
  inline void ResetSequence(void){ tile->ResetSequence(); };

  /**
   * @brief Initializes the segments.
   * @details This function should always be called before processing data.
   * The input segment structure is used:
   * - to initialize the analysis sequence (see Otile::SetSegments())
   * - to update the channel list in the FFL.
   *
   * Optionally, output segments (for triggers only!) can be specified.
   * If so, triggers outside the output segments are not be saved.
   * Use a pointer to NULL to not use this option.
   * @param[in] aInSeg Pointer to the input Segments structure.
   * @param[in] aOutSeg Pointer to the output Segments structure.
   */
  bool InitSegments(Segments *aInSeg, Segments *aOutSeg=NULL);

  /**
   * @brief Creates a specific directory tree for the output.
   * @details Two directory structures are possible:
   * - `[path_to_outdir]/aId/[channel_name]` if `aId` is not 0
   * - `[path_to_outdir]/[channel_name]` if `aId` is 0
   *
   * where `[path_to_outdir]` is the output directory specified by the user in the option file and `[channel_name]` is the channel name being processed.
   * 
   * The `aId` value is rounded to the third digit.
   *
   * If this function is never called, all the output is dumped in the output directory specified by the user in the option file.
   * @param[in] aId Directory id.
  */
  bool MakeDirectories(const double aId = 0.0);

  /**
   * @brief Calls a new time chunk.
   * @details The time chunks are called following the time sequence defined by the Otile class.
   * @note If a new segment is started, the PSD buffer is reset for all channels. 
   * @note If the sine-Gauss injections are activated, waveforms are generated.
   * @returns The returned value indicates the status of this operation:
   * - true: a new time chunk has been successfully called
   * - false: no more chunk to load
   */
  bool NewChunk(void);

  /**
   * @brief Defines a new time chunk.
   * @details Instead of defining a list of input segments (see InitSegments()) and processing sequentially the data (see NewChunk()), it is possible to define any new time chunk.
   *
   * Optionally, it is possible to reset the PSD buffer (for all channels).
   * @note The chunk duration must match the one defined in the option file.
   * @param[in] aTimeStart GPS start time of the chunk.
   * @param[in] aTimeEnd GPS end time of the chunk.
   * @param[in] aResetPsdBuffer Flag to reset the PSD buffers.
   * @returns The returned value indicates the status of this operation:
   * - true: the time chunk is successfully called
   * - false: otherwise
   */
  bool DefineNewChunk(const unsigned int aTimeStart, const unsigned int aTimeEnd,
                      const bool aResetPsdBuffer=false);

  /**
   * @brief Calls a new channel.
   * @details The channels defined in the option file are called sequentially.
   * If this function is called after the last channel, false is returned and the channel sequence is reset: at the next call, the first channel will be loaded again.
   * @note If the one-channel optimization is active, the PSD buffer is reset.
   */
  bool NewChannel(void);

  /**
   * @brief Returns the name prefix of a given channel.
   * @details Returns "" if no channel is defined.
   * @param[in] aChannelIndex Channel index.
   */
  inline string GetChannelPrefix(const unsigned int aChannelIndex){
    if(aChannelIndex<GetChannelsN()) return triggers[aChannelIndex]->Streams::GetNamePrefix();
    return "";
  };

  /**
   * @brief Returns the name of a given channel.
   * @details Returns "" if no channel is defined.
   * @param[in] aChannelIndex Channel index.
   */
  inline string GetChannelName(const unsigned int aChannelIndex){
    if(aChannelIndex<GetChannelsN()) return triggers[aChannelIndex]->Streams::GetName();
    return "";
  };

  /**
   * @brief Returns the name of the current channel.
   * @details Returns "" if no channel is defined.
   */
  inline string GetChannelName(void){
    if(chanindex>=0) return triggers[chanindex]->Streams::GetName();
    return "";
  };

  /**
   * @brief Returns the native sampling frequency of the current channel.
   * @details Returns 0 if no channel is defined.
   */
  inline unsigned int GetChannelNativeFrequency(void){
    if(chanindex>=0) return triggers[chanindex]->Streams::GetNativeFrequency();
    return 0;
  };

  /**
   * @brief Loads a data vector.
   * @details A data vector is returned, as well as its size.
   * - The data vector of the current channel and the current chunk is loaded.
   * - If requested in the option file, software injections (InjEct) are added to the data.
   * - If requested in the option file, the injection data stream is loaded and added to the data. 
   *
   * @warning The FFL option is mandatory to use this function.
   * @note It is the user's responsibility to delete the returned data vector.
   *
   * @returns If this function fails, false is returned and the data vector points to NULL.
   * @param[out] aDataVector Pointer to the data vector.
   * @param[out] aSize Size of the data vector.
   */
  bool LoadData(double **aDataVector, unsigned int &aSize);

  /**
   * @brief Conditions a data vector.
   * @details Before projecting the data onto the tiles, the data is conditioned and whitened with this function. The following operations are performed:
   * - Check if the Omicron object is sane (returns -1 otherwise)
   * - Check if the input data vector is sane (returns 1 otherwise)
   * - Check if the input vector is flat (prints a warning and set the output flag to true).
   * - Calculate the native sampling frequency based on the vector size.
   * If the native sampling frequency has changed, it is updated (returns 2 if this update fails).
   * - If requested in the option file, a SineGaus injection is added.
   * - The DC component is removed (returns 3 if it fails).
   * - If requested in the option file, the data is high-pass-filtered (returns 3 if it fails).
   * - The data vector is resampled to the working frequency (returns 3 if it fails).
   * - The data vector is applied a Tukey window.
   * - The data is used to update the PSD (the first one).
   * - The data is Fourier-transformed (returns 4 if it fails).
   * - The data is whitened in the Fourier domain using the first PSD (see Whiten()).
   * - The data Fourier-transformed back in the time domain.
   * - The data is used to update the PSD (the second one).
   * - The data is Fourier-transformed.
   * - The data is whitened in the Fourier domain using the second PSD (see Whiten()).
    * - The power in the tiling structure is computed (Otile::SetPower()).
  *
   * @warning The input vector size MUST MATCH the chunk size loaded with NewChunk(). NO check is performed against that!
   * @returns 0 if the data was correctly conditioned.
   *
   * @param[in] aInVectSize Input vector size.
   * @param[in] aInVect Input data vector (time domain).
   * @param[out] aIsFlat Flag set to true if the input data vector is detected to be flat.
   */
  int Condition(const unsigned int aInVectSize, double *aInVect, bool &aIsFlat);
  
  /**
   * @brief Projects whitened data onto the tiles and fills output structures.
   * @details Otile::ProjectData() is called to fill the tiling structure.
   * @returns The number of tiles above the threshold is returned.
   */
  long unsigned int Project(void);
  
  /**
   * @brief Writes output products to disk.
   * @details The output data products selected by the user in the option file and for the current chunk/channel are written to disk.
   */
  bool WriteOutput(void);

  /**
   * @brief Extracts and saves triggers above threshold.
   * @details Triggers are saved using Otile::SaveTriggers().
   * The trigger structure for each channel is filled with tiles with a SNR above threshold.
   * @warning If the number of triggers is greater than the maximum trigger rate specified in the option file,
   * the current chunk is ignored and this function returns false.
   * @param[out] aTriggerRate Trigger rate [Hz] measured over the chunk excluding half the overlap on both sides.
   */
  bool ExtractTriggers(double &aTriggerRate);

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
   * @sa Otile::GetTileSegments()
   * @note The user is in charge of deleting the returned Segments object.
   */
  inline Segments* GetTileSegments(TH1D *aSnrThreshold, const double aPadding){
    return tile->GetTileSegments(aSnrThreshold, aPadding);
  };

  /**
   * @brief Returns the tiling minimum frequency [Hz].
   */
  inline double GetTileFrequencyMin(void){
    return tile->GetFrequencyMin();
  };
  
  /**
   * @brief Returns the tiling maximum frequency [Hz].
   */
  inline double GetTileFrequencyMax(void){
    return tile->GetFrequencyMax();
  };
  
  /**
   * @brief Flushes triggers.
   * @details All triggers collected until now with ExtractTriggers() are flushed in the final MakeTriggers structure.
   * @note If requested, triggers are clustered.
   * @returns This function returns the number of triggers (or clusters if requested) in the final MakeTriggers structure.
   * It returns -1 if this function fails.
   */
  Long64_t FlushTriggers(void);

  /**
   * @brief Writes triggers to disk.
   * @details All triggers collected until now with FlushTriggers() are saved to disk.
   * @returns This function returns the trigger file path. "" is returned if this function fails.
   *
   * Optionnally, it is possible to use the LIGO/Virgo convention to save trigger files:
   * @verbatim
   [OUTPUT DIRECTORY]/[IFO]/[CHANNEL]_OMICRON/[GPS (5 first digits)]/file
   @endverbatim
   * @param[in] aUseLVDir Set to true to use the LIGO/Virgo convention for trigger files.
   */
  string WriteTriggers(const bool aUseLVDir = false);

  /**
   * @brief Returns the number of processed segments.
   */
  inline unsigned int GetTriggerSegmentsN(void){
    if(chanindex>=0) return triggers[chanindex]->Segments::GetN();
    return 0;
  }
  
  /**
   * @brief When time plots are requested in output, a time offset [s] can be added.
   * @param[in] aTimeOffset Time offset [s].
   */
  inline void SetPlotTimeOffset(const double aTimeOffset){ toffset=aTimeOffset; };
  
  /**
   * @brief Returns the number of channels.
   */
  inline unsigned int GetChannelsN(void){ return triggers.size(); };

  /**
   * @brief Returns the chunk duration [s].
   */
  inline unsigned int GetChunkDuration(void){return tile->GetTimeRange();};

  /**
   * @brief Returns the overlap duration [s].
   */
  inline unsigned int GetOverlapDuration(void){return tile->GetOverlapDuration();};

  /**
   * @brief Returns the current chunk start time [s].
   */
  inline unsigned int GetChunkTimeStart(void){return tile->GetChunkTimeStart();};

  /**
   * @brief Returns the current chunk end time [s].
   */
  inline unsigned int GetChunkTimeEnd(void){return tile->GetChunkTimeEnd();};

  /**
   * @brief Returns the number of q planes.
   */
  inline unsigned int GetQN(void){return tile->GetQN();};

  /**
   * @brief Returns the Q value of a given q planes.
   * @param[in] aQindex Q-plane index.
   * @pre The Q-plane index must be valid.
   */
  inline double GetQ(const unsigned int aQindex){return tile->GetQ(aQindex);};

  /**
   * @brief Returns the number of frequency rows of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   */
  inline unsigned int GetBandN(const unsigned int aQindex){return tile->GetBandN(aQindex);};

  /**
   * @brief Returns the list of frequency bands.
   * @details The returned array is of size GetBandN()+1 and contains the bin limits. 
   * @warning The returned array must be deleted by the user.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   */
  inline double* GetBands(const unsigned int aQindex){ return tile->GetBands(aQindex); };

  /**
   * @brief Returns the band central frequency [Hz] of a given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   * @param[in] aBandIndex Band index.
   */
  inline double GetBandFrequency(const unsigned int aQindex, const unsigned int aBandIndex){
    return tile->GetBandFrequency(aQindex, aBandIndex);
  };

  /**
   * @brief Returns the number of tiles in a frequency row of given Q-plane.
   * @param[in] aQindex Q-plane index. If the index is out of range, the full map is considered.
   * @param[in] aBandIndex Band index.
   * @pre The band index must be valid.
   */
  inline unsigned int GetBandTileN(const unsigned int aQindex, const unsigned int aBandIndex){ 
    return tile->GetBandTileN(aQindex, aBandIndex);
  };

  /**
   * @brief Returns the maximum SNR squared estimated in a given Q plane.
   * @param[in] aQindex Q-plane index.
   * @pre The Q-plane index must be valid.
   */
  inline double GetSnrSqMax(const unsigned int aQindex){ return tile->GetSnrSqMax(aQindex); };

  /**
   * @brief Returns the SNR squared of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileSnrSq(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){ 
    return tile->GetTileSnrSq(aQindex, aBandIndex, aTimeTileIndex);
  };

  /**
   * @brief Returns the amplitude squared of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileAmplitudeSq(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){
    return tile->GetTileAmplitudeSq(aQindex, aBandIndex, aTimeTileIndex);
  };

  /**
   * @brief Returns the amplitude of a given tile.
   * @param[in] aQindex Q-plane index.
   * @param[in] aBandIndex Band index.
   * @param[in] aTimeTileIndex Tile index in the band.
   * @pre The Q-plane index, the band index and the tile index must be valid.
   */
  inline double GetTileAmplitude(const unsigned int aQindex, const unsigned int aBandIndex, const unsigned int aTimeTileIndex){
    return tile->GetTileAmplitude(aQindex, aBandIndex, aTimeTileIndex);
  };

  /**
   * @brief Returns the number of tiles.
   * @param[in] aPadding Number of seconds excluded on both sides of the time range.
   * @pre The padding value is not checked! Make sure it is compatible with the time range.
   */
  inline long unsigned int GetTileN(const double aPadding=0.0){return tile->GetTileN(aPadding);};

  /**
   * @brief Returns the working sampling frequency [Hz].
   */
  inline unsigned int GetSampleFrequency(void){return triggers[0]->GetWorkingFrequency();};
 
  /**
   * @brief Returns the current number of triggers in memory for the current channel.
   */
  inline Long64_t GetTriggerN(void){
    if(chanindex>=0) return triggers[chanindex]->GetTriggerN();
    return 0;
  };

  /**
   * @brief Returns the SNR threshold used to save triggers.
   */
  inline double GetSnrThreshold(void){ return tile->GetSnrTriggerThr(); };
  
  /**
   * @brief Resets the PSD buffer of current channel.
   */
  inline void ResetPsdBuffer(void) {
    if(chanindex>=0) spectrum1[chanindex]->Reset();
    return;
  };

  /**
   * @brief Resets the trigger buffer of current channel.
   */
  inline void ResetTriggerBuffer(void) {
    if(chanindex>=0) triggers[chanindex]->TriggerBuffer::Reset();
    return;
  };
  
  /**
   * @brief Prints a formatted message with a timer.
   * @param[in] aMessage Message to print.
   */
  void PrintMessage(const string aMessage);

  /**
   * @brief Prints a progress report.
   */
  void PrintStatusInfo(void);
  
  /**
   * @brief Returns the list of channels.
   */
  vector <string> GetChannels(void);

  /**
   * @brief Returns class status.
   */
  inline bool GetStatus(void){ return status_OK; };

  /**
   * @brief Returns the number of injection channels.
   */
  inline unsigned int GetInjectionChannelN(void){ return GO_InjChan.size(); };

  /**
   * @brief Returns the SG injection flag.
   */
  inline bool GetSgInjectionFlag(void){ return GO_InjSg; };

  /**
   * @brief Returns the total number of InjGen injections.
   */
  inline unsigned int GetInjGenN(void){
    if(inject!=NULL) return inject[0]->GetN();
    return 0;
  };


 private:

  // STATUS
  bool status_OK;               ///< General status.
  time_t timer;                 ///< Timer.
  time_t timer_start;           ///< Timer start.
  struct tm * ptm;              ///< GMT time.
  int chanindex;                ///< Current channel index (-1 = no channel).

  // OPTIONS
  unsigned int GO_Verbosity;    ///< Verbosity level.
  string GO_OutProducts;        ///< Output product string.
  string GO_OutFormat;          ///< Output format string.
  string GO_MainDir;            ///< Output main directory (original).
  string MainDir;               ///< Output main directory.
  vector<string> GO_InjChan;    ///< List of injection channels.
  vector<double> GO_InjFact;    ///< List of injection factors.
  bool GO_InjSg;                ///< Flag to perform sine-Gaussian injections.
  double GO_RateMax;            ///< Maximum trigger rate.
  bool GO_thumb;                ///< Flag to produce thumbnails.

  // COMPONENTS
  ffl *FFL;                     ///< ffl object (NULL if none).
  vector<TriggerBuffer*> triggers; ///< Output triggers / channel.
  Otile *tile;                  ///< Tiling structure.
  unsigned int one_channel;     ///< Optimization flag to process one channel at a time.
  Spectrum **spectrum1;         ///< 1st spectrum structure / channel.
  Spectrum **spectrum2;         ///< 2nd spectrum structure / channel.
  Spectrum *spectrumw;          ///< Spectrum structure to test whitening.
  ffl *FFL_inject;              ///< FFL for injections.
  InjEct **inject;              ///< Software injections / channel.
  Oinject *oinj;                ///< Software Oinject injections.
  fft *offt;                    ///< FFT plan to FFT the input chunk.
  double *ChunkVect;            ///< Chunk raw data (time domain).
  double *TukeyWindow;          ///< Tukey window.

  // OUTPUT
  vector <string> outdir;       ///< Output directories / channel.
  ofstream osummaryfile;        ///< Output summary file.
  ofstream oinjfile;            ///< Output file with omicron injection parameters.
  double toffset;               ///< Time offset for plots [s].

  // MONITORING
  Segments *inSegments;         ///< Requested segments.
  Segments **outSegments;       ///< Processed segments /channel.
  unsigned int chunk_ctr;       ///< Number of called chunks.
  unsigned int *chan_data_ctr;  ///< Number of LoadData() calls /channel.
  unsigned int *chan_cond_ctr;  ///< Number of Condition() calls /channel.
  unsigned int *chan_proj_ctr;  ///< Number of Project() calls /channel.
  unsigned int *chan_write_ctr; ///< Number of WriteOutput() calls /channel.
  unsigned int *trig_ctr;       ///< Number of tiles above snr thr /channel.
  double *chan_mapsnrmax;       ///< Channel SNR max in maps (only for html)
  vector <unsigned int> chunkcenter;///< Chunk centers (only for html).
  vector <string> chunktfile;   ///< save chunk file (only for html)
  

  /**
   * @brief Reads the option file.
   * @param[in] aOptFile Path to the option file. This can be either a txt file or a root file.
   * @param[in] aStrict Set this flag to true to operate a stricter check of parameters.
   */
  void ReadOptions(const string aOptFile, const bool aStrict=false);
   
  /**
   * @brief Saves a selection of options.
   * @details This is mostly for optimization purposes. GwollumOptions::GetOptionValues() can be expensive when called multiple times.
   */
  void MakeOptions(void);

  /**
   * @brief Makes the FFL to access the data.
   * @details The FFL object is created if provided in the options (DATA/FFL or DATA/LCF).
   * @param[in] aGpsRef Provide a GPS time to load the channel list in the FFL. If not provided, the first frame is used.
   */
  void MakeFfl(const unsigned int aGpsRef=0);

  /**
   * @brief Makes the trigger objects.
   * @details The list of channels is extracted (blacklisted channels are removed). If a FFL file was provided, it is used to test the existence of a channel.
   */
  void MakeTriggers(void);

  /**
   * @brief Makes the tiling structure.
   */
  void MakeTiling(void);

  /**
   * @brief Makes the Spectrum objects.
   * @param[in] aOneChannel Set this flag to true to only create a single Spectrum object. Otherwise, one Spectrum object is created per channel.
   */
  void MakeSpectrum(const bool aOneChannel);

  /**
   * @brief Makes the injection engines.
   * @param[in] aGpsRef Provide a GPS time to load the injection channel list in the FFL. If not provided, the first frame is used.
   */
  void MakeInjections(const unsigned int aGpsRef=0);

  /**
   * @brief Whiten the chunk data vector.
   * @details The data vector (in the frequency domain) is whitened:
   * - The DC frequency is set to 0.
   * - The Fourier coefficients below the highpass frequency are set to 0.
   * - The Fourier coefficients are divided by the amplitude spectral density. Afactor \f$\sqrt{2}\f$ is included to account for the double whitening. 
   * - Optionally the data is multiplied by a normalization factor.
   * @param[in] aSpec Spectrum object to whiten the data.
   * @param[in] aNorm Normalization factor.
   */
  void Whiten(Spectrum *aSpec, const double aNorm=1.0);
 
  /**
   * @brief Generates a HTML report in the main output directory.
   */
  void MakeHtml(void);

  /**
   * @brief Generates a preliminary HTML report in the main output directory.
   * @details This report can be produced before running the Omicron analysis.
   */
  void MakeHtmlInit(void);

  /**
   * @brief Tests whether a data vector is flat.
   * @returns true if it is.
   * @param[in] aInVectSize Vector size: must be strictly positive.
   * @param[in] aInVect Pointer to data vector.
   */
  bool IsFlat(const unsigned int aInVectSize, double *aInVect);

  /**
   * @brief Returns a html color code based on a SNR ratio value.
   * @param[in] aSnrRatio SNR ratio value.
   */
  string GetColorCode(const double aSnrRatio);

  /**
   * @brief Prints the summary text file.
   */
  void SaveSummary(void);

  /**
   * @brief Prints the options in a ROOT file.
   */
  void SaveOptions(void);

  /**
   * @brief Prints the SineGaus injection parameters in a txt file.
   */
  void SaveSG(void);

  /**
   * @brief Prints the ASD/PSD to a file.
   * @param[in] aType "ASD" or "PSD".
  */
  void SaveAPSD(const string aType);

  /**
   * @brief Prints the timeseries to a file.
   * @param[in] aWhite Set to true to save the whitened timeseries.
   */
  void SaveTS(const bool aWhite=false);

  /**
   * @brief Prints the PSD after whitening to a file.
  */
  void SaveWPSD(void);

  ClassDef(Omicron,0)  
};

#endif


