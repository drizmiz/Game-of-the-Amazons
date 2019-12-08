
#pragma once

#include "game.h"

namespace ev	// evaluation adjusted
{
	using namespace amz;

	class player 
	{
	public:
		using single_player = STD array<STD pair<len_t, len_t>, 4>;
		player(bool is_black, const chess_status& cs) :_is_black(is_black), _cs(cs) {}
		const single_player self() const {
			return _is_black ? _Transform_4_to_ij(_cs.get_black()) : _Transform_4_to_ij(_cs.get_white());
		}
		const single_player opponent() const {
			return (!_is_black) ? _Transform_4_to_ij(_cs.get_black()) : _Transform_4_to_ij(_cs.get_white());
		}
		bool id() const { return !_is_black; }
	private:
		bool _is_black;
		const chess_status& _cs;
	};
	class teil
	{
	private:
		bool _is_empty;
	public:
		teil(bool i) :_is_empty(i) {}
		inline bool is_empty() const { return _is_empty; }
	};
	class merged_board
	{
	public:
		merged_board(const chess_status& cs) :_cs(cs) {}
		const teil operator()(off_i_t i) const {
			return is_empty(i);
		}
		const teil operator()(len_t x, len_t y) const {
			return is_empty(x, y);
		}
		const bool is_empty(off_i_t i)const {
			return !(_cs.get_all() & mask(i));
		}
		const bool is_empty(len_t x, len_t y)const {
			return !(_cs.get_all() & mask(x, y));
		}
		amz::bit_table get_table()const {
			return _cs.get_all();
		}
		void _Debug_paint() {
			amz::_Debug_paint(_cs);
		}
	private:
		const chess_status& _cs;
	};
}