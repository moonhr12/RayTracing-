/****************************************************
 * 2016-2020 Eric Bachmann and Mike Zmuda
 * All Rights Reserved.
 * NOTICE:
 * Dissemination of this information or reproduction
 * of this material is prohibited unless prior written
 * permission is granted..
 ****************************************************/

#pragma once
#include <iostream>
#include <vector>
#include "defs.h"
#include "hitrecord.h"
#include "ishape.h"

 /**
  * @struct	LightATParams
  * @brief	A light attenuation parameters.
  */

struct LightATParams {
	double constant, linear, quadratic;	//!< Parameters controlling attenuation.
	LightATParams(double C, double L, double Q) {
		constant = C;
		linear = L;
		quadratic = Q;
	}
	double factor(double distance) const {
		return 1.0 / (constant + linear * distance + quadratic * distance * distance);
	}
};

/**
 * @struct	LightColor
 * @brief	Represents the colors of each light component.
 */

struct LightColor {
	color ambient, diffuse, specular;	//!< The three light components.
	LightColor(const color& amb, const color& dif, const color& spec)
		: ambient(amb), diffuse(dif), specular(spec) {
	}
	LightColor(const color& oneColor)
		: ambient(oneColor), diffuse(oneColor), specular(oneColor) {
	}
	LightColor(const vector<double>& C)
		: LightColor(color(C[0], C[1], C[2]),
			color(C[3], C[4], C[5]),
			color(C[6], C[7], C[8])) {
	}
};

/**
 * @struct	LightSource
 * @brief	A generic light source.
 */

struct LightSource {
	bool isOn;			//!< True if the light is active; otherwise, has no effect.
	LightSource() {
		isOn = true;
	}
	virtual color illuminate(const dvec3& interceptWorldCoords,
		const dvec3& normal,
		const Material& material,
		const Frame& eyeFrame, bool inShadow) const = 0;
};

/**
 * @struct	PositionalLight
 * @brief	Represents a simple positional light source.
 */

struct PositionalLight : public LightSource {
	dvec3 pos;					//!< The position of the light.
	bool attenuationIsTurnedOn;	//!< true if attenuation is active.
	bool isTiedToWorld;			//!< true if the position is in world (or eye) coordinates.
	LightATParams atParams;
	LightColor lightColor;

	PositionalLight(const dvec3& position, const LightColor& color)
		: LightSource(), lightColor(color), pos(position), atParams(0.0, 1.0, 0.0) {
		attenuationIsTurnedOn = false;
		isTiedToWorld = true;
	}
	void setAttenuation(bool isOn) {
		attenuationIsTurnedOn = isOn;
	}
	void setAttenuationParams(const LightATParams& params) {
		atParams = params;
	}
	dvec3 actualPosition(const Frame& eyeFrame) const;
	virtual color illuminate(const dvec3& interceptWorldCoords,
		const dvec3& normal,
		const Material& material,
		const Frame& eyeFrame, bool inShadow) const;
};

/**
 * @struct	SpotLight
 * @brief	A spot light.
 */

struct SpotLight : public PositionalLight {
	double fov;				//!< Field of view of the light.
	dvec3 spotDir;			//!< Direction of spotlight.
	SpotLight(const dvec3& position, const dvec3& dir,
		double angleInRadians, const LightColor& lightColor)
		: PositionalLight(position, lightColor), spotDir(dir),
		fov(angleInRadians) {
	}
	dvec3 actualVector(const Frame& eyeFrame) const;
	virtual color illuminate(const dvec3& interceptWorldCoords,
		const dvec3& normal,
		const Material& material,
		const Frame& eyeFrame, bool inShadow) const;
	void setDir(double dx, double dy, double dz);
};

const LightColor pureWhiteLight(vector<double>{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0});

color ambientColor(const color& matAmbient, const color& lightAmbient);
color diffuseColor(const color& matDiffuse, const color& lightDiffuse,
	const dvec3& l, const dvec3& n);
color specularColor(const color& mat, const color& light,
	double shininess,
	const dvec3& r, const dvec3& v);
color totalColor(const Material& mat, const LightColor& lightColor,
	const dvec3& v, const dvec3& n,
	const dvec3& lightPos, const dvec3& intersectionPt,
	bool attenuationOn,
	const LightATParams& ATparams);
bool inCone(const dvec3& spotPos, const dvec3& spotDir, double spotFOV, const dvec3& intercept);
bool inShadow(const dvec3& lightPos, const dvec3& intercept, const dvec3& normal, const vector<VisibleIShapePtr>& objects);

typedef LightSource* LightSourcePtr;
typedef PositionalLight* PositionalLightPtr;
typedef SpotLight* SpotLightPtr;