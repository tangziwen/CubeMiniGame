# SurfaceNets LOD 接缝处理 — 观察、当前修改与遗留问题

本文档记录本仓库基于分块 (chunk) 的 Surface Nets 体素地形管线的现状：
当前所知的设计意图、已经做出的修改、以及仍未解决的问题
（尤其是相机持续移动后部分接缝再次出现的重叠条带）。
仅作为下一轮调试的工作笔记。

## 1. 系统概述

### 1.1 每个 chunk / 每个 LOD 的体素网格布局

LOD 等级 `L`（0 为最精细），chunk 拥有边长为
`N + MIN_PADDING + MAX_PADDING` 的体素网格：

- `N = MAX_BLOCK >> L`（该 LOD 下每轴的内部 cell 数）
- `MIN_PADDING = 1`，`MAX_PADDING = 2`，定义在
  [Transvoxel.h](CubeEngine/EngineSrc/3D/Terrain/Transvoxel.h#L10-L11)
- `LOD_SHIFT = 4`（全局 voxel 锚点偏移），见
  [GameMap.h](CubeEngine/Application/CubeGame/GameMap.h#L4)

某轴上局部索引 `i` 的体素对应全局体素
`chunk * MAX_BLOCK + (i - MIN_PADDING) * stride + LOD_SHIFT`，
其中 `stride = 1 << L`。因此：

- 局部 cell `[MIN_PADDING, MIN_PADDING + N)` 是**内部 cell**。
- 局部索引 `MIN_PADDING - 1 = 0` 是 `-X / -Y / -Z` padding 行，
  与 `+side` 邻居的第一行内部体素**完全重合**（同一个全局体素）。
- 局部索引 `MIN_PADDING + N` 与 `MIN_PADDING + N + 1` 是
  `+X / +Y / +Z` padding（共两行）。其中 `MIN_PADDING + N`
  与 `+side` 邻居的局部索引 `0` 完全重合。

也就是说**同一 LOD 的同轴邻居共享两行体素数据**（一行在 `-side`，一行在 `+side`）。
Surface Nets 的接缝连续性正是依赖这一恒等关系。

### 1.2 Surface Nets 三阶段结构

`SurfaceNets::generate` 的实现见
[SurfaceNets.cpp](CubeEngine/EngineSrc/3D/Terrain/SurfaceNets.cpp)：

- **Pass 1：dual vertex 生成。** 对每个 8 角 voxel 跨越等值面的 cell
  `(x, y, z)`，在表面边交点的质心位置放一个顶点。
  默认填充范围是 `[minCellExt, maxCell)`，其中
  `minCellExt = MIN_PADDING - 1`（填一行 `-side` padding），
  `maxCell = MIN_PADDING + N`（默认**不**填 `+side` padding）。

  当 `extendPositive[axis]` 被置位时，`+side` 上界变为
  `maxCell + 1`，多填一行 `+side` padding。
  这条路径用于本 chunk 是 LOD 转换的**粗侧**：精侧邻居会让自己的
  `-side` 接缝 quad 退化（被 Pass 3 折叠），所以必须由本侧画 `+side` 接缝。

- **Pass 2：三角形生成。** 遍历 `[minCellExt, maxCell]`，
  在等值面被穿越时发出 X / Y / Z 方向的 quad。quad 归属由两组开关控制：

  - `skipNeg{X,Y,Z}Quad`：当 `stitchFace[face]` 置位时，
    不在 `x|y|z == minCellExt` 处发出 `-side` 边界 quad，
    粗侧邻居会通过其 `extendPositive` 来画。
  - `skipPos{X,Y,Z}Quad`：当 `skipPositive[axis]` 置位时，
    不在 `x|y|z == maxCell - 1` 处发出 `+side` 边界 quad，
    `+side` 邻居会画。

- **Pass 3：LOD 接缝顶点对齐。** 对 `stitchFace` 标记的面上的边界 fine cell，
  把它们的 dual 顶点 snap 到对应粗侧 cell 在粗 buffer 上的 dual 顶点位置。
  覆盖同一个粗 cell 的 4 个 fine cell 折叠为一个点 ⇒ 4 个三角形退化，
  接缝由粗侧邻居画。

### 1.3 边界归属规则（当前意图）

每条接缝在所有相机移动 / streaming 状态组合下，都应该**有且只有一侧**发出 quad。
默认采用「+side 邻居拥有边界」的约定：

| 本侧 | +side 邻居 | 谁画边界 quad |
|---|---|---|
| LOADED，任意 LOD | 未 LOADED / 越界 | 本侧（不 skip） |
| LOADED | LOADED，同 LOD | 邻居（用其 `-side` padding 行） |
| LOADED，更粗 | LOADED，更细 | 本侧（`extendPositive[axis]` 覆盖 skip） |
| LOADED，更细 | LOADED，更粗 | 邻居（本侧设 `stitchFace`，Pass 3 折叠 quad） |

`skipPositive[axis]` 仅在以下条件**全部**满足时才置 true：

1. `+side` 邻居在 `m_chunkArray` 中存在（在世界范围内）。
2. 邻居 `m_currenState == LOADED`（仅指针非空不够；世界是预分配的，
   所以非空指针**不能**作为「已加载」的判据）。
3. 邻居 `m_currentLOD >= 我们的 LOD`（同 LOD 时邻居用 `-side` padding 画；
   更粗时邻居用自己的 `extendPositive` 画）。
4. 我们自己**没有** `extendPositive[axis]`（那种情况下接缝由本侧画）。

## 2. 已经做的修改

### 2.1 SurfaceNets 头文件
[SurfaceNets.h](CubeEngine/EngineSrc/3D/Terrain/SurfaceNets.h)：在
`SurfaceNetsStitchConfig` 中加了显式的 `skipPositive[3]` 标志，
默认 `false`。这样在没有 stitch config 的情况下，chunk 仍会自闭合所有
`+side` 边界（世界边缘行为正确）。

### 2.2 SurfaceNets 实现
[SurfaceNets.cpp](CubeEngine/EngineSrc/3D/Terrain/SurfaceNets.cpp)：
Pass 2 中 `+side` 边界 quad 的判定改为读取 `skipPositive[axis]`，
不再从 `extendPositive` 推导。修复了同 LOD 接缝两侧都发出同一个
boundary quad（导致重复条带）的问题，同时不会再让世界边缘的
chunk 丢面。

### 2.3 Chunk 端改动

[Chunk.h](CubeEngine/Application/CubeGame/Chunk.h)：

- 增加 `m_neighborPresentForGen[3][6]`，用于在 mesh 生成时同时快照
  邻居「是否真的处于 LOADED」。

[Chunk.cpp](CubeEngine/Application/CubeGame/Chunk.cpp)：

- `Chunk::genMesh` 现在同时快照 `neighborLodForLod[3][6]` 和
  `neighborPresent[6]`，分别取自 `neighborChunk->m_currentLOD` 和
  `neighborChunk->m_currenState == State::LOADED`。
  应用上面 §1.3 的 4 条件规则。
  生成后把这两组快照分别写入 `m_neighborLodForGen` 和
  `m_neighborPresentForGen`。
- `Chunk::logicUpdate` 现在每帧先做一次
  `m_currentLOD = calcChunkLod(this, playerPos)`，
  在任何绘制和 re-stitch 之前完成。`submitDrawCmd` 不再修改
  `m_currentLOD`。
- `Chunk::submitDrawCmd` 只读缓存值：自己用自己的 `m_currentLOD`，
  邻居用邻居的 `m_currentLOD`。失配检测仍由它驱动并设置 `m_needRegen`。
- `logicUpdate` 中消费 `m_needRegen` 的路径，从原本的
  `WorkerThreadSystem::pushOrder(...)` 改为**同步**在主线程调用
  `genMesh(...)`。`Chunk::load()` 的初次重建仍走 worker 线程
  （首建是重活），re-stitch 走主线程。

### 2.4 这些修改为什么必要

- 没有显式 `skipPositive` 时，原始版本会同时从「`-side` chunk 的最后一列内部 cell」
  和「`+side` chunk 的 `-side` padding 行」发出同一个 `+X / +Y / +Z` 边界 quad，
  形成沿接缝的共面重叠条带——这正是用户最初观察到的现象。
- 简单地「邻居指针非空就 skip」会让预分配的 `m_chunkArray` 世界中
  整面消失：`getChunk()` 对所有合法坐标都返回非空，但大多数还没 LOADED，
  于是边界谁都不画。改为以 `m_currenState == LOADED` 为判据可修复。
- `submitDrawCmd` 内修改 `m_currentLOD` 会让一帧内邻居 LOD 查询
  与遍历顺序相关（先被 submit 的 chunk 看见的是邻居的「新」LOD，
  后被 submit 的看见「旧」LOD）。把 LOD 刷新挪到 `logicUpdate`
  让一帧内的快照彼此一致。
- `genMesh` 跑在 worker 线程时读取主线程会改写的 `m_currentLOD`，
  worker 任务的延时远大于一帧，导致不一致窗口随相机持续移动而**累积扩大**。
  让 re-stitch 同步跑在主线程，把误差窗口收紧到「本帧 vs 上一帧」级别。

## 3. 仍存在的问题 — 部分接缝出现重叠

现象：相机静止时 mesh 看起来基本正确；持续移动一段时间后，
某些接缝又出现细窄的重叠条带（与最初「永远重复」的现象不同：
这次是**间歇性、位置相关**的）。这说明仍存在某些场景下两侧都画了同一个
boundary quad，或精侧没有正确折叠对应的 quad。

下面按嫌疑从高到低列出最可能的原因。

### 3.1 初次加载的 worker 仍读到陈旧的邻居 LOD

`Chunk::load()` 把 `genMesh(lodLevel)`（以及 init+gen 的版本）
push 到 `WorkerThreadSystem`。worker 任务里现在仍然要读邻居的
`m_currentLOD` 和 `m_currenState`，而这两个值会被主线程持续写。
后果：

- 「首次加载」的 chunk 可能持久化一份与邻居最终结果不一致的快照，
  直到本 chunk 的失配检测下一帧再次把它拉去主线程同步重建。
  在那次重建落地之前，两侧可能同时画接缝。
- 更糟的是，从 `load()` 入队到 worker 真正运行之间，
  相机可能已经导致 `m_currentLOD` 改变。worker 拿到的
  `lodLevel` 形参可能不再等于当前的 `m_currentLOD`，
  于是快照被写到了 `m_neighborLodForGen[lodLevel]` 这**错误的 LOD 行**。

**可能修法**：让 `Chunk::load()` 也走主线程同步路径（真正单线程），
或在入队时一次性把 `playerPos` 与 `(neighborLod, neighborPresent)`
数组**抓拍**进 lambda，worker 内部不再访问其它 chunk 的可变字段。
re-stitch 已经是主线程的；如果允许牺牲 streaming 流畅度，把首建也搬到
主线程是最稳的对照实验。

### 3.2 `extendPositive` 在 Pass 2 与 axis-corner 的相互作用

`extendPositive[axis]` 置位时，Pass 1 在该轴方向多填一行
（到 `maxCell + 1`），但 Pass 2 的外层循环范围保持
`[minCellExt, maxCell]` 不变。在 `x = maxCell - 1` 处
`+side` 的 X 边界 quad 不再被 skip（因为该情况下 `skipPositive[0]` 为 false），
使用的 Pass 1 顶点中包含新填的 `+side` padding 行——这正是预期。

需要审计的是「同时位于 stitched face 与 extended face 之上」的
**轴角 (axis-corner) cell**。Pass 3 中已经显式提到「为避免轴角 T-junction
而把 +side padding 也纳入 snap」，但 Pass 2 在新的归属规则下尚未做
等价的边界审计。可能存在某个角落里 quad 既被本侧（extend）画了
又被另一个轴方向的精侧邻居（未折叠）画的局面。

### 3.3 Pass 3 始终用 `coarseLodLevel = i+1`，跨 LOD 跳跃时不正确

`Chunk::genMesh` 当前只把 `coarseData = lodBuffer.mcPoints[i + 1]`
（即比本 LOD 粗 1 级的那个 buffer）传给 SurfaceNets。
但邻居的 `m_currentLOD` 完全可以比 `i + 1` 还粗
（例如本侧 LOD = 0，邻居 LOD = 2）。在这种「跨级」LOD 转换上：

- Pass 3 把 fine 顶点 snap 到「LOD = 1 的粗 cell」位置，
- 但邻居实际画的是「LOD = 2 的粗 cell」顶点，
- 两个位置不重合 ⇒ snap 不到正确的接缝点 ⇒ 退化失败 ⇒
  fine 侧的小三角仍有面积，与粗侧的接缝几何**几乎共面但不等**，
  视觉上就是细窄的重叠条带。

**可能修法**：当 stitch 面对应的邻居处于更粗的 LOD `Lcoarse` 时，
`coarseData / coarseVoxelSize / coarseLodLevel` 应使用
`mcPoints[Lcoarse]`，而不是恒定 `i + 1`。
`fetchChunkLodBuffer` 已经返回全部 3 级 buffer，可以直接选。
若担心一个 chunk 6 个面的邻居 LOD 不一致，可：
- 对每个面单独传粗 buffer（结构改动较大），或
- 退化策略：当存在不同 `Lcoarse` 时，把那些面 fallback 为
  「不 stitch、由粗侧 `extendPositive` 兜底」，从而仍闭合接缝。

### 3.4 失配检测漏掉了「自身 LOD 变化」

`submitDrawCmd` 的失配比较是
`m_neighborLodForGen[m_currentLOD][face] vs neighbor->m_currentLOD`。
注意这里以**本 chunk 当前的** `m_currentLOD` 作为快照行下标。
当本 chunk 的 LOD 从 0 切到 1，下标行也从行 0 切到行 1：
行 1 中保存的是「上次以 LOD 1 生成 mesh 时」的快照；
如果上次以 LOD 1 生成是很多帧之前的事，此时该行已严重过时，
但比较结果可能恰好「碰巧相等」，于是失配检测**不触发**重建，
仍沿用陈旧 mesh。在玩家来回穿越 LOD 阈值时，这种「ping-pong」
会让陈旧快照堆积。

**可能修法**：在 `submitDrawCmd` 检测到 `m_currentLOD != m_lastSubmittedLOD` 时，
**无条件**置 `m_needRegen.store(true)`，作为兜底。代价极小。

### 3.5 mesh 上传期间 regen 抑制（待确认，非首要）

```cpp
if (m_currenState == State::LOADED && !m_isNeedSubmitMesh && m_needRegen.exchange(false))
{
    genMesh(m_currentLOD);
}
```

`m_needRegen` 在 `m_isNeedSubmitMesh == true` 时不会被消费
（短路评估，标志保留）。下一帧上传完成后才被消费。
理论上是安全的，但如果上传窗口期内连续多次 `store(true)`，
合并成一次 regen 也是预期行为。此项列出以备排查，不是首要嫌疑。

### 3.6 vegetation / foliage 不随 regen 更新

`Chunk::generateVegetation()` 仅在 `Chunk::load()` 中调用一次，
`genMesh` re-stitch 之后不会重新放置植被。
re-stitch 后地形顶点位置变化，但植被的 `getHeight / getNormal`
仍按原始 mesh 形态采样，所以 LOD 接缝附近的草 / 树可能浮空或埋入。
这不是接缝重叠的成因，但会被一并目视到。

### 3.7 **[新增 / 高优先级]** Pass 2 在 `extendPositive` 下越界发 quad

这是上一轮修复未覆盖、但与「相机移动后部分接缝再次出现细窄重叠条带」**最贴合**的根因。

[SurfaceNets.cpp](CubeEngine/EngineSrc/3D/Terrain/SurfaceNets.cpp#L267) Pass 2 的外层循环是无条件的
`[minCellExt, maxCell]`：

```cpp
for (int z = minCellExt; z <= maxCell; ++z)
  for (int y = minCellExt; y <= maxCell; ++y)
    for (int x = minCellExt; x <= maxCell; ++x)
```

- 不带 `extendPositive` 时，`axis == maxCell` 处的 cell 顶点全是 `-1`
  （Pass 1 没填），所有 quad 通过 `c1..c4 != -1` 检查被丢弃，**无害**。
- 带 `extendPositive[axis]` 时，Pass 1 在该轴多填了 `axis == maxCell` 一行；
  此时 Pass 2 在 `axis == maxCell` 处可以**成功**发出 X / Y / Z 三个方向的 quad。
  这些 quad 的 active edge 落在 `axis-corner == maxCell + 1` ——
  在粗 stride 下，这相当于全局体素位置已经**越过 chunk 边界、进入了 +side 邻居领地**
  （偏出一个粗 stride，对应 +side 邻居的细 LOD 第 2 个内部 cell）。

而 +side 邻居（精侧）会在同一全局位置正常画细 quad。结果是：
**一条粗 quad（来自我们的 +side padding 行）与一组细 quad（来自邻居）几乎共面但不等价 → overlap 条带。**

之所以前几次修复没命中：
- `skipPositive` 的修复只对同 LOD 情形完全正确（同 LOD 不会触发 `extendPositive`）。
- 跨 LOD（self 是粗侧）会触发 `extendPositive=true` 并解除 `skipPositive`，
  这条路径上 +X seam quad（位于 `x == maxCell - 1`）本就该由本侧画，是正确的；
  但 Pass 2 同时在 `x == maxCell` **多发了一组本不该存在的 quad**。
- Pass 1 多填那一行的真实用途只有两个：(a) Pass 3 角落 snap；(b) 让 `x == maxCell - 1`
  处的 quad 在另一侧 `x+1` 时能拿到 `valX`（这个用 voxel data 而非 cellVertex，
  其实也不依赖 Pass 1 多填）。Pass 2 在 `x == maxCell` 处的发出**纯属副作用**。

**修法**：让 Pass 2 的「emit」行为与「Pass 1 是否填了 padding」解耦。具体可选其一：

- **(a) 收紧外层循环：** 把 `<= maxCell` 改为 `< maxCell`（即上限改为 `maxCell`，
  仍是半开区间风格）。需要复核：`x == maxCell` 时 Y-quad / Z-quad 是否承担了
  「+X 边界上的 Y 接缝」职责？经追溯 quad 索引（Y-quad 在 `x==maxCell-1` 处用
  `(maxCell-2, y, *)` 与 `(maxCell-1, y, *)`），+X 边界上的 Y / Z 接缝
  其实在 `x == maxCell - 1` 已经发出，所以收紧到 `< maxCell` 是安全的。
  对 Y、Z 维同理。
- **(b) 细粒度跳过：** 在三个 quad 发出条件里都额外加
  `&& !(extendPositive[0] && x == maxCell)`、
  `&& !(extendPositive[1] && y == maxCell)`、
  `&& !(extendPositive[2] && z == maxCell)`。
  与 (a) 等价但更显式，便于回归对照。

推荐 (b)，因为它把意图直接写在条件里，与 `skipPositive / stitchFace` 风格一致；
若回归显示 (a) 也安全，再合并简化。

修复后 §3.2 中关于「轴角 quad 重复」的疑虑也大概率消失：
那条嫌疑的实际触发面就是这里。

## 4. 关于「双 Mesh 预烘焙」方案 — 已搁置

最初设想：每个 chunk 每个 LOD 预烘焙 2 份 mesh（Variant A 不 stitch /
Variant B 假设邻居全部粗 1 级），按面 (face) 切 index range 组合提交，
以彻底解除「邻居状态 → 本侧 mesh 几何」的运行时耦合。

经复盘该方案不可行，原因：

1. **同接缝两侧假设并不冲突**（用户最初担心点）。例如 A=LOD0、B=LOD1，
   A 与 B 的接缝只用到 A 的 Variant B（snap 到 A 自烘焙的 LOD-1 粗
   buffer）和 B 的 Variant A（按本 LOD-1 自然画）。两份 buffer 来自同一
   全局体素函数的 LOD-1 采样，顶点位置完全一致，无缝。这一项**不是问题**。
2. **真正的问题在 Variant B 内部**：Pass 3 的 snap 把 6 个面边界 fine
   cell 顶点都拉到粗位置，而**这些顶点同时是 chunk 内部三角形（次外层
   cell 的内部 quad）的端点**。所以 Variant B 的「内部三角形」几何已经
   被「全 6 面都 stitched」这个全局假设污染。
3. 因此**不能** face 级混用「Variant B 的内部 + Variant A 的某面 quad」
   —— 共享顶点位置不同，会出现错位。要做面级混用，就得为 6 个面所有
   `stitched/不 stitched` 组合各烘焙一份 mesh = **2⁶ = 64 份**，不可行。
4. 退到「整体二选一」（任一邻居更粗就整 chunk 用 B）也不行：例如
   +X 邻居更粗、-X 邻居同 LOD 时，-X 边界 quad 在本侧 B 中已 collapse、
   邻居 A 按约定也不画 → 出洞。

后续若仍需消除运行时 re-stitch 路径，正确方向是 **Transvoxel 风格
的 base + per-face transition strip**：

- base mesh 完全不含边界 quad；
- 每个面单独烘焙一段「boundary strip」，包含**该面专属的边界顶点副本**
  （不与 base 共享），分两种风味（A: 自然位置；B: snap 到粗位置）；
- base 的次外层内部 quad 仍引用 base 自己的边界顶点（自然位置），与
  transition strip 的 snap 顶点之间形成的 T-junction 由 strip 内的
  额外细分三角形掩盖（这正是 Transvoxel 的 transition cell）。

这条路径实现复杂、留待后续单独评估。**当前阶段不动 mesh 拓扑结构**。

## 5. 新方向：放弃当前动态 LOD stitching，改走标准 LOD chunk 架构

当前结论：继续在现有「固定世界 chunk + 可变内部分辨率 + 邻居驱动
re-stitch」结构上修补，成本已经超过收益。这里的问题不只是某个
SurfaceNets bug，而是 chunk 主体 mesh 的几何会依赖运行时邻居状态、
异步加载时机、LOD 快照行和 stitch 配置组合。只要相机持续移动，系统就
很容易再次进入「两侧对接缝归属理解不一致」的状态。

本项目是个人爱好项目，不需要为了短期交付继续堆补丁。因此后续采用
「长痛不如短痛」策略：先把当前系统降回最朴素、稳定、可验证的 chunk
meshing；再引入业界更常见的固定分辨率 LOD chunk / octree node；最后
实现一版基础 stitching / transition。

### 5.1 阶段一：简化当前 chunk，回到无 LOD 的朴素 Surface Nets

目标：先获得一个没有 LOD 接缝问题、行为确定、易调试的体素地形基线。
这一步不是倒退，而是把 SurfaceNets 从复杂 LOD 状态机里解耦出来。

执行计划：

- 当前 `Chunk` 暂时不再记录或切换运行时 LOD。
- `SurfaceNets` 暂时不再计算、接收或处理 LOD stitching 信息。
- 移除或旁路当前 LOD / re-stitch 相关路径：
  - `m_currentLOD`
  - `m_neighborLodForGen`
  - `m_neighborPresentForGen`
  - 由邻居 LOD 失配触发的 `m_needRegen`
  - `SurfaceNetsStitchConfig`
  - `extendPositive`
  - `skipPositive`
  - `stitchFace`
  - coarse buffer snap / Pass 3
- 每个 chunk 只用固定分辨率生成 mesh，例如 `MAX_BLOCK` 对应的最细网格。
- 相邻 chunk 之间只保留最简单的同 LOD 边界归属规则，避免共面重复：
  - 可以继续采用「+side 邻居拥有边界」；
  - 或者让每个 chunk 只生成自身内部面，边界规则固定，不再依赖 LOD。
- mesh 生成不再因为相机移动改变拓扑或顶点位置。
- worker 线程只处理本 chunk 自己的数据，不读取邻居的可变 LOD 状态。

验收标准：

- 相机持续移动时，chunk 接缝不再出现间歇性 overlap 条带。
- 同一世界位置的 mesh 生成结果与相机位置无关。
- 加载 / 卸载只影响 chunk 是否显示，不影响已生成 chunk 的几何解释。
- SurfaceNets 重新变成单纯的 `density field -> mesh` 算法。

### 5.2 阶段二：改成标准 LOD chunk / octree node 结构

目标：不再让固定世界 chunk 内部缩分辨率，而是采用更标准的 LOD node：

- 每个渲染 node 使用固定 cell 分辨率，例如 `N = 16` 或 `N = 32`。
- 不同 LOD 的 node 覆盖不同大小的世界区域：
  - LOD0: `N * baseVoxelSize`
  - LOD1: `N * baseVoxelSize * 2`
  - LOD2: `N * baseVoxelSize * 4`
- LOD 不再表示「同一个 chunk 内部少采样多少格」，而表示「这个 node 的
  voxel size 和世界覆盖范围是多少」。
- 运行时选择一组不重叠的 leaf nodes 进行渲染。
- parent node 和 child node 不能同时渲染同一空间区域。
- 相邻可见 node 尽量保持 2:1 balance，即相邻 LOD 最多差 1。

这会把当前复杂的「固定 chunk 边界 + 动态邻居 LOD」问题，转换为更标准的：

- parent / child 覆盖关系
- leaf selection
- LOD transition boundary
- 2:1 balanced neighbor seam

新的核心数据应围绕 `TerrainNode` / `LodChunk` 设计，而不是让旧 `Chunk`
继续承担加载、LOD 选择、mesh 生成、接缝修补全部逻辑。

建议的新 node 概念：

```cpp
struct TerrainNode
{
    glm::ivec3 originVoxel;   // 当前 LOD 网格下的全局起点
    int lod;                  // voxelSize = 1 << lod
    int resolution;           // 固定，例如 32
    float voxelSize;
    Bounds worldBounds;

    TerrainMesh mesh;
    TerrainNode* parent;
    TerrainNode* children[8];

    bool isLeaf;
    bool isLoaded;
    bool needsMesh;
};
```

阶段二先不追求完美 stitching，只先完成：

- octree / loose octree / clipmap 风格的 node 选择；
- 固定分辨率 node 的 density 采样；
- 每个 leaf node 独立生成 Surface Nets base mesh；
- 禁止 parent 和 child 重叠渲染；
- 记录相邻 leaf 的 LOD，用于下一阶段 transition。

验收标准：

- LOD 切换后不会出现大块重叠 mesh。
- 每个 node 的 mesh 只由自己的 `(origin, lod, resolution)` 决定。
- SurfaceNets 本身仍然不知道「相机」和「邻居状态」。
- LOD 选择逻辑和 mesh 生成逻辑解耦。

### 5.3 阶段三：实现一版基础 stitching / transition

目标：在标准 LOD node 架构上实现第一版能用的 LOD 接缝处理。

原则：

- 不再让整个 chunk mesh 因为某一个邻居 LOD 不同而整体 re-stitch。
- base mesh 尽量保持与邻居无关。
- LOD seam 作为独立的 per-face transition 问题处理。
- 优先保证稳定和可调试，再追求拓扑完美。

可选方案按实现难度排序：

#### 方案 A：skirt

最简单，对每个 LOD 边界向下 / 向内拉一圈 skirt。

优点：

- 实现快。
- 不需要复杂拓扑表。
- 可以快速遮住裂缝。

缺点：

- 不能真正消除重叠或 T-junction。
- 在悬崖、洞穴、负空间地形上可能露馅。
- 更像视觉补丁，不是严格几何解。

适合作为第一版过渡方案。

#### 方案 B：fine side snap 到 coarse boundary

当前方案的简化版，但只用于标准 2:1 LOD seam。

约束：

- 相邻 LOD 最多差 1。
- 只处理 leaf node 面之间的 transition。
- snap 只发生在边界 transition strip，不污染整个 base mesh。

优点：

- 比当前方案清晰。
- 可以复用部分 Surface Nets 粗细对应逻辑。
- 适合快速得到一版可接受的 stitching。

缺点：

- 如果边界顶点同时被内部三角形共享，仍可能污染 base mesh。
- 最好给 transition strip 使用独立顶点副本。

#### 方案 C：Transvoxel 风格 transition cell

长期最正确的方向。

结构：

- base mesh 负责 node 内部主体。
- 每个 LOD seam 面单独生成 transition strip。
- transition strip 有自己的边界顶点，不与 base mesh 的内部顶点强共享。
- transition 只处理 coarse / fine 之间的局部拓扑匹配。

优点：

- 架构正确。
- seam 局部化。
- 不需要因邻居变化重建整个 chunk 主体 mesh。

缺点：

- 实现复杂。
- 需要查表或重新设计 transition cell 生成逻辑。
- 调试成本较高。

当前建议：阶段三先做方案 A 或 B，等 LOD node 架构跑顺后，再评估是否
实现方案 C。

### 5.4 当前旧方案的处理策略

旧的动态 re-stitch 方案暂时不继续扩展。

保留本文档前面的问题记录，作为复盘材料，但后续不再优先修：

- §3.1 worker 入队后读取陈旧邻居 LOD；
- §3.3 跨级 LOD 时 coarse buffer 选择错误；
- §3.4 自身 LOD 变化未强制 regen；
- §3.5 mesh 上传期间 regen 抑制；
- §3.6 vegetation 不随 regen 更新；
- §3.7 `extendPositive` 在 Pass 2 下越界发 quad；
- §4 双 mesh 预烘焙方案不可行。

其中 §3.7 仍可作为短期对照实验修复，但不再作为主路线。主路线改为：

1. 先恢复无 LOD、无 stitching 的稳定 Surface Nets。
2. 再实现固定分辨率、可变覆盖范围的标准 LOD node。
3. 最后做 per-face transition / stitching。

### 5.5 最终目标

最终希望 terrain pipeline 变成：

```text
density function
    -> fixed-resolution terrain node sampling
    -> Surface Nets base mesh
    -> LOD leaf selection
    -> per-face transition mesh
    -> render selected leaves + transitions
```

而不是当前的：

```text
chunk
    -> 查询邻居 LOD / 加载状态
    -> 修改本 chunk Surface Nets 行为
    -> 动态 snap / skip / extend
    -> 相机移动后不断 re-stitch
```

核心设计目标：

- SurfaceNets 只负责局部网格转 mesh。
- LOD 系统只负责选择哪些 node 可见。
- stitching 系统只负责 LOD seam 的局部 transition。
- 三者解耦，避免一个 chunk 的主体 mesh 依赖邻居的瞬时状态。

## 6. 范围外的观察（独立清理项）

- `Chunk::getPoint(x,y,z)` 在跨邻居读取分支里使用
  `(MAX_BLOCK + 1)^3` 的索引尺寸，但实际 buffer 是
  `(MAX_BLOCK + MIN_PADDING + MAX_PADDING)^3`。所有现存调用都会读到
  错位数据。需 grep 调用点，要么修正尺寸，要么删除该方法。
- `Chunk::generateVegetation()` 把 `grassDensity = 1.0` 同时用作
  循环步长和采样率，导致 `grassDensity` 实际上不影响密度。
  应该用独立的 `step` 参数。
- 该文件中已有大量 `C4244` narrowing 警告，掩盖了真正的新增警告，
  建议另开任务专门清理，与 LOD 调试解耦。
