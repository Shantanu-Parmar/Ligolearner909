/**
 * @file 
 * @brief Omicron utilities.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#include <ReadAscii.h>
#include <Streams.h>
#include "Oconfig.h"

using namespace std;

/**
 * @brief Returns the list of Omicron trigger files.
 * @details The standard omicron trigger directories are scanned:
 * - The offline area pointed by the environment variable `$OMICRON_TRIGGERS`
 * - The online area pointed by the environment variable `$OMICRON_TRIGGERS_ONLINE`
 *
 * The list of files matching the input time range are returned.
 * @note The priority is given to the offline area.
 * @note If the offline trigger directory is HPSS (CCIN2P3), GetOmicronFilePatternFromHpss() is called.
 * @param[in] aChannelName Channel name.
 * @param[in] aTimeStart GPS start time.
 * @param[in] aTimeEnd GPS end time.
 */
string GetOmicronFilePattern(const string aChannelName,
                             const unsigned int aTimeStart,
                             const unsigned int aTimeEnd);

