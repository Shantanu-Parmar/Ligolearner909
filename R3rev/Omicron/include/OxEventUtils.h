/**
 * @file 
 * @brief Ox analysis utilities.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __OxEventUtils__
#define __OxEventUtils__

#include "Oomicron.h"

/**
 * @brief List of Ox event types.
 */
enum OxEventType{
             oxevent_zerolag = 0,  ///< Zero-lag event.
             oxevent_background,   ///< Background event.
             oxevent_fakezerolag,  ///< Fake zero-lag event.
             oxevent_n             ///< Number of Ox event types.
};

/**
 * @brief Ox event parameters.
 * @details An Ox event is parameterized with:
 * - A rank, \f$Xi\f$, which is evaluated from the cross-correlation maps. See also OxEvent::Rank().
 * - A time shift \f$\delta \tau\f$ between the two detectors.
 * - A peak frequency.
 * - A slice index (0 is the zero-lag).
 * - A TTree index to link with metadata: see OxMeta.
 * - For each Q plane, the number of frequency rows which are discarded to compute the rank value: see OxEvent::ComputeXi().
 * - For each Q plane, the integrated \f$Xi\f$ value: see OxEvent::ComputeXi().
 * - For each Q plane, the fluctuation of the cross-correlation values around \f$\delta \tau\f$ along the frequency direction: see OxEvent::ComputeXi().
 */
class OxEventParam{

 public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  
   /**
   * @brief Constructor of the OxEventParam class.
   * @details The event parameters are initialized. They include both Omicron objects used to measure the cross-correlation.
   * @param[in] aOmicron1 Omicron object for the first detector.
   * @param[in] aOmicron2 Omicron object for the second detector.
   */ 
  OxEventParam(Omicron *aOmicron1, Omicron *aOmicron2);

  /**
   * @brief Destructor of the OxEventParam class.
   */
  virtual ~OxEventParam(void);

  /**
     @}
  */

  /**
   * @brief Returns the event rank.
   * @details TBC.
   */
  double GetRank(void);

  /**
   * @brief Returns the pointer to the Omicron object for the first detector.
   */
  inline Omicron* GetOmicron1(void){ return O1; };

  /**
   * @brief Returns the pointer to the Omicron object for the second detector.
   */
  inline Omicron* GetOmicron2(void){ return O2; };

  /**
   * @brief Sets the maximum SNR for the first detector.
   * @param[in] aSnr Maximum SNR value. Use a negative value to de-activate the veto
   */
  inline void SetVetoSnr1(const double aSnr){
    veto_snr1 = aSnr;
    veto_snrsqthr1 = veto_snr1*veto_snr1;
  };
  
  /**
   * @brief Sets the maximum SNR for the second detector.
   * @param[in] aSnr Maximum SNR value. Use a negative value to de-activate the veto
   */
  inline void SetVetoSnr2(const double aSnr){
    veto_snr2 = aSnr;
    veto_snrsqthr2 = veto_snr2*veto_snr2;
  };
  
  /**
   * @brief Returns the maximum SNR for the first detector.
   */
  inline double GetVetoSnr1(void){ return veto_snr1; };
  
  /**
   * @brief Returns the maximum SNR for the second detector.
   */
  inline double GetVetoSnr2(void){ return veto_snr2; };
  
  /**
   * @brief Sets the veto \f$Xi\f$ spread threshold.
   * @param[in] aXiSpreadThreshold Minimum spread value to reject the event. Use a negative value to de-activate the veto.
   */
  inline void SetVetoXiSpread(const double aXiSpreadThreshold){
    veto_xispread = aXiSpreadThreshold;
  };
  
  /**
   * @brief Returns the veto \f$Xi\f$ spread threshold.
   */
  inline double GetVetoXiSpread(void){ return veto_xispread; };
   
  /**
   * @brief Returns the spread of \f$\Xi\f$ values across Q planes.
   * @details Each Q plane is given a \f$\Xi\f$ value. This function returns the standard deviation of the \f$\Xi\f$ values.
   */
  double GetXiSpread(void);

  /**
   * @brief Returns the maximum spread of \f$\Xi\f$ values across Q planes.
   * @details Each Q plane is given a \f$\Xi\f$ value. This function returns the difference between the maximum \f$\Xi\f$ value and the minimum \f$\Xi\f$ value.
   */
  double GetXiSpreadMax(void);

  /**
   * @brief Sets the veto frequency rejection thresholds.
   * @param[in] aQindex Q plane index.
   * @param[in] aFrequencyRejectionThreshold Minimum rejection value to reject the event. It must be given as a percentage. Use a negative value to de-activate the veto.
   */
  inline void SetVetoFrequencyRejection(const unsigned int aQindex, const double aFrequencyRejectionThreshold){
    if(aQindex<O1->GetQN()) veto_freqrej[aQindex] = aFrequencyRejectionThreshold;
  };
  
  /**
   * @brief Returns the veto frequency rejection threshold.
   * @param[in] aQindex Q plane index.
   */
  inline double GetVetoFrequencyRejection(const unsigned int aQindex){
    if(aQindex<O1->GetQN()) return veto_freqrej[aQindex];
    return -1.0;
  };
   

  // these variables must be set for both read/write modes.
  Long64_t e_entry;                  ///< Event: TTree entry number.
  double e_Xi;                       ///< Event: \f$\Xi\f$.
  double e_frequency;                ///< Event: frequency [Hz].
  double e_delta_tau;                ///< Event: \f$\delta \tau\f$ [s].
  unsigned int *e_n_slice_veto;      ///< Event: number of frequency rows rejected by the slice veto / Q plane.
  double *e_Xi_q;                    ///< Event: \f$\Xi\f$ / Q plane.
  double *e_xi_tvar_q;                ///< Event: \f$\xi\f$ time variance around \f$\delta \tau\f$ and along the frequency axis / Q plane.
  int e_slice_index;                 ///< Event: slice index (0=zerolag).
  unsigned int e_meta_index;         ///< Event: OxMeta TTree index.
  double e_meta_snrsq1;              ///< Event: SNR1 squared.
  double e_meta_snrsq2;              ///< Event: SNR2 squared.

 private:
  
  // OMICRON
  Omicron *O1;                       ///< Omicron object for the first detector - do not delete.
  Omicron *O2;                       ///< Omicron object for the second detector - do not delete.

  // VETO
  double veto_snr1;                  ///< SNR threshold to veto events (first detector).
  double veto_snr2;                  ///< SNR threshold to veto events (second detector).
  double veto_snrsqthr1;             ///< SNR threshold squared (1).
  double veto_snrsqthr2;             ///< SNR threshold squared (2).
  double veto_xispread;              ///< \f$\Xi\f$ spread threshold to veto events.
  double *veto_freqrej;              ///< Frequency rejection threshold to veto events.
};


#endif


