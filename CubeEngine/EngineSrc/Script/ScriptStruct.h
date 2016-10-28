#ifndef TZW_SCRIPTSTRUCT_H
#define TZW_SCRIPTSTRUCT_H


#include <map>
#include <vector>
namespace tzw {
class ScriptValue;
class ScriptStruct
{
public:
	ScriptStruct();
	ScriptValue * get(std::string name);
	ScriptValue * getByIndex(int index);
	void append(ScriptValue * value);
	void set(std::string name, ScriptValue * value);
	void set(int index, ScriptValue * value);
private:
	std::map<std::string, ScriptValue *> m_valueMap;
	std::vector<ScriptValue *> m_valueList;
};

} // namespace tzw

#endif // TZW_SCRIPTSTRUCT_H
