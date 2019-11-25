
#include "std.h"

#include "sio.h"
#include "searcher.h"

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
	return 0;
}

/*

int _main_()

{

	using namespace std;

	amz::initialize();

	amz::chess_game cg = sio::input(cin);

	Timer t;

	t.start();

	using namespace amz;

	for (int j = 0; j < 1e8; ++j)

	{

		const int tmp = j & 0x3F;

		cg.generate_next_move();

	}

	t.end();

	std::cout << t.timespan() << "ms" << std::endl;

	//sio::output(cout,cg);

	return 0;

}



int __main()

{

	using namespace amz;

	constexpr bit_table i = 0xFF88888888888888;

	std::cout << "ORIGINAL BIT_TABLE:" << std::endl;

	_Debug_paint(i);

	_Debug_paint(_Rotate_90degree_right(i));

	// [7][7]	1 1 1 1 1 1 1 1		[7][0]

	//			1 0 0 0 1 0 0 0

	//			1 0 0 0 1 0 0 0

	//			1 0 0 0 1 0 0 0

	//			1 0 0 0 1 0 0 0

	//			1 0 0 0 1 0 0 0

	//			1 0 0 0 1 0 0 0

	// [0][7]	1 0 0 0 1 0 0 0		[0][0]

	Timer t;

	t.start();

	initialize();

	t.end();

	std::cout << "initialize(): " << t.timespan() << "ms" << std::endl;

	//constexpr int r = _Get_row_index_by_i(i, get_i(6, 3));

	//constexpr int c = _Get_col_index(i, 4);

	//constexpr int diag = _Get_main_diag_index(i, 2 * 8 + 3);

	//constexpr int cd = _Get_counter_diag_index(i, 3 * 8 + 3);

	//const uint64 _r = csbd::row_moves[(7 - 1) * 8 + (4 - 1)][r];

	//const uint64 _c = csbd::col_moves[7 * 8 + (5 - 1)][c];

	//const uint64 _diag = csbd::diag_main_moves[2 * 8 + 3][diag];

	//const uint64 _cd = csbd::diag_counter_moves[3 * 8 + 3][cd];

	//_Debug_paint(_r);

	//_Debug_paint(_c);

	//_Debug_paint(_diag);

	//_Debug_paint(_cd);

	std::cout << "ALL POSSIBLE MOVES AT (" << 3 << "," << 3 << "):" << std::endl;

	_Debug_paint(get_all_possible_moves(i, get_i(3, 3)));

	bit_table no_use = 0;

	bit_table bt_i = i;

	chess_board cb_i = i;

	t.start();

	for (int j = 0; j < 1e7; ++j)

	{

		const int tmp = j & 0x3F;

		place_bit(bt_i, tmp);

		move_bit(bt_i, tmp, tmp % 8);

		no_use += get_all_possible_moves(bt_i, tmp);

	}

	t.end();

	std::cout << t.timespan() << "ms" << std::endl << no_use << std::endl;

	no_use = 0;

	t.start();

	for (int j = 0; j < 1e7; ++j)

	{

		const int tmp = j & 0x3F;

		cb_i.place_piece(tmp);

		cb_i.move_piece(tmp, tmp % 8);

		no_use += cb_i.get_all_possible_moves(tmp);

	}

	t.end();

	std::cout << t.timespan() << "ms" << std::endl << no_use;

	//system("pause");

	return 0;

}



int _main(int argc) noexcept

{

	using namespace amz;

	bit_table bt = 0;

	place_bit(bt, get_i(7, argc));

	return check_bit(bt, get_i(argc, 1));

}

*/