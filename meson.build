project('dxvk', ['c', 'cpp'])

cpu_family = target_machine.cpu_family()

dxvk_compiler = meson.get_compiler('cpp')
dxvk_include_path = include_directories('./include')

is_msvc = dxvk_compiler.get_id() == 'msvc'

if(is_msvc)
	add_global_arguments('/std:c++latest', language : 'cpp')
	add_global_arguments('/bigobj', language : 'cpp')
else
	add_global_arguments('-std=c++1z', language : 'cpp') 
endif

add_global_arguments('-DNOMINMAX', language : 'cpp')

if (cpu_family == 'x86_64')
  dxvk_library_path = meson.source_root() + '/lib'
else
  dxvk_library_path = meson.source_root() + '/lib32'
endif

lib_vulkan = dxvk_compiler.find_library('vulkan-1', dirs : dxvk_library_path)

lib_d3d11          = dxvk_compiler.find_library('d3d11')
lib_dxgi           = dxvk_compiler.find_library('dxgi')
lib_d3dcompiler_47 = dxvk_compiler.find_library('d3dcompiler')

glsl_compiler = find_program('glslangValidator')
glsl_generator = generator(glsl_compiler,
  output    : [ '@BASENAME@.h' ],
  arguments : [ '-V', '--vn', '@BASENAME@', '@INPUT@', '-o', '@OUTPUT@' ])

subdir('src')
subdir('tests')
subdir('wine_utils')
