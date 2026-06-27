# Agent Instructions

## Context

- 个人 C++ 引擎项目，默认渲染器为 Vulkan，主要生产环境为 Windows。
- 结构约束、代码风格和命名规则以 `Doc/ProjectConventions.md` 为准；源码与文档不一致时以源码为准。

## Working Rules

- 优先按 UTF-8 读取；老旧文件无法识别时可尝试 GBK；编辑后保存为 UTF-8。
- 编辑已有文件时遵循原换行符；新文件使用 LF。
- 系统支持 PowerShell 7，执行命令时优先使用 `pwsh` 语义。
- 修改范围保持小，不做无关重构、格式化、渲染后端替换或架构重排
- 除非明确要求，不手动修改 `build/`、第三方依赖、生成物或大型资源包。
- 新增、删除或挪动参与编译的 C++ 文件时，同步更新对应的 `CMakeLists.txt`。

## Navigation

- 引擎层导航入口：`Doc/EngineCodeMap/README.md`。
- Gameplay/编辑器意图入口：`Doc/GameplayIntent/README.md`。
- 修改 `CubeEngine/Application/CubeGame` 下 gameplay/编辑器相关语义前，至少先阅读 `Doc/GameplayIntent/GameplayDesign.md`；涉及具体领域时，再阅读对应细分设计文档。
- 需要确认实现时，用 `rg` 搜索源码中的文件名、模块名或功能关键词作为入口。

## Build

- 需要编译时使用 `build-project` skill。
- 除非用户明确要求或任务本身必须构建验证，不主动编译。
- 无法完成构建或验证时，明确说明原因。
