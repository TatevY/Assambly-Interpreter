#pragma once

#include <vector>
#include <regex>
#include "CPU.h"
#include "Instruction List.h"


class Interpreter
{

public:
	Interpreter(const std::string& file_name);
	~Interpreter()=default;



private:
	void init(const std::string& file_name);
	void interpreter();



private:
	CPU m_cpu;
	TInstructionTable m_instruction_table;
};