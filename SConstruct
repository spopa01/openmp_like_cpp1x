env = Environment()

env.Tool('gcc')

env.Append(CPPPATH = ['/usr/include/'])

env.Append(CCFLAGS = ['-g', '-std=c++1y', '-pthread'])

env.Append(LIBPATH = ['/usr/lib/'])

env.Append(LIBS = ['pthread'])

env.Append(LINKFLAGS = ['-Wl,--no-as-needed'])

t = env.Program(target='main', source=['./openmplike.cpp', './main.cpp'])

Default(t)
