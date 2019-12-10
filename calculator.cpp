#define _USE_MATH_DEFINES

#include <iostream>
#include <string>
#include <map>
#include <math.h>

using namespace std;

#define DEFAULT_ARGS const string& s, unsigned int idx_start, unsigned int idx_end
#define MATH_FUNCTION(name,statements) inline double name (double a, double b) { \
statements \
}
#define EQUATION_IS_EMPTY_ID 0
#define NOT_A_NUMBER_NOT_A_CONST_ID 1

MATH_FUNCTION(_add, return a + b;)
MATH_FUNCTION(_sub, return a - b;)
MATH_FUNCTION(_mul, return a * b;)
MATH_FUNCTION(_div, return a / b;)
MATH_FUNCTION(_pow, return pow(a, b);)


class calculator_exeption : invalid_argument {
public:
	int id;
	calculator_exeption(int);
	string get_message() {
		if (this->id == EQUATION_IS_EMPTY_ID)
			return "Equation is empty!";
		else if (this->id == NOT_A_NUMBER_NOT_A_CONST_ID)
			return "Not a number nor a const!";
	}
};
calculator_exeption::calculator_exeption(int id) :invalid_argument("Syntax Error") { //Initializer list must be used 
	this->id = id;
}

const map<char, double (*)(double, double)> ACTION_TO_FUNCTION_MAP = map<char, double (*)(double, double)>(
	{
		{'+', _add},
		{'-',_sub},
		{'*',_mul},
		{'/',_div},
		{'^',_pow}
	}
);

const map<char, int> ACTION_TO_PRIORITY_MAP = map<char, int>(
	{
		{'+', 5},
		{'-',5},
		{'*',4},
		{'/',4},
		{'^',3}
	}
);

const map<string, double> CONSTANTS = map<string, double>(
	{
		{"e",M_E},
		{"pi",M_PI},
	}
);

inline string my_substr(const string & str, unsigned int idx_start, unsigned int idx_end) {
	return str.substr(idx_start, idx_end - idx_start);
}

inline bool is_digit(char c) {
	return c <= '9' && c >= '0';
}

bool is_number(DEFAULT_ARGS) {
	bool dot_flag = false;
	for (int i = idx_start; i < idx_end; i++) {
		if (!is_digit(s[i]))
			if (s[i] == '.' && !dot_flag) {
				dot_flag = true;
			}
			else {
				return false;
			}
	}
	return true;
}

double parse_constant(const string & s) {
	if (CONSTANTS.count(s))
		return CONSTANTS.at(s);
	return NAN;
}

int find_action_idx(DEFAULT_ARGS) {
	unsigned int tmp_idx_low = 0, tmp_level_low = -1, stk = 0, priority = 1;
	char tmp_char = 0;
	for (int i = idx_end - 1; i >= idx_start; i--) {
		if (i == idx_start - 1)
			break;
		tmp_char = s[i];
		if (tmp_char == ')') stk++;
		else if (tmp_char == '(') stk--;
		else {
			if (ACTION_TO_FUNCTION_MAP.count(tmp_char)) {
				if ((stk < tmp_level_low)
					||
					((stk == tmp_level_low || tmp_level_low == -1) && (priority < ACTION_TO_PRIORITY_MAP.at(tmp_char))))
				{
					tmp_level_low = stk;
					tmp_idx_low = i;
					priority = ACTION_TO_PRIORITY_MAP.at(tmp_char);
				}
			}
		}
	}
	if (tmp_level_low == -1) {
		throw "no action was found!";
	}
	return tmp_idx_low;
}

bool str_contains_action(DEFAULT_ARGS) {
	for (int i = idx_start; i < idx_end; i++) {
		if (ACTION_TO_FUNCTION_MAP.count(s[i]) > 0)
			return true;
	}
	return false;
}

bool remove_redundent_brackets(DEFAULT_ARGS) {
	signed int stk = 1;
	bool not_flag = false;
	char tmp_char = 0;
	for (unsigned int i = idx_start + 1; i < idx_end; i++) {
		tmp_char = s[i];
		if (tmp_char == ')') stk--;
		else if (tmp_char == '(') stk++;
		if (stk == 0 && i != idx_end - 1) {
			return false;
		}
	}
	return (stk == 0 && s[idx_end - 1] == ')') || not_flag;
}


double solve_eq(DEFAULT_ARGS) {
	if (idx_start == idx_end)
		throw calculator_exeption(EQUATION_IS_EMPTY_ID);

	while (s[idx_start] == '(') {
		if (remove_redundent_brackets(s, idx_start, idx_end)) {
			idx_start++;
			idx_end--;
		}
		else break;
	}
	if (str_contains_action(s, idx_start, idx_end)) {
		const unsigned int action_idx = find_action_idx(s, idx_start, idx_end);
		const char c = s[action_idx];
		double a, b;
		try {
			a = solve_eq(s, idx_start, action_idx);
		}
		catch (calculator_exeption e) {
			if (e.id == EQUATION_IS_EMPTY_ID && c == '-')
				a = 0;
			else
				throw e;
		}
		b = solve_eq(s, action_idx + 1, idx_end);
		double (*f)(double, double) = ACTION_TO_FUNCTION_MAP.at(c);
		return f(a, b);
	}
	else {
		if (is_number(s, idx_start, idx_end)) {
			return stod(my_substr(s, idx_start, idx_end));
		}
		else {
			double _const = parse_constant(my_substr(s, idx_start, idx_end));
			if (!isnan(_const)) {
				return _const;
			}
		}
		throw calculator_exeption(NOT_A_NUMBER_NOT_A_CONST_ID);
	}
}




int main()
{
	cout << "Advanced Calculator - Uriah Shaul Mandel" << endl;
	cout << "This calculator follows order of arithmetic operations" << endl;
	cout << "The operetors supported: +, -, *, /, ^, ()" << endl;
	cout << "The consts supported: e, pi" << endl;
	cout << "Have fun :)" << endl << endl << endl;

	string s;

	while (1) {
		cout << "Please enter a statement: ";
		cin >> s;
		try {
			cout << "Result: " << solve_eq(s, 0, s.length()) << endl << endl;
		}
		catch (calculator_exeption e) {
			cout << e.get_message() << endl;
		}
	}
}
