#include"visitors.hpp"
void expose_boxes(py::module& mod){
	py::class_<AlignedBox3r> cAB3r(mod,"AlignedBox3","Axis-aligned box object, defined by its minimum and maximum corners");
	AabbVisitor<AlignedBox3r>::visit(cAB3r);
	py::class_<AlignedBox2r> cAB2r(mod,"AlignedBox2","Axis-aligned box object in 2d, defined by its minimum and maximum corners");
	AabbVisitor<AlignedBox2r>::visit(cAB2r);
}
