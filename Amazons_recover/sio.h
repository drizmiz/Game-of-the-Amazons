
#pragma once

#include "std.h"
#include "game.h"

namespace sio // simple io
{
	amz::chess_game input(std::istream& in)
	{
		using namespace amz;
		using namespace std;

		chess_game cg;
		cg.set_color(chess_color::black);
		cg.init();

		int turn = 0;
		in >> turn;

		int from_i{}, from_j{}, to_i{}, to_j{}, obs_i{}, obs_j{};

		for (int i = 0; i < turn; ++i)
		{
			// 首先是对手行动
			in >> from_i >> from_j >> to_i >> to_j >> obs_i >> obs_j;

			if (from_i == -1)
				cg.set_color(chess_color::black);
			else
				cg.make_move(from_i, from_j, to_i, to_j, obs_i, obs_j);

			// 然后是自己当时的行动
			// 对手行动总比自己行动多一次
			if (i < turn - 1)
			{
				in >> from_i >> from_j >> to_i >> to_j >> obs_i >> obs_j;
				cg.make_move(from_i, from_j, to_i, to_j, obs_i, obs_j);
			}
		}

		cg.set_turn(turn);
		return cg;
	}

	void output(std::ostream& out, amz::movement mm)
	{
		auto [from, to, obs] = mm;
		out << static_cast<int>(amz::get_ij(from).first) << ' '
			<< static_cast<int>(amz::get_ij(from).second) << ' '
			<< static_cast<int>(amz::get_ij(to).first) << ' '
			<< static_cast<int>(amz::get_ij(to).second) << ' '
			<< static_cast<int>(amz::get_ij(obs).first) << ' '
			<< static_cast<int>(amz::get_ij(obs).second)
			<< std::endl;
	}
};