# Rail Design

## Design Intent

铁路是 CubeGame 当前最明确的强意图 gameplay 系统。它不是单纯的几何线段，而是玩家在世界中建立“连接、站点、路线、运输行为”的方式。

铁路设计最重要的边界是：物理轨道和运输语义要分开。轨道说明哪里可以走；站点和路径点说明玩家赋予轨道的意义；线路说明运输服务的组织方式；列车说明这个组织方式在运行时如何表现。

## Experience

- 玩家可以宽松地铺设和修改轨道，先获得可实验的沙盒体验。
- 玩家可以在轨道上放置有名字、有意义的站点和路径点。
- 玩家编辑线路时，应该感觉自己在选择站点和路径点，而不是操作底层图节点。
- 玩家可以创建列车、分配线路、暂停和恢复运行。
- 铁路对象应该在世界中可见、可检查、可点击，尤其是站点和路径点。
- 临时编辑提示应该明显高于普通标签，避免编辑时点不到真正想点的东西。
- 玩家读取世界后，铁路结构、站点、路径点、线路和列车状态应该作为同一个世界的一部分恢复。

## Design

### Physical Rail

物理轨道表达“列车可以在哪里行驶”。它需要能被铺设、删除、连接和拆分，但它本身不直接表达站点、线路或运输服务。

轨道由节点、轨道段和轨道位置共同表达。它们虽然接近实现层，但直接决定铺轨、拆分、删除、站点附着和线路运行等 gameplay 逻辑，因此属于铁路设计概念。

### Anchored Rail Concepts

站点、站台和路径点都应该依附在轨道上。它们不是独立漂浮的标签，而是玩家赋予轨道位置的语义。

### Line As Service

线路表达一种运输服务，而不是底层节点列表。玩家编辑线路时，应该面对站点和路径点这些有意义的控制点。

线路控制点是线路设计的关键结构：它把“线路要经过哪些站点/路径点”表达成可编辑的顺序。它不是底层拓扑节点，也不应该暴露成纯节点编辑。

### Train As Runtime Actor

列车是线路意图在运行时的表现。它可以被分配线路、暂停、恢复，并在世界中表现出当前位置和运行状态。

### Persistent And Editing Visuals

持久视觉表达已经存在的铁路事实；编辑视觉表达当前工具的临时意图。两者可以同时出现，但含义不同。

### Rail Persistence

铁路是世界持久化的一部分。存档应保留轨道、锚点、站点、站台、路径点、线路控制点和列车运行状态这些长期铁路事实；读档后可以重建线路派生路径和视觉表现。

临时编辑状态、预览几何和视觉对象不属于铁路存档真相。它们可以在读取后重新生成或清空，不能成为判断铁路是否存在的依据。

## Game Concepts

- 轨道：世界中真实存在的铁路结构。
- 拓扑节点：轨道连接关系中的节点，决定轨道如何连接和分叉。
- 轨道段：连接两个拓扑节点的一段可行驶轨道。
- 轨道位置：轨道段上的具体位置，用来附着站点、站台或路径点。
- 锚点：把站点、站台或路径点绑定到轨道上的稳定位置。
- 站点：玩家命名的停车和线路组织概念。
- 站台：站点在轨道上的具体停靠位置。
- 路径点：玩家命名的线路控制点，用来表达经过、转向或导航意图。
- 线路：由站点和路径点组织出来的运输服务。
- 线路控制点：线路中的一个站点或路径点，用来表达线路经过顺序。
- 列车：在线路上运行的实体。

## Concept Code Anchors

- 铁路系统 -> `RailSystem`
- 物理轨道 -> `RailNetwork`
- 拓扑节点 -> `RailNode`
- 轨道段 -> `RailSegment`
- 轨道位置 -> `RailTrackLocation`
- 锚点 -> `RailAnchor`
- 站点 -> `RailStation`
- 站台 -> `RailPlatform`
- 路径点 -> `RailRoutePoint`
- 线路 -> `RailLine`
- 线路控制点 -> `RailLineControlPoint`
- 列车 -> `RailTrain`

## Critical Code Anchors

- `RailAnchorManager` / `RailStationManager` / `RailRoutePointManager`：铁路逻辑点的关键管理锚点，不是独立 gameplay concept。
- `RailLineManager` / `RailTrainManager`：线路和列车运行状态的关键管理锚点，不进入 `Concept Code Anchors`。
- `RailBuildTool`：轨道铺设的 preview/commit 技术锚点。
- `RailPersistentVisualSystem` / `RailEditorVisualSystem`：持久视觉和编辑视觉的技术锚点；它们表达设计边界，但不是 gameplay concept。

## Design Boundaries

- `RailNode` 不是 `RailStation`，也不是 `RailRoutePoint`。
- 站点和路径点是玩家概念，应该保持可见、可命名、可检查。
- 线路应该由站点和路径点定义，而不是直接暴露底层节点编辑。
- 列车运行状态属于列车概念，不应该散落在 UI 或视觉对象中。
- 持久视觉和编辑视觉表达不同含义，不应该混成一个概念。
- manager、visual system、UI view、editing controller 等辅助类型不进入 `Game Concepts` 或 `Concept Code Anchors`，除非未来它们本身成为玩家/设计师概念。
- 铁路可以复用放置和 raycast 辅助，但铁路本身不属于 `BuildingSystem`。
- 当前铁路还不是完整交通经营游戏。
- Undo/redo 需要单独设计；存档/读档属于世界持久化，铁路只保存自己的长期事实。
- 曲线算法调优不应混入无关的 UI、输入或视觉改动。
