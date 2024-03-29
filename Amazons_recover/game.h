
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
		bool operator==(const movement& mm2) const
		{
			return (_from == mm2._from && _to == mm2._to && _obs == mm2._obs);
		}

		const STD pair<len_t, len_t> from() const {
			return get_ij(_from);
		}
		const STD pair<len_t, len_t> to() const {
			return get_ij(_to);
		}
		const STD pair<len_t, len_t> arrow() const {
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
		movement killer_moves[64][2];
	public:
		void clear()
		{
			memset(hash_table, 0, hashtable_size * sizeof(hash_tag));
			memset(history_table, 0, history_table_size * sizeof(int));
			for (int i = 0; i < 64; ++i)
			{
				killer_moves[i][0] = dft_movement;
				killer_moves[i][1] = dft_movement;
			}
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
		STD pair<eval_t, movement> probe_hash(const chess_status& cs,
			int depth, eval_t alpha, eval_t beta)
		{
			using STD make_pair;
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
		int distance;

		bool _Permit_null();
		eval_t _Alphabeta(int depth, eval_t alpha, eval_t beta, bool _no_null);
		// eval_t MTD_f(int depth, eval_t test);
		STD pair<eval_t, movement> _Root_search(int depth, movement lastmove);
		movement _Search_till_timeout();

		void _Set_best_move(movement mm_best, int depth)
		{
			rt.record_history(mm_best, depth);
			movement* lp_killers = rt.killer_moves[distance];
			if (lp_killers[0] != mm_best)
			{
				lp_killers[1] = lp_killers[0];
				lp_killers[0] = mm_best;
			}
		}
		
		bit_table _Mine() noexcept {
			return my_color == chess_color::black ? _cs.black : _cs.white;
		}
		STD vector<movement> _Get_all_possible_moves()
		{
			STD vector<movement> ret;
			ret.reserve(4 * max_moves * max_moves);
			movement cur = dft_movement;

			bit_table mine = _Mine();

			for (;;)
			{
				const off_i_t source = _Select_highest(mine);
				if (source == 64)
					break;
				cur.fromi() = source;

				bit_table dests = _cs.all.get_all_possible_moves(source);
				for (;;)
				{
					const off_i_t dest = _Select_highest(dests);
					if (dest == 64)
						break;

					cur.toi() = dest;
					_cs.move_piece(source, dest, my_color);

					bit_table obstacles = _cs.all.get_all_possible_moves(dest);
					for (;;)
					{
						const off_i_t obs = _Select_highest(obstacles);
						if (obs == 64)
							break;

						cur.obsi() = obs;
						ret.push_back(cur);

						undo_place_bit(obstacles, obs);
					}
					
					_cs.move_piece(dest, source, my_color);

					undo_place_bit(dests, dest);
				}

				undo_place_bit(mine, source);
			}

			return ret;
		}
	public:
		chess_game() noexcept :
			_cs(), my_color(chess_color::white), timespan(0), deadspan(970), turn_cnt(0), _add_turn(false)
		{}
		inline chess_color get_color() noexcept { return my_color; }
		inline void set_color(chess_color cc) noexcept { my_color = cc; }
		inline chess_status get_status() noexcept { return _cs; }
		inline void set_status(chess_status cs) noexcept { _cs = cs; }
		inline int get_turn()noexcept { return turn_cnt; }
		inline void set_turn(int turn)noexcept { turn_cnt = turn; }
		inline void make_move(off_i_t from, off_i_t to, off_i_t obs)noexcept
		{
			distance++;
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
			if (_add_turn)
				--turn_cnt;
			my_color = _Color_rev(my_color);
			_cs.unplace_obs(obs);
			_cs.move_piece(to, from, my_color);
			distance--;
		}
		inline void unmake_move(movement mm) noexcept
		{
			const auto& [from, to, obs] = mm;
			unmake_move(from, to, obs);
		}
		inline void null_move()
		{
			distance++;
			my_color = _Color_rev(my_color);
			if (_add_turn)
				++turn_cnt;
			_add_turn = !_add_turn;
		}
		inline void undo_null_move()
		{
			_add_turn = !_add_turn;
			if (_add_turn)
				--turn_cnt;
			my_color = _Color_rev(my_color);
			distance--;
		}
		inline void init() noexcept { _cs.init(); }

		movement generate_next_move()
		{
			starttime = std::chrono::steady_clock::now();
			timespan = turn_cnt == 1 ? 900 : 920;

			this->rt.clear();

			return _Search_till_timeout();
		}
	};
}