
#pragma once

#include "std.h"
#include "eval_head.h"
#include "container.h"

namespace eval_adj	// evaluation adjusted
{
	// 评估加权器
	constexpr double w_t1[28] = { 0.1080, 0.1080, 0.1235, 0.1332, 0.1400, 0.1468, 0.1565, 0.1720, 0.1949, 0.2217, 0.2476, 0.2680, 0.2800, 0.2884, 0.3000, 0.3208, 0.3535, 0.4000, 0.4613, 0.5350, 0.6181, 0.7075, 0.8000, 0.9000, 0.9000,0.9000, 0.9000, 0.9000 };
	constexpr double w_t2[28] = { 0.3940, 0.3940, 0.3826, 0.3753, 0.3700, 0.3647, 0.3574, 0.3460, 0.3294, 0.3098, 0.2903, 0.2740, 0.2631, 0.2559, 0.2500, 0.2430, 0.2334, 0.2200, 0.2020, 0.1800, 0.1550, 0.1280, 0.1000, 0.0500, 0.0500, 0.0500, 0.0500, 0.0500 };
	constexpr double w_c[28] = { 0.1160, 0.1160, 0.1224, 0.1267, 0.1300, 0.1333, 0.1376, 0.1440, 0.1531, 0.1640, 0.1754, 0.1860, 0.1944, 0.1995, 0.2000, 0.1950, 0.1849, 0.1700, 0.1510, 0.1287, 0.1038, 0.0773, 0.0500, 0.0250, 0.0250, 0.0500, 0.0500, 0.0500 };
	constexpr double w_m[28] = { 0.2300, 0.2300, 0.2159, 0.2067, 0.2000, 0.1933, 0.1841, 0.1700, 0.1496, 0.1254, 0.1010, 0.0800, 0.0652, 0.0557, 0.0500, 0.0464, 0.0436, 0.0400, 0.0346, 0.0274, 0.0190, 0.0097, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 };
	class evaluation_weight_function {
	public:
		evaluation_weight_function() {
#if false
#ifdef _BOTZONE_ONLINE
			ifstream ifs("./data/weight.txt");
#else
			ifstream ifs("weight.txt");
#endif // _BOTZONE_ONLINE
			for (int i = 0; i < 28; ++i)
				ifs >> w_t1[i];
			for (int i = 0; i < 28; ++i)
				ifs >> w_t2[i];
			for (int i = 0; i < 28; ++i)
				ifs >> w_c[i];
			for (int i = 0; i < 28; ++i)
				ifs >> w_m[i];
			ifs.close();
#endif
		}
		double f_w_t1(size_t w, double v) const
		{
			return v * w_t1[w];
		}

		double f_w_t2(size_t w, double v) const 
		{
			return v * w_t2[w];
		}
		double f_w_c(size_t w, double v) const 
		{
			return v * w_c[w];
		}
		double f_w_m(size_t w, double v) const 
		{
			return v * w_m[w];
		}
	};

	unsigned quick_pow(unsigned x, unsigned n)
	{
		int res = x;
		int ans = 1;
		for (; n;)
		{
			if (n & 1)
				ans *= res;
			res *= res;
			n >>= 1;
		}
		return ans;
	}

	double quick_pow_s(int x, int n) {
		if (n <= -31)return 0.0;
		if (n < 0)
			return 1.0 / quick_pow(x, -n);
		else
			return quick_pow(x, n);
	}

	// 评估器
	class evaluator {
		using distance_matrix = uint8_t[8][8];
		using distance_matrix_group = array<distance_matrix, 4>;
		friend int amz::_Debug_evaluate(const chess_status& cs, chess_color color, int turn_cnt, std::ostream& out);
	public:
		evaluator(const board& bd, const player& pl, int turn, const evaluation_weight_function& ewf) : _bd(bd), _pl(pl), _turn(turn), _ewf(ewf) {
			//for (auto& dmg : _dm_queen)
				//for (auto& dm : dmg)
					//memset(dm, (uint8_t)(-1), 64);
			//for (auto& dmg : _dm_king)
				//for (auto& dm : dmg)
					//memset(dm, (uint8_t)(-1), 64);
			memset(_merged_dm_queen[0], (uint8_t)(-1), 64);
			memset(_merged_dm_queen[1], (uint8_t)(-1), 64);
			memset(_merged_dm_king[0], (uint8_t)(-1), 64);
			memset(_merged_dm_king[1], (uint8_t)(-1), 64);
		}
		double evaluate() {
			double r = 0;
			_generate_distance_matrix();
			_output_board();
			double t = _territory_ingredient();
			double m = _mobility_ingredient();
			double d = _distribution_ingredient();
			double g = 0;// _guard_ingredient(); 代价太高了，没办法
			r = t + m + d + g;
			// append_log("t m d g r: " + to_string(t) + " " + to_string(m) + " " + to_string(d) + " " + to_string(g) + " " + to_string(r), true);
			return r;
		}
		void _output_board() {
#ifdef _DEBUG
			string str = "+----------DM1S----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_queen[0][j][i]) + string(4 - to_string(_merged_dm_queen[0][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM1O----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_queen[1][j][i]) + string(4 - to_string(_merged_dm_queen[1][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM2S----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_king[0][j][i]) + string(4 - to_string(_merged_dm_king[0][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM2O----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_king[1][j][i]) + string(4 - to_string(_merged_dm_king[1][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
#endif
		}
	private:
		void _generate_distance_matrix() {
			//int idx = 0;
			//for (auto& m : _pl.self())
				//_single_queen_min_moves(m, _dm_queen[0][idx++]);
			//idx = 0;
			_queen_min_moves(_pl.self(), _merged_dm_queen[0]);
			//for (auto& m : _pl.opponent())
			_queen_min_moves(_pl.opponent(), _merged_dm_queen[1]);
			//idx = 0;
			//for (auto& m : _pl.self())
			_king_min_moves(_pl.self(), _merged_dm_king[0]);
			//idx = 0;
			//for (auto& m : _pl.opponent())
			_king_min_moves(_pl.opponent(), _merged_dm_king[1]);

			//_merge_distance_matrix(_merged_dm_queen[0], _dm_queen[0]);
			//_merge_distance_matrix(_merged_dm_queen[1], _dm_queen[1]);
			//_merge_distance_matrix(_merged_dm_king[0], _dm_king[0]);
			//_merge_distance_matrix(_merged_dm_king[1], _dm_king[1]);
		}
		double _territory_determine_delta(uint8_t m, uint8_t n) {
			if (m == 255 && n == 255) return 0.0;
			if (m == n) return 0.125;		// 先手优势
			if (m < n) return 1.0;
			return -1.0;
		}
		void _merge_distance_matrix(distance_matrix& out, const distance_matrix_group& in) {
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					uint8_t min1 = min(in[0][i][j], in[1][i][j]);
					uint8_t min2 = min(in[2][i][j], in[3][i][j]);
					out[i][j] = min(min1, min2);
				}
		}
		pair<double, double> _t1_c1() {
			double t1 = 0, c1 = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (!_bd(i, j).is_empty()) continue;
					t1 += _territory_determine_delta(_merged_dm_queen[0][i][j], _merged_dm_queen[1][i][j]);
					c1 += quick_pow_s(2, -_merged_dm_queen[0][i][j]) - quick_pow_s(2, -_merged_dm_queen[1][i][j]);
				}
			c1 *= 2;
			return { t1, c1 };
		}
		pair<double, double>  _t2_c2() {
			double t2 = 0, c2 = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (!_bd(i, j).is_empty()) continue;
					t2 += _territory_determine_delta(_merged_dm_king[0][i][j], _merged_dm_king[1][i][j]);
					c2 += min(1.0, max(-1.0, (double)(_merged_dm_king[1][i][j] - _merged_dm_king[0][i][j]) / 6.0));
				}
			return { t2,c2 };
		}

		void _generate_empty(distance_matrix& _empty_num)
		{
			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					if (_bd.is_empty(get_i(i, j)))
						_empty_num[i][j] = _empty_neighbor_num(i, j);
		}
		uint8_t _empty_neighbor_num(len_t x, len_t y) {
			uint8_t sum = 0;
			/*
			sum = ((x + 1 < 8 && _bd.is_empty(x + 1, y)) ? sum + 1 : sum);
			sum = ((x - 1 >= 0 && _bd.is_empty(x - 1, y)) ? sum + 1 : sum);
			sum = ((y + 1 < 8 && _bd.is_empty(x, y + 1)) ? sum + 1 : sum);
			sum = ((y - 1 >= 0 && _bd.is_empty(x, y - 1)) ? sum + 1 : sum);
			sum = ((x + 1 < 8 && y + 1 < 8 && _bd.is_empty(x + 1, y + 1)) ? sum + 1 : sum);
			sum = ((x - 1 >= 0 && y + 1 < 8 && _bd.is_empty(x - 1, y + 1)) ? sum + 1 : sum);
			sum = ((x + 1 < 8 && y - 1 >= 0 && _bd.is_empty(x + 1, y - 1)) ? sum + 1 : sum);
			sum = ((x - 1 >= 0 && y - 1 >= 0 && _bd.is_empty(x - 1, y - 1)) ? sum + 1 : sum);
			*/
			for (int m = 0; m < 8; m++)
				sum = ((in_map(x + dx[m], y + dy[m]) && _bd.is_empty(x + dx[m], y + dy[m])) ? sum + 1 : sum);
			return sum;
		}
		int dx[8] = { -1,-1,-1,0,0,1,1,1 };
		int dy[8] = { -1,0,1,-1,1,-1,0,1 };
		void _queen_min_moves(array<pair<len_t, len_t>, 4> froms, distance_matrix& distance)
		{
			resizing_queue<pair<len_t, len_t>> que(64);

			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					distance[i][j] = 255;

			for (int i = 0; i < 4; i++)
			{
				pair<len_t, len_t>& from = froms[i];
				auto [x, y] = from;
				que.push(from);
				distance[x][y] = 0;
				while (!que.empty())
				{
					pair<len_t, len_t> p = que.pop();

					for (int j = 0; j < 8; j++)
					{
						for (int step = 1; step < 8; step++)
						{
							int nx = p.first + dx[j] * step, ny = p.second + dy[j] * step;
							if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)) && distance[nx][ny] > distance[p.first][p.second] + 1)
							{
								que.push(make_pair(nx, ny));
								distance[nx][ny] = distance[p.first][p.second] + 1;
							}
							else break;
						}
					}
				}
			}
		}
		void _single_queen_min_moves(pair<len_t, len_t> from, distance_matrix& distance)
		{
			/*
			vector<pair<int, int>> open;
			bitset<64> closed;

			open.reserve(32);

			open.push_back(from);
			auto [fx, fy] = from;
			distance[fx][fy] = 0;

			for (; !open.empty();)
			{
				auto tmp = open.back();
				auto [x, y] = tmp;
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[get_i(x, y)] = 1;

				auto my_moves = amz::get_all_possible_i(_bd.get_table(), get_i(x, y));
				for (auto i : my_moves)
				{
					auto [x1, y1] = get_ij(i);
					if (!closed[get_i(x1, y1)])
					{
						open.push_back({ x1,y1 });
						distance[x1][y1] = min(distance[x1][y1], (uint8_t)(w + 1));
					}
				}
			}
			distance[fx][fy] = 255;
			*/
			resizing_queue<pair<len_t, len_t>> que(64);

			auto [x, y] = from;
			que.push(from);
			distance[x][y] = 0;
			for (; !que.empty();)
			{
				pair<len_t, len_t> p = que.pop();

				for (int j = 0; j < 8; j++)
				{
					for (int step = 1; step < 8; step++)
					{
						int nx = p.first + dx[j] * step, ny = p.second + dy[j] * step;
						if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)) && distance[nx][ny] > distance[p.first][p.second] + 1)
						{
							que.push(make_pair(nx, ny));
							distance[nx][ny] = distance[p.first][p.second] + 1;
						}
						else break;
					}
				}
			}
			distance[x][y] = 255;
		}
		void _king_min_moves(array<pair<len_t, len_t>, 4> froms, distance_matrix& distance)
		{
			resizing_queue<pair<len_t, len_t>> que(64);

			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					distance[i][j] = 255;

			for (int i = 0; i < 4; i++)
			{
				pair<len_t, len_t>& from = froms[i];
				auto [x, y] = from;
				que.push(from);
				distance[x][y] = 0;
				while (!que.empty())
				{
					pair<len_t, len_t> p = que.pop();

					for (int j = 0; j < 8; j++)
					{
						len_t nx = p.first + dx[j], ny = p.second + dy[j];
						if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)) && distance[nx][ny] > distance[p.first][p.second] + 1)
						{
							que.push(make_pair(nx, ny));
							distance[nx][ny] = distance[p.first][p.second] + 1;
						}
					}
				}
			}
		}
		void _single_king_min_moves(pair<len_t, len_t> from, distance_matrix& distance)
		{
			/*
			vector<off_i_t> open;
			vector<bool> closed(64);

			open.reserve(32);

			auto [fx, fy] = from;
			open.push_back(get_i(fx, fy));
			distance[fx][fy] = 0;

			while (!open.empty()) {
				auto tmp = open.back();
				auto [x, y] = get_ij(tmp);
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[tmp] = 1;

				if (x < 7)
					if (_bd(tmp + 8).is_empty() && !closed[tmp + 8])
					{
						open.push_back(tmp + 8);
						distance[x + 1][y] = min(distance[x + 1][y], (uint8_t)(w + 1));
					}
				if (x >= 1)
					if (_bd(tmp - 8).is_empty() && !closed[tmp - 8])
					{
						open.push_back(tmp - 8);
						distance[x - 1][y] = min(distance[x - 1][y], (uint8_t)(w + 1));
					}
				if (y < 7)
					if (_bd(tmp + 1).is_empty() && !closed[tmp + 1])
					{
						open.push_back(tmp + 1);
						distance[x][y + 1] = min(distance[x][y + 1], (uint8_t)(w + 1));
					}
				if (y >= 1)
					if (_bd(tmp - 1).is_empty() && !closed[tmp - 1])
					{
						open.push_back(tmp - 1);
						distance[x][y - 1] = min(distance[x][y - 1], (uint8_t)(w + 1));
					}
				if (x < 7 && y < 7)
					if (_bd(tmp + 9).is_empty() && !closed[tmp + 9])
					{
						open.push_back(tmp + 9);
						distance[x + 1][y + 1] = min(distance[x + 1][y + 1], (uint8_t)(w + 1));
					}
				if (x >= 1 && y < 7)
					if (_bd(tmp - 7).is_empty() && !closed[tmp - 7])
					{
						open.push_back(tmp - 7);
						distance[x - 1][y + 1] = min(distance[x - 1][y + 1], (uint8_t)(w + 1));
					}
				if (x < 7 && y >= 1)
					if (_bd(tmp + 7).is_empty() && !closed[tmp + 7])
					{
						open.push_back(tmp + 7);
						distance[x + 1][y - 1] = min(distance[x + 1][y - 1], (uint8_t)(w + 1));
					}
				if (x >= 1 && y >= 1)
					if (_bd(tmp - 9).is_empty() && !closed[tmp - 9])
					{
						open.push_back(tmp - 9);
						distance[x - 1][y - 1] = min(distance[x - 1][y - 1], (uint8_t)(w + 1));
					}
			}

			distance[fx][fy] = 255;
			*/
			resizing_queue<pair<len_t, len_t>> que(64);

			auto [x, y] = from;
			que.push(from);
			distance[x][y] = 0;
			while (!que.empty())
			{
				pair<len_t, len_t> p = que.pop();
				for (int j = 0; j < 8; j++)
				{
					len_t nx = p.first + dx[j], ny = p.second + dy[j];
					if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)) && distance[nx][ny] > distance[p.first][p.second] + 1)
					{
						que.push(make_pair(nx, ny));
						distance[nx][ny] = distance[p.first][p.second] + 1;
					}
				}
			}
			distance[x][y] = 255;
		}
		bool in_map(len_t i, len_t j) { return !(((i << 4) + j) & 0x88); }
	
		double mother[9] = { 0.0,1.0,0.5,0.33,0.25,0.2,0.167,0.143,0.125 };

		double _amazons_mobility(size_t player_idx) {
			double a = 0.0;
			//for (int i = 0; i < 8; ++i)
				//for (int j = 0; j < 8; ++j)
					//if (_merged_dm_queen[1 - player_idx][i][j] != 255 && _dm_queen[player_idx][amazon_idx][i][j] <= 1)
						//a += pow(2.0, -_dm_king[player_idx][amazon_idx][i][j] + 1) * _empty_neighbor_num(i, j);
			const array<pair<len_t, len_t>, 4>& amazons = player_idx == 0 ? _pl.self() : _pl.opponent();
			distance_matrix empty;
			_generate_empty(empty);

			for (auto amazon : amazons) {
				for (int i = 0; i < 8; i++) {
					for (int step = 1; step < 8; step++)
					{
						int nx = amazon.first + dx[i] * step, ny = amazon.second + dy[i] * step;
						if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)) && _merged_dm_queen[player_idx][nx][ny] != 255)
							a += empty[nx][ny] * mother[step];
						else
							break;
					}
				}
			}
			return a;
		}
		double _territory_ingredient() {
			auto [t1, c1] = _t1_c1();
			auto [t2, c2] = _t2_c2();
			return
				_ewf.f_w_t1(_turn, t1) +
				_ewf.f_w_t2(_turn, t2) +
				_ewf.f_w_c(_turn, c1) +
				_ewf.f_w_c(_turn, c2);
		}
		inline int _pow2(int x) { return x * x; }
		double _distribution_ingredient() {
			double d1 = 0, d2 = 0;
			for (const auto& amazon1 : _pl.self())
				for (const auto& amazon2 : _pl.self()) {
					auto [x1, y1] = amazon1;
					auto [x2, y2] = amazon2;
					d1 += sqrt(_pow2(x1 - x2) + _pow2(y1 - y2));
				}
			for (const auto& amazon1 : _pl.opponent())
				for (const auto& amazon2 : _pl.opponent()) {
					auto [x1, y1] = amazon1;
					auto [x2, y2] = amazon2;
					d2 += sqrt(_pow2(x1 - x2) + _pow2(y1 - y2));
				}
			d1 = sqrt(d1 / 10.0);
			d2 = sqrt(d2 / 10.0);
			return (1 - _turn / 28) * (d1 - d2) * 0.6;
		}
		double _mobility_ingredient() {
			double m1 = _amazons_mobility(0);
			double m2 = _amazons_mobility(1);
			return _ewf.f_w_m(_turn, m1 - m2);
		}

		using boolmtx = bool[8][8];
		boolmtx _can_access[2][4] = {};

		vector<pair<len_t, len_t>> _adjacents(pair<len_t, len_t> from)
		{
			vector<pair<len_t, len_t>> ret;
			ret.reserve(8);
			/*
			if (from.first != 0 && _bd.is_empty(from.first - 1, from.second))
				ret.push_back({ from.first - 1,from.second });
			if (from.first != 7 && _bd.is_empty(from.first + 1, from.second))
				ret.push_back({ from.first + 1,from.second });
			if (from.second != 0 && _bd.is_empty(from.first, from.second - 1))
				ret.push_back({ from.first ,from.second - 1 });
			if (from.second != 7 && _bd.is_empty(from.first, from.second + 1))
				ret.push_back({ from.first ,from.second + 1 });
				*/
			for (int m = 0; m < 8; ++m)
				if (in_map(from.first + dx[m], from.second + dy[m]) && _bd.is_empty(from.first + dx[m], from.second + dy[m]))
					ret.push_back({ from.first + dx[m], from.second + dy[m] });
			return ret;
		}
		void _generate_access_mtx(const pair<len_t, len_t> from, boolmtx& distance)
		{
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
					distance[i][j] = false;

			pair<len_t, len_t> v;
			vector<pair<len_t, len_t>> stack;
			stack.reserve(64);

			stack.push_back(from);
			for (; stack.size();)
			{
				v = stack.back();
				distance[v.first][v.second] = true;
				stack.pop_back();

				for (int m = 0; m < 8; ++m)
				{
					auto x = v.first + dx[m];
					auto y = v.second + dy[m];
					if (in_map(x, y) && _bd.is_empty(x, y))
						if (!distance[x][y])
							stack.push_back({ x,y });
				}
			}

			distance[from.first][from.second] = false;
			/*
			resizing_queue<pair<len_t, len_t>> que(64);

			auto [x, y] = from;
			que.push(from);
			distance[x][y] = true;
			while (!que.empty())
			{
				pair<len_t, len_t> p = que.pop();
				for (int j = 0; j < 8; j++)
				{
					len_t nx = p.first + dx[j], ny = p.second + dy[j];
					if (in_map(nx, ny) && _bd.is_empty(get_i(nx, ny)))
					{
						que.push(make_pair(nx, ny));
						distance[nx][ny] = distance[p.first][p.second] + 1;
					}
				}
			}
			distance[x][y] = false;
			*/
		}
		void _generate_access()
		{
			int idx = 0;
			for (auto& amazon : _pl.self())
				_generate_access_mtx(amazon, _can_access[0][idx++]);
			idx = 0;
			for (auto& amazon : _pl.opponent())
				_generate_access_mtx(amazon, _can_access[1][idx++]);
		}

		double _guard_ingredient() {
			_generate_access();

			auto _flat_access_mtx = [this](size_t idx) {return _can_access[idx / 4][idx % 4]; };
			// auto _self_dm_queen = [this](size_t idx) {return _can_access[0][idx]; };
			// auto _opponent_dm_queen = [this](size_t idx) {return _can_access[1][idx]; };
			auto _move_once = [this](pair<len_t, len_t> from, pair<len_t, len_t>to) {
				for (int m = 0; m < 8; ++m)
					if (to.first == from.first + dx[m] && to.second == from.second + dy[m])
						return true;
				return false;
			};

			array<array<size_t, 4>, 2> exclusive_access_num = { };
			array<array<size_t, 4>, 2> common_access_num = { };

			auto self = _pl.self();
			auto opponent = _pl.opponent();

			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
				{
					constexpr int upper_bound = 8;
					for (int i0 = 0; i0 < upper_bound; ++i0)
						if (_flat_access_mtx(i0)[i][j])
						{
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0)
									continue;
								else
									if (_flat_access_mtx(j0)[i][j])
										goto end1;
							++exclusive_access_num[i0 / 4][i0 % 4];
							goto end1;
						}
				end1:
					;
				}
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
				{
					constexpr int upper_bound = 4;
					for (int i0 = 0; i0 < upper_bound; ++i0)
						if (_move_once(self[i0], { i,j }))
						{
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0)
									continue;
								else
									if (_move_once(self[j0], { i,j }))
										goto end2;
							if (_merged_dm_queen[1][i][j] == 1)
								++common_access_num[0][i0];
							goto end2;
						}
				end2:
					;
				}
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
				{
					constexpr int upper_bound = 4;
					for (int i0 = 0; i0 < upper_bound; ++i0)
						if (_move_once(opponent[i0], { i,j }))
						{
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0)
									continue;
								else
									if (_move_once(opponent[j0], { i,j }))
										goto end3;
							if (_merged_dm_queen[0][i][j] == 1)
								++common_access_num[1][i0];
							goto end3;
						}
				end3:
					;
				}

			double sum = 0;
			for (int i = 0; i < 4; ++i) {
				sum += min((double)exclusive_access_num[0][i], (double)common_access_num[0][i] / 2.0);
				sum -= min((double)exclusive_access_num[1][i], (double)common_access_num[1][i] / 2.0);
			}
			return sum / 5.0;
		}

	private:
		const board& _bd;
		const player& _pl;
		size_t _turn;
		const evaluation_weight_function& _ewf;
		// distance_matrix_group _dm_queen[2];
		// distance_matrix_group _dm_king[2];
		distance_matrix _merged_dm_queen[2];
		distance_matrix _merged_dm_king[2];
	};

}