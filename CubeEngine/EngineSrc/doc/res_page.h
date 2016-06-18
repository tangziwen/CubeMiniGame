/**
\page res_page 引擎资源目录的规划
\tableofcontents

\section 简介
引擎资源，无论是用户使用的还是引擎自身使用的，都被放在根目录下的Res文件夹中，为了正确运行引擎，需要将其放置在引擎可执行程序运行的工作目录(通常是与可执行文件平级或上层目录)下
\section 引擎内部使用资源
引擎内部使用的资源被存放在Res目录下的EngineCoreRes目录里，用户请勿修改其内容以及目录的结构
\section 用户使用资源
用户使用资源被放在Res目录下的User目录里，整个User目录下的结构，可根据程序的需要进行改动，如Textures或者Images来存放纹理资源等。
\section 引擎的脚本入口
在Res目录下有一个entry.as，这个是引擎的脚本入口文件，你可以在其中在该文件中include自己的脚本文件以作为扩展，具体的使用方式请参看\ref use_script
*/

