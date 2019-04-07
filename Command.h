#pragma once
#include <string>
#include <map>
#include<fstream>




class Lexer
{
public:
	std::map<std::string, int> Command_map
	{
		{ "ASSIGN", 0 },
		{ "PRINT", 1 },
		{ "CMP", 2 },
		{ "JUMP", 3 },
		{ "JZ", 4 },
		{ "JNZ", 5 },
		{ "JO", 6 },
		{ "JNO", 7 },
		{ "JC", 8 },
		{ "JNC", 9 },
		{ "ADD", 10 },
		{ "SUB", 11 },
		{ "MUL", 12 },
		{ "DIV", 13 },
		{ "MOV", 14 },
	};

	char* reg_vector;

	void is_command(std::string buffer)
	{
		for (std::map<std::string, int>::iterator it = Command_map.begin(); it != Command_map.end(); ++it)
			if (it->first == buffer)
		Lex_map.insert(std::make_pair(" Command",buffer));
	}

	void is_reg(std::string buffer)
	{
		if (reg_vector == buffer)
			Lex_map.insert(std::make_pair("Register", buffer));
	}

	void is_num(std::string buffer)
	{

	}

	void _Lexer(std::string)
	{
		std::fstream file;
		file.open("Assembly_file.txt", 
			std::fstream::in |
			std::fstream::out |
			std::fstream::app );
		for (int i = 0; i < out.size(); ++i)
		{

		}
	}

private:
	std::map<std::string, std::string> Lex_map
};
