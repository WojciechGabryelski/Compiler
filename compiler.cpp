#include "compiler.hpp"
#include <iostream>
#include <string>
#include <list>
#include <stack>
#include <map>

using namespace std;

map<string, Variable> declared;
map<string, Procedure*> proc_map;
long long mem = 7;
long long instr_num = 1;
long long proc_begin;
bool is_proc = false;
Procedure* curr_proc;
bool mul_occured = false;
bool div_occured = false;
stack<stack<string>* > assigned_after_cond;

list<string>* multCode() {
    list<string>* ptr = new list<string>();
    //ptr->push_back("[mnozenie]");
    ptr->push_back("SET 0");
    ptr->push_back("STORE 3");
    ptr->push_back("LOAD 2");
    ptr->push_back("JZERO 20");
    ptr->push_back("STORE 4");
    ptr->push_back("HALF");
    ptr->push_back("STORE 2");
    ptr->push_back("ADD 0");
    ptr->push_back("STORE 5");
    ptr->push_back("LOAD 4");
    ptr->push_back("SUB 5");
    ptr->push_back("JZERO 16");
    ptr->push_back("LOAD 3");
    ptr->push_back("ADD 1");
    ptr->push_back("STORE 3");
    ptr->push_back("LOAD 1");
    ptr->push_back("ADD 0");
    ptr->push_back("STORE 1");
    ptr->push_back("JUMP 3");
    ptr->push_back("JUMPI 6");
    return ptr;
}

list<string>* divCode(unsigned long long prev_instr_size) {
    list<string>* ptr = new list<string>();
    //ptr->push_back("[dzielenie]");
    ptr->push_back("SET 0");
    ptr->push_back("STORE 3");
    ptr->push_back("LOAD 2");
    ptr->push_back("JZERO " + to_string(40 + prev_instr_size));
    ptr->push_back("SET 1");
    ptr->push_back("STORE 4");
    ptr->push_back("LOAD 2");
    ptr->push_back("STORE 5");
    ptr->push_back("LOAD 1");
    ptr->push_back("ADD 4");
    ptr->push_back("STORE 1");
    ptr->push_back("LOAD 1");
    ptr->push_back("SUB 2");
    ptr->push_back("JZERO " + to_string(19 + prev_instr_size));
    ptr->push_back("LOAD 2");
    ptr->push_back("ADD 0");
    ptr->push_back("STORE 2");
    ptr->push_back("JUMP " + to_string(12 + prev_instr_size));
    ptr->push_back("LOAD 2");
    ptr->push_back("SUB 5");
    ptr->push_back("JZERO " + to_string(36 + prev_instr_size));
    ptr->push_back("LOAD 3");
    ptr->push_back("ADD 0");
    ptr->push_back("STORE 3");
    ptr->push_back("LOAD 2");
    ptr->push_back("HALF");
    ptr->push_back("STORE 2");
    ptr->push_back("LOAD 1");
    ptr->push_back("SUB 2");
    ptr->push_back("JZERO " + to_string(19 + prev_instr_size));
    ptr->push_back("STORE 1");
    ptr->push_back("SET 1");
    ptr->push_back("ADD 3");
    ptr->push_back("STORE 3");
    ptr->push_back("JUMP " + to_string(19 + prev_instr_size));
    ptr->push_back("LOAD 1");
    ptr->push_back("SUB 4");
    ptr->push_back("STORE 1");
    ptr->push_back("JUMPI 6");
    ptr->push_back("SET 0");
    ptr->push_back("STORE 1");
    ptr->push_back("JUMPI 6");
    return ptr;
}

list<string>* write(Value* value, long long lineno) {
    list<string>* ptr = new list<string>();
    if (value->isNumber) {
        ptr->push_back("SET " + value->content);
        ptr->push_back("PUT 0");
    } else {
        if (declared.find(value->content) == declared.end()) {
            cerr << "Error: undefined variable " << value->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[value->content].pointer;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            ptr->push_back("LOADI " + to_string(i));
            ptr->push_back("PUT 0");
        } else {
            if (!declared[value->content].assigned) {
                cerr << "Error: variable " << value->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            ptr->push_back("PUT " + to_string(i));
        }
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* read(string name, long long lineno) {
    if (declared.find(name) == declared.end()) {
        cerr << "Error: undefined variable " << name << " at line " << lineno << "\n";
        exit(1);
    }
    list<string> *ptr = new list<string>();
    long long i = declared[name].pointer;
    if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
        if (!assigned_after_cond.empty() && !curr_proc->will_be_assigned[i - curr_proc->arg_beg]
          && !curr_proc->must_be_assigned[i - curr_proc->arg_beg]) {
            stack<string> *s = assigned_after_cond.top();
            s->push(name);
        }
        curr_proc->will_be_assigned[i - curr_proc->arg_beg] = true;
        ptr->push_back("LOAD " + to_string(i));
        ptr->push_back("GET 0");
    } else {
        if (!assigned_after_cond.empty() && !declared[name].assigned) {
            stack<string> *s = assigned_after_cond.top();
            s->push(name);
        }
        declared[name].assigned = true;
        ptr->push_back("GET " + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* get(Value* value, long long lineno) {
    list<string>* ptr = new list<string>();
    if (value->isNumber) {
        ptr->push_back("SET " + value->content);
    } else {
        if (declared.find(value->content) == declared.end()) {
            cerr << "Error: undefined variable " << value->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[value->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[value->content].assigned) {
                cerr << "Error: variable " << value->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* add(Value* first, Value* second, long long lineno) {
    list<string>* ptr = new list<string>();
    if (second->isNumber) {
        ptr->push_back("SET " + second->content);
        ptr->push_back("STORE 1");
    }
    if (first->isNumber) {
        ptr->push_back("SET " + first->content);
    } else {
        if (declared.find(first->content) == declared.end()) {
            cerr << "Error: undefined variable " << first->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[first->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[first->content].assigned) {
                cerr << "Error: variable " << first->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    if (second->isNumber) {
        ptr->push_back("ADD 1");
    } else {
        if (declared.find(second->content) == declared.end()) {
            cerr << "Error: undefined variable " << second->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[second->content].pointer;
        string add_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            add_type = "ADDI ";
        } else {
            if (!declared[second->content].assigned) {
                cerr << "Error: variable " << second->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            add_type = "ADD ";
        }
        ptr->push_back(add_type + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* sub(Value* first, Value* second, long long lineno) {
    list<string>* ptr = new list<string>();
    if (second->isNumber) {
        ptr->push_back("SET " + second->content);
        ptr->push_back("STORE 1");
    }
    if (first->isNumber) {
        ptr->push_back("SET " + first->content);
    } else {
        if (declared.find(first->content) == declared.end()) {
            cerr << "Error: undefined variable " << first->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[first->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[first->content].assigned) {
                cerr << "Error: variable " << first->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    if (second->isNumber) {
        ptr->push_back("SUB 1");
    } else {
        if (declared.find(second->content) == declared.end()) {
            cerr << "Error: undefined variable " << second->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[second->content].pointer;
        string sub_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            sub_type = "SUBI ";
        } else {
            if (!declared[second->content].assigned) {
                cerr << "Error: variable " << second->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            sub_type = "SUB ";
        }
        ptr->push_back(sub_type + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* mul(string var, Value* first, Value* second, long long lineno) {
    mul_occured = true;
    list<string>* ptr = new list<string>();
    if (first->isNumber) {
        ptr->push_back("SET " + first->content);
    } else {
        if (declared.find(first->content) == declared.end()) {
            cerr << "Error: undefined variable " << first->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[first->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[first->content].assigned) {
                cerr << "Error: variable " << first->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    ptr->push_back("STORE 1");
    if (second->isNumber) {
        ptr->push_back("SET " + second->content);
    } else {
        if (declared.find(second->content) == declared.end()) {
            cerr << "Error: undefined variable " << second->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[second->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[second->content].assigned) {
                cerr << "Error: variable " << second->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    ptr->push_back("STORE 2");
    if (declared.find(var) == declared.end()) {
        cerr << "Error: undefined variable " << var << " at line " << lineno << "\n";
        exit(1);
    }
    ptr->push_back("cSET 3");
    ptr->push_back("STORE 6");
    ptr->push_back("JUMP 1");
    ptr->push_back("LOAD 3");
    declared[var].assigned = true;
    long long i = declared[var].pointer;
    if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
        curr_proc->will_be_assigned[i - curr_proc->arg_beg] = true;
        ptr->push_back("STOREI " + to_string(i));
    } else {
        ptr->push_back("STORE " + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

list<string>* divmod(string var, Value* first, Value* second, bool isMod, long long lineno) {
    div_occured = true;
    list<string>* ptr = new list<string>();
    if (first->isNumber) {
        ptr->push_back("SET " + first->content);
    } else {
        if (declared.find(first->content) == declared.end()) {
            cerr << "Error: undefined variable " << first->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[first->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[first->content].assigned) {
                cerr << "Error: variable " << first->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    ptr->push_back("STORE 1");
    if (second->isNumber) {
        ptr->push_back("SET " + second->content);
    } else {
        if (declared.find(second->content) == declared.end()) {
            cerr << "Error: undefined variable " << second->content << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[second->content].pointer;
        string load_type;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (!curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            load_type = "LOADI ";
        } else {
            if (!declared[second->content].assigned) {
                cerr << "Error: variable " << second->content << " might not be initialized at line " << lineno << "\n";
                exit(1);
            }
            load_type = "LOAD ";
        }
        ptr->push_back(load_type + to_string(i));
    }
    ptr->push_back("STORE 2");
    if (declared.find(var) == declared.end()) {
        cerr << "Error: undefined variable " << var << " at line " << lineno << "\n";
        exit(1);
    }
    ptr->push_back("cSET 3");
    ptr->push_back("STORE 6");
    ptr->push_back("eJUMP 1");
    string result = isMod ? "1" : "3";
    ptr->push_back("LOAD " + result);
    declared[var].assigned = true;
    long long i = declared[var].pointer;
    if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
        curr_proc->will_be_assigned[i - curr_proc->arg_beg] = true;
        ptr->push_back("STOREI " + to_string(i));
    } else {
        ptr->push_back("STORE " + to_string(i));
    }
    instr_num += ptr->size();
    return ptr;
}

Condition* eq(Value* first, Value* second, long long lineno) {
    Condition* cond = new Condition();
    if (first->isNumber && second->isNumber) {
        cond->instructions = new list<string>();
        if (first->content != second->content) {
            cond->instructions->push_back("cJUMP ");
            cond->jumps.push(Jump{&(cond->instructions->back()), 1});
            instr_num++;
        }
        return cond;
    }
    cond->instructions = sub(first, second, lineno);
    list<string> sec_sub = *sub(second, first, lineno);
    cond->instructions->push_back("cJPOS ");
    cond->jumps.push(Jump{&(cond->instructions->back()), (long long) sec_sub.size() + 2});
    cond->instructions->splice(cond->instructions->end(), sec_sub);
    cond->instructions->push_back("cJPOS ");
    cond->jumps.push(Jump{&(cond->instructions->back()), 1});
    instr_num += 2;
    return cond;
}

Condition* neq(Value* first, Value* second, long long lineno) {
    Condition* cond = new Condition();
    if (first->isNumber && second->isNumber) {
        cond->instructions = new list<string>();
        if (first->content == second->content) {
            cond->instructions->push_back("cJUMP ");
            Jump* jump = new Jump();
            cond->jumps.push(Jump{&(cond->instructions->back()), 1});
            instr_num++;
        }
        return cond;
    }
    cond->instructions = sub(first, second, lineno);
    list<string> sec_sub = *sub(second, first, lineno);
    cond->instructions->push_back("cJPOS " + to_string(sec_sub.size() + 2));
    cond->instructions->splice(cond->instructions->end(), sec_sub);
    cond->instructions->push_back("cJZERO ");
    cond->jumps.push(Jump{&(cond->instructions->back()), 1});
    instr_num += 2;
    return cond;
}

Condition* gtr(Value* first, Value* second, long long lineno) {
    Condition* cond = new Condition();
    if (first->isNumber && second->isNumber) {
        cond->instructions = new list<string>();
        if (first->content <= second->content) {
            cond->instructions->push_back("cJUMP ");
            cond->jumps.push(Jump{&(cond->instructions->back()), 1});
            instr_num++;
        }
        return cond;
    }
    cond->instructions = sub(first, second, lineno);
    cond->instructions->push_back("cJZERO ");
    cond->jumps.push(Jump{&(cond->instructions->back()), 1});
    instr_num++;
    return cond;
}

Condition* lss(Value* first, Value* second, long long lineno) {
    return gtr(second, first, lineno);
}

Condition* geq(Value* first, Value* second, long long lineno) {
    Condition* cond = new Condition();
    cond->instructions = new list<string>();
    if (second->isNumber) {
        if (first->isNumber) {
            if (first->content < second->content) {
                cond->instructions->push_back("cJUMP ");
                cond->jumps.push(Jump{&(cond->instructions->back()), 1});
                instr_num++;
            }
        }
        return cond;
    }
    cond->instructions = sub(second, first, lineno);
    cond->instructions->push_back("cJPOS ");
    cond->jumps.push(Jump{&(cond->instructions->back()), 1});
    instr_num++;
    return cond;
}

Condition* leq(Value* first, Value* second, long long lineno) {
    return geq(second, first, lineno);
}

list<string>* ifStatement(Condition* cond, list<string>* instructions) {
    stack<string> *s = assigned_after_cond.top();
    assigned_after_cond.pop();
    while (!s->empty()) {
        declared[s->top()].assigned = false;
        s->pop();
    }
    delete s;
    while (!cond->jumps.empty()) {
        Jump jump = cond->jumps.top();
        cond->jumps.pop();
        jump.lines_to_jump += instructions->size();
        *(jump.instruction) += to_string(jump.lines_to_jump);
    }
    //cond->instructions->push_front("[if]");
    //cond->instructions->push_back("[then]");
    cond->instructions->splice(cond->instructions->end(), *instructions);
    //cond->instructions->push_back("[endif]");
    return cond->instructions;
}

list<string>* ifElseStatement(Condition* cond, list<string>* instr_true, list<string>* instr_false) {
    stack<string> *tmp = assigned_after_cond.top();
    assigned_after_cond.pop();
    stack<string> *s = assigned_after_cond.top();
    assigned_after_cond.pop();
    while (!s->empty()) {
        declared[s->top()].assigned = false;
        s->pop();
    }
    delete s;
    while (!tmp->empty()) {
        declared[tmp->top()].assigned = !declared[tmp->top()].assigned;
        tmp->pop();
    }
    delete tmp;
    while (!cond->jumps.empty()) {
        Jump jump = cond->jumps.top();
        cond->jumps.pop();
        jump.lines_to_jump += instr_true->size() + 1;
        *(jump.instruction) += to_string(jump.lines_to_jump);
    }
    //cond->instructions->push_front("[if]");
    //cond->instructions->push_back("[then]");
    cond->instructions->splice(cond->instructions->end(), *instr_true);
    cond->instructions->push_back("cJUMP " + to_string(instr_false->size() + 1));
    //cond->instructions->push_back("[else]");
    cond->instructions->splice(cond->instructions->end(), *instr_false);
    //cond->instructions->push_back("[endif]");
    instr_num++;
    return cond->instructions;
}

list<string>* whileLoop(Condition* cond, list<string>* instructions) {
    stack<string> *s = assigned_after_cond.top();
    assigned_after_cond.pop();
    while (!s->empty()) {
        declared[s->top()].assigned = false;
        s->pop();
    }
    delete s;
    while (!cond->jumps.empty()) {
        Jump jump = cond->jumps.top();
        cond->jumps.pop();
        jump.lines_to_jump += instructions->size() + 1;
        *(jump.instruction) += to_string(jump.lines_to_jump);
    }
    //cond->instructions->push_front("[while]");
    //cond->instructions->push_back("[then]");
    cond->instructions->splice(cond->instructions->end(), *instructions);
    cond->instructions->push_back("cJUMP " + to_string(-(long long)(cond->instructions->size())-(long long)instructions->size()));
    //cond->instructions->push_back("[endwhile]");
    instr_num++;
    return cond->instructions;
}

list<string>* repeatUntil(Condition* cond, list<string>* instructions) {
    while (!cond->jumps.empty()) {
        Jump jump = cond->jumps.top();
        cond->jumps.pop();
        jump.lines_to_jump++;
        *(jump.instruction) += to_string(jump.lines_to_jump);
    }
    //instructions->push_front("[repeat]");
    //instructions->push_back("[until]");
    instructions->splice(instructions->end(), *(cond->instructions));
    instructions->push_back("cJUMP 2");
    instructions->push_back("cJUMP " + to_string(-(long long)cond->instructions->size() - (long long)instructions->size()));
    //instructions->push_back("[endloop]");
    instr_num += 2;
    return instructions;
}

list<string>* call(string name, list<string> args, long long lineno) {
    if (proc_map.find(name) == proc_map.end()) {
        cerr << "Error: undefined procedure " << name <<  " at line " << lineno << "\n";
        exit(1);
    }
    list<string>::iterator it;
    Procedure* proc = proc_map[name];
    list<string>* ptr = new list<string>();
    long long j = 0;
    for (it = args.begin(); it != args.end(); it++) {
        if (proc->arg_num == j) {
            cerr << "Error: too many arguments for procedure " << name << " at line " << lineno << "\n";
            exit(1);
        }
        if (declared.find(*it) == declared.end()) {
            cerr << "Error: undefined variable " << *it << " at line " << lineno << "\n";
            exit(1);
        }
        long long i = declared[*it].pointer;
        if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
            if (proc->must_be_assigned[j] && !curr_proc->will_be_assigned[i - curr_proc->arg_beg]) {
                curr_proc->must_be_assigned[i - curr_proc->arg_beg] = true;
            }
            if (proc->will_be_assigned[j]) {
                if (!assigned_after_cond.empty() && !curr_proc->will_be_assigned[i - curr_proc->arg_beg]
                  && !curr_proc->must_be_assigned[i - curr_proc->arg_beg]) {
                    stack<string> *s = assigned_after_cond.top();
                    s->push(name);
                }
                curr_proc->will_be_assigned[i - curr_proc->arg_beg] = true;
            }
            ptr->push_back("LOAD " + to_string(i));
        } else {
            if (!declared[*it].assigned && proc->must_be_assigned[j]) {
                cerr << "Error: variable " << *it << " might not be initialized but must be before calling procedure " << name << " at line " << lineno << "\n";
                exit(1);
            }
            if (proc->will_be_assigned[j]) {
                if (!assigned_after_cond.empty() && !declared[*it].assigned) {
                    stack<string> *s = assigned_after_cond.top();
                    s->push(name);
                }
                declared[*it].assigned = true;
            }
            ptr->push_back("SET " + to_string(i));
        }
        ptr->push_back("STORE " + to_string(proc->arg_beg + j));
        instr_num += 2;
        j++;
    }
    if (proc->arg_num > j) {
        cerr << "Error: too few arguments for procedure " << name << " at line " << lineno << "\n";
        exit(1);
    }
    ptr->push_back("cSET 3");
    ptr->push_back("STORE " + to_string(proc->arg_beg + proc->arg_num));
    ptr->push_back("dJUMP " + to_string(proc->start_line));
    //ptr->push_front("[call " + name + "]");
    //ptr->push_back("[endcall]");
    instr_num += 3;
    return ptr;
}

list<string>* assign(string name, list<string>* instructions, long long lineno) {
    if (declared.find(name) == declared.end()) {
        cerr << "Error: undefined variable " << name << " at line " << lineno << "\n";
        exit(1);
    }
    long long i = declared[name].pointer;
    list<string> *ptr = new list<string>();
    string store_type;
    if (is_proc && i < curr_proc->arg_beg + curr_proc->arg_num) {
        if (!assigned_after_cond.empty() && !curr_proc->will_be_assigned[i - curr_proc->arg_beg]
          && !curr_proc->must_be_assigned[i - curr_proc->arg_beg]) {
            stack<string> *s = assigned_after_cond.top();
            s->push(name);
        }
        curr_proc->will_be_assigned[i - curr_proc->arg_beg] = true;
        store_type = "STOREI ";
    } else {
        if (!assigned_after_cond.empty() && !declared[name].assigned) {
            stack<string> *s = assigned_after_cond.top();
            s->push(name);
        }
        declared[name].assigned = true;
        store_type = "STORE ";
    }
    ptr->push_back(store_type + to_string(i));
    instr_num++;
    instructions->splice(instructions->end(), *ptr);
    return instructions;
}

void declare(string name, long long lineno) {
    if (declared.find(name) != declared.end()) {
        cerr << "Error: reapeated declaration of variable " << name << " at line " << lineno << "\n";
        exit(1);
    }
    declared[name].pointer = mem++;
    declared[name].assigned = false;
}

Procedure* declareProcedure(string name, long long lineno) {
    if (proc_map.find(name) != proc_map.end()) {
        cerr << "Error: repeated declaration of procedure " << name << " at line " << lineno << "\n";
        exit(1);
    }
    Procedure *proc = new Procedure();
    proc->name = name;
    proc->start_line = instr_num;
    proc->arg_num = declared.size();
    proc->arg_beg = mem - proc->arg_num;
    mem++;
    is_proc = true;
    proc->must_be_assigned = new bool [proc->arg_num];
    proc->will_be_assigned = new bool [proc->arg_num];
    for (long long i = 0; i < proc->arg_num; i++) {
        proc->must_be_assigned[i] = false;
        proc->will_be_assigned[i] = false;
    }
    curr_proc = proc;
    return proc;
}

Main* declareMain(list<string>* instructions) {
    declared.clear();
    Main* mainObject = new Main();
    mainObject->start_line = instr_num - instructions->size();
    mainObject->instructions = instructions;
    //mainObject->instructions->push_front("[main]");
    return mainObject;
}

list<string>* finishProcedure(Procedure* proc, list<string>* prev_instr, list<string>* instructions) {
    declared.clear();
    is_proc = false;
    proc_map[proc->name] = proc;
    //instructions->push_front("[" + proc->name + "]");
    prev_instr->splice(prev_instr->end(), *instructions);
    prev_instr->push_back("LOAD " + to_string(proc->arg_beg + proc->arg_num));
    prev_instr->push_back("JUMPI 0");
    instr_num += 2;
    return prev_instr;
}

void finishProgram(list<string>* proc_instr, Main* mainObject) {
    list<string> instructions;
    long long mul_size;
    long long mul_div_size;
    if (mul_occured) {
        instructions = *multCode();
        mul_size = instructions.size();
        if (div_occured) {
            instructions.splice(instructions.end(), *divCode(instructions.size()));
        }
        mul_div_size = instructions.size();
        instructions.splice(instructions.end(), *proc_instr);
    } else if (div_occured) {
        instructions = *divCode(0);
        mul_size = mul_div_size = instructions.size();
        instructions.splice(instructions.end(), *proc_instr);
    } else {
        mul_size = mul_div_size = 0;
        instructions = *proc_instr;
    }
    instructions.splice(instructions.end(), *(mainObject->instructions));
    instructions.push_front("JUMP " + to_string(mainObject->start_line + mul_div_size));
    instructions.push_back("HALT");
    list<string>::iterator it;
    long long i = 0;
    for (it = instructions.begin(); it != instructions.end(); it++) {
        if ((*it)[0] == 'c' || (*it)[0] == 'd' || (*it)[0] == 'e') {
            int space_index = it->find(" ");
            string operation = it->substr(1, space_index);
            long long arg = stoll(it->substr(space_index + 1));
            *it = operation + to_string(arg + ((*it)[0] == 'c' ? i : ((*it)[0] == 'd' ? mul_div_size : mul_size)));
        }
        cout << *it << "\n";
        if ((*it)[0] != '[' || (*it)[it->size() - 1] != ']') {
            i++;
        }
    }
    instructions.clear();
}

void prepare_then() {
    assigned_after_cond.push(new stack<string>());
}

void prepare_else() {
    stack<string> *help = new stack<string>();
    stack<string> *s = assigned_after_cond.top();
    while(!s->empty()) {
        declared[s->top()].assigned = false;
        help->push(s->top());
        s->pop();
    }
    assigned_after_cond.pop();
    assigned_after_cond.push(help);
    assigned_after_cond.push(s);
}
