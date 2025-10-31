from pathlib import Path
from nbsgp import *

project_root = Path(__file__).parent.resolve()

Project = ProjectDefinition(
    name="NReflection",
    path=project_root,
    public_modules = 
    [
        load_module(project_root / "source/NReflectionCore"),
    ],
    private_modules=
    [
        load_module(project_root / "Source/NReflectionExe")    
    ]
)