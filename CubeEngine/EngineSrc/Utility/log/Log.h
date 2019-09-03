#ifndef TZW_LOG_H
#define TZW_LOG_H
namespace tzw
{
	void tlog(const char * pattern, ...);
	void tlogError(const char * pattern, ...);
	void initLogSystem();
}
#endif // !TZW_LOG_H
