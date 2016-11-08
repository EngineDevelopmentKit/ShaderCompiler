
project "ShaderCompiler"

    kind "StaticLib"       

    files {
        "src/**.cpp"
    } 

    zpm.export [[
        includedirs "include/"
        flags "C++11"
    ]]