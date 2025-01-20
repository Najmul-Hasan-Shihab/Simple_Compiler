#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cctype>

class SimpleCompiler
{
public:
    void compile(const std::string& code)
    {
        std::istringstream stream(code);
        std::string line;
        while (std::getline(stream, line))
        {
            parseLine(line);
        }
    }

private:
    std::unordered_map<std::string, int> variables;

    void parseLine(const std::string& line)
    {
        std::string trimmedLine = trim(line);

        // skip empty lines or lines with only semicolons
        if (trimmedLine.empty() || trimmedLine == ";") return;

        // handle var declarations and assignments
        if (trimmedLine.find("let") != std::string::npos)
        {
            size_t eqPos = trimmedLine.find("=");
            std::string varName = trimmedLine.substr(4, eqPos - 5); // get variable name
            std::string valueStr = trimmedLine.substr(eqPos + 2);  // get value

            valueStr = trim(valueStr);  // trim extra space
            valueStr = removeSemicolons(valueStr); // remove ending semicolons

            int value = evaluateExpression(valueStr);
            if (value != -1)
            {
                variables[varName] = value;
                std::cout << "Variable " << varName << " set to " << value << std::endl;
            }
        }
        // handle if-else
        else if (trimmedLine.find("if") != std::string::npos)
        {
            handleIfCondition(trimmedLine);
        }
        // handle print stmnt
        else if (trimmedLine.find("print") != std::string::npos)
        {
            handlePrintStatement(trimmedLine);
        }
        // handle exit
        else if (trimmedLine.find("exit") != std::string::npos)
        {
            std::cout << "Exiting the program." << std::endl;
            exit(0);
        }
    }

    void handleIfCondition(const std::string& line)
    {
        size_t conditionStart = line.find("(") + 1;
        size_t conditionEnd = line.find(")");
        std::string condition = line.substr(conditionStart, conditionEnd - conditionStart);

        // find operator in condition
        size_t operatorPos = condition.find(">");
        if (operatorPos == std::string::npos)
        {
            operatorPos = condition.find("<");
        }

        if (operatorPos != std::string::npos)
        {
            // find var/val on both side of operator
            std::string var1 = trim(condition.substr(0, operatorPos)); // left
            std::string var2 = trim(condition.substr(operatorPos + 1)); // right

            // evaluate
            bool conditionMet = evaluateCondition(var1, var2, condition[operatorPos]);

            if (conditionMet)
            {
                std::cout << "Condition met: " << var1 << " " << condition[operatorPos] << " " << var2 << std::endl;
                // reinitialize variable c
                variables["c"] = variables[var1]; // assign to c
            }
            else
            {
                std::cout << "Condition not met: " << var1 << " " << condition[operatorPos] << " " << var2 << std::endl;
                // reinitialize variable c
                variables["c"] = variables[var2]; // assign to c
            }
        }
    }

    void handlePrintStatement(const std::string& line)
    {
        // Find the position of the variable name after the "print " keyword
        size_t varPos = line.find("print") + 6; // Skip "print "
        std::string varName = trim(line.substr(varPos));

        // remove ending semicolon
        varName = removeSemicolons(varName);

        // print value of var
        if (variables.find(varName) != variables.end())
        {
            std::cout << variables[varName] << std::endl;
        }
        else
        {
            std::cerr << "Error: Variable " << varName << " not found!" << std::endl;
        }
    }

    bool evaluateCondition(const std::string& var1, const std::string& var2, char op)
    {
        if (variables.find(var1) != variables.end() && variables.find(var2) != variables.end())
        {
            int value1 = variables[var1];
            int value2 = variables[var2];

            switch (op)
            {
            case '>':
                return value1 > value2;
            case '<':
                return value1 < value2;
            default:
                return false;
            }
        }
        return false;
    }

    // evaluate mathematical expressions
    int evaluateExpression(const std::string& expr)
    {
        size_t operatorPos = expr.find_first_of("+-*/");

        if (operatorPos != std::string::npos)
        {
            std::string left = trim(expr.substr(0, operatorPos));
            std::string right = trim(expr.substr(operatorPos + 1));

            // check if both sides are variables or numbers
            int leftValue = getValue(left);
            int rightValue = getValue(right);

            if (leftValue != -1 && rightValue != -1)
            {
                char op = expr[operatorPos];
                switch (op)
                {
                case '+':
                    return leftValue + rightValue;
                case '-':
                    return leftValue - rightValue;
                case '*':
                    return leftValue * rightValue;
                case '/':
                    return leftValue / rightValue;
                default:
                    return -1;
                }
            }
        }
        else
        {
            // if no operator is found, the expression is single value
            return getValue(expr);
        }
        return -1;
    }

    // get the value of a variable or a number
    int getValue(const std::string& str)
    {
        // if it's a variable, get its value
        if (variables.find(str) != variables.end())
        {
            return variables[str];
        }
        // if it's a number, convert it to an integer
        try
        {
            return std::stoi(str);
        }
        catch (const std::invalid_argument&)
        {
            std::cerr << "Error: Invalid argument: " << str << std::endl;
            return -1;
        }
    }

    // trim leading/trailing spaces
    std::string trim(const std::string& str)
    {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    // remove trailing semicolons
    std::string removeSemicolons(const std::string& str)
    {
        size_t pos = str.find(";");
        if (pos != std::string::npos)
        {
            return str.substr(0, pos); // remove semicolon
        }
        return str;
    }
};

int main()
{
    std::string code = R"(
let a = 5;
let b = 3;
let c = 0;

if (a > b) {
    c = a;
} else {
    c = b;
}

print c;

exit(0);
)";

    SimpleCompiler compiler;
    compiler.compile(code);

    return 0;
}
