/**
 * @file 
 * @brief This module parameterizes a Omicron release.
 * @author Florent Robinet - <a href="mailto:florent.robinet@ijclab.in2p3.fr">florent.robinet@ijclab.in2p3.fr</a>
 */
#ifndef __Oconfig__
#define __Oconfig__

#include <CUtils.h>

using namespace std;

/**
 * @brief Project version number.
 */
#define O_PROJECT_NAME "Omicron"

/**
 * @brief Project version number.
 */
#define O_PROJECT_VERSION "4.1.1"

/**
 * @brief Ratio to produce thumbnails.
 */
#define O_THUMBNAIL_RATIO 0.3

/**
 * @brief Number of seconds in a year.
 * @todo Move this to GWOLLUM.
 */
#define O_SECONDS_IN_ONE_YEAR 31557600

/**
 * @brief Prints the version number.
 * @details This function also checks the Omicron environment:
 * - `$OMICRON_HTML` must point to an existing directory.
 *
 * @returns 0 if the environment is correctly set.
 */
inline int OmicronPrintVersion(void){
  cout<<(string)O_PROJECT_NAME<<" "<<(string)O_PROJECT_VERSION<<endl;

  // check OMICRON_HTML
  char *omicron_html = getenv("OMICRON_HTML");
  if(omicron_html==NULL){
    cerr<<"Env: $OMICRON_HTML is missing"<<endl;
    return 0;
  }
  if(!filesystem::is_directory(omicron_html)){
    cerr<<"Env: missing $OMICRON_HTML directory"<<endl;
    return 0;
  }

  return 0;
}

/**
 * @brief Prints the ASCII logo.
 */
////////////////////////////////////////////////////////////////////////////////////
inline void PrintAsciiLogo(void){
////////////////////////////////////////////////////////////////////////////////////

  cout<<endl;
  cout<<endl;
  cout<<"##############################################################"<<endl;
  cout<<"##############################################################"<<endl;
  cout<<endl;
  cout<<"   ██████╗ ███╗   ███╗██╗ ██████╗██████╗  ██████╗ ███╗   ██╗"<<endl;
  cout<<"  ██╔═══██╗████╗ ████║██║██╔════╝██╔══██╗██╔═══██╗████╗  ██║"<<endl;
  cout<<"  ██║   ██║██╔████╔██║██║██║     ██████╔╝██║   ██║██╔██╗ ██║"<<endl;
  cout<<"  ██║   ██║██║╚██╔╝██║██║██║     ██╔══██╗██║   ██║██║╚██╗██║"<<endl;
  cout<<"  ╚██████╔╝██║ ╚═╝ ██║██║╚██████╗██║  ██║╚██████╔╝██║ ╚████║"<<endl;
  cout<<"   ╚═════╝ ╚═╝     ╚═╝╚═╝ ╚═════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝"<<endl;
  cout<<"                                                    "<<(string)O_PROJECT_VERSION<<endl;
  cout<<"##############################################################"<<endl;
  cout<<"##############################################################"<<endl;
  cout<<endl;
  cout<<endl;

  return;
}

#endif


