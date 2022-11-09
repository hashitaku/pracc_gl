#version 460

layout(location = 0) uniform vec2 winsize;
layout(location = 1) uniform float scale;
layout(location = 2) uniform vec2[5] roots;
layout(location = 7) uniform vec3[5] colors;

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

struct Dual {
	Complex real;
	Complex imag;
};

Dual d_one() { return Dual(Complex(1.0, 0.0), Complex(0.0, 0.0)); }
Dual d_i() { return Dual(Complex(0.0, 0.0), Complex(1.0, 0.0)); }

Dual d_add(Dual self, Dual other) {
    return Dual(c_add(self.real, other.real), c_add(self.imag, other.imag));
}

Dual d_sub(Dual self, Dual other) {
    return Dual(c_sub(self.real, other.real), c_sub(self.imag, other.imag));
}

Dual d_mul(Dual self, Dual other) {
    return Dual(c_mul(self.real, other.real),
                c_add(c_mul(self.real, other.imag), c_mul(self.imag, other.real)));
}

Dual d_div(Dual self, Dual other) {
    Complex norm = c_mul(other.real, other.real);
    return Dual(c_div(c_mul(self.real, other.real), norm),
                c_div(c_sub(c_mul(self.imag, other.real), c_mul(self.real, other.imag)), norm));
}

Dual f(Dual z) {
	Dual ret = d_one();
	for (uint i = 0; i < roots.length(); i++) {
		ret = d_mul(ret, d_sub(z, Dual(Complex(roots[i].x, roots[i].y), Complex(0.0, 0.0))));
	}

	return ret;
}

Dual df(Dual z) {
	return f(z);
}

Complex newton(Complex init, uint n) {
	for (uint i = 0; i < n; i++) {
		init = c_sub(init, c_div(f(Dual(init, c_one())).real, df(Dual(init, c_one())).imag));
	}

	return init;
}

void main() {
    vec2 p = (gl_FragCoord.xy * 2.0 - winsize.xy) / min(winsize.x, winsize.y);

    p *= scale;
	Complex a = newton(Complex(p.x, p.y), 100);

	vec3 c = colors[0];
	float d = distance(vec2(a.real, a.imag), roots[0]);
	for (uint i = 1; i < roots.length(); i++) {
		if (d > distance(vec2(a.real, a.imag), roots[i])) {
			c = colors[i];
			d = distance(vec2(a.real, a.imag), roots[i]);
		}
	}

	fragment = vec4(c, 1.0);
}
