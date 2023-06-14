#ifndef LEX_ARRAY_T_H
#define LEX_ARRAY_T_H


#include "Lexer.hpp"


class Lex_array_t {

	int curr_lex_ = 0;
	int num_str_ = 1;
	std::vector<std::shared_ptr<Lex_t>> lex_array_;


public:
	Lex_array_t(std::istream &istr);

	int get_num_curr_lex() const { return curr_lex_; }
	void inc_lex();
	void set_curr_lex(int n);

	std::shared_ptr<Lex_t> operator[](int n) const;
	std::shared_ptr<Lex_t> get_elem(int n) const;

	std::shared_ptr<Lex_t> get_curr_lex() const { return lex_array_[curr_lex_]; }
	std::shared_ptr<Lex_t> get_prev_lex() const { return lex_array_[curr_lex_ - 1]; }

	void print() const;
	int size() const { return lex_array_.size(); }
	void clear();

	void push_binop(BinOp_t binop);
	void push_unop(Statements_t unop);
	void push_brace(Brace_t brace);
	void push_scope(Scope_t scope);
	void push_stmt(Statements_t kw);
	void push_var(int num_var);
	void push_function(int num_func);
	void push_value(int value);
	void push_compop(CompOp_t compop);
	void push_symbol(Symbols_t symbol);
	void push_bad_symbol(int symbol);

	int is_plus_minus() const;  //These functions check if the CURRENT (curr_lex_) token is ..
	int is_mul_div() const;	    //If need to check the token number n, then first use "set_curr_lex(n)"
	int is_brace() const;
	int is_compop() const;
	int is_scope() const;
	int is_unop() const;

	bool is_negation() const;
	bool is_semicol() const;
	bool is_binop() const;
	bool is_assign() const;
	bool is_return() const;
	bool is_comma() const;
	bool is_colon() const;
	bool is_scan() const;
	bool is_else() const;

};


#endif
