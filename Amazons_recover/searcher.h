
#pragma once

// #ifndef __GNUC__
// #include <algorithm>		// for sort
// #include <chrono>		// for clock
// #endif

#include "game.h"
#include "eval.h"

namespace amz
{
	enum class phase
	{
		hash,
		gen_moves,
		rest
	};

	// 走法排序
	class chess_game::mm_sort
	{
	public:
		movement mm_hash;
		phase cur_phase;
		int mm_index;
		std::vector<movement> mms;

		chess_game& cg_parent;
		mm_sort(chess_game& cg, movement _mm_hash) noexcept :
			cg_parent(cg), mm_hash(_mm_hash), cur_phase(phase::hash), mms{}
		{}

		const movement next();
	};

	// 得到下一个走法
	const movement chess_game::mm_sort::next()
	{
		switch (cur_phase)
		{
			// 不加break是故意的
			// 置换表启发
		case phase::hash:
			cur_phase = phase::gen_moves;
			if (!_Is_dft_move(mm_hash))
				return mm_hash;
			// 生成所有着法
		case phase::gen_moves:
			cur_phase = phase::rest;
			mms = std::move(cg_parent._Get_all_possible_moves());
			std::sort(mms.begin(), mms.end(),
				[this](movement mm1, movement mm2) {
					return cg_parent.rt.probe_history(mm1) >
						cg_parent.rt.probe_history(mm2);
				}
			);
			mm_index = 0;
			// 历史表启发
		case phase::rest:
			for (; mm_index < mms.size();)
			{
				movement mm = mms.at(mm_index);
				mm_index++;
				if (mm != mm_hash)
					return mm;
			}
			// 没有着法了
		default:
			return dft_movement;
		}
	}

	eval_t chess_game::_Alphabeta(int depth, eval_t alpha, eval_t beta)
	{
		using namespace std::chrono;
		// 置换表查询
		const auto [_val, mm_hash] = rt.probe_hash(this->get_status(), depth, alpha, beta);
		if (_val != unknown)
			return _val;
		// 超时
		auto end = steady_clock::now();
		auto diff = duration_cast<std::chrono::milliseconds>(end - starttime).count();

		if (diff > deadspan)
		{

			return inf;



		}
		if (diff > timespan)
		{
			timespan = 0;
			return _Evaluate(this->get_status(), this->get_color());
		}
		// 到达深度
		if (depth == 0)
		{
			const eval_t val = _Evaluate(this->get_status(), this->get_color());
			// rt.record_hash(this->get_status(), , depth, val, node_f::pv);
			return val;
		}
		// 初始化搜索
		node_f flag = node_f::alpha;
		eval_t eval_max = -inf;
		movement mm_best = dft_movement;

		mm_sort mmsrt(*this, mm_hash);

		for (movement mm; !_Is_dft_move(mm = mmsrt.next());)
		{
			make_move(mm);
			// PVS
			eval_t eval;
			if (eval_max == -inf)
				eval = -_Alphabeta(depth - 1, -beta, -alpha);
			else
			{
				eval = -_Alphabeta(depth - 1, -alpha - 1, -alpha);
				if ((eval > alpha) && (eval < beta))
					eval = -_Alphabeta(depth - 1, -beta, -alpha);
			}
			unmake_move(mm);

			if (eval > eval_max)
			{
				eval_max = eval;
				if (eval >= beta)
				{
					flag = node_f::beta;
					mm_best = mm;
					break;
					// rt.record_hash(this->get_status(), , depth, beta, node_f::beta);
					// return val;
				}
				if (eval > alpha)
				{
					flag = node_f::pv;
					mm_best = mm;
					alpha = eval;
				}
			}
		}
		// 记录置换表
		rt.record_hash(this->get_status(), mm_best, depth, eval_max, flag);
		if (!_Is_dft_move(mm_best)) // not alpha node
		{
			rt.record_history(mm_best, depth);
		}
		return eval_max;
	}

	movement chess_game::_Root_search(int depth, eval_t alpha, eval_t beta)
	{
		movement mm_hash = dft_movement;
		// 初始化搜索
		node_f flag = node_f::alpha;
		eval_t eval_max = -inf;
		movement mm_best = dft_movement;

		mm_sort mmsrt(*this, mm_hash);

		for (movement mm; !_Is_dft_move(mm = mmsrt.next());)
		{
			make_move(mm);
			// PVS
			eval_t eval;
			if (eval_max == -inf)
				eval = -_Alphabeta(depth - 1, -beta, -alpha);
			else
			{
				eval = -_Alphabeta(depth - 1, -alpha - 1, -alpha);
				if ((eval > alpha) && (eval < beta))
					eval = -_Alphabeta(depth - 1, -beta, -alpha);
			}
			unmake_move(mm);

			if (eval > eval_max)
			{
				eval_max = eval;
				if (eval >= beta)
				{
					flag = node_f::beta;
					mm_best = mm;
					break;
					// rt.record_hash(this->get_status(), , depth, beta, node_f::beta);
					// return val;
				}
				if (eval > alpha)
				{
					flag = node_f::pv;
					mm_best = mm;
					alpha = eval;
				}
			}
		}
		// 记录置换表
		rt.record_hash(this->get_status(), mm_best, depth, eval_max, flag);
		// 搜索根节点时，总有mm_best(因为全窗口搜索不会超出边界)
		rt.record_history(mm_best, depth);
		return mm_best;
	}
		movement chess_game::_Search_till_timeout() // unit: ms
	{
		using namespace std::chrono;
		this->rt.clear();

		movement mm_best = dft_movement;
		eval_t eval = _Evaluate(this->get_status(), this->get_color());
		for (int i = 1; ; ++i)
		{
			if constexpr (!false)
				mm_best = _Root_search(i, -inf, inf);
			else {
				mm_best = _Root_search(i, -inf, inf, eval);
			}
			if (timespan == 0)
			{



				break;
			}
		}
		if (_Is_dft_move(mm_best))
		{

			// 一层贪心
			movement mm_hash = dft_movement;
			// 初始化搜索
			node_f flag = node_f::alpha;
			eval_t eval_max = -inf;

			mm_sort mmsrt(*this, mm_hash);

			for (movement mm; !_Is_dft_move(mm = mmsrt.next());)
			{
				make_move(mm);
				const eval_t cur_eval = -_Evaluate(this->get_status(), this->get_color());
				if (eval_max <= cur_eval)
				{
					eval_max = cur_eval;
					mm_best = mm;
				}
				unmake_move(mm);
			}
		}
		return mm_best;
	}
}