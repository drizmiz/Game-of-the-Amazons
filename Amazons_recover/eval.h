
#pragma once

#include "game.h"

#include "eval_main.h"
#include "csbd.h"


namespace amz
{
	int _Evaluate(const chess_status& cs, chess_color color,int turn_cnt)
	{
		using namespace ev;
		player pl((color == chess_color::black), cs);
		merged_board brd(cs);
		evaluation_weighter ewf;
		evaluator eval(brd, pl, turn_cnt, ewf);
		return static_cast<int>(eval.evaluate() * 10000);
	}
	int _Debug_evaluate(const chess_status& cs, chess_color color, int turn_cnt, std::ostream& out = std::cout)
	{
		using namespace ev;
		player pl((color == chess_color::black), cs);
		merged_board brd(cs);
		evaluation_weighter ewf;
		evaluator ev(brd, pl, turn_cnt, ewf);

		double r = 0;
		ev._Generate_distance_matrix();
		out << "Evaluate: " << std::endl;
		double tmp = 0;
		tmp = ev._territory_ingredient();
		out << tmp << std::endl;
		r += tmp;

		tmp = ev._mobility_ingredient();
		out << tmp << std::endl;
		r += tmp;
		tmp = ev._guard_ingredient();
		out << tmp << std::endl;
		r += tmp;
		tmp = ev._distribution_ingredient();
		out << tmp << std::endl;
		r += tmp;
		//std::cout << "merged_dm_1 player0:" << std::endl;
		//ev._debug_printf_distance_matrix(ev.merged_dm_1[0]);
		//std::cout << "merged_dm_1 player1:" << std::endl;
		//ev._debug_printf_distance_matrix(ev.merged_dm_1[1]);
		//std::cout << "merged_dm_2 player0:" << std::endl;
		//ev._debug_printf_distance_matrix(ev.merged_dm_2[0]);
		//std::cout << "merged_dm_2 player1:" << std::endl;
		//ev._debug_printf_distance_matrix(ev.merged_dm_2[1]);
		out << "eval: " << static_cast<int>(r * 10000) << std::endl;
		return 0;
	}
	int _Debug_evaluate_bz(const chess_status& cs, chess_color color,int turn_cnt)
	{
		std::stringstream ss;
		_Debug_evaluate(cs, color, turn_cnt, ss);
		for (; ss;)
		{
			std::string s;
			getline(ss, s);
			if (s != "")
				std::cout << s << " \\n ";
		}
		return 0;
	}
}