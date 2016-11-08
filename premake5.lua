--
-- Copyright 2010-2016 Branimir Karadzic. All rights reserved.
-- License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
--

workspace "ShaderCompiler"
    
    configurations { "Debug", "Release", "OptimisedDebug" }
    
    startproject "ShaderCompiler"
    objdir "bin/obj/"
    
    vectorextensions "SSE"
    characterset "Unicode"
    warnings "Extra"
    
    flags "C++11"

    filter { "action:vs*" }
        platforms { "x86_64", "x86" }
     
    filter { "action:vs*", "platforms:x86" }
        targetdir "bin/x86/"
        debugdir "bin/x86/"
        architecture "x86"
    
    filter { "action:vs*", "platforms:x86_64" }
        targetdir "bin/x86_64/"
        debugdir "bin/x86_64/"
        architecture "x86_64"
      
    filter "*Debug"
        targetsuffix "d"
        defines "DEBUG"

        symbols "On"
        optimize "Off"
        
    filter "*OptimisedDebug"
        targetsuffix "od"
        symbols "On"
        optimize "Speed"
        
    filter "*Release"
        optimize "Speed"
        defines "NDEBUG"

    filter { "*Release", "system:not linux" }
        flags "LinkTimeOptimization"

    filter { "*OptimisedDebug", "system:not linux" }
        flags "LinkTimeOptimization"
       
    filter {}

	project "ShaderCompiler"
		uuid "f3cd2e90-52a4-11e1-b86c-0800200c9a66"
		kind "ConsoleApp"

		local GLSL_OPTIMIZER = "3rdparty/glsl-optimizer"
		local FCPP_DIR = "3rdparty/fcpp"
		local BX_DIR = "3rdparty/bx"

		includedirs {
			path.join(GLSL_OPTIMIZER, "src"),
		}

		removeflags {
			-- GCC 4.9 -O2 + -fno-strict-aliasing don't work together...
			"OptimizeSpeed",
		}

		configuration { "vs*" }
			includedirs {
				path.join(GLSL_OPTIMIZER, "src/glsl/msvc"),
			}

			defines { -- glsl-optimizer
				"__STDC__",
				"__STDC_VERSION__=199901L",
				"__STDC_LIMIT_MACROS",
				"__STDC_CONSTANT_MACROS",
				"strdup=_strdup",
				"alloca=_alloca",
				"isascii=__isascii",
			}

			buildoptions {
				"/wd4996" -- warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _strdup.
			}

		configuration { "mingw-*" }
			targetextension ".exe"

		configuration { "mingw* or linux or osx" }
			buildoptions {
				"-fno-strict-aliasing", -- glsl-optimizer has bugs if strict aliasing is used.
				"-Wno-unused-parameter",
			}
			removebuildoptions {
				"-Wshadow", -- glsl-optimizer is full of -Wshadow warnings ignore it.
			}

		configuration { "osx" }
			links {
				"Cocoa.framework",
			}

		configuration { "vs*" }
			includedirs {
				path.join(GLSL_OPTIMIZER, "include/c99"),
				path.join(BX_DIR, "include/compat/msvc"),
			}

		configuration {}

		defines { -- fcpp
			"NINCLUDE=64",
			"NWORK=65536",
			"NBUFF=65536",
			"OLD_PREPROCESSOR=0",
		}

		includedirs {
			"include",
			"3rdparty/dxsdk/include",
			FCPP_DIR,
			path.join(BX_DIR, "include"),

			path.join(GLSL_OPTIMIZER, "include"),
			path.join(GLSL_OPTIMIZER, "src/mesa"),
			path.join(GLSL_OPTIMIZER, "src/mapi"),
			path.join(GLSL_OPTIMIZER, "src/glsl"),
		}

		files {
			"src/**",

			path.join(FCPP_DIR, "**.h"),
			path.join(FCPP_DIR, "cpp1.c"),
			path.join(FCPP_DIR, "cpp2.c"),
			path.join(FCPP_DIR, "cpp3.c"),
			path.join(FCPP_DIR, "cpp4.c"),
			path.join(FCPP_DIR, "cpp5.c"),
			path.join(FCPP_DIR, "cpp6.c"),
			path.join(FCPP_DIR, "cpp6.c"),

			path.join(GLSL_OPTIMIZER, "src/mesa/**.c"),
			path.join(GLSL_OPTIMIZER, "src/glsl/**.cpp"),
			path.join(GLSL_OPTIMIZER, "src/mesa/**.h"),
			path.join(GLSL_OPTIMIZER, "src/glsl/**.c"),
			path.join(GLSL_OPTIMIZER, "src/glsl/**.cpp"),
			path.join(GLSL_OPTIMIZER, "src/glsl/**.h"),
			path.join(GLSL_OPTIMIZER, "src/util/**.c"),
			path.join(GLSL_OPTIMIZER, "src/util/**.h"),
		}

		removefiles {
			path.join(GLSL_OPTIMIZER, "src/glsl/glcpp/glcpp.c"),
			path.join(GLSL_OPTIMIZER, "src/glsl/glcpp/tests/**"),
			path.join(GLSL_OPTIMIZER, "src/glsl/glcpp/**.l"),
			path.join(GLSL_OPTIMIZER, "src/glsl/glcpp/**.y"),
			path.join(GLSL_OPTIMIZER, "src/glsl/ir_set_program_inouts.cpp"),
			path.join(GLSL_OPTIMIZER, "src/glsl/main.cpp"),
			path.join(GLSL_OPTIMIZER, "src/glsl/builtin_stubs.cpp"),
		}