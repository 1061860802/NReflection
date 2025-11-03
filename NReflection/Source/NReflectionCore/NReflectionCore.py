from pathlib import Path
from nbsgp_interface import *

module_root = Path(__file__).parent.resolve()

Module = ModuleDefinition(
    name="NReflectionCore",
    private_macros=["NReflectionCore"],
    path=Path(module_root),
    library_type="DYNAMIC",
    public_depends_modules=[],
    private_source=["private"],
    public_include=["public"]
)