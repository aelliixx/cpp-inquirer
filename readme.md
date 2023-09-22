# C++ Inquirer

A single header C++11 library for interactive command line prompts.

# Use

Simply include the header:
```cpp
#include "inquirer.h"
```
And initialise the inquirer object (with optional title):
```cpp
auto inquirer = alx::Inquirer("cpp-inquirer example");
```

To get the answers back, call `inquirer.answer("some key");` with the key of your question.
You will get a `std::string` as a value, so you will need to convert it to an approapriate type yourself.

# Propmts

## Text

![](assets/text-input.png)

```cpp
inquirer.add_question({ "query", "What do you want to do?" });
```

## Yes/No

![](assets/yesNo-input.png)

```cpp
inquirer.add_question({ "birthday", "Is this for a birthday?", alx::Type::yesNo });
```
## Integer or decimal

![](assets/int-input.png)

```cpp
inquirer.add_question({ "candles", "How many candles do you want?", alx::Type::integer });
// Or
inquirer.add_question({ "candles", "How many candles do you want?", alx::Type::decimal });
```

## Select

![](assets/options-input.png)

```cpp
inquirer.add_question({ "type", "What kind of a cake would you like?",
		        std::vector<std::string>{ "Chocolate", "Ice-cream", "Cheesecake", "Red velvet" }});
```

## Confirm

![](assets/confirm-input.png)

```cpp
inquirer.add_question({ "delivery", "Is this for delivery?", alx::Type::confirm });
```

## Regex validated input

![](assets/regex-input.png)

```cpp
inquirer.add_question({ "number", "Enter your contact details:", "\\d{9}" });
```

# Contributing

I'm more than happy to accept pull requests with some minor requirements:

- Prefix your commit messages with a relevant change type. If the commit relates to a GitHub issue, include the issue number in the message. Example:
> Types: added a new question type 'yes/no'
> 
> This addresses issue #123

or

> Answers: added a way to return an answer with the correct type

- Please follow the naming convention already established in the code. There is no clangformat, but I'm sure you can deduce it yourself.
