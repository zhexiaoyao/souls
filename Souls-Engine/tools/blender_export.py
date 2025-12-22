#!/usr/bin/env python3
"""
Blender 导出脚本示例。
用法（在命令行中）：
  blender -b input.blend --python blender_export.py -- output.obj
或者导出 glTF：
  blender -b input.blend --python blender_export.py -- output.gltf
"""
import sys
import os

def main():
    argv = sys.argv
    if "--" in argv:
        args = argv[argv.index("--") + 1:]
    else:
        args = []
    if len(args) < 1:
        print("Usage: blender -b input.blend --python blender_export.py -- output.obj")
        return
    out_path = args[0]
    ext = os.path.splitext(out_path)[1].lower()

    try:
        import bpy
    except Exception as e:
        print("This script must be run inside Blender (bpy available).", e)
        return

    if ext == ".gltf" or ext == ".glb":
        export_format = 'GLB' if ext == ".glb" else 'GLTF_SEPARATE'
        bpy.ops.export_scene.gltf(filepath=out_path, export_format=export_format)
    else:
        # 默认导出 OBJ（不导出材质）
        bpy.ops.export_scene.obj(filepath=out_path, use_materials=False)

if __name__ == "__main__":
    main()


