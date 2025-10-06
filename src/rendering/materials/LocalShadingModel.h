#pragma once
enum class LocalShadingModel
{
	COOK_TORRANCE = 0,
	SUBSURFACE_SCATTERING = 1,
	SHEEN = 2,
	CLEARCOAT = 3,
	THIN_TRANSCLUCENT = 4,

	PHONG_BLINN = 10,

	CEL = 20,
	TOON = 21,
};

