#include <iostream>
#include "inquirer.h"
#include <memory>

int main()
{
	auto inquirer = alx::Inquirer("cpp-inquirer example");
	inquirer.add_question({ "query", "What do you want to do?" });
	inquirer.add_question({ "birthday", "Is this for a birthday?", alx::Type::yesNo });
	inquirer.add_question({ "candles", "How many candles do you want?", alx::Type::integer });
	inquirer.add_question({ "type", "What kind of a cake would you like?",
							 std::vector<std::string>{ "Chocolate", "Ice-cream", "Cheesecake", "Red velvet" }});
	inquirer.add_question({ "delivery", "Is this for delivery?", alx::Type::confirm });
	inquirer.add_question({ "number", "Enter your contact details", "\\d{9}" });
	inquirer.add_question({"password", "Enter your password", alx::Type::password});
	inquirer.ask();
	std::cout << "------------\n";
	inquirer.print_questions();
	inquirer.print_answers();
	return 0;
}
