from pathlib import Path
from nbsgp_interface import *

module_root = Path(__file__).parent.resolve()

Module = ModuleDefinition(
    name="NReflectionExe",
    private_macros=["NReflectionExe"],
    path=Path(module_root),
    library_type="executable",
    public_depends_modules=[
        ModuleDependency(
            project_name = "NReflection",
            module_name = "NReflectionCore" 
        ),
    ],
    private_source=["private"],
    public_include=["public"],
)