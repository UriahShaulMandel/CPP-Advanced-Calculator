#define _USE_MATH_DEFINES

#include <iostream>
#include <string>
#include <map>
#include <math.h>
#include <algorithm>

using namespace std;

#define DEFAULT_ARGS const string& s, unsigned int idx_start, unsigned int idx_end
#define MATH_FUNCTION(name,statements) inline double name (double a, double b) { \
statements \
}
#define CONST(name,value) pair<string, double>{ pair<string, double>(name,value)},

MATH_FUNCTION(_add, return a + b;)
MATH_FUNCTION(_sub, return a - b;)
MATH_FUNCTION(_mul, return a * b;)
MATH_FUNCTION(_div, return a / b;)
MATH_FUNCTION(_pow, return pow(a, b);)

const map<char, double (*)(double, double)> ACTION_TO_FUNCTION_MAP = map<char, double (*)(double, double)>({
		{'+', _add},
		{'-',_sub},
		{'*',_mul},
		{'/',_div},
		{'^',_pow}
	}
);
const map<char, int> ACTION_TO_PRIORITY_MAP = map<char, int>({
		{'+', 5},
		{'-',5},
		{'*',4},
		{'/',4},
		{'^',3}
	}
);

pair<string, double> CONSTANTS[] = {
		CONST("e",M_E)
};

inline bool is_digit(char c) {
	return c <= '9' && c >= '0';
}

inline bool is_bracket(char c) {
	return c == '(' || c == ')';
}

unsigned int get_idx_of_last_char(DEFAULT_ARGS, char c) {
	for (unsigned int i = idx_end - 1; i >= idx_start; i--) {
		if (i == -1) return idx_start;

		if (s[i] == c)
			return i + 1;
	}
	return idx_start;
}

unsigned int get_idx_of_first_char(DEFAULT_ARGS, char c) {
	for (unsigned int i = idx_start; i < idx_end; i++) {
		if (s[i] == c)
			return i - 1;
	}
	return idx_end;
}


bool is_number(DEFAULT_ARGS) {
	if (idx_start == idx_end)
		return 0;
	bool dot_flag = false;
	for (int i = idx_start; i < idx_end; i++) {
		if (!is_digit(s[i]) && !is_bracket(s[i]))
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
	string actual_s = string(s);

	actual_s.erase(std::remove(actual_s.begin(), actual_s.end(), '('), actual_s.end());
	actual_s.erase(std::remove(actual_s.begin(), actual_s.end(), ')'), actual_s.end());
	for (const auto& pr : ACTION_TO_FUNCTION_MAP) {
		actual_s.erase(std::remove(actual_s.begin(), actual_s.end(), pr.first), actual_s.end());
	}


	for (int i = 0; i < size(CONSTANTS); i++) {
		if (actual_s == CONSTANTS[i].first)
			return CONSTANTS[i].second;
	}
	return NAN;
}


int find_action_idx(DEFAULT_ARGS) {
	unsigned int tmp_idx_low = idx_start, tmp_level_low = -1, stk = 0, priority = 1;
	char tmp_char = 0;
	for (unsigned int i = idx_end - 1; i >= idx_start; i--) {
		if (i == -1)
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


double solve_eq(DEFAULT_ARGS) {
	unsigned int idx_of_first_char = get_idx_of_first_char(s, idx_start, idx_end, ')');
	unsigned int idx_of_last_char = get_idx_of_last_char(s, idx_start, idx_end, '(');

	if (str_contains_action(s, idx_start, idx_end)) {
		unsigned int action_idx = find_action_idx(s, idx_start, idx_end);
		char c = s[action_idx];
		return ACTION_TO_FUNCTION_MAP.at(c)(solve_eq(s, idx_start, action_idx), solve_eq(s, action_idx + 1, idx_end));
	}
	else {
		if (is_number(s, idx_start, idx_end)) {
			return stod(s.substr(idx_of_last_char, idx_of_first_char));
		}
		else {/* TODO - fix constants
			double _const = parse_constant(s.substr(idx_of_last_char, idx_of_first_char));
			if (!isnan(_const)) {
				return _const;
			}
			*/
		}
	}
}


double solve_eq(string & s) {
	try {
		return solve_eq(s, 0, s.length());
	}
	catch (exception e) {
		throw e;
	}
}


int main()
{
	string s;

	while (1) {
		cout << "Please enter a mathematical statement. (+, -, *, /, ^, () and numbers)" << endl;
		cin >> s;
		try {
			cout << "Result: " << solve_eq(s, 0, s.length()) << endl << endl;
		}
		catch (exception e) {
			cout << e.what() << endl;
		}
	}
}