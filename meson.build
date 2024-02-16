project('imgc', 'c')

deps = []

if target_machine.system() != 'windows'
	deps += meson.get_compiler('c').find_library('m', required : true)
endif

executable(
	'imgc', ['./src/main.c', './src/impl.c', './src/image.c', './src/blend.c'],
	include_directories : ['./include/'],
	dependencies : [deps]
)

