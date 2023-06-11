#include <Stmt_methods.hpp>


//--------------------------------------------RUN_STATEMENTS------------------------------------------------


int If::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int res;
	int condition = lhs_->calculate(istr, ostr);

	if (condition)
	{
		res = rhs_->calculate(istr, ostr);
	}
	else
	{
		if (else_)
		{
			res = else_->calculate(istr, ostr);
		}
	}

	return res;
}


int While::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int condition = lhs_->calculate(istr, ostr);

	while (condition)
	{
		rhs_->calculate(istr, ostr);

		if (RETURN_COMMAND)
			break;

		condition = lhs_->calculate(istr, ostr);
	}

	return 0;
}


int Print::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int val = lhs_->calculate(istr, ostr);

	ostr << val << std::endl;

	return val;
}


int Return::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	if (!IN_FUNCTION)
	{
		throw_exception("You can return only from functions\n", lhs_->get_num() - 1);
	}

	RETURN_COMMAND = 1;

	int val = lhs_->calculate(istr, ostr);

	return val;
}


int Arithmetic::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	int res = lhs_->calculate(istr, ostr);

	return res;
}


int Declaration::run_stmt(std::istream &istr, std::ostream &ostr) const
{
	CURR_SCOPE->init_func(func_->short_name(), this_.lock());

	return 0;
}


//-------------------------------------------------NAMES----------------------------------------------------


std::string If::name() const
{
	return "if";
}

std::string While::name() const
{
	return "while";
}

std::string Print::name() const
{
	return "print";
}

std::string Return::name() const
{
	return "return";
}

std::string Arithmetic::name() const
{
	return lhs_->short_name();
}

std::string Declaration::name() const
{
	return func_->short_name();
}

