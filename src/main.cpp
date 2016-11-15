#include <assets/ShaderCompiler.h>

int main(int _argc, const char* _argv[])
{
  EDK::Assets::Shader shader = EDK::Assets::CompileShader("test/test_vs.sc", "test/test_fs.sc", "test/includes", "test/includes/varying.def.sc");
  return 0;
}