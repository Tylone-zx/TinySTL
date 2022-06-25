#ifndef _FUNCTIONAL_H_
#define _FUNCTIONAL_H_

namespace TinySTL{
	//********** [unary_function] ****************
	template <class Arg, class Result>
	struct unary_function {
		typedef Arg argument_type;
		typedef Result result_type;
	};
	//********** [binary_function] ****************
	template <class Arg1, class Arg2, class Result>
	struct binary_function {
		typedef Arg1 first_argument_type;
		typedef Arg2 second_argument_type;
		typedef Result result_type;
	};
	//********** [less] ****************
	template<class T>
	struct less{
		typedef T first_argument_type;
		typedef T second_argument_type;
		typedef bool result_type;

		result_type operator()(const first_argument_type& x, const second_argument_type& y){
			return x < y;
		}
	};
	//********** [equal_to] ****************
	template<class T>
	struct equal_to{
		typedef T first_argument_type;
		typedef T second_argument_type;
		typedef bool result_type;

		result_type operator()(const first_argument_type& x, const second_argument_type& y){
			return x == y;
		}
	};
	//********** [identity] ****************
	template<class T>
	struct identity :public unary_function<T, T> {
		const T& operator()(const T& x) const { return x; }
	};
	//********** [select1st] ****************
	template <class Pair>
	struct select1st : public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type& operator()(const Pair& x) const {
			return x.first;
		}
	};
	//********** [select2nd] ****************
	template <class Pair>
	struct select2nd : public unary_function<Pair, typename Pair::second_type> {
		const typename Pair::second_type& operator()(const Pair& x) const {
			return x.second;
		}
	};
}
#endif