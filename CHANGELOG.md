![](logo.png)
===================

#ChangeLog#

here is the changelog of the CubeEngine

###alpha v0.11 3.23.2015###
* add two water simulation classes - waterNaive & waterPG(Projected grid water),use waterNaive to simulate a small water area, use waterPG to simulate unlimited water surface like ocean.
* add RenderTarget class, you can use RenderTarget to control the render pipeline.for example you can use it to simulate mirror.
* Lot's of bug Fixed.

###alpha v0.10 3.10.2015###
* fix the normalmapping bug
* add the "RenderTarget" class,you can use it to simulate such as mirror effect
* add the "Water" class , begin to add support of the water simulation, currently, it caculate on CPU side,
and lack of some features.

###alpha v0.09b 3.10.2015###
* drop the support of the forward-rendering.

###alpha v0.09a 3.9.2015###
* support for cascaded shadow maps(CSM) for directional light,you can use it for huge scene shadow generation.

###alpha v0.08b 2.2.2015###
* support multiple shadowMap(finally support directional light)
* code refactor
* add a brief chinese doc

###alpha v0.08 1.27.2015###
* Add AABB
* Add Ray
* Fix material BUG
* SkyBox finnaly support in deferred-rendering mode
* Add Bloom Effect
* Add simple 2D element
* Add Node Groupping 

###alpha v0.07 12.18.2014###
* add basic deferred-shading support, the deferred shading system can allow you create lots of Lights.
* lot's of bug fixed

###alpha v0.06 12.2.2014###
* Refactor lot's of code
* Fix the Terrain's default position

###alpha v0.05 11.24.2014###
* Now lots's of Classes are inherited from the 'Node' class, you can easy handle them(Entity,Camera ,etc) hierarchically via unified interface  
engine now can render terrian from a height map , see demo

###alpha v0.04 11.19.2014#
* Add normal mapping

###alpha v0.03-1 11.17.2014#
* Refactor code

###alpha v0.03 11.13.2014#
* Add skybox
* Fix texture's path 
* Refactor code

###alpha v0.02 11.10.2014#
* Add skeleton animation
* Add shadow map

###alpha v0.01 10.30.2014#
* Basic camera
* Support for model loading.
* Material system
* Basic Lighting : directionalLight , spotLight , pointLight