project('pracc_gl', 'c', 'cpp',
    version: '0.1',
    license: 'MIT',
    default_options: [
    'warning_level=3',
    'c_std=c11',
    'cpp_std=c++20',
    ],
)

glfw = subproject('glfw').get_variable('glfw_dep')
glew = subproject('glew', default_options: ['warning_level=0']).get_variable('glew_dep')
imgui = subproject('imgui', default_options: ['warning_level=0']).get_variable('imgui_dep')
gl = dependency('gl')

includes = include_directories('include')

#shaders = files(
#    'src/newton_fractal/shader/newton_fractal.vert',
#    'src/newton_fractal/shader/newton_fractal.frag',
#    'src/mandelbrot/shader/mandelbrot.vert',
#    'src/mandelbrot/shader/mandelbrot.frag',
#    'src/mandelbrot/shader/julia.vert',
#    'src/mandelbrot/shader/julia.frag',
#)

#fs = import('fs')
#foreach f : shaders
#    fs.copyfile(f, meson.current_build_dir() / 'shader' / fs.name(f))
#endforeach

executable('newton_fractal',
    'src/newton_fractal/main.cc',
    include_directories: includes,
    dependencies: [glew, glfw, imgui, gl]
)

executable('mandelbrot',
    'src/mandelbrot/main.cc',
    include_directories: includes,
    dependencies: [glew, glfw, imgui, gl]
)
