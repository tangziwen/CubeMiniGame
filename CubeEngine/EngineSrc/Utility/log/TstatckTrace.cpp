#include "TstatckTrace.h"
#include "log.h"
namespace tzw {

TstatckTrace * TstatckTrace::instance = NULL;

TstatckTrace *TstatckTrace::get()
{
    if(!TstatckTrace::instance)
    {
        TstatckTrace::instance = new TstatckTrace();
    }
    return TstatckTrace::instance;
}

void TstatckTrace::push(Tstring file, Tstring func)
{
    Tstring a = "at ";
    m_trace.push (a + file + ", func: "+func);
}

void TstatckTrace::pop()
{
    if(m_trace.empty ()) return;
    m_trace.pop ();
}

void TstatckTrace::traceStack(Tstring file, int line, Tstring func)
{
    //Tlog out;
    //out<<TlogBegin;
    //out<<"at "<<file<<", lines: "<<line<<", in func: "<< func<<" trace back.";
    //while(!m_trace.empty ())
    //{
    //    out<<m_trace.top ();
    //    m_trace.pop();
    //}
    //out<<TlogEnd;
}

TstatckTrace::TstatckTrace()
{
}

} // namespace tzw

