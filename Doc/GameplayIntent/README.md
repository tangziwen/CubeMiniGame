# Gameplay Intent

这里记录 `CubeEngine/Application/CubeGame` 的长期 gameplay 设计意图。

这些文档不是源码索引，也不是实现说明。它们只用来说明玩家体验、玩法机制、游戏概念、概念边界，以及少量便于定位的代码锚点。具体实现仍以源码为准。

默认作者视角是技术策划（Technical Designer TD）：理解关键代码边界，但只记录设计意图、概念关系和必要锚点，不记录函数流程或局部实现技巧也不了解渲染和引擎底层实现。

## Header Meanings

- `Design Intent`：这个领域为什么存在，它服务的核心体验和不能轻易偏离的设计方向。
- `Experience`：玩家或设计者应该感受到的结果，描述体验验收口径，不描述代码流程。
- `Design`：主要玩法机制和对象关系；需要展开时可以用 `###` 拆成子机制。
- `Game Concepts`：玩家或设计者会直接认知的 gameplay 概念和必要别名。
- `Concept Code Anchors`：gameplay 概念到当前 canonical 代码入口的薄映射，只用于快速定位。
- `Critical Code Anchors`：调试或改动时很关键、但本身不属于 gameplay 概念的 system、manager 或 helper。
- `Code Rules`：gameplay 层通用代码组织原则，例如状态所有权、UI/visual 与逻辑真相的边界。
- `Design Boundaries`：概念边界、禁止事项和当前非目标。

## Maintenance

- 更新粒度以 TD 会关心的玩法语义为准，而不是以代码改动粒度为准。
- 只有 gameplay 概念、命名、对象关系、边界或设计目标变化时才更新。
- 普通 bugfix、helper 拆分、渲染细节、缓存策略和函数级流程通常不进入这里。
- 新增代码类型不自动成为 `Game Concepts`；先判断它是不是玩家或设计者会认知的游戏概念。
- 需要确认具体行为时搜索源码；这些文档只提供意图和锚点。
