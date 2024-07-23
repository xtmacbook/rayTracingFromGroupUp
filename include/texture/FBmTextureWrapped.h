#ifndef FBMTEXTUREWRAPPED_H
#define FBMTEXTUREWRAPPED_H

#include "Texture.h"
#include "LatticeNoise.h"

class FBmTextureWrapped : public Texture
{
	public:

		FBmTextureWrapped(void);

		FBmTextureWrapped(LatticeNoise* ln_ptr);

		FBmTextureWrapped(const FBmTextureWrapped& ft);

		virtual FBmTextureWrapped*
		clone(void) const;

		virtual
		~FBmTextureWrapped(void);

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

		void
		set_expansion_number(const float en);

		virtual RGBColor
		get_color(const ShadeRec& sr) const;

	protected:

		FBmTextureWrapped&
		operator= (const FBmTextureWrapped& rhs);

    private:

		LatticeNoise*		noise_ptr;
		RGBColor 	  		color;
		float		  		min_value, max_value;	// scaling factors
		float               expansion_number;       // expand the amplitude of the noise function
};

#endif // FBMTEXTUREWRAPPED_H
