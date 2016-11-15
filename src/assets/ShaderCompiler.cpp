/**
* @cond ___LICENSE___
*
* Copyright (c) 2016 Koen Visscher, Paul Visscher, Andrew Mac and individual contributors.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @endcond
*/

#include <assets/ShaderCompiler.h>
#include "shaderc/shaderc.h"

#include <fstream>
#include <sstream>
#include <string>

std::string _compileShader(
  const char* _filePath,
  const char* _type,
  const char* _platform,
  const char* _profile,
  const char* _bin2c,
  const char* _includeDir,
  const char* _varyingdef,
  char* _outputText,
  U16& _outputSize);

EDK::Assets::Shader EDK::Assets::CompileShader(std::string vertexShaderPath, std::string fragmentShaderPath, std::string includePath, std::string varyingDefPath)
{
  // Output from any shader compilations. Errors, etc.
  char shaderOutput[UINT16_MAX];
  U16 shaderOutputSize = 0;

  EDK::Assets::Shader newShader;

  // DirectX 12
  newShader.AddCompiledVertexShader(  "D3D12",
    _compileShader(vertexShaderPath.c_str(),   "v", "windows", "vs_5_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));
  newShader.AddCompiledFragmentShader("D3D12",
    _compileShader(fragmentShaderPath.c_str(), "f", "windows", "ps_5_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));

  // DirectX 11
  newShader.AddCompiledVertexShader(  "D3D11",
    _compileShader(vertexShaderPath.c_str(),   "v", "windows", "vs_4_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));
  newShader.AddCompiledFragmentShader("D3D11",
    _compileShader(fragmentShaderPath.c_str(), "f", "windows", "ps_4_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));

  // DirectX 9
  newShader.AddCompiledVertexShader(  "D3D9",
    _compileShader(vertexShaderPath.c_str(),   "v", "windows", "vs_3_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));
  newShader.AddCompiledFragmentShader("D3D9",
    _compileShader(fragmentShaderPath.c_str(), "f", "windows", "ps_3_0", NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));

  // Metal
  newShader.AddCompiledVertexShader(  "Metal",
    _compileShader(vertexShaderPath.c_str(),   "v", "osx",     "metal",  NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));
  newShader.AddCompiledFragmentShader("Metal",
    _compileShader(fragmentShaderPath.c_str(), "f", "osx",     "metal",  NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));

  // OpenGL
  newShader.AddCompiledVertexShader(  "OpenGL",
    _compileShader(vertexShaderPath.c_str(),   "v", "osx",     "120",    NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));
  newShader.AddCompiledFragmentShader("OpenGL",
    _compileShader(fragmentShaderPath.c_str(), "f", "osx",     "120",    NULL, includePath.c_str(), varyingDefPath.c_str(), shaderOutput, shaderOutputSize));

  return newShader;
}

std::string _compileShader(
  const char* _filePath,
  const char* _type,
  const char* _platform,
  const char* _profile,
  const char* _bin2c,
  const char* _includeDir,
  const char* _varyingdef,
  char* _outputText,
  U16& _outputSize)
{
  const char* argv[16];
  int argc = 0;

  // -f <file path>                 Input file path.
  argv[argc] = "-f";
  argv[argc + 1] = _filePath;
  argc += 2;

  // -o <file path>                 Output file path.
  argv[argc] = "-o";
  argv[argc + 1] = "tmp.bin";
  argc += 2;

  // --platform <platform>          Target platform.
  argv[argc] = "--platform";
  argv[argc + 1] = _platform;
  argc += 2;

  // --type <type>                  Shader type (vertex, fragment)
  argv[argc] = "--type";
  argv[argc + 1] = _type;
  argc += 2;

  // -i <include path>              Include path (for multiple paths use semicolon).
  if (_includeDir)
  {
    argv[argc] = "-i";
    argv[argc + 1] = _includeDir;
    argc += 2;
  }

  // --bin2c <file path>            Generate C header file.
  if (_bin2c)
  {
    argv[argc] = "--bin2c";
    argv[argc + 1] = _bin2c;
    argc += 2;
  }

  // --varyingdef <file path>       Path to varying.def.sc file.
  if (_varyingdef)
  {
    argv[argc] = "--varyingdef";
    argv[argc + 1] = _varyingdef;
    argc += 2;
  }

  // -p, --profile <profile>        Shader model (f.e. ps_3_0).
  if (_profile)
  {
    argv[argc] = "-p";
    argv[argc + 1] = _profile;
    argc += 2;
  }

  S32 result = bgfx::compileShader(argc, argv);
  bgfx::getShaderError(_outputText, _outputSize);

  // Temp hack.
  // TODO: modify shaderc to output compiled shader instead of file.
  std::ifstream tmpFile("tmp.bin");
  if (tmpFile)
  {
    std::stringstream buffer;
    buffer << tmpFile.rdbuf();
    tmpFile.close();
    std::remove("tmp.bin");

    return buffer.str();
  }

  return "";
}