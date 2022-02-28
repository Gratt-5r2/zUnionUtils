// Supported with union (c) 2020 Union team
// Union SOURCE file
namespace GOTHIC_ENGINE {
	int gettimeofday( struct timeval* tp, struct timezone* tzp ) {
		// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
		// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
		// until 00:00:00 January 1, 1970 
		static const uint64 EPOCH = ((uint64)116444736000000000ULL);

		SYSTEMTIME  system_time;
		FILETIME    file_time;
		uint64      time;

		GetSystemTime( &system_time );
		SystemTimeToFileTime( &system_time, &file_time );
		time = ((uint64)file_time.dwLowDateTime);
		time += ((uint64)file_time.dwHighDateTime) << 32;

		tp->tv_sec = (long)((time - EPOCH) / 10000000L);
		tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
		return 0;
	}

	uint64 GetTimeStamp() {
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return tv.tv_sec * (uint64)1000000 + tv.tv_usec;
	}

	static uint64 startTime = GetTimeStamp();


	void FpsLimit( const uint& fpsLimit ) {
		if( !fpsLimit )
			return;

		static uint64 timeLimit = 1000 / fpsLimit;
		uint64 now = GetTimeStamp();
		uint64 duration = uint64(now - startTime) / 1000ui64;
		if( duration < timeLimit ) {
			uint64 sleepTime = timeLimit - duration;
			if( sleepTime < 100 ) {
				Sleep( (int)sleepTime );
				ztimer->frameTime = (int)timeLimit;
				ztimer->frameTimeFloat = (float)timeLimit;
			}
		}

		startTime = GetTimeStamp();
	}
}