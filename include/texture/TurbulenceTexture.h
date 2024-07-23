#ifndef TURBULENCETEXTURE_H
#define TURBULENCETEXTURE_H

#include "Texture.h"

class LatticeNoise;

class TurbulenceTexture : public Texture
{
	public:

		TurbulenceTexture(void);

		TurbulenceTexture(LatticeNoise* ln_ptr);

		TurbulenceTexture(const TurbulenceTexture& tt);

		virtual TurbulenceTexture*
		clone(void) const;

		virtual
		~TurbulenceTexture(void);

		void
		set_noise(LatticeNoise* ln_ptr);

		void
		set_color(const RGBColor& c);

		void
		set_color(const float r, const float g, const float b);

		void
		set_color(const float c);

		void
		set_min_value(const float mi);

		void
		set_max_value(const float ma);

		virtual RGBColor
		get_color(const ShadeRec& sr) const;

	protected:

		TurbulenceTexture&
		operator= (const TurbulenceTexture& rhs);

    private:

		LatticeNoise*		noise_ptr;
		RGBColor 	  		color;
		float		  		min_value, max_value;	// scaling factors
};

#endif // TURBULENCETEXTURE_H
