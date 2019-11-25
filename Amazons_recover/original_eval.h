
#pragma once

#include "std.h"

#pragma region bitmap
namespace yrq {
	class bitmap {
		class strip {
			friend bitmap;
			bitmap* pbitmap;
			size_t idx;
			class reference {
				friend strip;
				strip* pstrip;
				size_t idx;
			public:
				reference() noexcept :idx(0), pstrip(nullptr) {}
				reference(strip& _strip, size_t _idx) :idx(_idx), pstrip(&_strip) {}
				~reference() noexcept {}
				reference& operator=(bool x) {
					pstrip->set(idx, x);
					return *this;
				}
				reference& operator=(const reference& _bitref) noexcept {
					pstrip->set(idx, (bool)_bitref);
					return *this;
				}
				reference& flip(size_t _pos) noexcept {
					pstrip->set(_pos, !pstrip->get(_pos));
					return *this;
				}
				operator bool() const noexcept {
					return pstrip->get(idx);
				}
				bool operator~() const noexcept {
					return !pstrip->get(idx);
				}
			};
		private:
			void _set(size_t _pos, bool _val) {
				pbitmap->_set(idx * 8 + _pos, _val);
			}
			bool _get(size_t _pos) {
				return pbitmap->_get(idx * 8 + _pos);
			}
		public:
			strip() noexcept :idx(0), pbitmap(nullptr) {}
			strip(bitmap& _bitmap, size_t _idx) : idx(_idx), pbitmap(&_bitmap) {}
			~strip() noexcept {}
			reference operator[](size_t _pos) {
				pbitmap->is_valid(_pos, 0, 7);
				return reference(*this, _pos);
			}
			void set(size_t _pos, bool _val) noexcept {
				pbitmap->is_valid(_pos, 0, 7);
				_set(_pos, _val);
			}
			bool get(size_t _pos) noexcept {
				pbitmap->is_valid(_pos, 0, 7);
				return _get(_pos);
			}
		};
	private:
		void _set(size_t _pos, bool _val) noexcept {
			if (_val) raw |= 1ULL << _pos;
			else raw &= ~(1ULL << _pos);
		}
		bool _get(size_t _pos) noexcept {
			return (raw & 1ULL << _pos) ? true : false;
		}
		void is_valid(size_t x, size_t lower_bound, size_t upper_bound) {



		}
		std::uint64_t raw;
	public:
		bitmap() noexcept :raw(0ULL) {};
		bitmap(const std::uint64_t v) :raw(v) {};
		~bitmap() noexcept {}
		strip operator[](size_t _pos) {
			is_valid(_pos, 0, 7);
			return strip(*this, _pos);
		}
		operator uint64_t() {
			return raw;
		}
		void output() noexcept {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					std::cout << ((*this)[i][j] ? "1 " : "0 ");
				std::cout << std::endl;
			}
		}
		void set(size_t _pos, bool _val) noexcept {
			is_valid(_pos, 0, 63);
			return _set(_pos, _val);
		}
		bool get(size_t _pos) noexcept {
			is_valid(_pos, 0, 63);
			return get(_pos);
		}
		bitmap operator|(const bitmap& v) {
			return bitmap(raw | v.raw);
		}
	};
}

#pragma endregion
#pragma region board

namespace yrq {
	class board {
		bitmap amazon;
		bitmap arrow;
	public:
		class piece {
			uint8_t idx;
		public:
			piece() noexcept :idx(0) {}
			piece(uint8_t x, uint8_t y, bool is_obstacle = false) noexcept :idx(x << 5 | y << 2 | (uint8_t)is_obstacle) {}
			piece(uint8_t _idx) noexcept : idx(_idx) {}
			uint8_t x() const { return idx >> 5; }
			uint8_t y() const { return (idx & 0x1C) >> 2; }
			bool is_obstacle() { return idx & 1; }
			bool operator==(const piece& v) {
				return v.idx == idx;
			}
			uint8_t eigen_value() const {
				return idx >> 2;
			}
			static uint8_t eigen_value(uint8_t x, uint8_t y) {
				return x << 3 | y;
			}
		};
	public:
		board() noexcept :amazon(), arrow() {};
		board(bitmap amazon, bitmap _arrow = 0) noexcept :amazon(amazon), arrow(_arrow) {};
		~board() noexcept {};
		bitmap& get_queen_map() { return amazon; }
		bitmap& get_arrow_map() { return arrow; }
		bitmap accessible(int y, int x) {
			bitmap r1, r2, r3, r4, obstacle(amazon | arrow);
			for (int i = x + 1; i < 8; ++i)
				if (!obstacle[i][y]) r1[i][y] = 1;
				else break;
			for (int i = x - 1; i >= 0; --i)
				if (!obstacle[i][y]) r2[i][y] = 1;
				else break;
			for (int i = y + 1; i < 8; ++i)
				if (!obstacle[x][i]) r3[x][i] = 1;
				else break;
			for (int i = y - 1; i >= 0; --i)
				if (!obstacle[x][i]) r4[x][i] = 1;
				else break;
			for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j)
				if (!obstacle[i][j]) r1[i][j] = 1;
				else break;
			for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j)
				if (!obstacle[i][j]) r2[i][j] = 1;
				else break;
			for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j)
				if (!obstacle[i][j]) r3[i][j] = 1;
				else break;
			for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j)
				if (!obstacle[i][j]) r4[i][j] = 1;
				else break;
			return r1 | r2 | r3 | r4;
		}
		bitmap accessible_raw(int offset)
		{
			int x = offset % 8; int y = offset / 8;
			return accessible(y, x);
		}
		bool is_obstacle(piece t) { return (/*amazon |*/ arrow)[t.y()][t.x()]; }
		bool is_obstacle(uint8_t x, uint8_t y) { return (/*amazon |*/ arrow)[y][x]; }
	private:
	};
}

#pragma endregion
#pragma region utility

std::ofstream ofs("records.log", std::ios::trunc);

//反转64bit整数
inline uint64_t bit_reverse(const uint64_t raw) {
	std::bitset<64> v = raw;
	for (uint64_t i = 0; i < 32; ++i)
		v[i] = v[63ull - i];
	return v.to_ullong();
}

struct bit_matrix_file {
	std::string id;
	int xy[8][2] = { 0 };
	uint64_t bd = 0;
};
template <typename Ty>
void emit_key_value(std::string key, Ty value, bool newline = false) {
	ofs.flush();
	ofs << key << ":" << value << "  ";
	ofs.flush();
	if (newline) ofs << std::endl;
	ofs.flush();
}

//读取bit_matrix(*.bm)文件名信息
inline std::string bit_matrix_filename_parse(std::string filename) {
	std::regex pattern(
		R"(([a-zA-z_]\w*)\-(\d+)\-(\d+)\-(0|1)\-wins)",
		std::regex::ECMAScript
		| std::regex::icase
		| std::regex::optimize
	);
	std::smatch match;
	std::string info;
	if (std::regex_match(filename, match, pattern)) {
		info += "GAME " + std::string(match[1]) + "\n";
		info += "TURN " + std::string(match[2]) + "/" + std::string(match[3]) + "\n";
		info += "PLAYER" + std::string(match[4]) + " WINS\n";
	}
	else {
		std::cout << "file name error!" << std::endl;
		throw std::exception();
	}
	return info;
}

//读入bit_matrix(*.bm)文件
inline bit_matrix_file bit_matrix_file_load(std::string filename) {
	std::ifstream ifs(filename);
	std::string all;
	std::regex pattern(
		R"(([a-zA-z_]\w*)\{\s{0,2}([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})([01][01][01][01][01][01][01][01]\s{0,2})\s{0,2}\}\s{0,2}pieces\s{0,2}(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2})(\{\d+,\d+\}\s{0,2}))",
		std::regex::ECMAScript
		| std::regex::icase
		| std::regex::optimize
	);
	std::smatch match;
	bit_matrix_file bm;
	if (!ifs.is_open())
		std::cout << "failed to open " << filename << std::endl;
	else
		for (;;) {
			std::string buf;
			if (!std::getline(ifs, buf)) break;
			all += buf;
		}
	if (std::regex_match(all, match, pattern)) {
		bm.id = match[1];
		std::bitset<64> bs;
		for (uint64_t i = 0; i < 8; ++i) {
			std::string s = match[i + 2];
			for (int j = 0; j < 8; ++j)
				bs[i * 8 + j] = s[j] - '0';
		}
		bm.bd = bs.to_ullong();
		for (uint64_t i = 0; i < 8; ++i) {
			std::string s = match[i + 10];
			int x, y;



			bm.xy[i][0] = x;
			bm.xy[i][1] = y;
		}
	}
	else {
		std::cout << "file format error!" << std::endl;
		throw std::exception();
	}
	return bm;
}

#pragma endregion
#pragma region evaluator

namespace yrq {
	class evaluator {
		board bd; //位棋盘
		using distance_matrix = uint8_t[8][8]; //距离矩阵
		using distance_matrix_group = std::array<distance_matrix, 4>; //距离矩阵组
		using player = std::array<board::piece, 4>; //玩家：包含四个棋子
		using piece = board::piece;
		std::array<distance_matrix_group, 2> dm_1; //玩家1,2的queen距离矩阵组
		std::array<distance_matrix_group, 2> dm_2; //玩家1,2的king距离矩阵组
		std::array<distance_matrix, 2> merged_dm_1; //玩家1,2的queen距离矩阵组合并后的最小queen距离矩阵
		std::array<distance_matrix, 2> merged_dm_2; //玩家1,2的king距离矩阵组合并后的最小king距离矩阵
	public:
		friend int amz::_Evaluate(const amz::chess_status& cs, amz::chess_color color);
		friend int amz::_Debug_evaluate(const amz::chess_status& cs, amz::chess_color color, std::ostream&);

		std::array<player, 2> players;
		double w = 0;
		evaluator(board _bd) noexcept :bd(_bd) {
			for (auto& dmg : dm_1)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);
			for (auto& dmg : dm_2)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);

			memset(merged_dm_1[0], (uint8_t)(-1), 64);
			memset(merged_dm_1[1], (uint8_t)(-1), 64);
			memset(merged_dm_2[0], (uint8_t)(-1), 64);
			memset(merged_dm_2[1], (uint8_t)(-1), 64);
		}
		evaluator() noexcept {
			for (auto& dmg : dm_1)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);
			for (auto& dmg : dm_2)
				for (auto& dm : dmg)
					memset(dm, (uint8_t)(-1), 64);

			memset(merged_dm_1[0], (uint8_t)(-1), 64);
			memset(merged_dm_1[1], (uint8_t)(-1), 64);
			memset(merged_dm_2[0], (uint8_t)(-1), 64);
			memset(merged_dm_2[1], (uint8_t)(-1), 64);
		};
		~evaluator() noexcept {};
		double evaluate() {
			double r = 0;
			_generate_distance_matrix();
			r += _territory_ingredient();
			r += _mobility_ingredient();
			r += _guard_ingredient();
			r += _distribution_ingredient();



			return r;
		}
	private:
		//输出距离矩阵
		void _debug_printf_distance_matrix(distance_matrix dm) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					std::cout << std::setw(4) << (unsigned)dm[j][i];
				std::cout << std::endl;
			}
		}
		//根据双方最小距离取值的delta函数
		double _territory_determine_delta(uint8_t m, uint8_t n) {
			if (m == 255 && n == 255) return 0;
			if (m == n) return 0.125;
			if (m < n) return 1;
			return -1;
		}
		//生成之后所需的距离矩阵
		void _generate_distance_matrix() {
			int idx = 0;
			for (auto& m : players[0])
				_single_queen_min_moves(m, dm_1[0][idx++]);
			idx = 0;
			for (auto& m : players[1])
				_single_queen_min_moves(m, dm_1[1][idx++]);
			idx = 0;
			for (auto& m : players[0])
				_single_king_min_moves(m, dm_2[0][idx++]);
			idx = 0;
			for (auto& m : players[1])
				_single_king_min_moves(m, dm_2[1][idx++]);

			_merge_distance_matrix(merged_dm_1[0], dm_1[0]);
			_merge_distance_matrix(merged_dm_1[1], dm_1[1]);
			_merge_distance_matrix(merged_dm_2[0], dm_2[0]);
			_merge_distance_matrix(merged_dm_2[1], dm_2[1]);
		}
		//生成领地（territory）参量
		double _territory_ingredient() {
			auto [t1, c1, w] = _t1_c1_w();
			auto [t2, c2] = _t2_c2();
			// f(t1,w) = [ 0.75 * 1.1 ^ (-w) + 0.25 ] * t1
			auto f_w_t1 = [=](double v) { return (0.75 * std::pow(1.1, -w) + 0.25) * v; };
			// f(t2,w) = [ 0.08 * sqrt( max { w-1 , 0 } ) ] * t2
			auto f_w_t2 = [=](double v) { return (0.08 * std::sqrt(w - 1 > 0 ? w - 1 : 0))* v; };
			// f(c1,w) = [ 1 - f_t1(w) - f_t2(w) ] * [ 0.6 * 1.1 ^ (-w) + 0.4 ] * c1
			auto f_w_c1 = [=](double v) { return (1 - 0.75 * std::pow(1.1, -w) - 0.25 - 0.08 * std::sqrt(w - 1 > 0 ? w - 1 : 0))* (0.4 + 0.6 * std::pow(1.1, -w))* v; };
			// f(c2,w) = [ 1 - f_t1(w) - f_t2(w) ] * [ 0.6 - 0.6 * 1.1 ^ (-w) ] * c2
			auto f_w_c2 = [=](double v) { return (1 - 0.75 * std::pow(1.1, -w) - 0.25 - 0.08 * std::sqrt(w - 1 > 0 ? w - 1 : 0))* (1 - (0.4 + 0.6 * std::pow(1.1, -w)))* v; };
				return f_w_t1(t1) + f_w_c1(c1) + f_w_t2(t2) + f_w_c2(c2);
		}
		//将距离矩阵组合并为最小距离矩阵
		void _merge_distance_matrix(distance_matrix& out, const distance_matrix_group& in) {
			for (const auto& mat : in)
				for (int i = 0; i < 8; ++i)
					for (int j = 0; j < 8; ++j)
						out[i][j] = std::min(mat[i][j], out[i][j]);
		}
		//两两计算所有amazon之间几何距离，取得棋子整体分布特征
		double _amazons_distribution(const player& p) {
			double sum = 0;
			for (const auto& amazon1 : p)
				for (const auto& amazon2 : p)
					sum += std::sqrt(std::pow(std::abs((double)amazon1.x() - amazon2.x()), 2) + std::pow(std::abs((double)amazon1.y() - amazon2.y()), 2));
				return std::sqrt(sum / 10.0) - 1.5;
		}
		//生成分布（distribution）参量
		double _distribution_ingredient() {
			double d0 = _amazons_distribution(players[0]);
			double d1 = _amazons_distribution(players[1]);




			return w / 20.0 * (d0 - d1);
		}
		//计算t1,c1和局势进度特征值w
		std::tuple<double, double, double> _t1_c1_w() {
			double t1 = 0, c1 = 0, w = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (bd.is_obstacle(i, j)) continue;
					t1 += _territory_determine_delta(merged_dm_1[0][i][j], merged_dm_1[1][i][j]);
					c1 += std::pow(2.0, -merged_dm_1[0][i][j]) - std::pow(2.0, -merged_dm_1[1][i][j]);
					w += std::pow(2.0, -std::abs(merged_dm_1[0][i][j] - merged_dm_1[1][i][j]));
				}
			c1 *= 2;
			this->w = w;
			return std::make_tuple(t1, c1, w);
		}
		//计算t2,c2
		std::tuple<double, double> _t2_c2() {
			double t2 = 0, c2 = 0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					if (bd.is_obstacle(i, j)) continue;
					t2 += _territory_determine_delta(merged_dm_2[0][i][j], merged_dm_2[1][i][j]);
					c2 += std::min(1.0, std::max(-1.0, (double)(merged_dm_2[1][i][j] - merged_dm_2[0][i][j]) / 6.0));
				}
			return std::make_tuple(t2, c2);
		}
		//计算相邻的空闲方格数量
		size_t _empty_neighbor_num(uint8_t x, uint8_t y) {
			size_t sum = 0;
			if (x + 1 < 8 && !bd.is_obstacle(x + 1, y)) ++sum;
			if (x - 1 >= 0 && !bd.is_obstacle(x - 1, y)) ++sum;
			if (y + 1 < 8 && !bd.is_obstacle(x, y + 1)) ++sum;
			if (y - 1 >= 0 && !bd.is_obstacle(x, y - 1)) ++sum;
			if (x + 1 < 8 && y + 1 < 8 && !bd.is_obstacle(x + 1, y + 1)) ++sum;
			if (x - 1 >= 0 && y + 1 < 8 && !bd.is_obstacle(x - 1, y + 1)) ++sum;
			if (x + 1 < 8 && y - 1 >= 0 && !bd.is_obstacle(x + 1, y - 1)) ++sum;
			if (x - 1 >= 0 && y - 1 >= 0 && !bd.is_obstacle(x - 1, y - 1)) ++sum;
			return sum;
		}
		//计算单个amazon的queen距离矩阵
		void _single_queen_min_moves(piece from, distance_matrix& distance) {
			std::vector<piece> open;
			std::bitset<64> closed;
			open.push_back(from);
			distance[from.x()][from.y()] = 0;

			while (!open.empty()) {
				piece tmp = open.back();
				uint8_t x = tmp.x();
				uint8_t y = tmp.y();
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[tmp.eigen_value()] = 1;

				for (int i = x + 1; i < 8; ++i) {
					if (bd.is_obstacle(i, y)) break;
					if (!closed[piece::eigen_value(i, y)]) {
						open.emplace_back(i, y);
						distance[i][y] = std::min(distance[i][y], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1; i >= 0; --i) {
					if (bd.is_obstacle(i, y)) break;
					if (!closed[piece::eigen_value(i, y)]) {
						open.emplace_back(i, y);
						distance[i][y] = std::min(distance[i][y], (uint8_t)(w + 1));
					}
				}
				for (int i = y + 1; i < 8; ++i) {
					if (bd.is_obstacle(x, i)) break;
					if (!closed[piece::eigen_value(x, i)]) {
						open.emplace_back(x, i);
						distance[x][i] = std::min(distance[x][i], (uint8_t)(w + 1));
					}
				}
				for (int i = y - 1; i >= 0; --i) {
					if (bd.is_obstacle(x, i)) break;
					if (!closed[piece::eigen_value(x, i)]) {
						open.emplace_back(x, i);
						distance[x][i] = std::min(distance[x][i], (uint8_t)(w + 1));
					}
				}
				for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) {
					if (bd.is_obstacle(i, j)) break;
					if (!closed[piece::eigen_value(i, j)]) {
						open.emplace_back(i, j);
						distance[i][j] = std::min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
					if (bd.is_obstacle(i, j)) break;
					if (!closed[piece::eigen_value(i, j)]) {
						open.emplace_back(i, j);
						distance[i][j] = std::min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
					if (bd.is_obstacle(i, j)) break;
					if (!closed[piece::eigen_value(i, j)]) {
						open.emplace_back(i, j);
						distance[i][j] = std::min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
					if (bd.is_obstacle(i, j)) break;
					if (!closed[piece::eigen_value(i, j)]) {
						open.emplace_back(i, j);
						distance[i][j] = std::min(distance[i][j], (uint8_t)(w + 1));
					}
				}
				distance[from.x()][from.y()] = -1;
			}
		}
		//计算单个amazon的king距离矩阵
		void _single_king_min_moves(piece from, distance_matrix& distance) {
			std::vector<piece> open;
			std::bitset<64> closed;
			open.push_back(from);
			distance[from.x()][from.y()] = 0;

			while (!open.empty()) {
				piece tmp = open.back();
				uint8_t x = tmp.x();
				uint8_t y = tmp.y();
				uint8_t w = distance[x][y];

				open.pop_back();
				closed[tmp.eigen_value()] = 1;

				if (x + 1 < 8) if (!bd.is_obstacle(x + 1, y) && !closed[piece::eigen_value(x + 1, y)]) {
					open.emplace_back(x + 1, y);
					distance[x + 1][y] = std::min(distance[x + 1][y], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0) if (!bd.is_obstacle(x - 1, y) && !closed[piece::eigen_value(x - 1, y)]) {
					open.emplace_back(x - 1, y);
					distance[x - 1][y] = std::min(distance[x - 1][y], (uint8_t)(w + 1));
				}
				if (y + 1 < 8) if (!bd.is_obstacle(x, y + 1) && !closed[piece::eigen_value(x, y + 1)]) {
					open.emplace_back(x, y + 1);
					distance[x][y + 1] = std::min(distance[x][y + 1], (uint8_t)(w + 1));
				}
				if (y - 1 >= 0) if (!bd.is_obstacle(x, y - 1) && !closed[piece::eigen_value(x, y - 1)]) {
					open.emplace_back(x, y - 1);
					distance[x][y - 1] = std::min(distance[x][y - 1], (uint8_t)(w + 1));
				}
				if (x + 1 < 8 && y + 1 < 8) if (!bd.is_obstacle(x + 1, y + 1) && !closed[piece::eigen_value(x + 1, y + 1)]) {
					open.emplace_back(x + 1, y + 1);
					distance[x + 1][y + 1] = std::min(distance[x + 1][y + 1], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0 && y + 1 < 8) if (!bd.is_obstacle(x - 1, y + 1) && !closed[piece::eigen_value(x - 1, y + 1)]) {
					open.emplace_back(x - 1, y + 1);
					distance[x - 1][y + 1] = std::min(distance[x - 1][y + 1], (uint8_t)(w + 1));
				}
				if (x + 1 < 8 && y - 1 >= 0) if (!bd.is_obstacle(x + 1, y - 1) && !closed[piece::eigen_value(x + 1, y - 1)]) {
					open.emplace_back(x + 1, y - 1);
					distance[x + 1][y - 1] = std::min(distance[x + 1][y - 1], (uint8_t)(w + 1));
				}
				if (x - 1 >= 0 && y - 1 >= 0) if (!bd.is_obstacle(x - 1, y - 1) && !closed[piece::eigen_value(x - 1, y - 1)]) {
					open.emplace_back(x - 1, y - 1);
					distance[x - 1][y - 1] = std::min(distance[x - 1][y - 1], (uint8_t)(w + 1));
				}
				distance[from.x()][from.y()] = -1;
			}
		}
		//计算特定棋子的移动力
		double _amazon_mobility(size_t player_idx, size_t amazon_idx) {
			double a = 0.0;
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j)
					if (merged_dm_1[1 - player_idx][i][j] != 255 && dm_1[player_idx][amazon_idx][i][j] <= 1)
						a += std::pow(2.0, -dm_2[player_idx][amazon_idx][i][j] + 1) * _empty_neighbor_num(i, j);



			return a;
		}
		//生成移动力（mobility）参量
		double _mobility_ingredient() {
			double m1 = 0, m2 = 0;
			auto f_w_m1 = [this](double m) {return (w < 5 ? 5 : w) * (50.0 - m) / 400.0; };
			auto f_w_m2 = [this, &f_w_m1](double m) {return f_w_m1(m); };
			for (int i = 0; i < 4; ++i)
				m1 += f_w_m1(_amazon_mobility(0, (size_t)i));
			for (int i = 0; i < 4; ++i)
				m2 += f_w_m2(_amazon_mobility(1, (size_t)i));
			return m2 - m1;
		}
		//扁平化二维距离数组
		class _distance_flat_wrapper {
			std::array<distance_matrix_group, 2>* p;
		public:
			_distance_flat_wrapper(std::array<distance_matrix_group, 2>& v) :p(&v) {};
			~_distance_flat_wrapper() = default;
			distance_matrix& operator[](size_t idx) {
				return (*p)[idx / 4][idx % 4];
			}
		};
		_distance_flat_wrapper _flat_dm_1{ dm_1 };
		//只有一项满足
		std::tuple<bool, size_t> _only_one_satisfy(std::function<bool(size_t)> condition) {
			for (int i = 0; i < 8; ++i)
				if (condition(i)) {
					bool satisfy = true;
					for (int j = 0; j < 8; ++j) {
						if (i == j) continue;
						if (condition(j)) goto end;
					}
					return std::make_tuple(true, i);
				}
		end:
			return std::make_tuple(false, 0);
		}
		//计算amazon的独占区域集合
		std::array<std::array<size_t, 4>, 2> _amazon_exclusive_access_num() {
			auto is_reachable_closure = [this](uint8_t x, uint8_t y) {
				return [x, y, this](size_t piece_idx) {
					return _flat_dm_1[piece_idx][x][y] != 255;
				};
			};
			std::array<std::array<size_t, 4>, 2> exclusive_access_num = { 0 };
			for (int i = 0; i < 8; ++i)
				for (int j = 0; j < 8; ++j) {
					auto [found, result] = _only_one_satisfy(is_reachable_closure(i, j));
					if (found) ++exclusive_access_num[result / 4][result % 4];
				}
				return exclusive_access_num;
		}
		//生成守卫（guard）参量
		double _guard_ingredient() {
			long long sum = 0;
			auto res = _amazon_exclusive_access_num();
			for (auto v : res[0]) sum += v;
			for (auto v : res[1]) sum -= v;




			return 0.2 * (sum > 0 ? std::pow(1.1, sum) : -std::pow(1.1, -sum));
		}
	};
}

#pragma endregion