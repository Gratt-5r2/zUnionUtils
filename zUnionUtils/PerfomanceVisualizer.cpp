// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#define SafeDivL( a, b ) \
    ( b == 0 ? 0 : a / b )

#define SafeDivF( a, b ) \
    ( b == 0.0f ? 0.0f : a / b )

  zPerfomanceVisualizer::zPerfomanceVisualizer() {
  }

  void zPerfomanceVisualizer::Tick() {
    if( ogame->IsOnPause() )
      return;

    Intervals.Insert( ztimer->frameTime );
    if( Intervals.GetNum() > 1024 )
      Intervals.RemoveAt( 0 );
  }

  void zPerfomanceVisualizer::Render() {
    long startIndex = 8192 - Intervals.GetNum() * 8;

    long start = Intervals[0];
    for( uint i = 1; i < Intervals.GetNum(); i++ ) {
      int end = Intervals[i];
      screen->Line( startIndex + i * 8, 7500 - start * 15, startIndex + i * 8 + 8, 7500 - end * 15, GFX_WHITE );
      start = end;
    }

    long min = 999999, mid = 0, max = 0;
    for( uint i = 0; i < Intervals.GetNum(); i++ ) {
      long interval = Intervals[i];
      if( interval < min ) min = interval;
      if( interval > max ) max = interval;
      mid += interval;
    }

    mid = SafeDivL( mid, Intervals.GetNum() );
    screen->Print( 200, 5000, Z "min: " + Z( int )min );
    screen->Print( 200, 5500, Z "mid: " + Z( int )mid );
    screen->Print( 200, 6000, Z "max: " + Z( int )max );
    screen->Line( 0, 7500, 8192, 7500, GFX_BLACK );
  }

  static zPerfomanceVisualizer* GetPerfomanceVisualizer() {
    static zPerfomanceVisualizer* visualizer = new zPerfomanceVisualizer();
    return visualizer;
  }
}