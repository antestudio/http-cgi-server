project('http-server', ['c', 'cpp'])

add_global_arguments('--std=c++20', language: 'cpp')

includes = include_directories('include/')
subdir('src/') # creates "sources"

subdir('lang/') # creates "www/cgi-bin/lang" executable

executable('server', sources, include_directories: includes, install_dir: '/')