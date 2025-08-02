cgi_sources = files(
  'main.cpp',
  'src/ast.cpp',
  'src/cgi_handler.cpp',
  'src/interpreter.cpp',
  'src/lexer.cpp',
  'src/parser.cpp',
  'src/value.cpp',
  'src/scope.cpp',
)

executable(
  'lang',
  cgi_sources,
  include_directories: 'include/',
  install_dir: 'www/cgi-bin/'
)