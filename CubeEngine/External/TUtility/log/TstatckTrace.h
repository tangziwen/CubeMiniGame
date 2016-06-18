#ifndef TZW_TSTATCKTRACE_H
#define TZW_TSTATCKTRACE_H
#include <stack>
#include "../string/Tstring.h"
namespace tzw {

class TstatckTrace
{
public:
    static TstatckTrace * get();
    void push(Tstring file, Tstring func);
    void pop();
    void traceStack(Tstring file, int line, Tstring func);
private:
    TstatckTrace();
    std::stack<Tstring> m_trace;
    static TstatckTrace * instance;
};

} // namespace tzw

#define T_ENTER TstatckTrace::get()->push(__FILE__, __func__);

#define T_RETURN TstatckTrace::get()->pop(); return

#define T_TRACE_STATCK TstatckTrace::get()->traceStack(__FILE__, __LINE__, __func__);

#endif // TZW_TSTATCKTRACE_H
