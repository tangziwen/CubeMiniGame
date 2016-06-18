/**
\mainpage CubeEngine
\tableofcontents
\section 作者

tangziwen <b>tangziwen.github.com</b>

\section 基本介绍
CubeEngine 是一个简单易用的2D/3D，开发其目的旨在提高我个人自身的图形学编程造诣，目前开发的版本为CubeEngine brand new 版本，
该版本与之前的老版本<b>完全</b>不兼容，架构上也有非常大的差异，但同时整体代码架构比之前合理了不少，设计上减少了大量的冗余与不合理的部分，
架构非常的精简。

目前CubeEngine brand new的开发仍然处在相当的初步的状态下(开发时长不到一周)，但是仍具备了相当多有趣的新特性：
*2D精灵绘制，可以使用引擎做一些2D游戏。
*分离的渲染与逻辑更新，和之前版本的Cube Engine 相比，将实际调用GL API的部分单独分离，使得整个渲染流程大大的简化，易于错误的查找，同时易于扩展。
*Technique机制的引入，新版本引入了Technique机制从而让使用者对每一个物件的渲染状态能进行很大程度上的把控，而之前版本仅仅是针对不同物件单独写死的代码，非常不灵活。
*完全的脚本化支持，使用AngelScript脚本语言作为Cube Engine brand new的脚本语言，引擎C++端的接口基本做到了一比一导出至脚本端，同时仍保留C++端类的层次结构，非常适合于偏向于使用脚本语言进行
严肃开发的用户。

老旧版本的CubeEngine 可以在github仓库：https://github.com/tangziwen/Cube-Engine 上找到

\section 如何编译与运行
\subsection 如何编译
CubeEngine是完全使用C++ 11编写的，因此理论上任意上的现代C++编译器都能够正确编译。
当然现阶段，CubeEngine使用Qt的一部分组建，所以你需要QtSdk来支持，同时CubeEngine的BackEnd模块若干类调用OpenGL的API,你还需要有一块
支持Opengl 2.0 desktop 或者 openGL es 2.0的显卡且安装了合适驱动的计算机(手机)才能运行

\subsection 如何运行
    当你将CubeEngine正确编译为可执行程序之后，只需要将根目录下的Res文件夹移至CubeEngine可执行程序对应的工作目录（通常是与其平级目录，或上一目录)。
    Res 文件夹内存放的是资源文件，资源文件包含三个部分:

    - 引擎自己内部使用的资源
    - 以及用户需要读取的资源
    - 引擎执行时的入口脚本文件

要知道详细的资源配置情况请查看资源页 \ref res_page 引擎资源目录的规划
*/
