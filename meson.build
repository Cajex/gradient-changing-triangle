project('opengl', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++20'])

opengl = executable('opengl', 'main.cpp', install : true, dependencies: [dependency('opengl'), dependency('glfw3'), dependency('glew')])

test('test', opengl)