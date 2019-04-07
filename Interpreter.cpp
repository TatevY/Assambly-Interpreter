#include "Interpreter.h"
#include "Instruction List.h"
#include "File.h"

Interpreter::Interpreter(const std::string & file_name)
{
	init(file_name);
	interpreter();
}



void Interpreter::init(const std::string & file_name)
{
	File srl(file_name);
	const auto code = srl.read_in();
	InstructionList instruction_list;
	m_instruction_table = instruction_list.generate_program(code);


}

void Interpreter::interpreter()
{
	while (m_cpu.m_IP < m_instruction_table.size())
	{
		const auto current_ip = m_cpu.m_IP;
		++m_cpu.m_IP;
		m_instruction_table[current_ip]->execute(m_cpu);
	}

}
