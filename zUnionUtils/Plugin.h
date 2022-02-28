
// This file added in headers queue
// File: "Headers.h"

namespace GOTHIC_ENGINE {
  template<class T>
  T ReadOption( const string& section, const string& parameter ) {
    T result;
    if( CHECK_THIS_ENGINE )
      Union.GetSysPackOption().Read( result, section, parameter );
    return result;
  }

  static float s_DistMult               = 1.0f;
  static int s_LowLimitWindowed         = 0;
  static int s_UpLimitWindowed          = 0;
  static bool s_AdaptiveSightInfo       = false;
  static bool s_AdaptiveSightOfView     = false;
  static bool s_VisualizePerfomance     = false;
}