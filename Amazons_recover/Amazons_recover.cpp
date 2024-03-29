﻿
#include "std.h"

#include "sio.h"
#include "searcher.h"

#ifndef _BOTZONE_ONLINE
int main()
{
	using namespace std;
	using namespace amz;

	fixed_queue<int, 9> fq;
	for (int i = 0; i < 8; ++i)
		fq.push(i);	
	fq._Debug();	// 01234567
	for (int i = 0; i < 4; ++i)
		cout << fq.pop();	//0123
	cout << endl;
	fq._Debug();	//4567
	for (int i = 0; i < 4; ++i)
		fq.push(i);	
	fq._Debug();	// 45670123
	fq.pop();
	fq.push(9); fq._Debug();		// 56701239
	fq.pop();
	fq.pop();
	fq.push(0); fq._Debug();		// 7012390

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