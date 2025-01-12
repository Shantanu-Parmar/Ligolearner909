/**
 * @file 
 * @brief Omicron analysis time sequence.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Osequence__
#define __Osequence__

#include <TMath.h>
#include <Segments.h>

using namespace std;

/**
 * @brief Omicron analysis time sequence.
 * @details This class offers an algorithm, called `Sequence`, to read an input segment list sequentially.
 * The Segments object is divided into overlapping time chunks.
 * The chunks are loaded sequentially any time the NewChunk() function is called.
 * The chunk sequence can be represented in the following way:
 * \verbatim
------------------------------------------------------------ current segment
 |------------------| chunk i-1
                |------------------| chunk i
                               |------------------| chunk i+1
 
                |---| overlap
 \endverbatim
 * In general, the Segments object contain multiple time segments.
 * The sequence described above does not necessarily match the size of the input segments.
 * The `Sequence` algorithm is designed to deal with such edge effects.
 * Firstly, segments shorter than the chunk duration are skipped.
 * When calling NewChunk() for the last chunk of a segment, the overlap duration is adjusted to fit the leftover:
 * \verbatim
 -----------------------------------------|   <--- input segment under processing

    |--------------------------|              <--- penultimate chunk 
  
 ###### call NextChunk() to cover the left-over

               |--------------------------|   <--- last chunk
	       |---------------|              <--- adjusted overlap
 \endverbatim 
 * Obviously, the user must be careful about this special case as the overlap duration is modified (the chunk duration is never changed).
 * Some functions are available to monitor the overlap size.
 *
 * When moving to a new segment, the overlap duration is set back to nominal values.
 */
class Osequence {

 public:

  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the Osequence class.
   * @details The analysis sequence is intialized.
   * @param[in] aTimeRange Time range [s]. It must be an even number larger than 4 s.
   * @param[in] aTimeOverlap Time overlap [s]. It must be an even number.
   * @param[in] aVerbosity Verbosity level.
   */
  Osequence(const unsigned int aTimeRange, const unsigned int aTimeOverlap,
            const unsigned int aVerbosity=0);

  /**
   * @brief Destructor of the Osequence class.
   */
  virtual ~Osequence(void);
  /**
     @}
  */
  
  /**
   * @brief Resets the time sequence.
   * @details The sequence is initialized.
   * @note The segments set with SetSegments() remain the same.
   */
  void ResetSequence(void);

  /**
   * @brief Sets new input/output segments.
   * @details The input list of segments will be read sequencially using the `Sequence` algorithm.
   * @warning The input segment times must be integer numbers. They will be considered as such!
   *
   * Optionally, an ouput segment list can be provided.
   * @returns The number of chunks to cover the input segments.
   * @param[in] aInSeg Input segment list.
   * @param[in] aOutSeg Output segment list.
   */
  unsigned int SetSegments(Segments *aInSeg, Segments *aOutSeg=NULL);

  /**
   * @brief Loads a new `Sequence` chunk.
   * @details The chunks are loaded following the definition presented in the description of this class.
   * This function should be called iteratively to cover the full data set defined with SetSegments().
   * @returns A flag which indicates the status of the sequence:
   * - true : a new chunk has been loaded.
   * - false : no more chunk to load.
   * @param[out] aNewSegFlag This flag is set to true if a new segment is started. 
   */
  bool NewChunk(bool &aNewSegFlag);

  /**
   * @brief Returns the active segments in the current chunk.
   * @details The chunk is corrected for:
   * - the overlaps with the previous and the next chunk.
   * - the output segments set with SetSegments().
   *
   * @note The user is in charge of deleting the returned Segments object.
   * @returns NULL if failure.
   */
  Segments* GetChunkOut(void);
  
  /**
   * @brief Returns the sequence time range [s].
   */
  inline unsigned int GetTimeRange(void){ return SeqTimeRange; };

  /**
   * @brief Returns the central GPS time of current chunk [s].
   */
  inline unsigned int GetChunkTimeCenter(void){ return SeqT0; };
  
  /**
   * @brief Returns the starting GPS time of current chunk [s].
   */
  inline unsigned int GetChunkTimeStart(void){ return SeqT0-SeqTimeRange/2; };
  
  /**
   * @brief Returns the ending GPS time of current chunk [s].
   */
  inline unsigned int GetChunkTimeEnd(void){ return SeqT0+SeqTimeRange/2; };
  
  /**
   * @brief Returns the current overlap duration [s].
   * @details In most cases the overlap duration is nominal
   * unless the special case of the end of an input segment is hit.
   */
  inline unsigned int GetCurrentOverlapDuration(void){ return SeqOverlapCurrent; };

  /**
   * @brief Returns the nominal overlap duration [s].
   */
  inline unsigned int GetOverlapDuration(void){ return SeqOverlap; };
  
 private:

  unsigned int fVerbosity;      ///< Verbosity level.
  Segments *SeqOutSegments;     ///< Output trigger segments (current - request).
  Segments *SeqInSegments;      ///< Input segments (current - request).
  unsigned int SeqTimeRange;    ///< Time range [s].
  unsigned int SeqOverlap;      ///< Nominal overlap duration [s].
  unsigned int SeqOverlapCurrent;///< Current overlap duration [s].
  unsigned int SeqT0;           ///< Current chunk center.
  unsigned int SeqSeg;          ///< Current segment index.

  ClassDef(Osequence,0)
};

#endif


