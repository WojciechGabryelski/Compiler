#ifndef COMPILEP_HPP
#define COMPILER_HPP
#include <string>
#include <map>
#include <list>
#include <stack>

struct Procedure {
    std::string name;
    long long start_line;
    long long arg_num;
    long long arg_beg;
    bool* will_be_assigned;
    bool* must_be_assigned;
};
struct Main {
    long long start_line;
    std::list<std::string>* instructions;
};
struct Argument {
    long long index;
    std::list<std::string> instructions;
};
struct Value {
    bool isNumber;
    std::string content;
};
struct Jump {
    std::string* instruction;
    long long lines_to_jump;
};
struct Condition {
    std::list<std::string>* instructions;
    std::stack<Jump> jumps;
};
struct Variable {
    long long pointer;
    bool assigned;
};

std::list<std::string>* multCode();
std::list<std::string>* divCode(unsigned long long prev_instr_size);
std::list<std::string>* write(Value* value, long long lineno);
std::list<std::string>* read(std::string name, long long lineno);
std::list<std::string>* get(Value* value, long long lineno);
std::list<std::string>* add(Value* first, Value* second, long long lineno);
std::list<std::string>* sub(Value* first, Value* second, long long lineno);
std::list<std::string>* mul(std::string var, Value* first, Value* second, long long lineno);
std::list<std::string>* divmod(std::string var, Value* first, Value* second, bool isMod, long long lineno);
Condition* eq(Value* first, Value* second, long long lineno);
Condition* neq(Value* first, Value* second, long long lineno);
Condition* gtr(Value* first, Value* second, long long lineno);
Condition* lss(Value* first, Value* second, long long lineno);
Condition* geq(Value* first, Value* second, long long lineno);
Condition* leq(Value* first, Value* second, long long lineno);
std::list<std::string>* ifStatement(Condition* cond, std::list<std::string>* instructions);
std::list<std::string>* ifElseStatement(Condition* cond, std::list<std::string>* instr_true, std::list<std::string>* instr_false);
std::list<std::string>* whileLoop(Condition* cond, std::list<std::string>* instructions);
std::list<std::string>* repeatUntil(Condition* cond, std::list<std::string>* instructions);
std::list<std::string>* call(std::string name, std::list<std::string> args, long long lineno);
std::list<std::string>* assign(std::string name, std::list<std::string>* instructions, long long lineno);
void declare(std::string name, long long lineno);
Procedure* declareProcedure(std::string name, long long lineno);
Main* declareMain(std::list<std::string>* instructions);
std::list<std::string>* finishProcedure(Procedure* proc, std::list<std::string>* prev_instr, std::list<std::string>* instructions);
void finishProgram(std::list<std::string>* proc_instr, Main* mainObject);
void prepare_then();
void prepare_else();

#endif // COMPILER_HPP
