/**
 * @file 
 * @brief Inject sinusoidal Gaussian waveforms.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Oinject__
#define __Oinject__
#include <Spectrum.h>
#include <TRandom3.h>
#include <TMath.h>

#define sqrt2pi 2.5066282746310002416123552393401042

using namespace std;

/**
 * @brief Inject sinusoidal Gaussian waveforms.
 * @author Florent Robinet
 */
class Oinject{

 public:
  
  /**
   * @name Constructors and destructors
   @{
  */
  /**
   * @brief Constructor of the Oinject class.
   * @brief The ranges for the parameters are set to default values:
   * - Time: \f$\tau = 0\f$
   * - Frequency: \f$32\le f_0 < 512\f$ Hz
   * - Q: \f$4\leQ < 100\f$
   * - amplitude: \f$A = 10^{-21}\f$
   * - Phase: \f$0\le\varphi < 2\pi\f$
  *
   * A set of random parameters is generated with MakeWaveform().
   * The user must specify the duration of the injection waveform.
   * @param[in] aDuration Waveform duration [s].
   */
  Oinject(const double aDuration);

  /**
   * @brief Destructor of the Oinject class.
   */
  virtual ~Oinject(void);
  /**
     @}
  */

  
  /**
   * @brief Generates a new set of waveform parameters.
   * @sa GenerateParameters() for definitions.
   */
  void MakeWaveform(void);

  /**
   * @brief Returns the waveform amplitude \f$s\f$ for a given time index \f$i\f$.
   * @details The waveform amplitude is computed for the set of parameters previously generated with MakeWaveform().
   * The user must specify the frequency \f$f_0\f$ at which the data vector is sampled.
   * This way, combined with the duration \f$T\f$ set in the constructor, the index can be converted to a time value: \f$t_i = -T/2+i/f_0\f$.
   * @returns The waveform amplitude given by:
   * \f[
   s[i] = A \times \frac{W_g}{\sigma_t\sqrt{2\pi}}\times \exp\left( -\frac{(t_i-\tau)^2}{2\sigma_t^2}\right) \times \cos\left(2\pi\phi t_i+\varphi\right)
   \f]
   * where \f$W_g = \sqrt{\sqrt{2/\pi} \times Q/\phi}\f$ and \f$\sigma_t = Q/(\sqrt{8}\pi\phi)\f$.
   * 
   * @param[in] aIndex Waveform time index \f$i\f$.
   * @param[in] aSamplingFrequency Sampling frequency \f$f_0\f$ [Hz].
   */  
  inline double GetWaveform(const unsigned int aIndex, const unsigned int aSamplingFrequency){
    return
      amp* // signal amplitude
      Wg/sigma_t/sqrt2pi*exp(-(-duration/2.0+(double)aIndex/(double)aSamplingFrequency-tau)*(-duration/2.0+(double)aIndex/(double)aSamplingFrequency-tau)/2.0/sigma_t/sigma_t)* // Gaussian window
      TMath::Cos(2.0*TMath::Pi()*phi*(-duration/2.0+(double)aIndex/(double)aSamplingFrequency)+phase);// sine
  };

  /**
   * @brief Returns the true value of SNR.
   * @details TBC.
   * @param[in] aSpec1 Noise spectrum (1)
   * @param[in] aSpec2 Noise spectrum (2)
   */
  double GetTrueSNR(Spectrum *aSpec1, Spectrum *aSpec2);

  /**
   * @brief Sets a new range for \f$\tau\f$: \f$\tau_{min}\f$ - \f$\tau_{max}\f$.
   * @param[in] aTimeMin Minimum time \f$\tau_{min}\f$ [s].
   * @param[in] aTimeMax Maximum time \f$\tau_{max}\f$ [s].
   */
  inline void SetTimeRange(const double aTimeMin, const double aTimeMax){
    taumin=aTimeMin; taumax=aTimeMax;
  };

  /**
   * @brief Sets a new range for \f$\phi\f$: \f$\phi_{min}\f$ - \f$\phi_{max}\f$.
   * @param[in] aFreqMin Minimum frequency \f$\phi_{min}\f$ [Hz].
   * @param[in] aFreqMax Maximum frequency \f$\phi_{max}\f$ [Hz].
   */
  inline void SetFrequencyRange(const double aFreqMin, const double aFreqMax){
    phimin=aFreqMin; phimax=aFreqMax;
  };

  /**
   * @brief Sets a new range for \f$A\f$: \f$A_{min}\f$ - \f$A_{max}\f$.
   * @param[in] aAmpMin Minimum amplitude \f$A_{min}\f$.
   * @param[in] aAmpMax Maximum amplitude \f$A_{max}\f$.
   */
  inline void SetAmplitudeRange(const double aAmpMin, const double aAmpMax){
    ampmin=aAmpMin; ampmax=aAmpMax;
  };

  /**
   * @brief Sets a new range for \f$Q\f$: \f$Q_{min}\f$ - \f$Q_{max}\f$.
   * @param[in] aQMin Minimum Q \f$Q_{min}\f$.
   * @param[in] aQMax Maximum Q \f$Q_{max}\f$.
   */
  inline void SetQRange(const double aQMin, const double aQMax){
    Qmin=aQMin; Qmax=aQMax;
  };

  /**
   * @brief Returns the injection time \f$\tau\f$ [s].
   */  
  inline double GetTime(void){ return tau; };

  /**
   * @brief Returns the minimum time \f$\tau_{min}\f$ [s].
   */  
  inline double GetTimeMin(void){ return taumin; };

  /**
   * @brief Returns the maximum time \f$\tau_{max}\f$ [s].
   */  
  inline double GetTimeMax(void){ return taumax; };

  /**
   * @brief Returns the injection frequency \f$\phi\f$ [Hz].
   */  
  inline double GetFrequency(void){ return phi; };

  /**
   * @brief Returns the minimum frequency \f$\phi_{min}\f$ [Hz].
   */  
  inline double GetFrequencyMin(void){ return phimin; };

  /**
   * @brief Returns the maximum frequency \f$\phi_{max}\f$ [Hz].
   */  
  inline double GetFrequencyMax(void){ return phimax; };

  /**
   * @brief Returns the injection Q \f$Q\f$.
   */  
  inline double GetQ(void){ return Q; };

  /**
   * @brief Returns the minimum Q \f$Q_{min}\f$.
   */  
  inline double GetQMin(void){ return Qmin; };

  /**
   * @brief Returns the maximum Q \f$Q_{max}\f$.
   */  
  inline double GetQMax(void){ return Qmax; };

  /**
   * @brief Returns the injection amplitude \f$A\f$.
   */  
  inline double GetAmplitude(void){ return amp; };

  /**
   * @brief Returns the minimum amplitude \f$A_{min}\f$.
   */  
  inline double GetAmplitudeMin(void){ return ampmin; };

  /**
   * @brief Returns the maximum amplitude \f$A_{max}\f$.
   */  
  inline double GetAmplitudeMax(void){ return ampmax; };

  /**
   * @brief Returns the injection phase \f$\varphi\f$.
   */  
  inline double GetPhase(void){ return phase; };

  /**
   * @brief Returns the injection duration \f$\sigma_t\f$.
   */  
  inline double GetSigmat(void){ return sigma_t; };

  /**
   * @brief Returns the injection bandwidth \f$\sigma_f\f$.
   */  
  inline double GetSigmaf(void){ return sigma_f; };

 private:
  
  double duration;     ///< Vector duration [s].
  TRandom3 *randgen;   ///< Random generator

  double Wg;           ///< Gaussian window normalization.
  double sigma_t;      ///< Gaussian window width (time).
  double sigma_f;      ///< Gaussian window width (frequency).

  double tau;          ///< Injection time \f$\tau\f$.
  double taumin;       ///< Minimum injection time.
  double taumax;       ///< Maximum injection time.
  double phi;          ///< Injection frequency \f$\phi\f$.
  double phimin;       ///< Minimum injection frequency.
  double phimax;       ///< Maximum injection frequency.
  double Q;            ///< Injection Q \f$Q\f$.
  double Qmin;         ///< Minimum injection Q.
  double Qmax;         ///< Maximum injection Q.
  double amp;          ///< Injection amplitude \f$A\f$.
  double ampmin;       ///< Minimum injection amplitude.
  double ampmax;       ///< Maximum injection amplitude.
  double snr;          ///< Injection SNR \f$\rho\f$.
  double phase;        ///< Injection phase \f$\varphi\f$.

  /**
   * @brief Generates a random set of parameters.
   * @details Random parameters:
   * - \f$\tau\f$ (peak time): uniform distribution between \f$\tau_{min}\f$ and \f$\tau_{max}\f$
   * - \f$\varphi\f$ (phase): uniform distribution between \f$0\f$ and \f$2\pi\f$
   * - \f$A\f$ (amplitude): log-uniform distribution between \f$A_{min}\f$ and \f$A_{max}\f$
   * - \f$\phi\f$ (frequency): uniform distribution between \f$\phi_{min}\f$ and \f$\phi_{max}\f$
   * - \f$Q\f$ (quality factor): uniform distribution between \f$Q_{min}\f$ and \f$Q_{max}\f$
   */
  void GenerateParameters(void);


  ClassDef(Oinject,0)  
};


#endif


