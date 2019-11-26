
#pragma once

#include "csbd.h"

namespace amz
{
	struct movement
	{
		off_i_t _from;
		off_i_t _to;
		off_i_t _obs;

		bool operator!=(const movement& mm2) const
		{
			return !(_from == mm2._from && _to == mm2._to && _obs == mm2._obs);
		}

		const std::tuple<len_t, len_t> from() const {
			return get_ij(_from);
		}
		const std::tuple<len_t, len_t> to() const {
			return get_ij(_to);
		}
		const std::tuple<len_t, len_t> arrow() const {
			return get_ij(_obs);
		}

		off_i_t& fromi() {
			return _from;
		}
		off_i_t& toi() {
			return _to;
		}
		off_i_t& obsi() {
			return _obs;
		}
	};

	constexpr movement dft_movement = { 64,64,64 };

	bool _Is_dft_move(movement mm)
	{
		return mm.fromi() == 64;
	}

	constexpr eval_t inf = static_cast<eval_t>(1e7);
	constexpr eval_t unknown = static_cast<eval_t>(1e8);

	constexpr int hashtable_size = 8 * 1024 * 1024;
	constexpr int history_table_size = 64 * 64 * 64;
	enum class node_f :unsigned char
	{
		empty = 0,
		pv,
		alpha,
		beta
	};
	class record_table
	{
	private:
		struct hash_tag
		{
			uint64 key;
			short depth;
			node_f flag; // dft: empty
			eval_t value;
			movement best_move;
		};
		hash_tag hash_table[hashtable_size];
		int history_table[history_table_size];
	public:
		void clear()
		{
			memset(hash_table, 0, hashtable_size * sizeof(hash_tag));
			memset(history_table, 0, history_table_size * sizeof(int));
		}
		void record_hash(const chess_status& cs, const movement& cur_move,
			int depth, eval_t val, node_f hashf)
		{
			uint64 key = cs._Hash();
			hash_tag& hashe = hash_table[key % hashtable_size];

			if (hashe.flag != node_f::empty && hashe.depth > depth)
				return;

			hashe.key = key;
			hashe.best_move = cur_move;
			hashe.value = val;
			hashe.flag = hashf;
			hashe.depth = depth;
		}
		std::pair<eval_t, movement> probe_hash(const chess_status& cs,
			int depth, eval_t alpha, eval_t beta)
		{
			using std::make_pair;
			uint64 key = cs._Hash();
			hash_tag& hashe = hash_table[key % hashtable_size];

			if (hashe.key == key)
			{
				movement retmm = hashe.best_move;
				if (hashe.depth >= depth)
				{
					if ((hashe.flag == node_f::alpha) && (hashe.value <= alpha))
						return make_pair(alpha, retmm);
					if ((hashe.flag == node_f::beta) && (hashe.value >= beta))
						return make_pair(beta, retmm);
					if (hashe.flag == node_f::pv)
						return make_pair(hashe.value, retmm);
				}
				// RememberBestMove();
			}

			return make_pair(unknown, dft_movement);
		}
		void record_history(movement mm, int depth)
		{
			// 64 = 2^6
			int index = (mm._from << 12) | (mm._to << 6) | mm._obs;
			history_table[index] += depth * depth;
		}
		int probe_history(movement mm)
		{
			const int index = (mm._from << 12) | (mm._to << 6) | mm._obs;
			return history_table[index];
		}
	};
	record_table _rt;
	class chess_game
	{
	private:
		chess_status _cs;
		chess_color my_color;
		record_table& rt = _rt;
		class mm_sort;
		decltype(std::chrono::steady_clock::now()) starttime;
		long long timespan;
		long long deadspan;
		int turn_cnt;
		bool _add_turn;

		bool _Permit_null();
		eval_t _Alphabeta(int depth, eval_t alpha, eval_t beta, bool _no_null);
		eval_t _Alphabeta(int depth, eval_t beta);
		//movement _Root_search(int depth);
		eval_t MTD_f(int depth, eval_t test);
		movement _Root_search(int depth, eval_t alpha, eval_t beta, eval_t& test);
		std::pair<eval_t, movement> _Root_search(int depth, eval_t alpha, eval_t beta);
		std::pair<eval_t, movement> _Root_search(int depth);
		movement _Search_till_timeout();
		
		bit_table _Mine() noexcept
		{
			return my_color == chess_color::black ? _cs.black : _cs.white;
		}
		std::vector<movement> _Get_all_possible_moves()
		{
			std::vector<movement> ret;
			ret.reserve(4 * max_moves * max_moves);
			movement cur = dft_movement;
			for (const off_i_t chess : _Transform_to_i(_Mine()))
			{
				cur.fromi() = chess;
				for (const off_i_t dest : get_all_possible_i(_cs.all.board(), chess))
				{
					cur.toi() = dest;
					_cs.move_piece(chess, dest, my_color);
					for (const off_i_t obs : get_all_possible_i(_cs.all.board(), dest))
					{
						cur.obsi() = obs;
						ret.push_back(cur);
					}
					_cs.move_piece(dest, chess, my_color);
				}
			}
			return ret;
		}
	public:
		chess_game() noexcept :
			_cs(), my_color(chess_color::white), deadspan(950), turn_cnt(0),_add_turn(false)
		{}
		inline chess_color get_color() noexcept { return my_color; }
		inline void set_color(chess_color cc) noexcept { my_color = cc; }
		inline chess_status get_status() noexcept { return _cs; }
		inline void set_status(chess_status cs) noexcept { _cs = cs; }
		inline int get_turn()noexcept { return turn_cnt; }
		inline void set_turn(int turn)noexcept { turn_cnt = turn; }
		inline void make_move(off_i_t from, off_i_t to, off_i_t obs)noexcept
		{
			_cs.move_piece(from, to, my_color);
			_cs.place_obs(obs);
			my_color = _Color_rev(my_color);
			if (_add_turn)++turn_cnt;
			_add_turn = !_add_turn;
		}
		inline void make_move(off_i_t from_i, off_i_t from_j, off_i_t to_i, off_i_t to_j,
			off_i_t obs_i, off_i_t obs_j)noexcept
		{
			const off_i_t from = get_i(from_i, from_j);
			const off_i_t to = get_i(to_i, to_j);
			const off_i_t obs = get_i(obs_i, obs_j);
			make_move(from, to, obs);
		}
		inline void make_move(movement mm) noexcept
		{
			const auto& [from, to, obs] = mm;
			make_move(from, to, obs);
		}
		inline void unmake_move(off_i_t from, off_i_t to, off_i_t obs)noexcept
		{
			_add_turn = !_add_turn;
			if (_add_turn)--turn_cnt;
			my_color = _Color_rev(my_color);
			_cs.unplace_obs(obs);
			_cs.move_piece(to, from, my_color);
		}
		inline void unmake_move(movement mm) noexcept
		{
			const auto& [from, to, obs] = mm;
			unmake_move(from, to, obs);
		}
		inline void null_move()
		{
			my_color = _Color_rev(my_color);
			if (_add_turn)++turn_cnt;
			_add_turn = !_add_turn;
		}
		inline void undo_null_move()
		{
			_add_turn = !_add_turn;
			if (_add_turn)--turn_cnt;
			my_color = _Color_rev(my_color);
		}
		inline void init() noexcept { _cs.init(); }

		movement generate_next_move()
		{
			starttime = std::chrono::steady_clock::now();

			timespan = get_turn() <= 1 ? 400 : get_turn() <= 2 ? 500 : get_turn() <= 3 ? 650 : get_turn() <= 5 ? 700 : get_turn() <= 6 ? 750 : 800;

			return _Search_till_timeout();
		}
	};

}