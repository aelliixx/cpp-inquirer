// Copyright © 2023 Donatas Mockus
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the “Software”), to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
// to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
// the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef CPP_INQUIRER_SRC_INQUIRER_H
#define CPP_INQUIRER_SRC_INQUIRER_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <regex>
#include <cassert>

#ifdef _WIN32
#include <conio.h>
#endif

namespace alx {

#define CTRL_KEYPRESS(k) ((k)  & 0x1f)

#ifdef _WIN32
static int const keyDw = 80;
static int const keyUp = 72;
static int const keySx = 75;
static int const keyDx = 77;
static int const keyEnter = 13;
#else
static int const keyUp = 65;
static int const keyDw = 66;
static int const keySx = 68;
static int const keyDx = 67;
static int const keyEnter = 13;
#endif
static int const keyBackspace = 127;

class Inquirer;

enum class Type
{
	text,
	integer,
	decimal,
	yesNo,
	confirm,
	options,
	regex,
	password
};

class Question
{
public:
	Question(const Question& q)
	{
		m_key = q.m_key;
		m_question = q.m_question;
		m_answer = q.m_answer;
		m_type = q.m_type;
		if (m_type == Type::options)
			m_options = q.m_options;
		else if (m_type == Type::regex)
			m_regex = q.m_regex;
	}

	Question(std::string key, std::string question, const Type type = Type::text)
		: m_key(std::move(key)),
		  m_question(std::move(question)),
		  m_type(type) {}

	Question(std::string key, std::string question, std::vector<std::string> options)
		: m_key(std::move(key)),
		  m_question(std::move(question)),
		  m_type(Type::options),
		  m_options(std::move(options))
	{
		if (m_options.empty())
			throw std::runtime_error("Must have one or more options");
	}

	Question(std::string key, std::string question, std::string regex)
		: m_key(std::move(key)),
		  m_question(std::move(question)),
		  m_type(Type::regex),
		  m_regex(std::move(regex)) {}

	[[nodiscard]] std::string ask(const bool askAgainIfAnswered = false)
	{
		if (m_asked && !askAgainIfAnswered) return m_answer;
		auto printQuestion = [&](const std::string& append = "")
		{
		  std::cout << "\033[1m\033[34m?\033[0m \033[1m" << m_question << "\033[0m " << append;
		};

		auto takeInput = [](std::string& destination)
		{
		  std::cout << "\033[34m";
		  if (!std::getline(std::cin, destination))
			  exit(0);
		  std::cout << "\033[0m";
		};

		switch (m_type)
		{
		case Type::confirm:
		{
			printQuestion("(y/N) ");
			std::string answer;
			takeInput(answer);
			while (!(answer == "y" || answer == "Y" || answer == "n" || answer == "N"))
			{
				erase_lines(2);
				printQuestion("(y/N) ");
				takeInput(answer);
			}
			m_answer = answer;
			break;
		}
		case Type::text:
			printQuestion();
			takeInput(m_answer);
			break;
		case Type::integer:
		{
			printQuestion();
			std::string answer;
			takeInput(answer);
			while (!is_integer(answer))
			{
				erase_lines(2);
				printQuestion();
				takeInput(answer);
			}
			m_answer = answer;
			break;
		}
		case Type::decimal:
		{
			printQuestion();
			std::string answer;
			takeInput(answer);
			while (!is_decimal(answer))
			{
				erase_lines(2);
				printQuestion();
				takeInput(answer);
			}
			m_answer = answer;
			break;
		}
		case Type::yesNo:
		{
			const std::string yes = "\033[34myes\033[0m no\n";
			const std::string no = "yes \033[34mno\033[0m\n";
			std::cout << std::flush;
			printQuestion(yes);
			bool position = true;
			while (true)
			{
				const int key = getch();
				if (key == keySx)
				{
					position = true;
					erase_lines(2);
					printQuestion(yes);
				}
				else if (key == keyDx)
				{
					position = false;
					erase_lines(2);
					printQuestion(no);
				}
				if (key == keyEnter)
				{
					m_answer = position ? "yes" : "no";
					break;
				}
			}
			break;
		}
		case Type::options:
		{
			unsigned int selectedIndex = 0;
			auto printOptions = [&]()
			{
			  std::cout << '\n';
			  for (int i = 0; i < m_options.size(); ++i)
			  {
				  if (i == selectedIndex)
					  std::cout << "\033[34m> " << m_options.at(i) << "\033[0m\n";
				  else
					  std::cout << "  " << m_options.at(i) << "\n";
			  }
			};
			printQuestion();
			printOptions();

			while (true)
			{
				const int key = getch();
				if (key == keyDw)
				{
					selectedIndex = wrap_int(selectedIndex + 1, 0, m_options.size() - 1);
					erase_lines(m_options.size() + 2);
					printQuestion();
					printOptions();
				}
				else if (key == keyUp)
				{
					selectedIndex = wrap_int(selectedIndex - 1, 0, m_options.size() - 1);
					erase_lines(m_options.size() + 2);
					printQuestion();
					printOptions();
				}
				if (key == keyEnter)
				{
					m_answer = m_options.at(selectedIndex);
					erase_lines(m_options.size() + 2);
					printQuestion("\033[34m" + m_options.at(selectedIndex) + "\033[0m\n");
					break;
				}
			}
			break;
		}
		case Type::password:
		{
			printQuestion();
			std::string answer;
			int c;
			while (true)
			{
				c = getch();
				if (c == keyEnter)
					break;
				char character = static_cast<char>(c);
				if (c == keyBackspace)
				{
					if (!answer.empty()) 
						answer = answer.substr(0, answer.length() - 1);
					continue;
				}
				answer += character;
			}
			std::cout << '\n';
			m_answer = answer;
			break;
		}
		case Type::regex:
		{
			printQuestion();
			std::string answer;
			takeInput(answer);
			while (!std::regex_match(answer, std::regex(m_regex)))
			{
				erase_lines(2);
				printQuestion();
				takeInput(answer);
			}
			m_answer = answer;
			break;
		}
		}
		m_asked = true;
		return m_answer;
	}

	[[nodiscard]] std::string get_answer() const { return m_answer; }
	
	~Question() {}

private:
	friend Inquirer;
	std::string m_key;
	std::string m_question;
	std::string m_answer;
	Type m_type;
	std::vector<std::string> m_options;
	std::string m_regex;
	bool m_asked = false;

	static void erase_lines(const unsigned count = 1)
	{
		if (count == 0)
			return;

		std::cout << "\x1b[2K"; // Delete current line
		for (int i = 1; i < count; ++i)
		{
			std::cout << "\x1b[1A" // Move cursor one line up
					  << "\x1b[2K"; // Delete current line
		}
		std::cout << '\r';
	}

	static bool is_integer(const std::string& string)
	{
		if (string.empty() || ((!isdigit(string[0])) && (string[0] != '-') && (string[0] != '+'))) return false;
		char* p;
		strtol(string.c_str(), &p, 10);
		return (*p == 0);
	}

	static bool is_decimal(const std::string& string)
	{
		if (string.empty() || ((!isdigit(string[0])) && (string[0] != '-') && (string[0] != '+'))) return false;
		char* p;
		strtod(string.c_str(), &p);
		return (*p == 0);
	}

	static unsigned wrap_int(unsigned int k, const unsigned lowerBound, const unsigned upperBound)
	{
		const unsigned range_size = upperBound - lowerBound + 1;

		if (k < lowerBound)
			k += range_size * ((lowerBound - k) / range_size + 1);

		return lowerBound + (k - lowerBound) % range_size;
	}

	static int getch()
	{
		int c; // This function should return the keystroke without allowing it to echo on screen

#ifdef _WIN32
		c = _getch();
#else
		system("stty raw");    // Raw input - wait for only a single keystroke
		system("stty -echo");  // Echo off
		c = getchar();
		system("stty cooked"); // Cooked input - reset
		system("stty echo");   // Echo on - Reset
#endif
		if (c == CTRL_KEYPRESS('c') || c == CTRL_KEYPRESS('d'))
			exit(0);
		return c;
	}
};

class Inquirer
{
	std::vector<std::pair<std::string, Question>> m_questions;
	std::string m_title;

public:
	explicit Inquirer(std::string title)
		: m_title(std::move(title)) {}

	Question& add_question(const Question& question)
	{
		m_questions.emplace_back(question.m_key, question);
		return m_questions.at(m_questions.size() - 1).second;
	}
	
	Question& get_question(const std::string key)
	{
		auto found = std::find_if(m_questions.begin(), m_questions.end(), [&](std::pair<std::basic_string<char>,
																						Question>& question){
			if (question.second.m_key == key)
				return true;
			return false;
		});
		if (found != m_questions.end())
			return found->second;
		throw std::runtime_error("No question with key: " + key);
	}

	void ask(const bool askAgainIfAnswered = false)
	{
		if (!m_title.empty())
			std::cout << "\033[34m>\033[0m " << m_title << '\n';
		for (auto& question : m_questions)
		{
			(void)question.second.ask(askAgainIfAnswered);
		}
	}

	void print_questions() const
	{
		for (const auto& q : m_questions)
		{
			std::cout << "Key: " << q.second.m_key << ", question: " << q.second.m_question << ", type: ";
			switch (q.second.m_type)
			{
			case Type::text:
				std::cout << "Text\n";
				break;
			case Type::integer:
				std::cout << "Integer\n";
				break;
			case Type::decimal:
				std::cout << "Decimal\n";
				break;
			case Type::yesNo:
				std::cout << "Yes/No\n";
				break;
			case Type::confirm:
				std::cout << "Confirm\n";
				break;
			case Type::options:
				std::cout << "Options\n";
				break;
			case Type::regex:
				std::cout << "Regex\n";
				break;
			case Type::password:
				std::cout << "Password\n";
				break;
			}
		}
	}

	void print_answers() const
	{
		for (const auto& q : m_questions)
			std::cout << q.second.m_question << ": " << q.second.m_answer << '\n';
	}

	std::string answer(const std::string& key) const
	{
		for (const auto& question : m_questions)
			if (question.first == key)
				return question.second.m_answer;
		return "";
	}
};
}
#endif //CPP_INQUIRER_SRC_INQUIRER_H
