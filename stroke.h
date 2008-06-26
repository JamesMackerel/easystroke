#ifndef __STROKE_H__
#define __STROKE_H__

#include <gdkmm/pixbuf.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

#define STROKE_SIZE 64

class Stroke;
class PreStroke;

typedef boost::shared_ptr<Stroke> RStroke;
typedef boost::shared_ptr<PreStroke> RPreStroke;

struct Integral {
	struct {
		struct {
			double x;
			double y;
		} s, i; // square or not
	} d, i; // derivative or not
};

template<class T> class EasyIterator {
public:
	virtual const T operator++(int) = 0;
	virtual operator bool() = 0;
};

class PreStroke;
class Stroke {
	friend class PreStroke;
	friend class boost::serialization::access;
	struct Point {
		double x;
		double y;
		double time;
		template<class Archive> void serialize(Archive & ar, const unsigned int version) {
			ar & x; ar & y; ar & time;
		}
	};
	class RefineIterator;
	class InterpolateIterator;
	class RIIterator;

	class StrokeIntegral;
	class DiffIntegral;
	static inline Integral integral(EasyIterator<Point>&);
	Integral integral() const;
	static Integral diff_integral(RStroke a, RStroke b);
	static void integral2(RStroke a, RStroke b, double &int_x, double &int_y, double &int_dx, double &int_dy);
	double length() const;
	int size() const { return points.size(); }

	Stroke(PreStroke &s, int button_);

        Glib::RefPtr<Gdk::Pixbuf> draw_(int) const;
	mutable Glib::RefPtr<Gdk::Pixbuf> pb;
	std::vector<Point> points;
	int button;

	static Glib::RefPtr<Gdk::Pixbuf> drawEmpty_(int);
	static Glib::RefPtr<Gdk::Pixbuf> pbEmpty;

	template<class Archive> void serialize(Archive & ar, const unsigned int version) {
		ar & points;
		if (version == 0) return;
		ar & button;
	}
public:
	Stroke() : button(0) {}
	static RStroke create(PreStroke &s, int button_) { return RStroke(new Stroke(s, button_)); }

        Glib::RefPtr<Gdk::Pixbuf> draw(int size) const;
	void draw(Cairo::RefPtr<Cairo::Surface> surface, int x, int y, int w, int h, bool invert = true) const;
	void draw_svg(std::string filename) const;

	static RStroke trefoil();
	static double compare(RStroke, RStroke);
	static double compare(RStroke, RStroke, double);
	static Glib::RefPtr<Gdk::Pixbuf> drawEmpty(int);

	void print() const;
	void normalize();
	bool trivial() const { return size() == 0 && button == 0; }
};
BOOST_CLASS_VERSION(Stroke, 1)

class PreStroke {
	friend class Stroke;
	typedef Stroke::Point Point;
	std::vector<Point> points;
	PreStroke() {}
public:
	static RPreStroke create() { return RPreStroke(new PreStroke()); }

	void add(double x, double y, double time) {
		Point p = {x, y, time};
		points.push_back(p);
	}

	void add(double x, double y) {
		if (points.empty())
			add(x,y,0); else
			add(x,y, points.back().time + 1);
	}

	void clear() { points.clear(); }

	bool valid() const { return points.size() > 2; }
	int size() const { return points.size(); }
};

#endif
