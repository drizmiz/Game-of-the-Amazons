
#pragma once

#include "std.h"

namespace amz
{
#pragma region using declaration
	using uint64 = uint64_t;
	using bit_table = uint64;
	using byte = uint8_t;
	using index_t = byte;
	using off_i_t = byte;
	using len_t = byte;
	using eval_t = int;
#pragma endregion
 // the board looks like this:
 // 8	A8(index=0)	...	H8(index=7)
 // ↓		:				:
 // 1	A1(index=56)...	H1(index=63)
 //		A B C	->	F G H

#pragma region DATA
	constexpr len_t length_main_diag[64] = {
	 8, 7, 6, 5, 4, 3, 2, 1,
	 7, 8, 7, 6, 5, 4, 3, 2,
	 6, 7, 8, 7, 6, 5, 4, 3,
	 5, 6, 7, 8, 7, 6, 5, 4,
	 4, 5, 6, 7, 8, 7, 6, 5,
	 3, 4, 5, 6, 7, 8, 7, 6,
	 2, 3, 4, 5, 6, 7, 8, 7,
	 1, 2, 3, 4, 5, 6, 7, 8
	};
	constexpr len_t length_counter_diag[64] = {
	  1, 2, 3, 4, 5, 6, 7, 8,
	  2, 3, 4, 5, 6, 7, 8, 7,
	  3, 4, 5, 6, 7, 8, 7, 6,
	  4, 5, 6, 7, 8, 7, 6, 5,
	  5, 6, 7, 8, 7, 6, 5, 4,
	  6, 7, 8, 7, 6, 5, 4, 3,
	  7, 8, 7, 6, 5, 4, 3, 2,
	  8, 7, 6, 5, 4, 3, 2, 1
	};
	constexpr off_i_t rotate_45degree_left_map[64] = {
	 28, 21, 15, 10, 6, 3, 1, 0,
	 36, 29, 22, 16, 11, 7, 4, 2,
	 43, 37, 30, 23, 17, 12, 8, 5,
	 49, 44, 38, 31, 24, 18, 13, 9,
	 54, 50, 45, 39, 32, 25, 19, 14,
	 58, 55, 51, 46, 40, 33, 26, 20,
	 61, 59, 56, 52, 47, 41, 34, 27,
	 63, 62, 60, 57, 53, 48, 42, 35
	};
	constexpr off_i_t rotate_45degree_right_map[64] = {
	 0, 1, 3, 6, 10, 15, 21, 28,
	 2, 4, 7, 11, 16, 22, 29, 36,
	 5, 8, 12, 17, 23, 30, 37, 43,
	 9, 13, 18, 24, 31, 38, 44, 49,
	 14, 19, 25, 32, 39, 45, 50, 54,
	 20, 26, 33, 40, 46, 51, 55, 58,
	 27, 34, 41, 47, 52, 56, 59, 61,
	 35, 42, 48, 53, 57, 60, 62, 63
	};
	constexpr off_i_t rotate_90degree_right_map[64] = {
	 7, 7 + 8, 7 + 8 * 2, 7 + 8 * 3, 7 + 8 * 4, 7 + 8 * 5, 7 + 8 * 6, 7 + 8 * 7,
	 6, 6 + 8, 6 + 8 * 2, 6 + 8 * 3, 6 + 8 * 4, 6 + 8 * 5, 6 + 8 * 6, 6 + 8 * 7,
	 5, 5 + 8, 5 + 8 * 2, 5 + 8 * 3, 5 + 8 * 4, 5 + 8 * 5, 5 + 8 * 6, 5 + 8 * 7,
	 4, 4 + 8, 4 + 8 * 2, 4 + 8 * 3, 4 + 8 * 4, 4 + 8 * 5, 4 + 8 * 6, 4 + 8 * 7,
	 3, 3 + 8, 3 + 8 * 2, 3 + 8 * 3, 3 + 8 * 4, 3 + 8 * 5, 3 + 8 * 6, 3 + 8 * 7,
	 2, 2 + 8, 2 + 8 * 2, 2 + 8 * 3, 2 + 8 * 4, 2 + 8 * 5, 2 + 8 * 6, 2 + 8 * 7,
	 1, 1 + 8, 1 + 8 * 2, 1 + 8 * 3, 1 + 8 * 4, 1 + 8 * 5, 1 + 8 * 6, 1 + 8 * 7,
	 0, 0 + 8, 0 + 8 * 2, 0 + 8 * 3, 0 + 8 * 4, 0 + 8 * 5, 0 + 8 * 6, 0 + 8 * 7,
	};
	constexpr off_i_t main_rshift_map[64] = {
	 28, 21, 15, 10, 6, 3, 1, 0,
	 36, 28, 21, 15, 10, 6, 3, 1,
	 43, 36, 28, 21, 15, 10, 6, 3,
	 49, 43, 36, 28, 21, 15, 10, 6,
	 54, 49, 43, 36, 28, 21, 15, 10,
	 58, 54, 49, 43, 36, 28, 21, 15,
	 61, 58, 54, 49, 43, 36, 28, 21,
	 63, 61, 58, 54, 49, 43, 36, 28
	};
	constexpr off_i_t counter_rshift_map[64] = {
	 0, 1, 3, 6, 10, 15, 21, 28,
	 1, 3, 6, 10, 15, 21, 28, 36,
	 3, 6, 10, 15, 21, 28, 36, 43,
	 6, 10, 15, 21, 28, 36, 43, 49,
	 10, 15, 21, 28, 36, 43, 49, 54,
	 15, 21, 28, 36, 43, 49, 54, 58,
	 21, 28, 36, 43, 49, 54, 58, 61,
	 28, 36, 43, 49, 54, 58, 61, 63
	};
#pragma endregion

#pragma region 非常基础的函数

	constexpr off_i_t get_i(len_t row, len_t col)
	{
		return (row << 3) + col;
	}
	constexpr std::pair<len_t, len_t> get_ij(off_i_t i)
	{
		return std::make_pair(i >> 3, i & 0x7);
	}

	bit_table _mask[64];
	constexpr void _Initialize_mask() // 现在它没有任何调用
	{
		for (int i = 0; i < 64; i++)
			_mask[i] = static_cast<uint64>(1) << i;
	}
	inline constexpr bit_table mask(off_i_t i)
	{
		// 这里尝试两种方式
		//if constexpr (false)
		 //return _mask[index];
		//else
		return 1ULL << i; // 事实证明这种更为高效
	}
	inline constexpr bit_table mask(len_t i, len_t j)
	{
		return mask(get_i(i, j));
	}

#pragma endregion

#pragma region 初始化器
	bit_table row_moves[64][256];
	void _Initialize_row() noexcept
	{
		for (int i = 0; i < 64; ++i)
		{
			for (int row_status = 0; row_status < 256; ++row_status)
			{
				const int offset = i & 0x7; // offset是i在本行的位置 offset = i % 8
				// for example
				// when i = 51, the row_status is [7] 1000 1000 [0]
				// then offset is 3, 1 << offset == 0000 1000
				if (!(row_status & (1 << offset))) // 1 左移 offset 位
					continue;
				int ans = 0;
				int i_mask = 1 << (offset + 1);
				for (; !((i_mask & row_status) || i_mask & 0x100); i_mask <<= 1)
					ans |= i_mask;
				if (offset)
				{
					i_mask = 1 << (offset - 1);
					for (; !((i_mask & row_status) || !i_mask); i_mask >>= 1)
						ans |= i_mask;
				}
				row_moves[i][row_status] = static_cast<uint64>(ans) << ((i >> 3) << 3);
			}
		}
	}
	inline constexpr byte _Byte_rev(byte bt)
	{
		byte btbr = bt;
		btbr = ((btbr >> 1) & 0x55) | ((btbr & 0x55) << 1);
		btbr = ((btbr >> 2) & 0x33) | ((btbr & 0x33) << 2);
		btbr = (btbr >> 4) | (btbr << 4);
		return btbr;
	}
	bit_table col_moves[64][256];
	void _Initialize_col() noexcept
	{
		for (int i = 0; i < 64; ++i)
		{
			for (int col_status = 0; col_status < 256; ++col_status)
			{
				// i = row * 8 + col
				// so i & 0x7 = col!
				// to get row ->
				const int offset = i >> 3; // offset是i在本列的位置 offset = i % 8
				// for example
				// when i = 60, the col_status is [7] 1000 0000 [0]
				// then offset is 7, 1 << offset == 1000 0000
				if (!(col_status & (1 << offset))) // 1 左移 offset 位
					continue;
				int ans = 0;
				int i_mask = 1 << (offset + 1);
				for (; !((i_mask & col_status) || i_mask & 0x100); i_mask <<= 1)
					ans |= i_mask;
				if (offset)
				{
					i_mask = 1 << (offset - 1);
					for (; !((i_mask & col_status) || !i_mask); i_mask >>= 1)
						ans |= i_mask;
				}
				// to get pure row
				// e.g. if i = 5
				// then i >> 3 << 3 = 0
				// if i = 13
				// then i >> 3 << 3 = 8
				// now we want pure col
				// from [i%8] to [56+i%8]
				// bit_table[8k+i%8] = ans[k]
				for (int k = 0; k < 8; ++k)
				{
					int ans_k = (ans >> k) & 1;

					col_moves[i][_Byte_rev(col_status)]
						|= (
							static_cast<uint64>(ans_k) <<
							((k << 3) + (i & 0x7))
							);
				}
			}
		}
	}
	bit_table diag_main_moves[64][256];
	bit_table diag_counter_moves[64][256];
	// 这是没有用的部分
		void _Initialize_diag() noexcept
	{
		// FOR main direction
		for (int i = 0; i < 64; ++i)
		{
			const int diag_length = length_main_diag[i];
			const int diag_status_cnt = 1 << diag_length;

			for (int diag_main_status = 0;
				diag_main_status < diag_status_cnt;
				++diag_main_status)
			{
				// i = row * 8 + col
				// so i & 0x7 = col!
				// to get diag ->
				const int row = i >> 3, col = i & 0x7;
				const int offset = std::min(row, col); // offset是i在本对角线的位置

				if (!(diag_main_status & (1 << offset))) // 1 左移 offset 位
					continue;

				int ans = 0;
				int i_mask = 1 << (offset + 1);
				for (; !((i_mask & diag_main_status) || i_mask & diag_status_cnt); i_mask <<= 1)
					ans |= i_mask;
				if (offset)
				{
					i_mask = 1 << (offset - 1);
					for (; !((i_mask & diag_main_status) || !i_mask); i_mask >>= 1)
						ans |= i_mask;
				}

				int low_i = 0;
				if (row <= col)
					low_i = i % 9;
				else
					for (low_i = i; low_i & 0x7; low_i -= 9);
				for (int k = 0; k < diag_length; ++k)
				{
					bool ans_k = (ans >> k) & 1;
					diag_main_moves[i][diag_main_status] |= (
						static_cast<uint64>(ans_k) <<
						(low_i + k * 9)
						);
				}
			}
		}
		// FOR counter direction
		for (int i = 0; i < 64; ++i)
		{
			const int diag_length = length_counter_diag[i];
			const int diag_status_cnt = 1 << diag_length;

			for (int diag_counter_status = 0;
				diag_counter_status < diag_status_cnt;
				++diag_counter_status)
			{
				// i = row * 8 + col
				// so i & 0x7 = col!
				// to get diag ->
				const int row = i >> 3, col = i & 0x7;
				const int offset = std::min(row, 7 - col); // offset是i在本对角线的位置

				if (!(diag_counter_status & (1 << offset))) // 1 左移 offset 位
					continue;

				int ans = 0;
				int i_mask = 1 << (offset + 1);
				for (; !((i_mask & diag_counter_status) || i_mask & diag_status_cnt); i_mask <<= 1)
					ans |= i_mask;
				if (offset)
				{
					i_mask = 1 << (offset - 1);
					for (; !((i_mask & diag_counter_status) || !i_mask); i_mask >>= 1)
						ans |= i_mask;
				}

				int low_i = 0;
				if (row < 7 - col)
					low_i = i % 7;
				else
					for (low_i = i; (low_i & 0x7) != 0x7; low_i -= 7);
				for (int k = 0; k < diag_length; ++k)
				{
					bool ans_k = (ans >> k) & 1;
					diag_counter_moves[i][diag_counter_status] |= (
						static_cast<uint64>(ans_k) <<
						(low_i + k * 7)
						);
				}
			}
		}
	}

	void initialize() noexcept
	{
		//_Initialize_mask();
		_Initialize_row();
		_Initialize_col();
		_Initialize_diag();
	}
#pragma endregion

#pragma region get_index

	///<summary>
	/// 获取bit_table行号row所对应行的索引
	///</summary>
	constexpr index_t _Get_row_index(bit_table bt, len_t row) // row from 1 to 8
	{
		return (bt >> (row << 3)) & 0xFF;
	}
	constexpr index_t _Get_row_index_by_i(bit_table bt, off_i_t i)
	{
		return _Get_row_index(bt, i >> 3); // i / 8
	}
	constexpr bit_table _Rotate_90degree_right(bit_table in_bt)
	{
		bit_table bt_rotated = 0;
		for (int i = 0; i < 64; ++i)
			if ((in_bt >> i) & 1)
				bt_rotated |= mask(rotate_90degree_right_map[i]);
		return bt_rotated;
	}
	constexpr bit_table _Rotate_90degree_right_for_just_one_col(bit_table in_bt, int col)
	{
		bit_table bt_rotated = 0;
		for (int i = col; i < 64; i += 8)
			if ((in_bt >> i) & 1)
				bt_rotated |= mask(rotate_90degree_right_map[i]);
		return bt_rotated;
	}
	constexpr index_t _Get_col_index(bit_table bt, int col)
	{
		return _Get_row_index(
			_Rotate_90degree_right_for_just_one_col(bt, col),
			col
		);
	}
	constexpr index_t _Get_col_index_by_i(bit_table bt, off_i_t i)
	{
		return _Get_col_index(bt, i & 0x7); // i % 8
	}
	constexpr index_t _Get_col_index_by_i_on_rotated(bit_table bt_rotated, off_i_t i)
	{
		return _Get_row_index(bt_rotated, i & 0x7);
	}
	constexpr bit_table _Rotate_45degree_left(bit_table in_bt)
	{
		bit_table bt_rotated = 0;
		for (int i = 0; i < 64; ++i)
			if ((in_bt >> i) & 1)
				bt_rotated |= mask(rotate_45degree_left_map[i]);
		return bt_rotated;
	}
#pragma warning(push)
#pragma warning(disable:4244)
	constexpr index_t _Get_main_diag_index_on_rotated(bit_table bt_rotated, off_i_t i)
	{
		const int mask = (1 << length_main_diag[i]) - 1;
		return (bt_rotated >> (main_rshift_map[i]))
			& mask;
	}
	constexpr index_t _Get_main_diag_index(bit_table bt, off_i_t i)
	{
		const bit_table bt_rotated = _Rotate_45degree_left(bt);
		return _Get_main_diag_index_on_rotated(bt_rotated, i);
	}
	constexpr bit_table _Rotate_45degree_right(bit_table in_bt)
	{
		bit_table bt_rotated = 0;
		for (int i = 0; i < 64; ++i)
			if ((in_bt >> i) & 1)
				bt_rotated |= mask(rotate_45degree_right_map[i]);
		return bt_rotated;
	}
	// act as low bit's row is low
	// i.e. [0][7] the low bit; [7][0] the high bit
	constexpr index_t _Get_counter_diag_index_on_rotated(bit_table bt_rotated, off_i_t i)
	{
		const int mask = (1 << length_counter_diag[i]) - 1;
		return (bt_rotated >> (counter_rshift_map[i]))
			& mask;
	}
	constexpr index_t _Get_counter_diag_index(bit_table bt, off_i_t i)
	{
		const bit_table bt_rotated = _Rotate_45degree_right(bt);
		return _Get_counter_diag_index_on_rotated(bt_rotated, i);
	}
#pragma warning(pop)

	enum class direction
	{
		row,
		col,
		main_diag,
		counter_diag
	};
	template<direction dxy>
	constexpr index_t get_index(bit_table bt, off_i_t i)
	{
		if constexpr (dxy == direction::row)
			return _Get_row_index_by_i(bt, i);
		else if constexpr (dxy == direction::col)
			return _Get_col_index_by_i(bt, i);
		else if constexpr (dxy == direction::main_diag)
			return _Get_main_diag_index(bt, i);
		else if constexpr (dxy == direction::counter_diag)
			return _Get_counter_diag_index(bt, i);
		else
			return 0;
	}
#pragma endregion

#pragma region 移动/落子
	constexpr void place_bit(bit_table& table, off_i_t i)
	{
		table |= mask(i);
	}
	constexpr void undo_place_bit(bit_table& table, off_i_t i) // assume there is a 1
	{
		table ^= mask(i);
	}
	constexpr bit_table place_copy(const bit_table table, off_i_t i)
	{
		return table | mask(i);
	}
	constexpr void move_bit(bit_table& table, off_i_t fromI, off_i_t toI)
	{
		// bit_table this_mask = mask(fromIndex) | mask(toIndex);
		// table ^= this_mask;
		table ^= mask(fromI);
		table |= mask(toI);
	}
	constexpr bit_table move_copy(const bit_table table, off_i_t fromI, off_i_t toI)
	{
		return (table ^ mask(fromI)) | mask(toI);
	}
	template<int table_cnt>
	constexpr void move_bit_on_many_tables(bit_table tables[table_cnt], off_i_t fromI, off_i_t toI)
	{
		const bit_table this_mask = mask(fromI) | mask(toI);
		for (int i = 0; i < table_cnt; ++i)
			tables[i] ^= this_mask;
	}
	constexpr bool check_bit(bit_table table, off_i_t i)
	{
		return (table >> i) & 1;
	}
	constexpr off_i_t _Select_highest(bit_table in)
	{
		//for (off_i_t i = 0; i < 64; ++i)
		 //if ((in >> i) & 1)
		  //return i;
		off_i_t n = 62;

		if ((in >> 32) == 0) { n -= 32; in <<= 32; }
		if ((in >> 48) == 0) { n -= 16; in <<= 16; }
		if ((in >> 56) == 0) { n -= 8; in <<= 8; }
		if ((in >> 60) == 0) { n -= 4; in <<= 4; }
		if ((in >> 62) == 0) { n -= 2; in <<= 2; }
		n += (in >> 63);

		if (in == 0) return 64;

		return n; // 这里找到的是最高位的位置
	}
	constexpr bit_table _Select_highest_mask(bit_table in)
	{
		in |= (in >> 1);
		in |= (in >> 2);
		in |= (in >> 4);
		in |= (in >> 8);
		in |= (in >> 16);
		in |= (in >> 32);
		return in ^ (in >> 1);
	}
#pragma endregion

#pragma region 着法生成
	bit_table get_all_possible_moves(bit_table chessboard, off_i_t i) noexcept
	{
		const bit_table row = row_moves[i][_Get_row_index_by_i(chessboard, i)];
		const bit_table col = col_moves[i][_Get_col_index_by_i(chessboard, i)];
		const bit_table main_diag = diag_main_moves[i][_Get_main_diag_index(chessboard, i)];
		const bit_table counter_diag = diag_counter_moves[i][_Get_counter_diag_index(chessboard, i)];
		return row | col | main_diag | counter_diag;
	}
	constexpr int max_moves = 32;
	std::vector<off_i_t> _Transform_to_i(bit_table moves)
	{
		std::vector<off_i_t> ret;
		ret.reserve(max_moves);
		for (;;)
		{
			const off_i_t tmp = _Select_highest(moves);
			if (tmp == 64)
				break;
			ret.push_back(tmp);
			undo_place_bit(moves, tmp);
		}
		return ret;
	}
	std::array<off_i_t, 4> _Transform_4_to_i(bit_table moves)
	{
		std::array<off_i_t, 4> ret;
		byte i = 0;
		for (;;)
		{
			const off_i_t tmp = _Select_highest(moves);
			ret[i++] = tmp;
			if (i == 4)
				break;
			undo_place_bit(moves, tmp);
		}
		return ret;
	}
	std::array<std::pair<len_t, len_t>, 4> _Transform_4_to_ij(bit_table moves)
	{
		std::array<std::pair<len_t, len_t>, 4> ret;
		byte i = 0;
		for (;;)
		{
			const off_i_t tmp = _Select_highest(moves);
			ret[i++] = get_ij(tmp);
			if (i == 4)
				break;
			undo_place_bit(moves, tmp);
		}
		return ret;
	}
	std::vector<off_i_t> get_all_possible_i(bit_table chessboard, off_i_t i)
	{
		return _Transform_to_i(get_all_possible_moves(chessboard, i));
	}
#pragma endregion

#pragma region 一些结构

	class chess_board
	{
	private:
		bit_table _board;
		bit_table _board_90_right;
		bit_table _board_45_left;
		bit_table _board_45_right;
	public:
		chess_board(bit_table merged_board) noexcept :
			_board(merged_board),
			_board_45_left(_Rotate_45degree_left(merged_board)),
			_board_45_right(_Rotate_45degree_right(merged_board)),
			_board_90_right(_Rotate_90degree_right(merged_board))
		{}
		explicit chess_board() noexcept : chess_board(0)
		{}

		void place_piece(off_i_t i) noexcept
		{
			place_bit(_board, i);
			place_bit(_board_45_left, rotate_45degree_left_map[i]);
			place_bit(_board_45_right, rotate_45degree_right_map[i]);
			place_bit(_board_90_right, rotate_90degree_right_map[i]);
		}
		void unplace_piece(off_i_t i) noexcept
		{
			undo_place_bit(_board, i);
			undo_place_bit(_board_45_left, rotate_45degree_left_map[i]);
			undo_place_bit(_board_45_right, rotate_45degree_right_map[i]);
			undo_place_bit(_board_90_right, rotate_90degree_right_map[i]);
		}
		void move_piece(off_i_t from_i, off_i_t to_i) noexcept
		{
			// bit_table this_mask = mask(fromIndex) | mask(toIndex);
			// table ^= this_mask;
			move_bit(_board, from_i, to_i);
			move_bit(_board_45_left,
				rotate_45degree_left_map[from_i],
				rotate_45degree_left_map[to_i]);
			move_bit(_board_45_right,
				rotate_45degree_right_map[from_i],
				rotate_45degree_right_map[to_i]);
			move_bit(_board_90_right,
				rotate_90degree_right_map[from_i],
				rotate_90degree_right_map[to_i]);
		}

		bit_table get_all_possible_moves(off_i_t i) const noexcept
		{
			const bit_table row = row_moves[i][_Get_row_index_by_i(_board, i)];
			const bit_table col = col_moves[i]
				[_Get_col_index_by_i_on_rotated(_board_90_right, i)];
			const bit_table main_diag = diag_main_moves[i]
				[_Get_main_diag_index_on_rotated(_board_45_left, i)];
			const bit_table counter_diag = diag_counter_moves[i]
				[_Get_counter_diag_index_on_rotated(_board_45_right, i)];
			return row | col | main_diag | counter_diag;
		}

		const bit_table merged_board() const noexcept
		{
			return _board;
		}
	};

	enum class chess_color : int
	{
		black = -1,
		white = 1
	};
	inline constexpr chess_color _Color_rev(chess_color cc)
	{
		return static_cast<chess_color>
			(-static_cast<int>(cc));
	}

	constexpr uint64 u64_big_prime = 1099511628211ULL;
	constexpr uint64 initial_big_num = 14695981039346656037ULL;
	inline void _Hash_append_u64(uint64& val, const uint64 app) noexcept
	{
		val ^= app;
		val *= u64_big_prime;
	}

	class chess_status
	{
	private:
		bit_table black;
		bit_table white;
		chess_board all; // all includes black, white and obstacles
		inline void init() noexcept
		{
			// black: (0, 2), (2, 0), (5, 0), (7, 2)
			// white: (0, 5), (2, 7), (5, 7), (7, 5)
			black = mask(0, 2) |
				mask(2, 0) |
				mask(5, 0) |
				mask(7, 2);
			white = mask(0, 5) |
				mask(2, 7) |
				mask(5, 7) |
				mask(7, 5);
			all = black | white;
		}
	public:
		chess_status() noexcept :black(), white(), all()
		{}

		friend class chess_game;
		friend void _Debug_paint(const chess_status&);
		friend int _Evaluate(const chess_status& cs, chess_color color, int turn_cnt);
		friend int _Debug_evaluate(const amz::chess_status& cs, chess_color color, int turn_cnt, std::ostream&);

		uint64 _Hash() const noexcept
		{
			uint64 val = initial_big_num;
			_Hash_append_u64(val, this->black);
			_Hash_append_u64(val, this->white);
			_Hash_append_u64(val, this->all.merged_board());
			return val;
		}

		template<chess_color color>
		inline void move_piece(off_i_t from, off_i_t to)
		{
			if constexpr (color == chess_color::black)
				move_bit(black, from, to);
			else// if constexpr (color == chess_color::white)
				move_bit(white, from, to);
			all.move_piece(from, to);
		}
		inline void move_piece(off_i_t from, off_i_t to, chess_color color) noexcept
		{
			if (color == chess_color::black)
				move_bit(black, from, to);
			else// if (color == chess_color::white)
				move_bit(white, from, to);
			all.move_piece(from, to);
		}
		inline void place_obs(off_i_t i) noexcept
		{
			all.place_piece(i);
		}
		inline void unplace_obs(off_i_t i) noexcept
		{
			all.unplace_piece(i);
		}

		bit_table get_black() const { return black; }
		bit_table get_white() const { return white; }
		bit_table get_all() const { return all.merged_board(); }
	};

#pragma endregion

#pragma region DEBUG用函数
	void _Debug_paint(bit_table bt)
	{
		std::cout << "BIT_TABLE DEBUG_PAINT:" << std::endl;
		for (int i = 63; i >= 0; --i)
		{
			std::cout << ((bt >> i) & 1) << " ";
			if (!(i & 0x7))
				std::cout << std::endl;
		}
	}
	constexpr bit_table _Up_down_flip(bit_table bt)
	{
		bit_table ret = 0;
		for (int i = 0; i < 8; ++i)
			for (int j = 0; j < 8; ++j)
				if (check_bit(bt, get_i(i, j)))
					ret |= mask(7 - i, j);
		return ret;
	}
	void _Debug_paint(const chess_status& cs)
	{
		std::cout << "CHESS_STATUS DEBUG_PAINT:" << std::endl;
		//std::cout << "BLACK:" << std::endl;
		for (int i = 63; i >= 0; --i)
		{
			if ((cs.black >> i) & 1)
				std::cout << 'B';
			else if ((cs.white >> i) & 1)
				std::cout << 'W';
			else if ((cs.all.merged_board() >> i) & 1)
				std::cout << 'A';
			else
				std::cout << '0';
			std::cout << " ";
			if (!(i & 0x7))
				std::cout << std::endl;
		}
		//std::cout << "WHITE:" << std::endl;
		//_Debug_paint(cs.white);
		//std::cout << "ALL:" << std::endl;
		//_Debug_paint(/*_Up_down_flip(_Rotate_90degree_right(*/cs.all.board());
	}
#pragma endregion
}
