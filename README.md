# rayTracing checklist

- [x] Reflection
 * fairly simple, just compute angle of reflection and send off a ray with a tiny offset
 
- [x] Shadow
 * current implementation follows a ray from light source to intersection. If a different intersection occurs, we're in shadow.
 * correct to account for opaque and transparent objects
 
- [ ] Anti Aliasing
 * currently implemented but want to create a distribution which scales nicely
 
- [ ] Texture Mapping
 * very bland implementation based on mathematically defined texture. ie, checkerboard 
 
- [ ] Refraction
 * seems to be problems in the sphere. Need to take another look at it

- [x] Cylinder

- [x] Sphere

- [x] Plane
 * shapes are fairly straight forward. All mathematically defined with intersections based on parametric definitions

- [ ] shapes defined by series of triangles. 
 * Need BSP trees for this one as a high polygon object may be very costly.

- [ ] Area Light
 * figure out issue with area light source. Some objects aren't correctly displaying shadow

- [x] Point Light

- [ ] GPU accelerated
