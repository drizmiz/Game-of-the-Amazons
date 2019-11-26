
#pragma once

#include "std.h"
#include "eval_head.h"

// #define THROW_EXCEPTION

namespace eval_adj	// evaluation adjusted
{
	//ÆÀ¹À¼ÓÈ¨Æ÷

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
		double f_w_t1(size_t w, double v) const {
			return v * w_t1[w];
		}

		double f_w_t2(size_t w, double v) const {
			return v * w_t2[w];
		}
		double f_w_c(size_t w, double v) const {

			return v * w_c[w];
		}
		double f_w_m(size_t w, double v) const {
			return v * w_m[w];
		}
	};

	//ÆÀ¹ÀÆ÷
	class evaluator {
		using distance_matrix = uint8_t[8][8];
		using distance_matrix_group = array<distance_matrix, 4>;
		friend int amz::_Debug_evaluate(const chess_status& cs, chess_color color,int turn_cnt, std::ostream& out);
	public:
		evaluator(const board& bd, const player& pl, size_t turn, const evaluation_weight_function& ewf) : _bd(bd), _pl(pl), _turn(turn), _ewf(ewf) {
			for (auto& dmg : _dm_1)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);
			for (auto& dmg : _dm_2)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);
			memset(_merged_dm_1[0], (uint8_t)(-1), 64);
			memset(_merged_dm_1[1], (uint8_t)(-1), 64);
			memset(_merged_dm_2[0], (uint8_t)(-1), 64);
			memset(_merged_dm_2[1], (uint8_t)(-1), 64);
		}
		double evaluate() {
			double r = 0;
			_generate_distance_matrix();
			_output_board();
			double t = _territory_ingredient();
			double m = _mobility_ingredient();
			double d = _distribution_ingredient();
			double g = _guard_ingredient();
			r = t + m + d + g;
			append_log("t m d g r: " + to_string(t) + " " + to_string(m) + " " + to_string(d) + " " + to_string(g) + " " + to_string(r), true);
			return r;
		}
		void _output_board() {
#ifdef _DEBUG
			string str = "+----------DM1S----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_1[0][j][i]) + string(4 - to_string(_merged_dm_1[0][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM1O----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_1[1][j][i]) + string(4 - to_string(_merged_dm_1[1][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM2S----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_2[0][j][i]) + string(4 - to_string(_merged_dm_2[0][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
			str = "+----------DM2O----------+\n";
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					str += to_string(_merged_dm_2[1][j][i]) + string(4 - to_string(_merged_dm_2[1][j][i]).size(), ' ');
				str += "\n";
			}
			append_log(str);
#endif
		}
	private:
		void _generate_distance_matrix() {
			int idx = 0;
			for (auto& m : _pl.self())
				_single_queen_min_moves(m, _dm_1[0][idx++]);
			idx = 0;
			for (auto& m : _pl.opponent())
				_single_queen_min_moves(m, _dm_1[1][idx++]);
			idx = 0;
			for (auto& m : _pl.self())
				_single_king_min_moves(m, _dm_2[0][idx++]);
			idx = 0;
			for (auto& m : _pl.opponent())
				_single_king_min_moves(m, _dm_2[1][idx++]);

			_merge_distance_matrix(_merged_dm_1[0], _dm_1[0]);
			_merge_distance_matrix(_merged_dm_1[1], _dm_1[1]);
			_merge_distance_matrix(_merged_dm_2[0], _dm_2[0]);
			_merge_distance_matrix(_merged_dm_2[1], _dm_2[1]);
		}
		double _territory_determine_delta(uint8_t m, uint8_t n) {
			if (m == 255 && n == 255) return 0;
			if (m == n) return 0.125;
			if (m < n) return 1;
			return -1;
		}
		void _merge_distance_matrix(distance_matrix& out, const distance_matrix_group& in) {
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					uint8_t min1 = min(in[0][i][j], in[1][i][j]);
					uint8_t min2 = min(in[2][i][j], in[3][i][j]);
					out[i][j] = min(min1, min2);
				}
		}
		tuple<double, double> _t1_c1() {
			double t1 = 0, c1 = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (!_bd(i, j).is_empty()) continue;
					t1 += _territory_determine_delta(_merged_dm_1[0][i][j], _merged_dm_1[1][i][j]);
					c1 += pow(2.0, -_merged_dm_1[0][i][j]) - pow(2.0, -_merged_dm_1[1][i][j]);
				}
			c1 *= 2;
			return { t1, c1 };
		}
		tuple<double, double>  _t2_c2() {
			double t2 = 0, c2 = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (!_bd(i, j).is_empty()) continue;
					t2 += _territory_determine_delta(_merged_dm_2[0][i][j], _merged_dm_2[1][i][j]);
					c2 += min(1.0, max(-1.0, (double)(_merged_dm_2[1][i][j] - _merged_dm_2[0][i][j]) / 6.0));
				}
			return { t2,c2 };
		}
		size_t _empty_neighbor_num(uint8_t x, uint8_t y) {
			size_t sum = 0;
			if (x + 1 < 8 && _bd(x + 1, y).is_empty()) ++sum;
			if (x - 1 >= 0 && _bd(x - 1, y).is_empty()) ++sum;
			if (y + 1 < 8 && _bd(x, y + 1).is_empty()) ++sum;
			if (y - 1 >= 0 && _bd(x, y - 1).is_empty()) ++sum;
			if (x + 1 < 8 && y + 1 < 8 && _bd(x + 1, y + 1).is_empty()) ++sum;
			if (x - 1 >= 0 && y + 1 < 8 && _bd(x - 1, y + 1).is_empty()) ++sum;
			if (x + 1 < 8 && y - 1 >= 0 && _bd(x + 1, y - 1).is_empty()) ++sum;
			if (x - 1 >= 0 && y - 1 >= 0 && _bd(x - 1, y - 1).is_empty()) ++sum;
			return sum;
		}
		void _single_queen_min_moves(tuple<int, int> from, distance_matrix& distance) {
			vector<tuple<int, int>> open;
			bitset<64> closed;

			open.reserve(64);

			auto eigen_value = [](tuple<int, int> pair) {
				auto [x, y] = pair;
				return (uint8_t)x << 3 | (uint8_t)y;
			};

			open.push_back(from);
			auto [fx, fy] = from;
			distance[fx][fy] = 0;

			while (!open.empty()) {
				auto tmp = open.back();
				auto [x, y] = tmp;
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[eigen_value(tmp)] = 1;

				for (int i = x + 1; i < 8; ++i) {
					if (!_bd(i, y).is_empty()) break;
					if (!closed[eigen_value({ i, y })]) {
						open.emplace_back(i, y);
						distance[i][y] = min(distance[i][y], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1; i >= 0; --i) {
					if (!_bd(i, y).is_empty()) break;
					if (!closed[eigen_value({ i, y })]) {
						open.emplace_back(i, y);
						distance[i][y] = min(distance[i][y], (uint8_t)(w + 1));
					}
				}
				for (int i = y + 1; i < 8; ++i) {
					if (!_bd(x, i).is_empty()) break;
					if (!closed[eigen_value({ x, i })]) {
						open.emplace_back(x, i);
						distance[x][i] = min(distance[x][i], (uint8_t)(w + 1));
					}
				}
				for (int i = y - 1; i >= 0; --i) {
					if (!_bd(x, i).is_empty()) break;
					if (!closed[eigen_value({ x, i })]) {
						open.emplace_back(x, i);
						distance[x][i] = min(distance[x][i], (uint8_t)(w + 1));
					}
				}
				for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) {
					if (!_bd(i, j).is_empty()) break;
					if (!closed[eigen_value({ i, j })]) {
						open.emplace_back(i, j);
						distance[i][j] = min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
					if (!_bd(i, j).is_empty()) break;
					if (!closed[eigen_value({ i, j })]) {
						open.emplace_back(i, j);
						distance[i][j] = min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
					if (!_bd(i, j).is_empty()) break;
					if (!closed[eigen_value({ i, j })]) {
						open.emplace_back(i, j);
						distance[i][j] = min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
					if (!_bd(i, j).is_empty()) break;
					if (!closed[eigen_value({ i, j })]) {
						open.emplace_back(i, j);
						distance[i][j] = min(distance[i][j], (uint8_t)(w + 1));
					}
				}
			}
			distance[fx][fy] = 255;
		}
		void _single_king_min_moves(tuple<int, int> from, distance_matrix& distance) {
			vector<tuple<int, int>> open;
			bitset<64> closed;

			open.reserve(32);

			auto eigen_value = [](tuple<int, int> pair) {
				auto [x, y] = pair;
				return (uint8_t)x << 3 | (uint8_t)y;
			};

			open.push_back(from);
			auto [fx, fy] = from;
			distance[fx][fy] = 0;

			while (!open.empty()) {
				auto tmp = open.back();
				auto [x, y] = tmp;
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[eigen_value(tmp)] = 1;

				if (x + 1 < 8) if (_bd(x + 1, y).is_empty() && !closed[eigen_value({ x + 1, y })]) {
					open.emplace_back(x + 1, y);
					distance[x + 1][y] = min(distance[x + 1][y], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0) if (_bd(x - 1, y).is_empty() && !closed[eigen_value({ x - 1, y })]) {
					open.emplace_back(x - 1, y);
					distance[x - 1][y] = min(distance[x - 1][y], (uint8_t)(w + 1));
				}
				if (y + 1 < 8) if (_bd(x, y + 1).is_empty() && !closed[eigen_value({ x, y + 1 })]) {
					open.emplace_back(x, y + 1);
					distance[x][y + 1] = min(distance[x][y + 1], (uint8_t)(w + 1));
				}
				if (y - 1 >= 0) if (_bd(x, y - 1).is_empty() && !closed[eigen_value({ x, y - 1 })]) {
					open.emplace_back(x, y - 1);
					distance[x][y - 1] = min(distance[x][y - 1], (uint8_t)(w + 1));
				}
				if (x + 1 < 8 && y + 1 < 8) if (_bd(x + 1, y + 1).is_empty() && !closed[eigen_value({ x + 1, y + 1 })]) {
					open.emplace_back(x + 1, y + 1);
					distance[x + 1][y + 1] = min(distance[x + 1][y + 1], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0 && y + 1 < 8) if (_bd(x - 1, y + 1).is_empty() && !closed[eigen_value({ x - 1, y + 1 })]) {
					open.emplace_back(x - 1, y + 1);
					distance[x - 1][y + 1] = min(distance[x - 1][y + 1], (uint8_t)(w + 1));
				}
				if (x + 1 < 8 && y - 1 >= 0) if (_bd(x + 1, y - 1).is_empty() && !closed[eigen_value({ x + 1, y - 1 })]) {
					open.emplace_back(x + 1, y - 1);
					distance[x + 1][y - 1] = min(distance[x + 1][y - 1], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0 && y - 1 >= 0) if (_bd(x - 1, y - 1).is_empty() && !closed[eigen_value({ x - 1, y - 1 })]) {
					open.emplace_back(x - 1, y - 1);
					distance[x - 1][y - 1] = min(distance[x - 1][y - 1], (uint8_t)(w + 1));
				}
			}

			distance[fx][fy] = 255;
		}
		double _amazon_mobility(size_t player_idx, size_t amazon_idx) {
			double a = 0.0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
					if (_merged_dm_1[1 - player_idx][i][j] != 255 && _dm_1[player_idx][amazon_idx][i][j] <= 1)
						a += pow(2.0, -_dm_2[player_idx][amazon_idx][i][j] + 1) * _empty_neighbor_num(i, j);
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
		double _distribution_ingredient() {
			double d1 = 0, d2 = 0;
			for (const auto& amazon1 : _pl.self())
				for (const auto& amazon2 : _pl.self()) {
					auto [x1, y1] = amazon1;
					auto [x2, y2] = amazon2;
					d1 += sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
				}
			for (const auto& amazon1 : _pl.opponent())
				for (const auto& amazon2 : _pl.opponent()) {
					auto [x1, y1] = amazon1;
					auto [x2, y2] = amazon2;
					d2 += sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
				}
			d1 = sqrt(d1 / 10.0);
			d2 = sqrt(d2 / 10.0);
			return (1 - _turn / 28) * (d1 - d2) * 0.6;
		}
		double _mobility_ingredient() {
			double m1 = 0, m2 = 0;
			for (int i = 0; i < 4; ++i)
				m1 += _amazon_mobility(0, (size_t)i);
			for (int i = 0; i < 4; ++i)
				m2 += _amazon_mobility(1, (size_t)i);
			return _ewf.f_w_m(_turn, m1 - m2);
		}
		double _guard_ingredient() {
			auto _flat_dm_1 = [this](size_t idx) {return _dm_1[idx / 4][idx % 4]; };
			auto _self_dm_1 = [this](size_t idx) {return _dm_1[0][idx]; };
			auto _opponent_dm_1 = [this](size_t idx) {return _dm_1[1][idx]; };

			array<array<size_t, 4>, 2> exclusive_access_num = { 0 };
			array<array<size_t, 4>, 2> common_access_num = { 0 };

			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
				{
					constexpr int upper_bound = 8;
					for (int i0 = 0; i0 < upper_bound; ++i0)
						if (_flat_dm_1(i0)[i][j] != 255) {
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0) continue;
								else if (_flat_dm_1(j0)[i][j] != 255)
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
						if (_self_dm_1(i0)[i][j]) {
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0) continue;
								else if (_self_dm_1(j0)[i][j])
									goto end2;
							if (_merged_dm_1[1][i][j] == 1)  ++common_access_num[0][i0];
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
						if (_opponent_dm_1(i0)[i][j]) {
							for (int j0 = 0; j0 < upper_bound; ++j0)
								if (i0 == j0) continue;
								else if (_opponent_dm_1(j0)[i][j])
									goto end3;
							if (_merged_dm_1[0][i][j] == 1)  ++common_access_num[1][i0];
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
		distance_matrix_group _dm_1[2];
		distance_matrix_group _dm_2[2];
		distance_matrix _merged_dm_1[2];
		distance_matrix _merged_dm_2[2];
	};

}