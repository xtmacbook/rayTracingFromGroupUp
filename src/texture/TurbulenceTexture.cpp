#include "LatticeNoise.h"

#include "TurbulenceTexture.h"
// ---------------------------------------------------------------- default constructor

TurbulenceTexture::TurbulenceTexture(void)
	: Texture(),
	noise_ptr(NULL),
	color(0.0),
	min_value(-0.1),
	max_value(1.1)
{}

// ----------------------------------------------------------------constructor

TurbulenceTexture::TurbulenceTexture(LatticeNoise* ln_ptr)
	: Texture(),
	noise_ptr(ln_ptr),
	color(0.0),
	min_value(-0.1),
	max_value(1.1)
{}

// ---------------------------------------------------------------- copy constructor

TurbulenceTexture::TurbulenceTexture(const TurbulenceTexture& tt)
	: Texture(tt),
	noise_ptr(tt.noise_ptr),
	color(tt.color),
	min_value(tt.min_value),
	max_value(tt.max_value)
{}


// ---------------------------------------------------------------------- clone

TurbulenceTexture*
TurbulenceTexture::clone(void) const {
	return (new TurbulenceTexture(*this));
}


// ---------------------------------------------------------------------- destructor

TurbulenceTexture::~TurbulenceTexture(void) {}


// ---------------------------------------------------------------------- assignment operator

TurbulenceTexture&
TurbulenceTexture::operator= (const TurbulenceTexture& rhs) {
	if (this == &rhs)
		return (*this);

	Texture::operator= (rhs);

	noise_ptr = rhs.noise_ptr;
	color = rhs.color;
	min_value = rhs.min_value;
	max_value = rhs.max_value;

	return (*this);
}


// --------------------------------------------------- get_color

RGBColor
TurbulenceTexture::get_color(const ShadeRec& sr) const {

	float value = noise_ptr->value_turbulence(sr.local_hit_point); // in the range (0, 1)
	value = min_value + (max_value - min_value) * value;	// in the range (min_value, max_value)

	if ((value < 0.0) || (value > 1.0)) {
		return (RGBColor(1.0, 0.0, 0.0));
	}
	else
		return (value * color);
}



// ---------------------------------------------------------------- set_color

void
TurbulenceTexture::set_noise(LatticeNoise* ln_ptr) {
	noise_ptr = ln_ptr;
}


// ---------------------------------------------------------------- set_color

void
TurbulenceTexture::set_color(const RGBColor& c) {
	color = c;
}


// ---------------------------------------------------------------- set_color

void
TurbulenceTexture::set_color(const float r, const float g, const float b) {
	color.r = r; color.g = g; color.b = b;
}


// ---------------------------------------------------------------- set_color

void
TurbulenceTexture::set_color(const float c) {
	color.r = c; color.g = c; color.b = c;
}


// ---------------------------------------------------------------- set_min_value

void
TurbulenceTexture::set_min_value(const float mi) {
	min_value = mi;
}


// ---------------------------------------------------------------- set_max_value

void
TurbulenceTexture::set_max_value(const float ma) {
	max_value = ma;
}
