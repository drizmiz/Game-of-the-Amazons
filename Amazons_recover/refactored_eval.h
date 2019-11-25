
#include "std.h"

// #define THROW_EXCEPTION

namespace yrq_new 
{
  using namespace std;
  ofstream ofs("records.log");

  void unexpect(bool condition, string msg) {
#ifdef _DEBUG
    if (condition) {
      ofs << "unexpected condition: " << msg << endl;
#ifdef THROW_EXCEPTION 
      throw exception(msg.c_str());
#endif
    }
#endif
  }
  void append_log(string msg, bool always = false) {
    if (always) ofs << msg << endl;
    else {
#ifdef _DEBUG
      ofs << msg << endl;
#endif
    }
  }
  class teil {
  public:
    enum class type : char { empty, black, white, arrow };
    bool is_empty() const { return _t == type::empty; }
    bool is_black() const { return _t == type::black; }
    bool is_white() const { return _t == type::white; }
    bool is_arrow() const { return _t == type::arrow; }
    void set_empty() { _t = type::empty; }
    void set_black() { _t = type::black; }
    void set_white() { _t = type::white; }
    void set_arrow() { _t = type::arrow; }
    type get_type() { return _t; }
    void set_type(type t) { _t = t; }
    teil() :_t(type::empty) {}
    teil(type t) :_t(t) {}
    ~teil() {}
  private:
    type _t;
  };
  class move {
    tuple<int, int> _from, _to, _arrow;
  public:
    move() {
      _from = { -1,-1 };
      _to = { -1,-1 };
      _arrow = { -1 ,-1 };
    }
    move(int fx, int fy, int tx, int ty, int ax, int ay) {
      _from = { fx,fy };
      _to = { tx,ty };
      _arrow = { ax ,ay };
    }
    move(tuple<int, int> f, tuple<int, int> t, tuple<int, int> a) {
      _from = f;
      _to = t;
      _arrow = a;
    }
    ~move() {}
    const tuple<int, int>& from() const {
      return _from;
    }
    const tuple<int, int>& to() const {
      return _to;
    }
    const tuple<int, int>& arrow() const {
      return _arrow;
    }
  private:
  };
  class player {
  public:
    using single_player = tuple<int, int>[4];
    player(bool is_black) :_is_black(is_black) {}
    ~player() {}
    single_player& self() {
      return players[1 - _is_black];
    }
    const single_player& self() const {
      return players[1 - _is_black];
    }
    single_player& opponent() {
      return players[_is_black];
    }
    const single_player& opponent() const {
      return players[_is_black];
    }
    bool id() const { return !_is_black; }
    void make_move(move mv) {
      _check_move_valid(mv);
      for (int i = 0; i < 4; ++i) {
        if (self()[i] == mv.from())
          self()[i] = mv.to();
        if (opponent()[i] == mv.from())
          opponent()[i] = mv.to();
      }
    }
    void undo_move(move mv) {
      _check_undo_move_valid(mv);
      for (int i = 0; i < 4; ++i) {
        if (self()[i] == mv.to())
          self()[i] = mv.from();
        if (opponent()[i] == mv.to())
          opponent()[i] = mv.from();
      }
    }
    void make_moves(vector<move> mvs) {
      for (const auto& mv : mvs)
        make_move(mv);
    }
  private:
    void _check_move_valid(const move& mv) const {
#ifdef _DEBUG
      auto between_0_8 = [](int v) {return v >= 0 && v < 8; };
      auto [fx, fy] = mv.from();
      unexpect(!between_0_8(fx) || !between_0_8(fy), "move::from out of range");
      auto [tx, ty] = mv.to();
      unexpect(!between_0_8(tx) || !between_0_8(ty), "move::to out of range");
      auto [ax, ay] = mv.arrow();
      unexpect(!between_0_8(ax) || !between_0_8(ay), "move::arrow out of range");
      bool from_is_one_amazon = false;
      for (int i = 0; i < 4; ++i)
        if (self()[i] == mv.from() || opponent()[i] == mv.from())
          from_is_one_amazon = true;
      unexpect(!from_is_one_amazon, "from is not a movable amazon");
#endif
    }
    void _check_undo_move_valid(const move& mv) const {
#ifdef _DEBUG
      auto between_0_8 = [](int v) {return v >= 0 && v < 8; };
      auto [fx, fy] = mv.from();
      unexpect(!between_0_8(fx) || !between_0_8(fy), "move::from out of range");
      auto [tx, ty] = mv.to();
      unexpect(!between_0_8(tx) || !between_0_8(ty), "move::to out of range");
      auto [ax, ay] = mv.arrow();
      unexpect(!between_0_8(ax) || !between_0_8(ay), "move::arrow out of range");
      bool to_is_one_amazon = false;
      for (int i = 0; i < 4; ++i)
        if (self()[i] == mv.to() || opponent()[i] == mv.to())
          to_is_one_amazon = true;
      unexpect(!to_is_one_amazon, "to is not a movable amazon");
#endif
    }
  private:
    bool _is_black;
    single_player players[2] = { {{0,2},{2,0},{5,0},{7,2}},{{0,5},{2,7},{5,7},{7,5}} };
  };
  class board {
    friend teil;
  public:
    board() {}
    ~board() {}
    teil& operator()(int x, int y) {
      return _locate_teil({ x,y });
    }
    const teil& operator()(int x, int y) const {
      return _locate_teil({ x,y });
    }
    void check_consistency(const player& pl) {
#ifdef _DEBUG
      for (const auto& p : pl.self()) {
        auto [x, y] = p;
        if ((*this)(x, y).get_type() == teil::type::black ? 0 : 1 != pl.id()) {
          throw exception("consistency broken");
        }
      }
      for (const auto& p : pl.opponent()) {
        auto [x, y] = p;
        if ((*this)(x, y).get_type() == teil::type::black ? 0 : 1 == pl.id()) {
          throw exception("consistency broken");
        }
      }
#endif
    }
    void _output_board() {
      auto f = [](teil::type t) {
        switch (t) {
        case teil::type::empty:
          return "O";
          break;
        case teil::type::black:
          return "B";
          break;
        case teil::type::white:
          return "W";
          break;
        case teil::type::arrow:
          return "A";
          break;
        default:
          return "?";
        }
      };
      string str = "\n+----BOARD----+\n";
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
          str += f(_map[j][i].get_type()) + string(" ");
        str += "\n";
      }
      append_log(str, true);
    }
    void make_move(move mv) {
      _check_move_valid(mv);
      auto t = _locate_teil(mv.from()).get_type();
      _locate_teil(mv.from()).set_empty();
      _locate_teil(mv.to()).set_type(t);
      _locate_teil(mv.arrow()).set_arrow();
      _count_amazons();
    }
    void undo_move(move mv) {
      _check_undo_move_valid(mv);
      auto t = _locate_teil(mv.to()).get_type();
      _locate_teil(mv.to()).set_empty();
      _locate_teil(mv.arrow()).set_empty();
      _locate_teil(mv.from()).set_type(t);
      _count_amazons();
    }
    void make_moves(vector<move> mvs) {
      for (const auto& mv : mvs) {
        make_move(mv);
      }
    }
  private:
    teil& _locate_teil(const tuple<int, int>& xy) {
      auto [x, y] = xy;
      return _map[x][y];
    }
    const teil& _locate_teil(const tuple<int, int>& xy) const {
      auto [x, y] = xy;
      return _map[x][y];
    }
    void _count_amazons() const {
      size_t w = 0, b = 0;
      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
          if (_map[i][j].is_white()) ++w;
          else if (_map[i][j].is_black()) ++b;
      unexpect(w < 4, "white amazons less than 4");
      unexpect(b < 4, "black amazons less than 4");
    }
    void _check_move_valid(const move& mv) const {
#ifdef _DEBUG
      auto between_0_8 = [](int v) {return v >= 0 && v < 8; };
      auto [fx, fy] = mv.from();
      unexpect(!between_0_8(fx) || !between_0_8(fy), "move::from out of range");
      auto [tx, ty] = mv.to();
      unexpect(!between_0_8(tx) || !between_0_8(ty), "move::to out of range");
      auto [ax, ay] = mv.arrow();
      unexpect(!between_0_8(ax) || !between_0_8(ay), "move::arrow out of range");
      unexpect(!(_locate_teil(mv.from()).is_black() || _locate_teil(mv.from()).is_white()), "object to be move is not an amazon");
      unexpect(!_locate_teil(mv.to()).is_empty(), "amazon target is not empty");
      unexpect(mv.arrow() != mv.from() && !_locate_teil(mv.arrow()).is_empty(), "arrow target is not empty");
#endif
    }
    void _check_undo_move_valid(const move& mv) const {
#ifdef _DEBUG
      auto between_0_8 = [](int v) {return v >= 0 && v < 8; };
      auto [fx, fy] = mv.from();
      unexpect(!between_0_8(fx) || !between_0_8(fy), "move::from out of range");
      auto [tx, ty] = mv.to();
      unexpect(!between_0_8(tx) || !between_0_8(ty), "move::to out of range");
      auto [ax, ay] = mv.arrow();
      unexpect(!between_0_8(ax) || !between_0_8(ay), "move::arrow out of range");
      unexpect(!(_locate_teil(mv.to()).is_black() || _locate_teil(mv.to()).is_white()), "object to be undo move is not an amazon");
      unexpect(mv.from() != mv.arrow() && !_locate_teil(mv.from()).is_empty(), "amazon to be undo move doesn't have an empty source");
      unexpect(!_locate_teil(mv.arrow()).is_arrow(), "arrow to be undo placement is not exist");
#endif
    }
  private:
    teil _map[8][8] = {
      teil::type::empty , teil::type::empty ,teil::type::black ,teil::type::empty ,teil::type::empty ,teil::type::white ,teil::type::empty ,teil::type::empty,
      teil::type::empty , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty,
      teil::type::black , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::white,
      teil::type::empty , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty,
      teil::type::empty , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty,
      teil::type::black , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::white,
      teil::type::empty , teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty ,teil::type::empty,
      teil::type::empty , teil::type::empty ,teil::type::black ,teil::type::empty ,teil::type::empty ,teil::type::white ,teil::type::empty ,teil::type::empty
    };
  };

  class evaluator {
    using distance_matrix = uint8_t[8][8];
    using distance_matrix_group = array<distance_matrix, 4>;
  public:
    evaluator(const player& pl, const board& bd) :_pl(pl), _bd(bd) {
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
    double evaluate() {
      double r = 0;
      _generate_distance_matrix();
      _output_board();
      double t = _territory_ingredient();
      double m = _mobility_ingredient();
      double d = _distribution_ingredient();
      r = t + m + d;
      append_log("t m d r:" + to_string(t) + " " + to_string(m) + " " + to_string(d) + " " + to_string(r), true);
      return r;
    }
    void _output_board() {
#ifdef _DEBUG
      string str = "+----------DM1S----------+\n";
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
          str += to_string(merged_dm_1[0][j][i]) + std::string(4 - to_string(merged_dm_1[0][j][i]).size(), ' ');
        str += "\n";
      }
      append_log(str);
      str = "+----------DM1O----------+\n";
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
          str += to_string(merged_dm_1[1][j][i]) + std::string(4 - to_string(merged_dm_1[1][j][i]).size(), ' ');
        str += "\n";
      }
      append_log(str);
      str = "+----------DM2S----------+\n";
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
          str += to_string(merged_dm_2[0][j][i]) + std::string(4 - to_string(merged_dm_2[0][j][i]).size(), ' ');
        str += "\n";
      }
      append_log(str);
      str = "+----------DM2O----------+\n";
      for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j)
          str += to_string(merged_dm_2[1][j][i]) + std::string(4 - to_string(merged_dm_2[1][j][i]).size(), ' ');
        str += "\n";
      }
      append_log(str);
#endif
    }
  private:
    void _generate_distance_matrix() {
      int idx = 0;
      for (auto& m : _pl.self())
        _single_queen_min_moves(m, dm_1[0][idx++]);
      idx = 0;
      for (auto& m : _pl.opponent())
        _single_queen_min_moves(m, dm_1[1][idx++]);
      idx = 0;
      for (auto& m : _pl.self())
        _single_king_min_moves(m, dm_2[0][idx++]);
      idx = 0;
      for (auto& m : _pl.opponent())
        _single_king_min_moves(m, dm_2[1][idx++]);

      _merge_distance_matrix(merged_dm_1[0], dm_1[0]);
      _merge_distance_matrix(merged_dm_1[1], dm_1[1]);
      _merge_distance_matrix(merged_dm_2[0], dm_2[0]);
      _merge_distance_matrix(merged_dm_2[1], dm_2[1]);
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
    tuple<double, double, double> _t1_c1_w() {
      double t1 = 0, c1 = 0, w = 0;
      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
          if (!_bd(i, j).is_empty()) continue;
          t1 += _territory_determine_delta(merged_dm_1[0][i][j], merged_dm_1[1][i][j]);
          c1 += pow(2.0, -merged_dm_1[0][i][j]) - pow(2.0, -merged_dm_1[1][i][j]);
          w += pow(2.0, -abs(merged_dm_1[0][i][j] - merged_dm_1[1][i][j]));
        }
      c1 *= 2;
      this->w = w;
      return { t1, c1, w };
    }
    tuple<double, double>  _t2_c2() {
      double t2 = 0, c2 = 0;
      for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j) {
          if (!_bd(i, j).is_empty()) continue;
          t2 += _territory_determine_delta(merged_dm_2[0][i][j], merged_dm_2[1][i][j]);
          c2 += min(1.0, max(-1.0, (double)(merged_dm_2[1][i][j] - merged_dm_2[0][i][j]) / 6.0));
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
          if (merged_dm_1[1 - player_idx][i][j] != 255 && dm_1[player_idx][amazon_idx][i][j] <= 1)
            a += pow(2.0, -dm_2[player_idx][amazon_idx][i][j] + 1) * _empty_neighbor_num(i, j);
      return a;
    }
    double _territory_ingredient() {
      auto [t1, c1, w] = _t1_c1_w();
      auto [t2, c2] = _t2_c2();
      // f(t1,w) = [ 0.75 * 1.1 ^ (-w) + 0.25 ] * t1
      auto f_w_t1 = [=](double v) { return (0.75 * pow(1.1, -w) + 0.25) * v; };
      // f(t2,w) = [ 0.08 * sqrt( max { w-1 , 0 } ) ] * t2
      auto f_w_t2 = [=](double v) { return (0.08 * sqrt(w - 1 > 0 ? w - 1 : 0))* v; };
      // f(c1,w) = [ 1 - f_t1(w) - f_t2(w) ] * [ 0.6 * 1.1 ^ (-w) + 0.4 ] * c1
      auto f_w_c1 = [=](double v) { return (1 - 0.75 * pow(1.1, -w) - 0.25 - 0.08 * sqrt(w - 1 > 0 ? w - 1 : 0))* (0.4 + 0.6 * pow(1.1, -w))* v; };
      // f(c2,w) = [ 1 - f_t1(w) - f_t2(w) ] * [ 0.6 - 0.6 * 1.1 ^ (-w) ] * c2
      auto f_w_c2 = [=](double v) { return (1 - 0.75 * pow(1.1, -w) - 0.25 - 0.08 * sqrt(w - 1 > 0 ? w - 1 : 0))* (1 - (0.4 + 0.6 * pow(1.1, -w)))* v; };
      return f_w_t1(t1) + f_w_c1(c1) + f_w_t2(t2) + f_w_c2(c2);
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
      return w / 20.0 * (d1 - d2);
    }

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
  private:
    const player& _pl;
    const board& _bd;
    double w = 0.0;
    distance_matrix_group dm_1[2];
    distance_matrix_group dm_2[2];
    distance_matrix merged_dm_1[2];
    distance_matrix merged_dm_2[2];
  };
  class greedy_searcher {
  public:
    greedy_searcher(board& bd, player& pl) : _bd(bd), _pl(pl) {}
    move search_and_select() {
      move mv;
      double current_evaluation = -INFINITY;
      for (const auto& amazon : _pl.self()) {
        auto res = _possible_moves(amazon);
        for (const auto& each_move : res) {
          _bd.make_move(each_move);
          _bd._output_board();
          _pl.make_move(each_move);
          evaluator ev(_pl, _bd);
          double evaluation = ev.evaluate();
          _bd.undo_move(each_move);
          _pl.undo_move(each_move);
          if (evaluation > current_evaluation) {
            current_evaluation = evaluation;
            mv = each_move;
          }
        }
      }
      return mv;
    }
  private:
    vector<move> _possible_moves(tuple<int, int> amazon) {
      vector<move> mvs;
      mvs.reserve(32);
      auto [x, y] = amazon;
      for (int i = x + 1; i < 8; ++i) {
        if (!_bd(i, y).is_empty()) break;
        mvs.push_back({ { x,y }, { i,y }, { -1,-1 } });
      }
      for (int i = x - 1; i >= 0; --i) {
        if (!_bd(i, y).is_empty()) break;
        mvs.push_back({ { x,y }, { i,y }, { -1,-1 } });
      }
      for (int i = y + 1; i < 8; ++i) {
        if (!_bd(x, i).is_empty()) break;
        mvs.push_back({ { x,y }, { x,i }, { -1,-1 } });
      }
      for (int i = y - 1; i >= 0; --i) {
        if (!_bd(x, i).is_empty()) break;
        mvs.push_back({ { x,y }, { x,i }, { -1,-1 } });
      }
      for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) {
        if (!_bd(i, j).is_empty()) break;
        mvs.push_back({ { x,y }, { i,j }, { -1,-1 } });
      }
      for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
        if (!_bd(i, j).is_empty()) break;
        mvs.push_back({ { x,y }, { i,j }, { -1,-1 } });
      }
      for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
        if (!_bd(i, j).is_empty()) break;
        mvs.push_back({ { x,y }, { i,j }, { -1,-1 } });
      }
      for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
        if (!_bd(i, j).is_empty()) break;
        mvs.push_back({ { x,y }, { i,j }, { -1,-1 } });
      }
      return _possible_arrow_placements(mvs);
    }
    vector<move> _possible_arrow_placements(const vector<move>& mvs) {
      vector<move> res;
      res.reserve(1024);
      for (const auto& amazon : mvs) {
        auto [x, y] = amazon.to();
        for (int i = x + 1; i < 8; ++i) {
          if (amazon.from() != make_tuple(i, y) && !_bd(i, y).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,y } });
        }
        for (int i = x - 1; i >= 0; --i) {
          if (amazon.from() != make_tuple(i, y) && !_bd(i, y).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,y } });
        }
        for (int i = y + 1; i < 8; ++i) {
          if (amazon.from() != make_tuple(x, i) && !_bd(x, i).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { x,i } });
        }
        for (int i = y - 1; i >= 0; --i) {
          if (amazon.from() != make_tuple(x, i) && !_bd(x, i).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { x,i } });
        }
        for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) {
          if (amazon.from() != make_tuple(i, j) && !_bd(i, j).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,j } });
        }
        for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
          if (amazon.from() != make_tuple(i, j) && !_bd(i, j).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,j } });
        }
        for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
          if (amazon.from() != make_tuple(i, j) && !_bd(i, j).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,j } });
        }
        for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
          if (amazon.from() != make_tuple(i, j) && !_bd(i, j).is_empty()) break;
          res.push_back({ amazon.from(),amazon.to(), { i,j } });
        }
      }
      return res;
    }
    board& _bd;
    player& _pl;
  };
}