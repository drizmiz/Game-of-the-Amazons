
#include "std.h"

// #define __GNUC__

#include "sio.h"
#include "searcher.h"

#ifndef __GNUC__
int main()
{
	using namespace std;
	using namespace amz;
	ios::sync_with_stdio(false);
	amz::initialize();
	amz::chess_game cg = sio::input(cin);
	for (;;)
	{
		cout << "------ turn " << cg.get_turn() << " ------" << endl;
		using namespace chrono;
		auto starttime = steady_clock::now();
		const movement mm = cg.generate_next_move();
		auto end = steady_clock::now();
		auto diff = duration_cast<milliseconds>(end - starttime).count();
		cout << "time: " << diff << "ms" << endl;
		if (_Is_dft_move(mm))
			break;
		cg.make_move(mm);
		_Debug_paint(cg.get_status());
		cout << (cg.get_color() == chess_color::black ? "black move: " : "white move: ") << endl;
		sio::output(cout, mm);
		cout << "DEBUG Info: ";
		_Debug_evaluate(cg.get_status(), _Color_rev(cg.get_color()), cg.get_turn());
		cout << endl;
	}
	return 0;
}
#else
int main()
{
	using namespace std;
	using namespace amz;
	ios::sync_with_stdio(false);
	amz::initialize();
	amz::chess_game cg = sio::input(cin);
	const movement mm = cg.generate_next_move();
	cg.make_move(mm);
	sio::output(cout, mm);
	cout << "DEBUG Info: ";
	_Debug_evaluate_bz(cg.get_status(), _Color_rev(cg.get_color()), cg.get_turn());
	cout << endl;
	return 0;
}
#endif