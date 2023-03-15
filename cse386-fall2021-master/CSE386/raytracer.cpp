/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/
#include "raytracer.h"
#include "ishape.h"
#include "io.h"

/**
 * @fn	RayTracer::RayTracer(const color &defa)
 * @brief	Constructs a raytracers.
 * @param	defa	The clear color.
 */

RayTracer::RayTracer(const color &defa)
	: defaultColor(defa) {
}

/**
 * @fn	void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth, const IScene &theScene) const
 * @brief	Raytrace scene
 * @param [in,out]	frameBuffer	Framebuffer.
 * @param 		  	depth	   	The current depth of recursion.
 * @param 		  	theScene   	The scene.
 */

void RayTracer::raytraceScene(FrameBuffer &frameBuffer, int depth,
								const IScene &theScene, int N) const {
	const RaytracingCamera &camera = *theScene.camera;
	const vector<VisibleIShapePtr> &objs = theScene.opaqueObjs;
	const vector<VisibleIShapePtr>& transObjs = theScene.transparentObjs;
	const vector<PositionalLightPtr> &lights = theScene.lights;

	for (int y = 0; y < frameBuffer.getWindowHeight(); ++y) {
		for (int x = 0; x < frameBuffer.getWindowWidth(); ++x) {
			DEBUG_PIXEL = (x == xDebug && y == yDebug);
			/* CSE 386 - todo  */
		
			HitRecord hit; 
			HitRecord transHit; // trans hit
			color sum = black; // anti-ailising
			color clr;

			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					// off set antiaisling
					Ray ray = camera.getRay(x + (1.0 / (2.0 * N)) + (j * ( 1.0 / N )), y + (1.0 / (2.0 * N)) + (i * ( 1.0 / N))); 
					VisibleIShape::findIntersection(ray, objs, hit); // opaque hit
					VisibleIShape::findIntersection(ray, transObjs, transHit);
					
					// backfaces
					dvec3 d = ray.origin - hit.interceptPt;
					if (glm::dot(hit.normal, -d) > 0) {
						hit.normal = -hit.normal;
					}

					if (hit.t != FLT_MAX && transHit.t == FLT_MAX) { // opaque hit no trans hit
						if (hit.texture != nullptr) {
							color texel = hit.texture->getPixelUV(hit.u, hit.v);
							clr = traceIndividualRay(ray, theScene, depth); 
							color mixture = texel / 2.0 + clr / 2.0;
							sum += mixture;
						}
						else {
							clr = traceIndividualRay(ray, theScene, depth);
							sum += clr;
						}
					}
					else if (hit.t != FLT_MAX && transHit.t != FLT_MAX) { // opaque hit and trans hit true
						if (hit.t < transHit.t) {
							if (DEBUG_PIXEL) {
								cout << "";
							}
							clr = calTotalColor(theScene, hit, objs);
							sum += clr;

							if (hit.texture != nullptr) {
								color texel = hit.texture->getPixelUV(hit.u, hit.v);
								clr = 0.5 * clr + 0.5 * texel;
								sum += clr;
							}
						}
						else {
							color source = transHit.material.ambient;
							color des;
							des = calTotalColor(theScene, hit, objs);
							clr = (1 - transHit.material.alpha) * des + transHit.material.alpha * source;
							if (hit.texture != nullptr) {
								color texel = hit.texture->getPixelUV(hit.u, hit.v);
								clr = 0.5 * clr + 0.5 * texel;
							}
							sum += clr;
						}

					}
					else if (transHit.t != FLT_MAX && hit.t == FLT_MAX) { // only trans hit 
						color backG = defaultColor;
						color blend = calTotalColor(theScene, transHit, transObjs) + backG;
						sum += blend;
					}
					else { // no hit 
						color c = defaultColor;
						sum += c;
					}
				}
			}
			sum /=  N * N;
			frameBuffer.setColor(x, y, sum);
			
			frameBuffer.showAxes(x, y, camera.getRay(x,y), 0.25);			// Displays R/x, G/y, B/z axes
		}
	}

	frameBuffer.showColorBuffer();
}

/**
 * @fn	color RayTracer::traceIndividualRay(const Ray &ray, 
 *											const IScene &theScene,
 *											int recursionLevel) const
 * @brief	Trace an individual ray.
 * @param	ray			  	The ray.
 * @param	theScene	  	The scene.
 * @param	recursionLevel	The recursion level.
 * @return	The color to be displayed as a result of this ray.
 */

color RayTracer::traceIndividualRay(const Ray& ray, const IScene& theScene, int recursionLevel) const {
	/* CSE 386 - todo  */
	// This might be a useful helper function.
	HitRecord hit, reflectHit;
	color clr;
	color totalLight;
	const vector<PositionalLightPtr>& lights = theScene.lights;
	const RaytracingCamera& camera = *theScene.camera;

	VisibleIShape::findIntersection(ray, theScene.opaqueObjs, hit);
	dvec3 origin = hit.interceptPt + EPSILON * hit.normal; // reflection origin
	dvec3 direction = ray.dir - 2 * (glm::dot(ray.dir, hit.normal)) * hit.normal; // reflection direction
	VisibleIShape::findIntersection(Ray(origin, direction), theScene.opaqueObjs, reflectHit);
	if (hit.t != FLT_MAX) {
		
		if (reflectHit.t != FLT_MAX) {
			for (int j = 0; j < lights.size(); j++) {
				color c = lights[j]->illuminate(hit.interceptPt, hit.normal, hit.material, camera.getFrame(),
					inShadow(lights[j]->actualPosition(theScene.camera->getFrame()), hit.interceptPt, hit.normal, theScene.opaqueObjs));
				totalLight += c;
			} 

			if (recursionLevel == 0) { return totalLight; }
			totalLight += 0.3 * traceIndividualRay(Ray(origin, direction), theScene, recursionLevel - 1);
		} 
		else {
			for (int j = 0; j < lights.size(); j++) {
				color c = lights[j]->illuminate(hit.interceptPt, hit.normal, hit.material, camera.getFrame(),
					inShadow(lights[j]->actualPosition(theScene.camera->getFrame()), hit.interceptPt, hit.normal, theScene.opaqueObjs));
				clr += c;
			}
			totalLight = clr;
		}
	}
	return totalLight;
}

/**
* Helper method to calculate the total color
*/
color RayTracer::calTotalColor(const IScene& theScene, HitRecord& hit, const vector<VisibleIShapePtr>& objs) const {
	color clr;

	const vector<PositionalLightPtr>& lights = theScene.lights;
	const RaytracingCamera& camera = *theScene.camera;

	for (int j = 0; j < lights.size(); j++) {
		color c = lights[j]->illuminate(hit.interceptPt, hit.normal, hit.material, camera.getFrame(),
			inShadow(lights[j]->actualPosition(theScene.camera->getFrame()), hit.interceptPt, hit.normal, objs));
		clr += c;
	}
	return clr;
}
