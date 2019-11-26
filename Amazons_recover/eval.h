
#pragma once

#include "game.h"
// #include "original_eval.h"

// #define _DEBUG
// #include "refactored_eval.h"
// #include "eval_rewrite.h"
#ifdef __GNUC__
#define _BOTZONE_ONLINE
#endif
#include "eval_adj_rewrite.h"
#include "csbd.h"
// #undef _DEBUG

namespace amz
{
	double evaluate(chess_status cs)
	{
		static std::default_random_engine engine;
		static std::uniform_real_distribution<> dis(-100, 100);
		return dis(engine);
	}
#if false
	int _Evaluate(const chess_status& cs, chess_color color)	// 这里是调用入口
	{

		bit_table mine, enemy;
		if (color == chess_color::black)
		{
			mine = cs.black;
			enemy = cs.white;
		}
		else
		{
			mine = cs.white;
			enemy = cs.black;
		}

		yrq::board brd(
			yrq::bitmap(amz::_Up_down_flip(mine)),
			yrq::bitmap(amz::_Up_down_flip(cs.all.board()))
		);
		yrq::evaluator ev(brd);

		yrq::evaluator::player player0, player1;
		auto my_amzs = _Transform_to_i(mine);
		for (int i = 0; i < 4; ++i)
		{
			auto [x, y] = get_ij(my_amzs[i]);
			player0[i] = yrq::board::piece(x, y);
		}
		auto enemy_amzs = _Transform_to_i(enemy);
		for (int i = 0; i < 4; ++i)
		{
			auto [x, y] = get_ij(enemy_amzs[i]);
			player1[i] = yrq::board::piece(x, y);
		}

		ev.players[0] = player0;
		ev.players[1] = player1;


		return static_cast<int>(ev.evaluate() * 100);



	}
	int _Debug_evaluate(const chess_status& cs, chess_color color, std::ostream& out = std::cout)
	{
		bit_table mine, enemy;
		if (color == chess_color::black)
		{
			mine = cs.black;
			enemy = cs.white;
			out << "black eval: ";
		}
		else
		{
			mine = cs.white;
			enemy = cs.black;
			out << "white eval: ";
		}

		yrq::board brd(
			yrq::bitmap(amz::_Up_down_flip(mine)),
			yrq::bitmap(amz::_Up_down_flip(cs.all.board()))
		);
		yrq::evaluator ev(brd);

		yrq::evaluator::player player0, player1;
		auto my_amzs = _Transform_to_i(mine);
		for (int i = 0; i < 4; ++i)
		{
			auto [x, y] = get_ij(my_amzs[i]);
			player0[i] = yrq::board::piece(x, y);
		}
		auto enemy_amzs = _Transform_to_i(enemy);
		for (int i = 0; i < 4; ++i)
		{
			auto [x, y] = get_ij(enemy_amzs[i]);
			player1[i] = yrq::board::piece(x, y);
		}

		ev.players[0] = player0;
		ev.players[1] = player1;

		double r = 0;
		ev._generate_distance_matrix();
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
		out << "eval: " << static_cast<int>(r * 1000) << std::endl;
		return 0;
	}
#else
	int _Evaluate(const chess_status& cs, chess_color color,int turn_cnt)
	{
		using namespace eval_adj;
		player pl((color == chess_color::black), cs);
		board brd(cs);
		evaluation_weight_function ewf;
		evaluator eval(brd, pl, turn_cnt, ewf);
		return static_cast<int>(eval.evaluate() * 50);
	}
	int _Debug_evaluate(const chess_status& cs, chess_color color, int turn_cnt, std::ostream& out = std::cout)
	{
		using namespace eval_adj;
		player pl((color == chess_color::black), cs);
		board brd(cs);
		evaluation_weight_function ewf;
		evaluator ev(brd, pl, turn_cnt, ewf);

		double r = 0;
		ev._generate_distance_matrix();
		out << "Evaluate: " << std::endl;
		double tmp = 0;
		tmp = ev._territory_ingredient();
		out << tmp << std::endl;
		r += tmp;

		tmp = ev._mobility_ingredient();
		out << tmp << std::endl;
		r += tmp;
		// tmp = ev._guard_ingredient();
		// out << tmp << std::endl;
		// r += tmp;
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
		out << "eval: " << static_cast<int>(r * 50) << std::endl;
		return 0;
	}
#endif
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