// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct zPerfomanceVisualizer {
		Array<long> Intervals;

		zPerfomanceVisualizer();
		void Tick();
		void Render();
	};
}