#pragma once
#include <typeinfo>
#include <vector>
#include <string>
#include <map>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <unordered_map>  
#include <unordered_set>  
#include <iostream>
#include <set>

namespace tinynn {

struct TestTrace;
using RunRoutinue = ::std::function<void(TestTrace&)>;

inline auto& global_routinues() {
    static ::std::unordered_map<::std::string, RunRoutinue> run_routinues;
    return run_routinues;
}

inline int register_func(const ::std::string& name,RunRoutinue rout) {
    global_routinues().emplace(name, rout);
    return 0;
}

struct TestTrace {
    ::std::string trace_string() {
        ::std::string detail_str;
        detail_str.append(::std::to_string(_success_cnt)).append("/")
                  .append(::std::to_string(_traces.size())).append("\n");
        return detail_str;
    }
    void add_trace(const ::std::string& trace, const bool success) {
       _traces.emplace_back(trace, success); 
       _success_cnt += success;
    }
    int _success_cnt = 0;
    ::std::vector<::std::pair<::std::string, bool>> _traces;
};

inline void run_all_routinues(int argc, char** argv) {
    if (argc == 1) {
        for (auto& func : global_routinues()) {
            if (func.second !=  nullptr) {
                ::std::cout << "Begin Func: " << func.first << ::std::endl;
                TestTrace test_trace;
                func.second(test_trace);
                ::std::cout << test_trace.trace_string();
                ::std::cout << "End Func: " << func.first << ::std::endl << ::std::endl;
            }
        }
    } else {
        for (int idx = 1; idx < argc; ++idx) {
            if (global_routinues().count(argv[idx]) == 1 and global_routinues()[argv[idx]] != nullptr) {
                ::std::cout << "Begin Func: " << argv[idx] << ::std::endl;
                TestTrace test_trace;
                global_routinues()[argv[idx]](test_trace);
                ::std::cout << test_trace.trace_string();
                ::std::cout << "End Func: " << argv[idx] << ::std::endl << ::std::endl;
            }
        }
    }
}

#define Local_Register_Func2(name, funct) \
    static int g_##name = register_func(#name, funct)

#define Local_Register_Func1(name, funct) Local_Register_Func2(name##_##funct, funct)

#define Register_Func(name, funct) \
    extern void funct(TestTrace&); \
    Local_Register_Func1(name, funct); \
    void funct(TestTrace& trace) 
    
#define EXPECT_TRUE(cmd) \
    { \
        bool success = (cmd); \
        trace.add_trace(#cmd, success); \
    }
}
