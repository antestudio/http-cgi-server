#include "interpreter.h"
#include "parser.h"
#include <memory>

Interpreter::Interpreter(OutputFunc outFun):
  output(outFun),
  global(new Scope(outFun))
{}

void Interpreter::interpret(const std::string& source) {
  Lexer lexer(source);
  std::unique_ptr<Program> prog = Parser(lexer).parse();
  prog->execute(*global);
}

void Interpreter::setOutput(OutputFunc aOutput) {
  output = aOutput;
  global->setOutput(aOutput);
}