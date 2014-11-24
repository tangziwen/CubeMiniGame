		Cube Engine
	Cube Engine is my own hobby porject, It's a small Graphic Engine written in c++ and powered by OpenGL (ES 2.0 or higher), I develop so that I can improve my knowledges of CG and 
	common programming Skills(generic algorithm ,code style ,design patterns).
	any questions or idea please contace me :) 
				tangziwen 	tzwtangziwen@163.com , ziwen.tang@chukong-inc.com;
Features:
	Camera Control
	Commons Model Format loading with Skeleton Animation , SkyBox ,Terrain rendering by HeightMap
	lighting
	NormalMapping 
	ShadowMapping with soft edges

--alpha v0.05 11.24.2014
now lots's of Classes are inherited from the 'Node' class, you can easy handle them(Entity,Camera ,etc) hierarchically via unified interface  
engine now can render terrian from a height map , see demo

--alpha v0.04 11.19.2014
add normal mapping

--alpha v0.03-1 11.17.2014
refactor code

--alpha v0.03 11.13.2014
add skybox
fix texture's path 
refactor code

--alpha v0.02 11.10.2014
add skeleton animation
add shadow map

--alpha v0.01 10.30.2014
basic camera
support for model loading.
material system
basic lighting : directionalLight, spotLight,pointLight
compatible with OpenGL ES 2.0
