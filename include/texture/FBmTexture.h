#ifndef FBMTEXTURE_H
#define FBMTEXTURE_H

#include "Texture.h"
#include "LatticeNoise.h"

class FBmTexture : public Texture
{
	public:

		FBmTexture(void);

		FBmTexture(LatticeNoise* ln_ptr);

		FBmTexture(const FBmTexture& ft);

		virtual FBmTexture*
		clone(void) const;

		virtual
		~FBmTexture(void);

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

		FBmTexture&
		operator= (const FBmTexture& rhs);

    private:

		LatticeNoise*		noise_ptr;
		RGBColor 	  		color;
		float		  		min_value, max_value;	// scaling factors
};


#endif // FBMTEXTURE_H
