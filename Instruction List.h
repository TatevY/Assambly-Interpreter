#pragma once

#include "Commands.h"
#include "Instruction.h"
#include <string>
#include <regex>
#include <cassert>
#include <charconv>
#include "String to tokens.h"
#include <forward_list>
#include<algorithm>
#include <stdexcept>

using TInstructionTable = std::vector<std::unique_ptr<BaseInstruction>>;


class InstructionList
{
public:

	template <class... TArgs>
	static std::unique_ptr<BaseInstruction> get_instruction(const std::string& command, TArgs&&... args)
	{
		const auto instruction_type_it = s_string_enum_map.find(command);
		if (instruction_type_it == s_string_enum_map.end())
		{
			auto inst = std::make_unique<InstructionLabel>();
			s_label_to_name_map[inst.get()] = command;
			return inst;
		}


		switch ((*instruction_type_it).second)
		{
		case Command::assign:
			return create_instruction_assign(args...);
		case Command::print:
			return create_instruction_print(to_size_t(args)...);
		case Command::cmp:
			return std::make_unique<InstructionCmp>(to_size_t(args)...);
		case Command::jump:
			return create_instruction_jump<InstructionJump>(args...);
		case Command::jz:
			return create_instruction_jump<InstructionJz>(args...);
		case Command::jnz:
			return create_instruction_jump<InstructionJnz>(args...);
		case Command::jo:
			return create_instruction_jump<InstructionJo>(args...);
		case Command::jno:
			return create_instruction_jump<InstructionJno>(args...);
		case Command::jc:
			return create_instruction_jump<InstructionJc>(args...);
		case Command::jnc:
			return create_instruction_jump<InstructionJnc>(args...);
		case Command::add:
			return std::make_unique<InstructionAdd>(to_size_t(args)...);
		case Command::sub:
			return std::make_unique<InstructionSub>(to_size_t(args)...);
		case Command::mul:
			return std::make_unique<InstructionMul>(to_size_t(args)...);
		case Command::div:
			return std::make_unique<InstructionDiv>(to_size_t(args)...);
		case Command::mov:
			return std::make_unique<InstructionMov>(to_size_t(args)...);
		case Command::call:
			return create_instruction_jump<InstructionCall>(args...);
		case Command::ret:
			return std::make_unique<InstructionRet>();
		case Command::end:
			return std::make_unique<InstructionEnd>();
		default:
			assert(false);
		}
		return {};
	}



	static TInstructionTable generate_program(const std::vector<std::string>& code)
	{
		TInstructionTable program;
		std::transform(std::begin(code), std::end(code), std::back_insert_iterator{ program }
			, [](const auto& line)
		{
			std::cout << line << std::endl;
			const auto tokens = tokenizer(line);
			std::unique_ptr<BaseInstruction> instruction = nullptr;
			if (auto size = tokens.size(); size == 3)
			{
				instruction = get_instruction(tokens[0], tokens[1], tokens[2]);
			}
			else if (size == 2)
			{
				instruction = get_instruction(tokens[0], tokens[1], "");
			}
			else if (size == 1)
			{
				instruction = get_instruction(tokens[0], "", "");
			}

			return (instruction);
			});
		make_jamps(program);
		return program;
	}


	static void make_jamps(TInstructionTable& program)
	{
		for (auto i = 0u; i < program.size(); ++i)
		{
			auto label_to_name_pair = s_label_to_name_map.find(static_cast<Label*>(program[i].get()));
			if (label_to_name_pair == s_label_to_name_map.end())
			{
				continue;
			}

			auto[label, name] = *label_to_name_pair;
			s_label_to_name_map.erase(static_cast<Label*>(program[i].get()));
			auto name_to_jumps_pair = s_label_name_to_label_map.find(name);

			if (name_to_jumps_pair == s_label_name_to_label_map.end())
			{
				continue;
			}

			auto[n, jumps] = *name_to_jumps_pair;
			for (auto ji : jumps)
			{
				ji->set_ip(i);
			}
			s_label_name_to_label_map.erase(name);
		}

		if (!s_label_name_to_label_map.empty() || !s_label_to_name_map.empty())
		{
			throw std::invalid_argument{ "Invalid Jump or label" };
		}

		s_label_name_to_label_map.clear();
		s_label_to_name_map.clear();
	}



private:
	static std::unique_ptr<BaseInstruction> create_print(const std::size_t id, const std::size_t arg)
	{
		std::ignore = arg;
		return std::unique_ptr<BaseInstruction>(std::make_unique<Print>(id));
	}

	static std::unique_ptr<BaseInstruction> create_assign(const std::string& id, const std::string& value)
	{
		return std::unique_ptr<BaseInstruction>(std::make_unique<Assign>(to_size_t(id), to_int_t(value)));
	}


	template <class T, class... TArgs>
	static std::unique_ptr<BaseInstruction> create_jump(const std::string& label_name, TArgs&... arg)
	{
		auto inst = std::make_unique<T>();
		s_label_name_to_label_map[label_name].push_front(inst.get());
		return inst;
	}


	static std::size_t to_size_t(const std::string& num)
	{
		auto result = 0u;
		if (auto[p, ec] = std::from_chars(num.data(), num.data() + num.length(), result); ec != std::error_code())
		{
			return result;
		}
		throw std::logic_error{ "is not a size_t" };
	}



	static int to_int_t(const std::string& num)
	{
		auto result = 0;
		if (auto[p, ec] = std::from_chars(num.data(), num.data() + num.length(), result); ec != std::error_code())
		{
			return result;
		}

		throw std::logic_error{ "is not a size_t" };
	}




	static double to_double(const std::string& num)
	{
		return std::stod(num);
	}


private:
  static  std::unordered_map<std::string, std::forward_list<JumpInstructions*>> s_label_name_to_label_map;
  static  std::unordered_map<Label*, std::string> s_label_to_name_map;

  const std::unordered_map<std::string, Command> s_string_enum_map
	  {
		{ "ASSIGN", Command::assign },
		{ "PRINT", Command::print },
		{ "CMP", Command::cmp },
		{ "JUMP", Command::jump },
		{ "JZ", Command::jz },
		{ "JNZ", Command::jnz },
		{ "JO", Command::jo },
	    { "JNO", Command::jno },
		{ "JC", Command::jc },
		{ "JNC", Command::jnc },
		{ "ADD", Command::add },
		{ "SUB", Command::sub },
		{ "MUL", Command::mul },
		{ "DIV", Command::div },
        { "AND", Command::and_ },
		{ "OR", Command::or_ },
		{ "XOR", Command::xor_ },
		{ "NOT", Command::not_ },
		{ "MOV", Command::mov },
		{ "CALL", Command::call},
		{ "RET", Command::ret },
		{ "END", Command::end },
	  };
}



