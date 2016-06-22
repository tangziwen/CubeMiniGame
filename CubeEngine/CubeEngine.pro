#-------------------------------------------------
#
# Project created by QtCreator 2015-08-27T16:28:01
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += opengl

TARGET = CubeEngine
CONFIG += c++11
CONFIG   -= app_bundle

TEMPLATE = app

#OpenGL
LIBS += -L.
####External library BEGIN####
INCLUDEPATH += ./External

#FreeType#
LIBS += -L$$PWD/lib -lfreetype.dll
INCLUDEPATH += ./External/FreeType
#GLFW 3.0#
LIBS += -L$$PWD/lib -lglfw3dll
INCLUDEPATH += ./External/GLFW3/include

#GLEW #
LIBS += -L$$PWD/lib -lglew32s
INCLUDEPATH += ./External/GLEW

LIBS += -lopengl32 -lglu32 -lglut32
####External library END####

SOURCES += main.cpp \
    EngineSrc/Mesh/Mesh.cpp \
    EngineSrc/Math/vec3.cpp \
    EngineSrc/Math/vec2.cpp \
    EngineSrc/Mesh/VertexData.cpp \
    EngineSrc/Rendering/Renderer.cpp \
    EngineSrc/Rendering/RenderCommand.cpp \
    EngineSrc/Technique/Technique.cpp \
    EngineSrc/Base/Node.cpp \
    EngineSrc/2D/Sprite.cpp \
    EngineSrc/Rendering/RenderBuffer.cpp \
    EngineSrc/BackEnd/RenderBackEnd.cpp \
    EngineSrc/Technique/TechniqueVar.cpp \
    EngineSrc/Texture/Texture.cpp \
    EngineSrc/Base/Camera.cpp \
    EngineSrc/Interface/Drawable.cpp \
    EngineSrc/Texture/TextureMgr.cpp \
    EngineSrc/Scene/Scene.cpp \
    EngineSrc/Scene/SceneMgr.cpp \
    EngineSrc/Engine/Engine.cpp \
    External/SOIL/image_DXT.c \
    External/SOIL/image_helper.c \
    External/SOIL/SOIL.c \
    External/SOIL/stb_image_aug.c \
    EngineSrc/Shader/ShaderProgram.cpp \
    External/TUtility/file/Data.cpp \
    External/TUtility/file/Tfile.cpp \
    External/TUtility/log/Tlog.cpp \
    External/TUtility/log/TstatckTrace.cpp \
    External/TUtility/math/TbaseMath.cpp \
    External/TUtility/math/Tvector.cpp \
    External/TUtility/misc/tmisc.cpp \
    External/TUtility/string/Tstring.cpp \
    EngineSrc/Math/vec4.cpp \
    EngineSrc/3D/Primitive/Cube.cpp \
    EngineSrc/Event/EventMgr.cpp \
    EngineSrc/Event/Event.cpp \
    EngineSrc/Shader/ShaderMgr.cpp \
    EngineSrc/Math/AABB.cpp \
    EngineSrc/Interface/Drawable3D.cpp \
    EngineSrc/Math/Ray.cpp \
    EngineSrc/Scene/OctreeScene.cpp \
    EngineSrc/Math/Frustum.cpp \
    EngineSrc/Math/t_Plane.cpp \
    External/noise/noiseutils.cpp \
    External/noise/latlon.cpp \
    External/noise/noisegen.cpp \
    External/noise/model/cylinder.cpp \
    External/noise/model/line.cpp \
    External/noise/model/plane.cpp \
    External/noise/model/sphere.cpp \
    External/noise/module/abs.cpp \
    External/noise/module/add.cpp \
    External/noise/module/billow.cpp \
    External/noise/module/blend.cpp \
    External/noise/module/cache.cpp \
    External/noise/module/checkerboard.cpp \
    External/noise/module/clamp.cpp \
    External/noise/module/const.cpp \
    External/noise/module/curve.cpp \
    External/noise/module/cylinders.cpp \
    External/noise/module/displace.cpp \
    External/noise/module/exponent.cpp \
    External/noise/module/invert.cpp \
    External/noise/module/max.cpp \
    External/noise/module/min.cpp \
    External/noise/module/modulebase.cpp \
    External/noise/module/multiply.cpp \
    External/noise/module/perlin.cpp \
    External/noise/module/power.cpp \
    External/noise/module/ridgedmulti.cpp \
    External/noise/module/rotatepoint.cpp \
    External/noise/module/scalebias.cpp \
    External/noise/module/scalepoint.cpp \
    External/noise/module/select.cpp \
    External/noise/module/spheres.cpp \
    External/noise/module/terrace.cpp \
    External/noise/module/translatepoint.cpp \
    External/noise/module/turbulence.cpp \
    External/noise/module/voronoi.cpp \
    EngineSrc/BackEnd/FontEngine.cpp \
    EngineSrc/Font/Font.cpp \
    EngineSrc/Math/Rect.cpp \
    EngineSrc/Game/ConsolePanel.cpp \
    EngineSrc/Font/FontMgr.cpp \
    EngineSrc/Action/Action.cpp \
    EngineSrc/Action/ActionMgr.cpp \
    EngineSrc/Action/ActionInterval.cpp \
    EngineSrc/Action/MoveTo.cpp \
    EngineSrc/Interface/DepthPolicy.cpp \
    EngineSrc/Game/DebugInfoPanel.cpp \
    EngineSrc/Texture/TextureFrame.cpp \
    EngineSrc/Texture/TextureAtlas.cpp \
    EngineSrc/Font/GlyphData.cpp \
    EngineSrc/Font/GlyphAtlas.cpp \
    EngineSrc/2D/LabelNew.cpp \
    EngineSrc/2D/GUIFrame.cpp \
    EngineSrc/2D/Button.cpp \
    EngineSrc/2D/GUITitledFrame.cpp \
    EngineSrc/2D/GUIRadioFrame.cpp \
    EngineSrc/Engine/AppEntry.cpp \
    Application/GameEntry.cpp \
    EngineSrc/Script/Tokenizer.cpp \
    EngineSrc/Script/ScriptAST.cpp \
    EngineSrc/Script/ScriptVM.cpp \
    EngineSrc/Script/ScriptByteCode.cpp \
    EngineSrc/Script/ScriptValue.cpp \
    EngineSrc/Script/ScriptVariableFrame.cpp \
    EngineSrc/2D/GUIWindowMgr.cpp \
    EngineSrc/Engine/EngineDef.cpp \
    EngineSrc/2D/GUIStyle.cpp \
    EngineSrc/2D/GUIStyleMgr.cpp \
    EngineSrc/Math/Matrix44.cpp \
    EngineSrc/Math/Quaternion.cpp \
    EngineSrc/Action/RotateTo.cpp \
    EngineSrc/Action/RotateBy.cpp \
    EngineSrc/BackEnd/WindowBackEnd.cpp \
    EngineSrc/BackEnd/GLFW/GLFW_BackEnd.cpp \
    EngineSrc/BackEnd/WindowBackEndMgr.cpp \
    EngineSrc/BackEnd/AbstractDevice.cpp

HEADERS += \
    EngineSrc/Mesh/Mesh.h \
    EngineSrc/Math/vec3.h \
    EngineSrc/Math/vec2.h \
    EngineSrc/Mesh/VertexData.h \
    EngineSrc/Rendering/Renderer.h \
    EngineSrc/Rendering/RenderCommand.h \
    EngineSrc/Technique/Technique.h \
    EngineSrc/Base/Node.h \
    EngineSrc/2D/Sprite.h \
    EngineSrc/Rendering/RenderBuffer.h \
    EngineSrc/BackEnd/RenderBackEnd.h \
    EngineSrc/Technique/TechniqueVar.h \
    EngineSrc/Texture/Texture.h \
    EngineSrc/Base/Camera.h \
    EngineSrc/Interface/Drawable.h \
    EngineSrc/Texture/TextureMgr.h \
    EngineSrc/Scene/Scene.h \
    EngineSrc/Scene/SceneMgr.h \
    EngineSrc/Engine/Engine.h \
    External/SOIL/image_DXT.h \
    External/SOIL/image_helper.h \
    External/SOIL/SOIL.h \
    External/SOIL/stb_image_aug.h \
    External/SOIL/stbi_DDS_aug.h \
    External/SOIL/stbi_DDS_aug_c.h \
    EngineSrc/CubeEngine.h \
    EngineSrc/Shader/ShaderProgram.h \
    External/TUtility/file/Data.h \
    External/TUtility/file/Tfile.h \
    External/TUtility/log/Tlog.h \
    External/TUtility/log/TstatckTrace.h \
    External/TUtility/math/TbaseMath.h \
    External/TUtility/math/Tvector.h \
    External/TUtility/misc/tmisc.h \
    External/TUtility/string/Tstring.h \
    External/TUtility/TUtility.h \
    EngineSrc/doc/mainpage.h \
    EngineSrc/doc/res_page.h \
    EngineSrc/doc/use_scripte.h \
    EngineSrc/Math/vec4.h \
    EngineSrc/3D/Primitive/Cube.h \
    EngineSrc/Event/EventMgr.h \
    EngineSrc/Event/Event.h \
    EngineSrc/Shader/ShaderMgr.h \
    EngineSrc/Math/AABB.h \
    EngineSrc/Interface/Drawable3D.h \
    EngineSrc/Math/Ray.h \
    EngineSrc/Math/Plane.h \
    EngineSrc/Scene/OctreeScene.h \
    EngineSrc/Math/Frustum.h \
    External/noise/basictypes.h \
    External/noise/exception.h \
    External/noise/interp.h \
    External/noise/latlon.h \
    External/noise/mathconsts.h \
    External/noise/misc.h \
    External/noise/noise.h \
    External/noise/noisegen.h \
    External/noise/noiseutils.h \
    External/noise/vectortable.h \
    External/noise/model/cylinder.h \
    External/noise/model/line.h \
    External/noise/model/model.h \
    External/noise/model/plane.h \
    External/noise/model/sphere.h \
    External/noise/module/abs.h \
    External/noise/module/add.h \
    External/noise/module/billow.h \
    External/noise/module/blend.h \
    External/noise/module/cache.h \
    External/noise/module/checkerboard.h \
    External/noise/module/clamp.h \
    External/noise/module/const.h \
    External/noise/module/curve.h \
    External/noise/module/cylinders.h \
    External/noise/module/displace.h \
    External/noise/module/exponent.h \
    External/noise/module/invert.h \
    External/noise/module/max.h \
    External/noise/module/min.h \
    External/noise/module/module.h \
    External/noise/module/modulebase.h \
    External/noise/module/multiply.h \
    External/noise/module/perlin.h \
    External/noise/module/power.h \
    External/noise/module/ridgedmulti.h \
    External/noise/module/rotatepoint.h \
    External/noise/module/scalebias.h \
    External/noise/module/scalepoint.h \
    External/noise/module/select.h \
    External/noise/module/spheres.h \
    External/noise/module/terrace.h \
    External/noise/module/translatepoint.h \
    External/noise/module/turbulence.h \
    External/noise/module/voronoi.h \
    EngineSrc/BackEnd/FontEngine.h \
    EngineSrc/Font/Font.h \
    EngineSrc/Math/Rect.h \
    EngineSrc/Game/ConsolePanel.h \
    EngineSrc/Font/FontMgr.h \
    EngineSrc/Action/Action.h \
    EngineSrc/Action/ActionMgr.h \
    EngineSrc/Action/ActionInterval.h \
    EngineSrc/Action/MoveTo.h \
    EngineSrc/Interface/DepthPolicy.h \
    EngineSrc/Rendering/RenderFlag.h \
    EngineSrc/Game/DebugInfoPanel.h \
    EngineSrc/Texture/TextureFrame.h \
    EngineSrc/Texture/TextureAtlas.h \
    EngineSrc/Font/GlyphData.h \
    EngineSrc/Font/GlyphAtlas.h \
    EngineSrc/2D/LabelNew.h \
    EngineSrc/2D/GUIFrame.h \
    EngineSrc/2D/Button.h \
    EngineSrc/2D/GUITitledFrame.h \
    EngineSrc/2D/GUIRadioFrame.h \
    EngineSrc/Engine/AppEntry.h \
    Application/GameEntry.h \
    EngineSrc/Script/Tokenizer.h \
    EngineSrc/Script/ScriptAST.h \
    EngineSrc/Script/ScriptVM.h \
    EngineSrc/Script/ScriptByteCode.h \
    EngineSrc/Script/ScriptValue.h \
    EngineSrc/Script/ScriptVariableFrame.h \
    EngineSrc/2D/GUIWindowMgr.h \
    EngineSrc/Engine/EngineDef.h \
    EngineSrc/2D/GUIStyle.h \
    EngineSrc/2D/GUIStyleMgr.h \
    EngineSrc/Math/Matrix44.h \
    EngineSrc/Math/Quaternion.h \
    EngineSrc/Action/RotateTo.h \
    EngineSrc/Action/RotateBy.h \
    EngineSrc/BackEnd/WindowBackEnd.h \
    EngineSrc/BackEnd/GLFW/GLFW_BackEnd.h \
    EngineSrc/BackEnd/WindowBackEndMgr.h \
    EngineSrc/BackEnd/AbstractDevice.h
