// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
	// int __cdecl vidGetFPSRate(void)
	typedef int(*LPVIDGETFPSRATE)();
	LPVIDGETFPSRATE vidGetFPSRate = (LPVIDGETFPSRATE)ZenDef( 0x004EF790, 0x00502030, 0x004FB050, 0x004FDCD0 );

	float GetSightOfView() {
		return ogame->GetWorld()->GetActiveSkyControler()->GetFarZScalability();
	}

	void SetSightOfView( const float& sight ) {
		return ogame->GetWorld()->GetActiveSkyControler()->SetFarZScalability( sight );
	}

	bool IsFullScreen() {
		return zrenderer->Vid_GetScreenMode() == zTRnd_ScreenMode::zRND_SCRMODE_FULLSCREEN;
	}

	void AdaptiveSightOfView() {
		static Timer timer;
    static uint ID0 = 0;
    static uint ID1 = 1;
    static uint ID2 = 2;
    static uint ID3 = 3;
    static uint ID4 = 4;
		timer.ClearUnused();

		static bool canDown			= false;
		static bool canUp				= false;
		static const float step	= 0.01f;
		static float sight			= GetSightOfView();

		bool isFullScreen = IsFullScreen();
		const int lowLimit = isFullScreen ? 55 : s_LowLimitWindowed;
		const int upLimit  = isFullScreen ? 59 : s_UpLimitWindowed;

		int fps = vidGetFPSRate();
		if( fps < lowLimit ) {
			if( timer[ID0].Await( 1000 ) )
				canDown = true;

			timer[ID1].Delete();
		}
		else {
			if( timer[ID1].Await( 500 ) )
				canDown = false;

			timer[ID0].Delete();
		}

		if( fps >= upLimit ) {
			if( timer[ID2].Await( 1000 ) )
				canUp = true;

			timer[ID3].Delete();
		}
		else {
			if( timer[ID3].Await( 500 ) )
				canUp = false;

			timer[ID2].Delete();
		}

		if( ogame->IsOnPause() )
			return;

		if( s_AdaptiveSightInfo ) {
					 if( canDown ) screen->Print( 300, 300, string::Combine( "FPS:       [ %i ]->  %i", fps, upLimit ) );
      else if( canUp   ) screen->Print( 300, 300, string::Combine( "FPS: %i  <-[ %i ]      ", lowLimit, fps ) );
			else						   screen->Print( 300, 300, string::Combine( "FPS: %i  <-[ %i ]->  %i", lowLimit, fps, upLimit ) );
												 screen->Print( 300, 550, string::Combine( "Adaptive distance: %i%%", int(100.0f / 5.0f * sight) ) );
		}

		if( !canDown && !canUp ) {
			SetSightOfView( sight );
			return;
		}

		while( timer[ID4].AwaitExact( 15 ) ) {
			if( fps < lowLimit && sight > 0.4f )
				sight -= step;
			else if( fps > upLimit && sight < 2.0 + 3.0f * s_DistMult )
				sight += step;
		}

		SetSightOfView( sight );
	}
}