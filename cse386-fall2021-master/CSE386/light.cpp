/****************************************************
 * 2016-2021 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted.
 ****************************************************/

#include "light.h"
#include "io.h"

/**
 * @fn	color ambientColor(const color &mat, const color &light)
 * @brief	Computes the ambient color produced by a single light at a single point.
 * @param	mat  	Ambient material property.
 * @param	lightAmbient	Light's ambient color.
 * @return	Ambient color.
  */

color ambientColor(const color &mat, const color &lightAmbient) {
	/* CSE 386 - todo  */
	return glm::clamp(mat * lightAmbient, 0.0, 1.0);
}

/**
 * @fn	color diffuseColor(const color &mat, const color &light, const dvec3 &l, const dvec3 &n)
 * @brief	Computes diffuse color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightDiffuse	 	The light color.
 * @param	l		 	Light vector.
 * @param	n		 	Normal vector.
 * @return	Diffuse color.
 */

color diffuseColor(const color &mat, const color &lightDiffuse,
					const dvec3 &l, const dvec3 &n) {
	/* CSE 386 - todo  */
	double dp = glm::dot(l, n);
	return glm::clamp(mat * lightDiffuse * dp, 0.0, 1.0);
}

/**
 * @fn	color specularColor(const color &mat, const color &light, double shininess, 
 *							const dvec3 &r, const dvec3 &v)
 * @brief	Computes specular color produce by a single light at a single point.
 * @param	mat		 	Material.
 * @param	lightSpecular	 	The light's color.
 * @param	shininess	Material shininess.
 * @param	r		 	Reflection vector.
 * @param	v		 	Viewing vector.
 * @return	Specular color.
 */

color specularColor(const color &mat, const color &lightSpecular,
					double shininess,
					const dvec3 &r, const dvec3 &v) {
	/* CSE 386 - todo  */
	double dp = glm::clamp(glm::dot(r, v), 0.0, 1.0);
	return glm::clamp(mat * lightSpecular * std::pow(dp,shininess), 0.0, 1.0);
}

/**
 * @fn	color totalColor(const Material &mat, const LightColor &lightColor, 
 *						const dvec3 &viewingDir, const dvec3 &normal, 
 *						const dvec3 &lightPos, const dvec3 &intersectionPt, 
 *						bool attenuationOn, const LightAttenuationParameters &ATparams)
 * @brief	Color produced by a single light at a single point.
 * @param	mat			  	Material.
 * @param	lightColor	  	The light's color.
 * @param	v	  			The v vector.
 * @param	n   		  	Normal vector.
 * @param	lightPos	  	Light position.
 * @param	intersectionPt	(x,y,z) of intersection point.
 * @param	attenuationOn 	true if attenuation is on.
 * @param	ATparams	  	Attenuation parameters.
 * @return	Color produced by a single light at a single point.
 */
 
color totalColor(const Material &mat, const LightColor &lightColor,
				const dvec3 &v, const dvec3 &n,
				const dvec3 &lightPos, const dvec3 &intersectionPt,
				bool attenuationOn, 
				const LightATParams &ATparams) {
	/* CSE 386 - todo  */

	
	
	dvec3 l = glm::normalize(lightPos - intersectionPt);
	dvec3 r = 2 * (glm::dot(l, n)) * n - l;
	if (attenuationOn) {
		double length = glm::length(lightPos - intersectionPt);
		double at = 1 / (ATparams.constant + ATparams.linear * length +
			ATparams.quadratic * std::pow(length, 2));
		return  glm::clamp(ambientColor(mat.ambient, lightColor.ambient) +
			at * (diffuseColor(mat.diffuse, lightColor.diffuse, l, n)
				+ specularColor(mat.specular, lightColor.specular, mat.shininess, r, v)), 0.0, 1.0);
	} else {
		return glm::clamp(ambientColor(mat.ambient, lightColor.ambient)
				+ diffuseColor(mat.diffuse, lightColor.diffuse, l, n) 
					+ specularColor(mat.specular, lightColor.specular, mat.shininess, r, v), 0.0, 1.0);
	}
}

/**
 * @fn	color PositionalLight::illuminate(const dvec3 &interceptWorldCoords, 
 *										const dvec3 &normal, const Material &material, 
 *										const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in RAYTRACING applications.
 * @param	interceptWorldCoords	(x, y, z) at the intercept point.
 * @param	normal				The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color PositionalLight::illuminate(const dvec3& interceptWorldCoords,
									const dvec3& normal,
									const Material& material,
									const Frame& eyeFrame, bool inShadow) const {
	/* CSE 386 - todo  */
	if (!isOn) {
		return black;
	}
	else if (inShadow) {

		return ambientColor(material.ambient, lightColor.ambient);
	}
	else {
		dvec3 cameraPos = eyeFrame.origin;
		dvec3 v = glm::normalize(cameraPos - interceptWorldCoords);
		return totalColor(material, lightColor, v, normal, actualPosition(eyeFrame), interceptWorldCoords, 
			attenuationIsTurnedOn, atParams);
	}
	
}

/*
* @fn PositionalLight::actualPosition(const Frame& eyeFrame) const
* @briefReturns the global world coordinates of this light.
* @param eyeFrame The camera's frame
* @return The global world coordinates of this light. This will the light's
* raw position. Or, it will be the position relative to the camera's
* frame (transformed into the world coordinate frame).
*/

dvec3 PositionalLight::actualPosition(const Frame& eyeFrame) const {
	return isTiedToWorld ? pos : eyeFrame.toWorldCoords(pos);

}

dvec3 SpotLight::actualVector(const Frame& eyeFrame) const {
	return isTiedToWorld ? spotDir : eyeFrame.toWorldVector(spotDir);
}
/**
 * @fn	color SpotLight::illuminate(const dvec3 &interceptWorldCoords, 
 *									const dvec3 &normal, const Material &material, 
 *									const Frame &eyeFrame, bool inShadow) const
 * @brief	Computes the color this light produces in raytracing applications.
 * @param	interceptWorldCoords				The surface properties of the intercept point.
 * @param	normal					The normal vector.
 * @param	material			The object's material properties.
 * @param	eyeFrame			The coordinate frame of the camera.
 * @param	inShadow			true if the point is in a shadow.
 * @return	The color produced at the intercept point, given this light.
 */

color SpotLight::illuminate(const dvec3 &interceptWorldCoords,
							const dvec3 &normal,
							const Material &material,
							const Frame &eyeFrame, bool inShadow) const {
	/* CSE 386 - todo  */
	bool isCone = inCone(actualPosition(eyeFrame), actualVector(eyeFrame), fov, interceptWorldCoords);
	if (!isCone || !isOn) {
		return black;
	}
	else if (inShadow) {
		return ambientColor(material.ambient, lightColor.ambient);
	}else {
		dvec3 cameraPos = eyeFrame.origin;
		dvec3 v = glm::normalize(cameraPos - interceptWorldCoords);
		return totalColor(material, lightColor, v, normal, actualPosition(eyeFrame), interceptWorldCoords,
			attenuationIsTurnedOn, atParams);
	}
}

/**
* @fn	void setDir (double dx, double dy, double dz)
* @brief	Sets the direction of the spotlight.
* @param	dx		x component of the direction
* @param	dy		y component of the direction
* @param	dz		z component of the direction
*/

void SpotLight::setDir(double dx, double dy, double dz) {
	spotDir = glm::normalize(dvec3(dx, dy, dz));
}

/**
* @fn	bool inCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept)
* @brief	Determines if an intercept point falls within a spotlight's cone.
* @param	spotPos		where the spotlight is positioned
* @param	spotDir		normalized direction of spotlight's pointing direction
* @param	spotFOV		spotlight's field of view, which is 2X of the angle from the viewing axis
* @param	intercept	the position of the intercept.
*/

bool inCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept) {
	/* CSE 386 - todo  */
	dvec3 l = glm::normalize((intercept - spotPos));
	double spotCosine = glm::dot(l, spotDir);
	if (spotCosine > glm::cos(spotFOV / 2)) {
		return true;
	}
	else {
		return false;
	}
}

/**
* @fn	bool inShadow(const dvec3& lightPos, const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr> objects)
* @brief	Determines if an intercept point falls in a shadow.
* @param	lightPos		where the spotlight is positioned
* @param	intercept	the position of the intercept.
* @param	normal		the normal vector at the intercept point
* @param	objects		the collection of opaque objects in the scene
*/

bool inShadow(const dvec3& lightPos, const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr>& objects) {
	/* CSE 386 - todo  */
	HitRecord hits;
	double lightDistance = glm::distance(lightPos, intercept);
	dvec3 lightV = glm::normalize(lightPos - intercept);
	Ray ray = Ray(intercept, lightV);
	
	for (int i = 0; i < objects.size(); i++) {
		Ray shadowFeeler = Ray(intercept + EPSILON * normal, lightV);
		objects[i]->findClosestIntersection(shadowFeeler, hits);

		if (hits.t != FLT_MAX && glm::distance(hits.interceptPt, intercept) < lightDistance) {
			return true;
		}
		else {
	//		return false;
		}
	}
	return false;
}
