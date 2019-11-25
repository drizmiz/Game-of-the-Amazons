
#include "std.h"

#include "sio.h"
#include "searcher.h"

#ifndef __GNUC__
int main()
{
	using namespace std;
	using namespace amz;
	amz::initialize();
	amz::chess_game cg = sio::input(cin);
	for (;;)
	{
		const movement mm = cg.generate_next_move();
		if (_Is_dft_move(mm))
			break;
		cg.make_move(mm);
		_Debug_paint(cg.get_status());
		cout << (cg.get_color() == chess_color::black ? "black move: " : "white move: ") << endl;
		sio::output(cout, mm);
		cout << "DEBUG Info: ";
		_Debug_evaluate_bz(cg.get_status(), _Color_rev(cg.get_color()));
		cout << endl;
	}
	return 0;
}
#else
int main()
{
	using namespace std;
	using namespace amz;
	amz::initialize();
	amz::chess_game cg = sio::input(cin);
	const movement mm = cg.generate_next_move();
	cg.make_move(mm);
	sio::output(cout, mm);
	cout << "DEBUG Info: ";
	_Debug_evaluate_bz(cg.get_status(), _Color_rev(cg.get_color()));
	cout << endl;
	return 0;
}
#endif