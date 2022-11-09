#version 460

layout(location = 0) uniform vec2 winsize;
layout(location = 1) uniform vec2 init;

layout(location = 0) out vec4 fragment;

struct Complex {
	float real;
	float imag;
};

Complex c_one() { return Complex(1.0, 0.0); }
Complex c_i() { return Complex(0.0, 1.0); }

float c_norm(Complex c) {
    return length(vec2(c.real, c.imag));
}

Complex c_add(Complex self, Complex other) {
    return Complex(self.real + other.real, self.imag + other.imag);
}

Complex c_sub(Complex self, Complex other) {
    return Complex(self.real - other.real, self.imag - other.imag);
}

Complex c_mul(Complex self, Complex other) {
    return Complex(self.real * other.real - self.imag * other.imag,
                self.real * other.imag + self.imag * other.real);
}

Complex c_div(Complex self, Complex other) {
    float norm = c_norm(other);
    return Complex((self.real * other.real + self.imag * other.imag) / (norm * norm),
                   (self.imag * other.real - self.real * other.imag) / (norm * norm));
}

vec3 julia(Complex z, Complex c, uint n) {
	Complex ret = c_add(c_mul(z, z), c);
	uint i = 0;
	for (i = 0; i < n; i++) {
		ret = c_add(c_mul(ret, ret), c);

		if (c_norm(ret) > 2.0) break;
	}

	return vec3(ret.real, ret.imag, i);
}

void main() {
    vec2 p = (gl_FragCoord.xy * 2.0 - winsize) / min(winsize.x, winsize.y);
	p *= 2.0;
    p.x -= 4.0;
    vec2 c = init * 1.5;
    c.x -= 0.5;

	vec3 a = julia(Complex(p.x, p.y), Complex(c.x, c.y), 50);
	vec3 color;
	color.x = 1.0 * (a.z / 50) * float(length(a.xy) > 2.0);
	color.y = 1.0 * (a.z / 50) * float(length(a.xy) > 2.0);
	color.z = 1.0 * (1.0 - a.z / 100) * float(length(a.xy) > 2.0);

	fragment = vec4(color, 1.0);
}
