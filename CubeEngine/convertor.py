import os
import sys
import argparse

########################################################################################################################
# console reporting

def report_info(msg):
    print "(I) " + msg

def report_error(msg):
    print "(X) " + msg

def report_warning(msg):
    print "(!) " + msg

########################################################################################################################
# parsing utils

def _parse_two_operand(line, sep):
    components = [component.strip() for component in line.split(sep)]
    if len(components) != 2 or len(components[0]) == 0 or len(components[1]) == 0:
        report_error("operation requires two non-empty arguments")
        return None
    return components

def _process_vars_in_literal(literal):
    predefined_replaces = {"$$PWD" : os.getcwd()}
    for src in predefined_replaces.iterkeys():
        literal = literal.replace(src, predefined_replaces[src])

    return literal.replace("$${", "${") # CMake variable useage syntax

########################################################################################################################
# statements

class StatementEmptyLine:
    def cmake_code(self):
        return ""

class StatementComment:
    def __init__(self, line):
        self.text = line

    def cmake_code(self):
        return self.text

class StatementCondition:
    def __init__(self, line):
        self.parse(line)

    def parse(self, line):
        self.condition = line.strip("{").strip()

    def cmake_code(self):
        self._process_platform_condition()
        self._process_build_type_condition()
        self._process_non_empty_condition()
        return "if(" + self.condition + ")"

    def _process_platform_condition(self):
        platforms = {"linux": "UNIX", "win32" : "WIN32", "macx" : "APPLE"}
        if self.condition in platforms.keys():
            self.condition = platforms[self.condition]

    def _process_build_type_condition(self):
        prefix = "CONFIG("
        if self.condition.startswith(prefix):
            report_warning("support for build type conditions like CONFIG(release, release|debug) is very poor")
            qmake_build_type = self.condition[len(prefix):self.condition.find(",")]
            build_types = {"debug": "Debug", "release": "Release"}
            self.condition = "CMAKE_BUILD_TYPE MATCHES " + build_types[qmake_build_type]

    def _process_non_empty_condition(self):
        prefix = "!isEmpty("
        if self.condition.startswith(prefix):
            report_warning("support for !isEmpty condition is not fully correct")
            self.condition = self.condition[len(prefix):self.condition.index(")")]


class StatementConditionElse:
    def cmake_code(self):
        return "else()"

class StatementConditionEnd:
    def cmake_code(self):
        return "endif()"

class StatementAssignment:
    def __init__(self, line):
        self.special = None
        self.parse(line)

    def parse(self, line):
        try:
            self.var, self.val = _parse_two_operand(line, " = ")
            self._process_special_target()
            self.val = _process_vars_in_literal(self.val)
            if ":" in self.var:
                platform, var = self.var.split(':')
                self.special = StatementCondition(platform).cmake_code() + "\n" + \
                               StatementAssignment(var + " = " + self.val).cmake_code() + "\n" + \
                               StatementConditionEnd().cmake_code()
        except ValueError:
            pass

    def cmake_code(self):
        if self.special is not None:
            return self.special
        return "set(" + self.var + " " + self.val + ")"

    def _process_special_target(self):
        if self.var == "TARGET":
            self.special = "project(" + self.val + ")"
            report_info("detected project name: " + self.val)

class StatementConcatenation:
    def __init__(self, line):
        self.special = None
        self.parse(line)

    def parse(self, line):
        try:
            self.var, self.val = _parse_two_operand(line, " += ")
            self.val = _process_vars_in_literal(self.val)
            self._process_special()
            self._process_mappings()
        except ValueError:
            pass

    def cmake_code(self):
        if self.special is not None:
            return self.special
        return "set(" + self.var + r" ${" + self.var + "} " + self.val + ")"

    def _process_special(self):
        self._process_special_qt()
        self._process_special_define()
        self._process_special_includes()
        self._process_config()

    def _process_special_define(self):
        if self.var == "DEFINES":
            qmake_definitions = " ".join(["-D" + define.strip() for define in self.val.split()])
            self.special = "add_definitions(" + qmake_definitions + ")"

    def _process_special_qt(self):
        if self.var == "QT":
            self.special = ""
            qt_libraries = ""
            for qt_component in self.val.split():
                self.special += "find_package(Qt5" + qt_component.strip().title() + " REQUIRED)\n"
                qt_libraries += "Qt5::" + qt_component.strip().title() + " "
            self.special +=  StatementConcatenation("LIBS += " + qt_libraries.strip()).cmake_code() + "\n"

    def _process_special_includes(self):
        if self.var == "INCLUDEPATH":
            self.special = "include_directories(" + self.val + ")"

    def _process_mappings(self):
        if self.var == "QMAKE_CXXFLAGS":
            self.var = "CMAKE_CXX_FLAGS"
            self.val = "\"" + self.val + "\""

        if self.var == "QMAKE_LFLAGS":
            self.val = "\"" + self.val + "\""

    def _process_config(self):
        if self.var == "CONFIG":
            new_statements = []
            for config_option in self.val.split():
                statement = QMakeConfigProcessor.process_config(config_option)
                if statement is not None:
                    new_statements.append(statement)
            self.special = "\n".join([s.cmake_code() for s in new_statements])

class StatementFunction:
    def __init__(self, line):
        self.line = line
        self.functions = {"message": self._message, "greaterThan": self._greaterThan, "system": self._message}

    def cmake_code(self):
        left_bracket_pos = self.line.index('(')
        func_name = self.line[:left_bracket_pos].strip()
        right_bracket_pos = self.line.index(')')
        func_args = [arg.strip() for arg in self.line[left_bracket_pos+1:right_bracket_pos].split(",")]

        try:
            return self.functions[func_name](func_args)
        except KeyError:
            report_error("unknown function, writing as is: " + func_name)
            return self.line

    def _message(self, args):
        return "message(\"" + " ".join(args) + "\")"

    def _greaterThan(self, args):
        report_info("requested qt version " + ":".join(args))
        return ""

    def _system(self, args):
        return "execute_process(COMMAND \"" + " ".join(args) + "\")"


########################################################################################################################
# conversion

class QMakeConfigProcessor:
    @staticmethod
    def process_config(option_name):
        if option_name == "thread":
            report_warning("threads support is UNIX only")
            return StatementConcatenation("LIBS += -lpthread")
        report_warning("unsupported qmake config option " + option_name)

        if "debug" in option_name or "release" in option_name:
            # ignoring this as it is not useful in CMake
            pass

        if option_name == "precompile_header":
            report_warning("CMake doesn't support precompiled headers")


class QMakeParser:
    def __init__(self, input_file_name):
        self.input_file_name = input_file_name
        self.current_line_num = 0

    def iter_statements(self):
        for line in self._iter_lines():
            # handle includes
            include_prefix = "include("
            if line.startswith(include_prefix):
                include_file_name = line[len(include_prefix):line.index(")")]
                report_info("including qmake file " + include_file_name)
                for statement in QMakeParser(include_file_name).iter_statements():
                    yield statement

            elif len(line) == 0:
                yield StatementEmptyLine()
            elif line.startswith("#"):
                yield StatementComment(line)
            elif line.count("{") == 1 and not "}" in line and not "$$" in line:
                yield StatementCondition(line)
            elif line.count("}") == 1 and not "{" in line:
                yield StatementConditionEnd()
            elif line.count("}") == 1 and line.count("{") == 1 and "else" in line: #todo: better determine this
                yield StatementConditionElse()

            elif line.count(" = ") == 1:
                yield StatementAssignment(line)
            elif line.count(" += ") == 1:
                yield StatementConcatenation(line)
            elif line.count("(") == 1 and line.count(")") == 1:
                yield StatementFunction(line)
            else:
                report_warning(self._report_on_line("skipping unknown qmake statement"))

    def _iter_lines(self):
        line = ""
        with open(self.input_file_name, "r") as qmake_file:
            for input_file_line in qmake_file:
                self.current_line_num += 1
                input_file_line = input_file_line.strip()
                if len(input_file_line) == 0 or input_file_line.startswith("#"):
                    yield input_file_line

                line += input_file_line.rstrip("\\")
                if input_file_line.endswith("\\"):
                    continue  # gather multiline statements
                else:
                    yield line
                    line = ""

    def _report_on_line(self, message):
        return self.input_file_name + ":" + str(self.current_line_num) + " " + message


class Qmake2CmakeConverter:
    def __init__(self, qmake_file_name, cmake_file_name):
        self.qmake_file_name = qmake_file_name
        self.cmake_file_name = cmake_file_name

    def convert(self):
        with open(self.cmake_file_name, "w") as cmake_file:
            self._write_predefined(cmake_file)
            for statement in QMakeParser(self.qmake_file_name).iter_statements():
                cmake_code = statement.cmake_code()
                if cmake_code is not None and cmake_code != "":
                    cmake_file.write(cmake_code + "\n")
            self._write_target(cmake_file)

    def _write_predefined(self, cmake_file):
        cmake_file.write("cmake_minimum_required(VERSION 2.8.11)\n")

        predefined_variables = ["SOURCES", "HEADERS", "RESOURCES", "FORMS", "INCLUDEPATH", "LIBS", "QMAKE_LFLAGS"]
        statements = [StatementComment("#predefined variables")]
        statements.extend([StatementAssignment(var + " = \"\"") for var in predefined_variables])
        statements.append(StatementAssignment("PWD = \"" + os.getcwd() + "\""))

        for statement in statements:
            cmake_file.write(statement.cmake_code() + "\n")

    def _write_target(self, cmake_file):
        cmake_file.writelines([line + "\n" for line in [
            "set(CMAKE_AUTOMOC ON)",
            "set(CMAKE_INCLUDE_CURRENT_DIR ON)",
            "include_directories(${CMAKE_BUILD_FILES_DIRECTORY}) # fixes syntax analysis in CLion"
            "",
            "set(TARGET ${CMAKE_PROJECT_NAME})",
            "add_executable(${TARGET} ${SOURCES} ${UI_GENERATED_HEADERS} ${RESOURCES_GENERATED})",
            "target_link_libraries(${TARGET} ${QMAKE_LFLAGS})\n"
            "target_link_libraries(${TARGET} ${LIBS})",
            ""
        ]])

########################################################################################################################
# entry point

output_cmake_file_name = "CMakeLists.txt"
input_qmake_file_name  = "CubeEngine.pro"

# parser = argparse.ArgumentParser()
# parser.add_argument("input_file")
# parser.parse_args()
# for property, value in vars(parser).iteritems():
#     print property, ": ", value
# if parser.input_file != "":
#     input_qmake_file_name = parser.input_file

print "converting", input_qmake_file_name
Qmake2CmakeConverter(input_qmake_file_name, output_cmake_file_name).convert()
print "converted file saved to", output_cmake_file_name